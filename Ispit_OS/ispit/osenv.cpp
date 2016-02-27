#include<WinSock2.h>
#include <windows.h>
#include <sddl.h>

#include"defines.h"
#include"defines.h"
#include "osenv.h"

#include "mem.h"
#include "str.h"
#include "fs.h"
#include "registry.h"
#include "WinAPITypes.h"

void OsEnv::init(void)
{


}

void OsEnv::uninit(void)
{


}

DWORD OsEnv::_getVersion(void)
{
  DWORD ver = VERSION_UNKNOWN;
  OSVERSIONINFOEXW osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

  if(CWA(kernel32, GetVersionExW)((OSVERSIONINFOW *)&osvi) != FALSE && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if(osvi.wProductType == VER_NT_WORKSTATION)
    {
      //Windows 2000 - 5.0
      if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)ver = VERSION_2000;
      //Windows XP -  5.1
      else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)ver = VERSION_XP;
      //Windows XP Professional x64 Edition - 5.2
      else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)ver = VERSION_XP;
      //Windows Vista - 6.0
      else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 0)ver = VERSION_VISTA;
      //Windows 7 - 6.1
      else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 1)ver = VERSION_SEVEN;
	  //Windows 8 - 6.2
	  else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 2)ver = VERSION_EIGHT;
    }
    else if(osvi.wProductType == VER_NT_DOMAIN_CONTROLLER || osvi.wProductType == VER_NT_SERVER)
    {
      //Windows Server 2003 - 5.2, Windows Server 2003 R2 - 5.2, Windows Home Server - 5.2
      if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)ver = VERSION_S2003;
      //Windows Server 2008 - 6.0
      else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 0)ver = VERSION_S2008;
      //Windows Server 2008 R2 - 6.1
      else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 1)ver = VERSION_S2008R2;
	  //Windows Server 2012 - 6.2
	  else if(osvi.dwMajorVersion == 6  && osvi.dwMinorVersion == 2)ver = VERSION_S2012;
    }
  }
  return ver;
}

void OsEnv::_getVersionEx(OSINFO *oi)
{
  Mem::_zero(oi, sizeof(OSINFO));

  OSVERSIONINFOEXW osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

  if(CWA(kernel32, GetVersionExW)((OSVERSIONINFOW *)&osvi) != FALSE)
  {
    SYSTEM_INFO si;
    CWA(kernel32, GetNativeSystemInfo)(&si);

    oi->version      = _getVersion();
    oi->sp           = (osvi.wServicePackMajor > 0xFF || osvi.wServicePackMinor != 0) ? 0 : LOBYTE(osvi.wServicePackMajor);
    oi->build        = osvi.dwBuildNumber > 0xFFFF ? 0 : LOWORD(osvi.dwBuildNumber);
    oi->architecture = si.wProcessorArchitecture;
  }
}

bool OsEnv::_getUserProfileDirectoryhBySid(PSID sid, LPWSTR buffer)
{
	bool retVal = false;
	LPWSTR sidStr;
	if(CWA(kernel32, ConvertSidToStringSidW)(sid, &sidStr) != FALSE)
	{
		WCHAR regPathFormat[]=L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s";
		WCHAR regPath[MAX_PATH];
		if(Str::_sprintfW(regPath, MAX_PATH, regPathFormat, sidStr) > 0)
		{
			WCHAR regValuePath[]=L"ProfileImagePath";
			DWORD r = Registry::_getValueAsString(HKEY_LOCAL_MACHINE, regPath, regValuePath, regPath, MAX_PATH);
			if(r != 0 && r != (DWORD)-1)retVal = Fs::_unquoteAndExpandPath(regPath, buffer);
		}
		CWA(kernel32, LocalFree)(sidStr);
	}
	return retVal;
}