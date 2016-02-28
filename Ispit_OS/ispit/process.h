/*
  Work with processes.
*/
#pragma once

#include "defines.h"

//ID of current process.
#define CURRENT_PROCESS ((HANDLE)-1)

namespace Process
{
  //The application level.
  enum
  {
    INTEGRITY_UNKNOWN,
    INTEGRITY_LOW,     //Untrusted (Process can only write to low integrity locations, such as the Temporary Internet Files\Low folder or the HKEY_CURRENT_USER\Software\LowRegistry key)
    INTEGRITY_MEDIUM,  //User (Process can create and modify files in the user's Documents folder and write to user-specific areas of the registry, such as HKEY_CURRENT_USER.)
    INTEGRITY_HIGH     //Administrative (Process can install files to the Program Files folder and write to sensitive registry areas like HKEY_LOCAL_MACHINE.)
  };
  
  //flags for showShellRunDialog().
  enum
  {
    RFD_NOBROWSE        = 0x00000001, //Do not display the "Browse".
    RFD_NODEFFILE       = 0x00000002, //Do not select the default file.
    RFD_USEFULLPATHDIR  = 0x00000004, //Use the working directory of the executable file path (ignored if workingDir! = NULL).
    RFD_NOSHOWOPEN      = 0x00000008  //Do not display a label for the Edit.
  };

  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Retrieve a handle of the kernel32 module from the PE file.

    Return        - kernel32 module handle, or NULL if not found.
  */
  HMODULE ASM_INTERNAL_DEF _getKernel32Handle(void);

  /*
    Retrieve SID of the process.

    IN process    - process handle with permission PROCESS_QUERY_INFORMATION.
    OUT sessionId - session ID, or NULL if not important.
    
    Return        - data SID (must be freed by Mem), or NULL on error.
  */
  TOKEN_USER *_getUserByProcessHandle(HANDLE process, LPDWORD sessionId);

  /*
    Retrieve SID of the process.

    IN id         - ID of process.
    OUT sessionId - session ID, or NULL if not important.

    Return        - data SID (must be freed by Mem), or NULL on error.
  */
  TOKEN_USER *_getUserByProcessId(DWORD id, LPDWORD sessionId);

  /*
    Setting privileges for the current thread or process.

    IN privilegeName - privilege name.
    IN enable        - true - enable, false - disable.

    Return           - true - in cases of successful privilege change,
                       false - in the case of an error, or if the privilege has been already selected.
  */
  bool _enablePrivilege(LPWSTR privilegeName, bool enable);

  /*
    Retrieve the INTEGRITY level of a process.

	IN process - process handle (required permission PROCESS_QUERY_INFORMATION), -1 for the
				 the current process.

    Return     - INTEGRITY_*.
  */
  BYTE _getIntegrityLevel(HANDLE process);

  /*
    Checks whether the process is under WOW64.

    IN process - process handle with PROCESS_QUERY_INFORMATION or PROCESS_QUERY_LIMITED_INFORMATION.

    Return     - true - if under WOW64,
                 false - if not under WOW64.
  */
#if !defined _WIN64
  bool _isWow64(HANDLE process);
#endif

  /*
    Wrapper for CreateThread().

    IN stackSize    - stack size.
    IN startAddress - entry point. 
    IN parameter    - parameter.

    Return          - ID thread, or 0 in case of error.
  */
  DWORD _createThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE startAddress, LPVOID parameter);

};
