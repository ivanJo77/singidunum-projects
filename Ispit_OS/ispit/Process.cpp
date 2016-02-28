#include <WTypes.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>

#include "defines.h"
#include "mem.h"
#include "process.h"
#include "winsecurity.h"

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


