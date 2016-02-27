/*
  OS data.
*/
#pragma once

namespace OsEnv
{
  //OS version.
  enum
  {
    VERSION_UNKNOWN,
    VERSION_2000,
    VERSION_XP,
    VERSION_S2003,
    VERSION_VISTA,
    VERSION_S2008,
    VERSION_SEVEN,
    VERSION_S2008R2,
	VERSION_EIGHT,
	VERSION_S2012,
  };
  
  //short information on OS.
# pragma pack(push, 1)
  typedef struct
  {
    BYTE version;      //VERSION_*.
    BYTE sp;           //Service Pack.
    WORD build;        //build version.
    WORD architecture; //computer architecture.
  }OSINFO;
# pragma pack(pop)  
  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Retrieve current Windows version.

    Return - VERSION_*.
  */
  DWORD _getVersion(void);

  /*
    Retrieve current Windows version.

    OUT oi - Windows version info.
  */
  void _getVersionEx(OSINFO *oi);

  /*
    Obtaining directory by user profile SID.

    IN sid     - SID.
    OUT buffer - buffer for the path, must be at least MAX_PATH.

    Return     - true - path successfully retrieved,
                 false - path not retrieved.
  */
  bool _getUserProfileDirectoryhBySid(PSID sid, LPWSTR buffer);
};
