#include <windows.h>

#include "defines.h"
#include "defines.h"
#include "sync.h"

HANDLE Sync::_waitForMutex(SECURITY_ATTRIBUTES *mutexAttributes, LPWSTR name)
{
  HANDLE mutexHandle = CWA(kernel32, CreateMutexW)(mutexAttributes, FALSE, name);
  if(mutexHandle != NULL)
  {
    DWORD r = CWA(kernel32, WaitForSingleObject)(mutexHandle, INFINITE);
    if(r == WAIT_OBJECT_0 || r == WAIT_ABANDONED)return mutexHandle;
    CWA(kernel32, CloseHandle)(mutexHandle);
  }
  return NULL;
}

void Sync::_freeMutex(HANDLE mutex)
{
  CWA(kernel32, ReleaseMutex)(mutex);
  CWA(kernel32, CloseHandle)(mutex);
}

HANDLE Sync::_createUniqueMutex(SECURITY_ATTRIBUTES *mutexAttributes, LPWSTR name)
{
  HANDLE mutexHandle = CWA(kernel32, CreateMutexW)(mutexAttributes, TRUE, name);
  if(mutexHandle != NULL && CWA(kernel32, GetLastError)() == ERROR_ALREADY_EXISTS)
  {
    CWA(kernel32, CloseHandle)(mutexHandle);
    mutexHandle = NULL;
  }
  
  return mutexHandle;  
}

