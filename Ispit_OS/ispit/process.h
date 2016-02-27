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
    Retrieve a handle of the process module.

    IN process    - process. Required permissions PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
    IN moduleName - module name to search. Can be either the base name or fully qualified.

    Return        - module handle, or NULL if not found.
  */
  HMODULE _getModuleHandle(HANDLE process, LPWSTR moduleName);


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
    Count of threads within process.

    IN id  - ID of process.
    
    Return - number of threads, or (DWORD)-1 in case of error.
  */
  DWORD _getCountOfThreadsByProcessId(DWORD id);

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
    Create of a new process.

    IN module      - module name, may be NULL.
    IN commandLine - command line, may be NULL. The specified address can not be a constant!
    IN workDir     - working directory of the new process, may be NULL.
    IN starupInfo  - STARTUPINFO, may be NULL.
    OUT pi         - PROCESS_INFORMATION, may be NULL.

    Return         - ID of process - in case of success,
                     0 - in case of error.
  */
  DWORD _create(const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi);

  /*
    Extended _create(), which creates a process with the command line, where the first argument 
	is the name of the module. That is, in the most frequently used form of running processes, 
	it is recommended to use exactly this function instead of _create(), this is due to the 
	stupidity of some decoders that always put in the first argument, the process name.

    IN module      - module name, may not be NULL.
    IN commandLine - command line, may be NULL. The specified address can not be a constant!
    IN workDir     - working directory, may be NULL.
    IN starupInfo  - STARTUPINFO, may be NULL.
    OUT pi         - PROCESS_INFORMATION, may be NULL.

	Return         - ID of process - in case of success,
					 0 - in case of error.
  */
  DWORD _createEx(const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo,  PROCESS_INFORMATION *pi);


  /*
    Create of a new process.

    IN token       - a primary user token is created for the process. Required permissions:
                     TOKEN_QUERY, TOKEN_DUPLICATE, TOKEN_ASSIGN_PRIMARY.
    IN desktop     - Desktop in the format "machine\desktop". may be NULL. It makes no sense if
                     starupInfo != NULL.
    IN module      - module name, may be NULL.
    IN commandLine - command line, may be NULL. The specified address can not be a constant!
    IN workDir     - working directory of new process, may be NULL.
    IN starupInfo  - STARTUPINFO, may be NULL.
    OUT pi         - PROCESS_INFORMATION, may be NULL.

	Return         - ID of process - in case of success,
					 0 - in case of error.
  */
  DWORD _createAsUser(HANDLE token, const LPWSTR desktop, const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi);

  /*
    Extended _createAsUser(), which creates a process with the command line, where the first argument 
	is the name of the module. That is, in the most frequently used form of running processes, it is 
	recommended to useexactly this function instead _createAsUser(), this is due to the stupidity of 
	some decoders that always put in the first argument, the process name.

    IN token       - primary user token is created for the process. Required permissions:
                     TOKEN_QUERY, TOKEN_DUPLICATE, TOKEN_ASSIGN_PRIMARY.
    IN desktop     - Desktop in the format "machine\desktop". may be NULL. It makes no sense if
                     starupInfo != NULL.
    IN module      - module name, may be NULL.
    IN commandLine - command line, may be NULL. The specified address can not be a constant!
    IN workDir     - working directory of new process, may be NULL.
    IN starupInfo  - STARTUPINFO, may be NULL.
    OUT pi         - PROCESS_INFORMATION, may be NULL.

	Return         - ID of process - in case of success,
					 0 - in case of error.
  */
  DWORD _createAsUserEx(HANDLE token, const LPWSTR desktop, const LPWSTR module, const LPWSTR commandLine, const LPWSTR workDir, const STARTUPINFOW *starupInfo, PROCESS_INFORMATION *pi);

  /*
    Closing the handles and zeroing PROCESS_INFORMATION.

    IN OUT pi - PROCESS_INFORMATION.
  */
  void _closeProcessInformation(PROCESS_INFORMATION *pi);
  
  /*
    Wrapper for CreateThread().

    IN stackSize    - stack size.
    IN startAddress - entry point. 
    IN parameter    - parameter.

    Return          - ID thread, or 0 in case of error.
  */
  DWORD _createThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE startAddress, LPVOID parameter);

  /*
    Calling the standard Run dialog (RunFileDlg).

    IN owner      - parent, may be NULL.
    IN workingDir - working directory, may be NULL.
    IN title      - dialog title, may be NULL for default value.
    IN promt      - dialog prompt, may be NULL for default value.
    IN flags      - flags RFD_*.
  */
  void _showShellRunDialog(HWND owner, const LPWSTR workingDir, const LPWSTR title, const LPWSTR promt, DWORD flags);

  /*
    Remove batch file.

    IN fileForRemoving - the full name of the file to delete.

    Return             - true - in case of success,
                         false - in case of error.
  */
  bool _runBatchForRemoveFile(const LPWSTR fileForRemoving);

  /*
    Running bat-file with arbitrary content and it's removal.

    IN context - contents of a file encoded in OEM, will add an additional prefix and postfix.
    
	Return     - true - in case of success,
				 false - in case of error.
  */
  bool _runTempBatch(const LPSTR context);
};
