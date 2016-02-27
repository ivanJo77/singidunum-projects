#pragma once
#include<AccCtrl.h>
#include<wincrypt.h>
#include<lmcons.h>
#include<shellapi.h>
#include<wtypes.h>
#include<security.h>
#include<Ole2.h>
#include<WinInet.h>
#include<windows.h>
#include<winsock2.h>
#include<TlHelp32.h>
#include<ShlObj.h>
#include<Shlwapi.h>
#include<OleAuto.h>
#include"defines.h"

//////////////////////////////////////////////////////////////////////////
// ADVAPI32
//////////////////////////////////////////////////////////////////////////
typedef BOOL WINAPI api_advapi32_InitiateSystemShutdownExW
	(LPWSTR lpMachineName,LPWSTR lpMessage,DWORD dwTimeout,BOOL bForceAppsClosed,BOOL bRebootAfterShutdown,DWORD dwReason);
typedef BOOL WINAPI api_advapi32_EqualSid
	(PSID pSid1,PSID pSid2);
typedef BOOL WINAPI api_advapi32_IsWellKnownSid
	(PSID pSid,WELL_KNOWN_SID_TYPE WellKnownSidType);
typedef BOOL WINAPI api_advapi32_GetLengthSid
	(PSID pSid);
typedef BOOL WINAPI api_advapi32_RegEnumKeyExW(
	HKEY hKey,
	DWORD dwIndex,
	LPWSTR lpName,
	LPDWORD lpcchName,
	LPDWORD lpReserved,
	LPWSTR lpClass,
	LPDWORD lpcchClass,
	PFILETIME lpftLastWriteTime
	);
typedef BOOL WINAPI api_advapi32_OpenProcessToken(
	HANDLE ProcessHandle,
	DWORD DesiredAccess,
	PHANDLE TokenHandle
	);
typedef PDWORD WINAPI api_advapi32_GetSidSubAuthority(
	PSID pSid,
	DWORD nSubAuthority
	);
typedef PUCHAR WINAPI api_advapi32_GetSidSubAuthorityCount(
	PSID pSid
	);
typedef BOOL WINAPI api_advapi32_OpenThreadToken(
	HANDLE ThreadHandle,
	DWORD DesiredAccess,
	BOOL OpenAsSelf,
	PHANDLE TokenHandle
	);
typedef BOOL WINAPI api_advapi32_GetTokenInformation(
	HANDLE TokenHandle,
	TOKEN_INFORMATION_CLASS TokenInformationClass,
	LPVOID TokenInformation,
	DWORD TokenInformationLength,
	PDWORD ReturnLength
	);
typedef LSTATUS WINAPI api_advapi32_RegSetValueExW(
	HKEY hKey,
	LPCWSTR lpValueName,
	DWORD Reserved,
	DWORD dwType,
	CONST BYTE * lpData,
	DWORD cbData
	);
typedef LSTATUS WINAPI api_advapi32_RegCloseKey(
	HKEY hKey
	);
typedef LSTATUS WINAPI api_advapi32_RegCloseKey(
	HKEY hKey
	);
typedef LSTATUS WINAPI api_advapi32_RegCreateKeyExW(
	HKEY hKey,
	LPCWSTR lpSubKey,
	DWORD Reserved,
	LPWSTR lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition
	);
typedef LSTATUS WINAPI api_advapi32_RegQueryValueExW(
	HKEY hKey,
	LPCWSTR lpValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE lpData,
	LPDWORD lpcbData
	);
typedef LSTATUS WINAPI api_advapi32_RegOpenKeyExW(
	HKEY hKey,
	LPCWSTR lpSubKey,
	DWORD ulOptions,
	REGSAM samDesired,
	PHKEY phkResult
	);
typedef LSTATUS WINAPI api_advapi32_RegCloseKey(
	HKEY hKey
	);
typedef BOOL WINAPI api_advapi32_CreateProcessAsUserW(
	HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);
typedef BOOL WINAPI api_advapi32_InitializeSecurityDescriptor(
	PSECURITY_DESCRIPTOR pSecurityDescriptor,
	DWORD dwRevision
	);
typedef BOOL WINAPI api_advapi32_SetSecurityDescriptorDacl(
	PSECURITY_DESCRIPTOR pSecurityDescriptor,
	BOOL bDaclPresent,
	PACL pDacl,
	BOOL bDaclDefaulted
	);
typedef BOOL WINAPI api_advapi32_ConvertStringSecurityDescriptorToSecurityDescriptorW(
	LPCWSTR StringSecurityDescriptor,
	DWORD StringSDRevision,
	PSECURITY_DESCRIPTOR * SecurityDescriptor,
	PULONG SecurityDescriptorSize
	);
typedef BOOL WINAPI api_advapi32_GetSecurityDescriptorSacl(
	PSECURITY_DESCRIPTOR pSecurityDescriptor,
	LPBOOL lpbSaclPresent,
	PACL * pSacl,
	LPBOOL lpbSaclDefaulted
	);
typedef BOOL WINAPI api_advapi32_SetSecurityDescriptorSacl(
	PSECURITY_DESCRIPTOR pSecurityDescriptor,
	BOOL bSaclPresent,
	PACL pSacl,
	BOOL bSaclDefaulted
	);
typedef DWORD WINAPI api_advapi32_SetNamedSecurityInfoW(
	LPWSTR               pObjectName,
	SE_OBJECT_TYPE        ObjectType,
	SECURITY_INFORMATION  SecurityInfo,
	PSID              psidOwner,
	PSID              psidGroup,
	PACL              pDacl,
	PACL              pSacl
	);
typedef BOOL WINAPI api_advapi32_LookupPrivilegeValueW(
	LPCWSTR lpSystemName,
	    LPCWSTR lpName,
	   PLUID   lpLuid
	);
typedef BOOL WINAPI api_advapi32_AdjustTokenPrivileges(
	HANDLE TokenHandle,
	BOOL DisableAllPrivileges,
	PTOKEN_PRIVILEGES NewState,
	DWORD BufferLength,
	PTOKEN_PRIVILEGES PreviousState,
	PDWORD ReturnLength
	);
typedef BOOL WINAPI api_advapi32_ConvertSidToStringSidW(
	PSID Sid,
	LPWSTR * StringSid
	);
typedef BOOL WINAPI api_advapi32_CryptAcquireContextW(
	      HCRYPTPROV  *phProv,
	   LPCWSTR    szContainer,
	   LPCWSTR    szProvider,
	       DWORD       dwProvType,
	       DWORD       dwFlags
	);
typedef BOOL WINAPI api_advapi32_CryptCreateHash(
	   HCRYPTPROV  hProv,
	   ALG_ID      Algid,
	   HCRYPTKEY   hKey,
	   DWORD       dwFlags,
	  HCRYPTHASH  *phHash
	);
typedef BOOL WINAPI api_advapi32_CryptHashData(
	                   HCRYPTHASH  hHash,
	CONST BYTE  *pbData,
	                   DWORD   dwDataLen,
	                   DWORD   dwFlags
	);
typedef BOOL WINAPI api_advapi32_CryptGetHashParam(
	                                       HCRYPTHASH  hHash,
	                                       DWORD   dwParam,
	BYTE    *pbData,
	                                    DWORD   *pdwDataLen,
	                                       DWORD   dwFlags
	);
typedef BOOL WINAPI api_advapi32_CryptDestroyHash(
	   HCRYPTHASH  hHash
	);
typedef BOOL WINAPI api_advapi32_CryptReleaseContext(
	   HCRYPTPROV  hProv,
	   DWORD       dwFlags
	);

//////////////////////////////////////////////////////////////////////////
// NETAPI32
//////////////////////////////////////////////////////////////////////////
typedef NET_API_STATUS NET_API_FUNCTION api_netapi32_NetApiBufferFree(
	IN LPVOID Buffer
	);
typedef NET_API_STATUS NET_API_FUNCTION api_netapi32_NetUserEnum (
	   LPCWSTR     servername OPTIONAL,
	       DWORD      level,
	       DWORD      filter,
	LPBYTE     *bufptr,
	       DWORD      prefmaxlen,
	      LPDWORD    entriesread,
	      LPDWORD    totalentries,
	PDWORD resume_handle OPTIONAL
	);
typedef NET_API_STATUS NET_API_FUNCTION api_netapi32_NetUserGetInfo (
	 LPCWSTR     servername OPTIONAL,
	     LPCWSTR     username,
	     DWORD      level,
	LPBYTE     *bufptr
	);


//////////////////////////////////////////////////////////////////////////
// SHELL32
//////////////////////////////////////////////////////////////////////////
typedef int __stdcall api_shell32_ShellExecuteW(
	HWND hwnd, 
	LPCWSTR lpOperation, 
	LPCWSTR lpFile, 
	LPCWSTR lpParameters,
	LPCWSTR lpDirectory, INT nShowCmd
	);

//////////////////////////////////////////////////////////////////////////
// SECUR32
//////////////////////////////////////////////////////////////////////////
typedef BOOLEAN __stdcall api_secur32_GetUserNameExW(
	EXTENDED_NAME_FORMAT NameFormat,
	LPWSTR lpNameBuffer,
	PULONG nSize
	);

//////////////////////////////////////////////////////////////////////////
// OLE32
//////////////////////////////////////////////////////////////////////////
typedef void __stdcall api_ole32_CoUninitialize(
	void
	);
typedef HRESULT __stdcall api_ole32_CoCreateInstance(
	  REFCLSID rclsid,
	  LPUNKNOWN pUnkOuter,
	  DWORD dwClsContext,
	  REFIID riid,
	 LPVOID *ppv
	);
typedef HRESULT __stdcall api_ole32_CoInitializeEx(
	LPVOID pvReserved,
	DWORD dwCoInit
	);
typedef HRESULT __stdcall api_ole32_CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclsid);
typedef int __stdcall api_ole32_StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cchMax);

//////////////////////////////////////////////////////////////////////////
// USER32
//////////////////////////////////////////////////////////////////////////
typedef DWORD WINAPI api_user32_GetWindowThreadProcessId(
	HWND hWnd,
	LPDWORD lpdwProcessId
	);
typedef BOOL WINAPI	api_user32_GetUserObjectInformationW(
	HANDLE hObj,
	int nIndex,
	PVOID pvInfo,
	DWORD nLength,
	LPDWORD lpnLengthNeeded
	);
typedef HWINSTA WINAPI api_user32_OpenWindowStationW(
	LPCWSTR lpszWinSta,
	BOOL fInherit,
	ACCESS_MASK dwDesiredAccess
	);
typedef HWINSTA	WINAPI api_user32_CreateWindowStationW(
	LPCWSTR lpwinsta,
	DWORD dwFlags,
	ACCESS_MASK dwDesiredAccess,
	LPSECURITY_ATTRIBUTES lpsa
	);
typedef BOOL WINAPI	api_user32_SetThreadDesktop(
	HDESK hDesktop
	);
typedef BOOL WINAPI	api_user32_SetProcessWindowStation(
	HWINSTA hWinSta
	);
typedef	HWINSTA	WINAPI api_user32_GetProcessWindowStation(
	VOID
	);
typedef	HDESK WINAPI api_user32_OpenDesktopW(
	LPCWSTR lpszDesktop,
	DWORD dwFlags,
	BOOL fInherit,
	ACCESS_MASK dwDesiredAccess
	);
typedef	HDESK WINAPI api_user32_CreateDesktopW(
	LPCWSTR lpszDesktop,
	LPCWSTR lpszDevice,
	DEVMODEW* pDevmode,
	DWORD dwFlags,
	ACCESS_MASK dwDesiredAccess,
	LPSECURITY_ATTRIBUTES lpsa
	);
typedef BOOL WINAPI	api_user32_GetKeyboardState(
	PBYTE lpKeyState
	);
typedef HWND WINAPI	api_user32_GetCapture(
	VOID
	);
typedef HWND WINAPI api_user32_SetCapture(
	HWND hWnd
	);
typedef HANDLE WINAPI api_user32_GetClipboardData(
	UINT uFormat
	);
typedef BOOL WINAPI api_user32_PeekMessageW(
	LPMSG lpMsg,
	HWND hWnd,
	UINT wMsgFilterMin,
	UINT wMsgFilterMax,
	UINT wRemoveMsg
	);
typedef BOOL WINAPI	api_user32_PeekMessageA(
	LPMSG lpMsg,
	HWND hWnd,
	UINT wMsgFilterMin,
	UINT wMsgFilterMax,
	UINT wRemoveMsg
	);
typedef int WINAPI	api_user32_GetClassNameW(
  HWND hWnd,
  LPWSTR lpClassName,
  int nMaxCount
  );
typedef int WINAPI	api_user32_GetClassNameA(
  HWND hWnd,
  LPSTR lpClassName,
  int nMaxCount
  );
typedef BOOL WINAPI	api_user32_SystemParametersInfoW(
  UINT uiAction,
  UINT uiParam,
  PVOID pvParam,
  UINT fWinIni);
typedef BOOL WINAPI	api_user32_SetKeyboardState(
  LPBYTE lpKeyState);
typedef UINT WINAPI	api_user32_RegisterWindowMessageW(
  LPCWSTR lpString);
typedef BOOL WINAPI	api_user32_CloseDesktop(
  HDESK hDesktop);
typedef BOOL WINAPI	api_user32_CloseWindowStation(
  HWINSTA hWinSta);
typedef HWND WINAPI	api_user32_GetShellWindow(
  VOID);
typedef BOOL WINAPI	api_user32_PostThreadMessageW(
  DWORD idThread,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);
typedef BOOL WINAPI	api_user32_PrintWindow(
  HWND hwnd,
  HDC hdcBlt,
  UINT nFlags);
typedef HWND WINAPI	api_user32_GetParent(
  HWND hWnd);
typedef BOOL WINAPI	api_user32_GetWindowInfo(
  HWND hwnd,
  PWINDOWINFO pwi);
typedef DWORD WINAPI	api_user32_GetClassLongW(
  HWND hWnd,
  int nIndex);
typedef HWND WINAPI	api_user32_GetAncestor(
  HWND hwnd,
  UINT gaFlags);
typedef BOOL WINAPI	api_user32_IsWindow(
  HWND hWnd);
typedef LRESULT WINAPI	api_user32_SendMessageW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);
typedef UINT WINAPI	api_user32_MapVirtualKeyW(
  UINT uCode,
  UINT uMapType);
typedef INT WINAPI	api_user32_GetSystemMetrics(
  int nIndex);
typedef BOOL WINAPI	api_user32_ExitWindowsEx(
  UINT uFlags,
  DWORD dwReason);
typedef DWORD WINAPI	api_user32_MsgWaitForMultipleObjects(
  DWORD nCount,
  CONST HANDLE *pHandles,
  BOOL fWaitAll,
  DWORD dwMilliseconds,
  DWORD dwWakeMask);
typedef LRESULT WINAPI	api_user32_SendMessageTimeoutW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam,
  UINT fuFlags,
  UINT uTimeout,
  PDWORD_PTR lpdwResult);
typedef HWND WINAPI	api_user32_GetWindow(
  HWND hWnd,
  UINT uCmd);
typedef LRESULT WINAPI	api_user32_DispatchMessageW(
  CONST MSG *lpMsg);
typedef BOOL WINAPI api_user32_PostMessageW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);
typedef HDESK WINAPI api_user32_GetThreadDesktop(
  DWORD dwThreadId);
typedef UINT WINAPI api_user32_GetMenuItemID(
  HMENU hMenu,
  int nPos);
typedef BOOL WINAPI api_user32_SetKeyboardState(
  LPBYTE lpKeyState);
typedef HMENU WINAPI api_user32_GetSubMenu(
  HMENU hMenu,
  int nPos);
typedef int WINAPI api_user32_MenuItemFromPoint(
  HWND hWnd,
  HMENU hMenu,
  POINT ptScreen);
typedef HWND WINAPI api_user32_WindowFromPoint(
  POINT Point);
typedef HMENU WINAPI api_user32_GetMenu(
  HWND hWnd);
typedef BOOL WINAPI api_user32_GetMenuItemRect(
  HWND hWnd,
  HMENU hMenu,
  UINT uItem,
  LPRECT lprcItem);
typedef UINT WINAPI api_user32_GetMenuState(
  HMENU hMenu,
  UINT uId,
  UINT uFlags);
typedef int WINAPI api_user32_GetMenuItemCount(
  HMENU hMenu);
typedef BOOL WINAPI api_user32_HiliteMenuItem(
  HWND hWnd,
  HMENU hMenu,
  UINT uIDHiliteItem,
  UINT uHilite);
typedef BOOL WINAPI api_user32_EndMenu(
  VOID);
typedef int WINAPI api_user32_FillRect(
  HDC hDC,
  CONST RECT *lprc,
  HBRUSH hbr);
typedef BOOL WINAPI api_user32_DrawEdge(
  HDC hdc,
  LPRECT qrc,
  UINT edge,
  UINT grfFlags);
typedef BOOL WINAPI api_user32_IntersectRect(
  LPRECT lprcDst,
  CONST RECT *lprcSrc1,
  CONST RECT *lprcSrc2);
typedef BOOL WINAPI api_user32_EqualRect(
  CONST RECT *lprc1,
  CONST RECT *lprc2);
typedef BOOL WINAPI api_user32_GetWindowRect(
  HWND hWnd,
  LPRECT lpRect);
typedef DWORD WINAPI api_user32_GetClassLongPtrW(
  HWND hWnd,
  int nIndex);
typedef BOOL WINAPI api_user32_SetWindowPos(
  HWND hWnd,
  HWND hWndInsertAfter,
  int X,
  int Y,
  int cx,
  int cy,
  UINT uFlags);
typedef int WINAPI api_user32_MapWindowPoints(
  HWND hWndFrom,
  HWND hWndTo,
  LPPOINT lpPoints,
  UINT cPoints);
typedef BOOL WINAPI api_user32_IsRectEmpty(
  CONST RECT *lprc);
typedef BOOL WINAPI api_user32_DrawIcon(
  HDC hDC,
  int X,
  int Y,
  HICON hIcon);
typedef BOOL WINAPI api_user32_GetIconInfo(
  HICON hIcon,
  PICONINFO piconinfo);
typedef LONG WINAPI api_user32_GetWindowLongPtrW(
  HWND hWnd,
  int nIndex);
typedef LONG WINAPI api_user32_SetWindowLongPtrW(
  HWND hWnd,
  int nIndex,
  LONG dwNewLong);
typedef LONG WINAPI api_user32_SetWindowLongW(
  HWND hWnd,
  int nIndex,
  LONG dwNewLong);
typedef HWND WINAPI api_user32_GetTopWindow(
  HWND hWnd);
typedef HANDLE WINAPI api_user32_LoadImageW(
  HINSTANCE hInst,
  LPCWSTR name,
  UINT type,
  int cx,
  int cy,
  UINT fuLoad);
typedef BOOL WINAPI api_user32_TrackPopupMenuEx(
  HMENU,
  UINT,
  int,
  int,
  HWND,
  LPTPMPARAMS);
typedef LPWSTR WINAPI api_user32_CharUpperW(
  LPWSTR lpsz);


//////////////////////////////////////////////////////////////////////////
// WININET
//////////////////////////////////////////////////////////////////////////
typedef HINTERNET __stdcall api_wininet_InternetOpenA(
	LPCSTR lpszAgent,
	DWORD dwAccessType,
	LPCSTR lpszProxy,
	LPCSTR lpszProxyBypass,
	DWORD dwFlags
	);
typedef BOOL __stdcall api_wininet_HttpSendRequestA(
	HINTERNET hRequest,
	LPCSTR lpszHeaders,
	DWORD dwHeadersLength,
	LPVOID lpOptional,
	DWORD dwOptionalLength
	);
typedef BOOL __stdcall api_wininet_HttpSendRequestW(
	HINTERNET hRequest,
	LPCWSTR lpszHeaders,
	DWORD dwHeadersLength,
	LPVOID lpOptional,
	DWORD dwOptionalLength
	);
typedef HINTERNET __stdcall api_wininet_HttpOpenRequestA(
	HINTERNET hConnect,
	LPCSTR lpszVerb,
	LPCSTR lpszObjectName,
	LPCSTR lpszVersion,
	LPCSTR lpszReferrer,
	LPCSTR FAR * lplpszAcceptTypes,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_InternetSetOptionA(
	HINTERNET hInternet,
	      DWORD dwOption,
	LPVOID lpBuffer,
	      DWORD dwBufferLength
	);
typedef BOOL __stdcall api_wininet_InternetReadFileExA(
	HINTERNET hFile,
	LPINTERNET_BUFFERSA lpBuffersOut,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_InternetReadFile(
	HINTERNET hFile,
	LPVOID lpBuffer,
	DWORD dwNumberOfBytesToRead,
	LPDWORD lpdwNumberOfBytesRead
	);
typedef BOOL __stdcall api_wininet_InternetCrackUrlA(
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	DWORD dwFlags,
	LPURL_COMPONENTSA lpUrlComponents
	);
typedef HINTERNET __stdcall api_wininet_InternetConnectA(
	HINTERNET hInternet,
	LPCSTR lpszServerName,
	INTERNET_PORT nServerPort,
	LPCSTR lpszUserName,
	LPCSTR lpszPassword,
	DWORD dwService,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_HttpQueryInfoA(
	HINTERNET hRequest,
	DWORD dwInfoLevel,
	LPVOID lpBuffer,
	LPDWORD lpdwBufferLength,
	LPDWORD lpdwIndex
	);
typedef BOOL __stdcall api_wininet_InternetCloseHandle(
	HINTERNET hInternet
	);
typedef BOOL __stdcall api_wininet_InternetQueryOptionA(
	HINTERNET hInternet,
	DWORD dwOption,
	LPVOID lpBuffer,
	LPDWORD lpdwBufferLength
	);
typedef BOOL __stdcall api_wininet_InternetQueryOptionW(
	HINTERNET hInternet,
	DWORD dwOption,
	LPVOID lpBuffer,
	LPDWORD lpdwBufferLength
	);
typedef BOOL __stdcall api_wininet_InternetQueryDataAvailable(
	HINTERNET hFile,
	LPDWORD lpdwNumberOfBytesAvailable,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_HttpAddRequestHeadersA(
	HINTERNET hRequest,
	LPCSTR lpszHeaders,
	DWORD dwHeadersLength,
	DWORD dwModifiers
	);
typedef BOOL __stdcall api_wininet_HttpAddRequestHeadersW(
	HINTERNET hRequest,
	LPCWSTR lpszHeaders,
	DWORD dwHeadersLength,
	DWORD dwModifiers
	);
typedef BOOL __stdcall api_wininet_HttpSendRequestExA(
	HINTERNET hRequest,
	LPINTERNET_BUFFERSA lpBuffersIn,
	LPINTERNET_BUFFERSA lpBuffersOut,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_HttpSendRequestExW(
	HINTERNET hRequest,
	LPINTERNET_BUFFERSW lpBuffersIn,
	LPINTERNET_BUFFERSW lpBuffersOut,
	DWORD dwFlags,
	DWORD_PTR dwContext
	);
typedef BOOL __stdcall api_wininet_GetUrlCacheEntryInfoW(
	LPCWSTR lpszUrlName,
	LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
	LPDWORD lpcbCacheEntryInfo
	);

typedef INTERNET_STATUS_CALLBACK __stdcall api_wininet_InternetSetStatusCallback(
	HINTERNET hInternet,
	INTERNET_STATUS_CALLBACK lpfnInternetCallback
	);


//////////////////////////////////////////////////////////////////////////
// CRYPT32
//////////////////////////////////////////////////////////////////////////
typedef BOOL __stdcall api_crypt32_CertDeleteCertificateFromStore(
	PCCERT_CONTEXT pCertContext
	);
typedef HCERTSTORE WINAPI api_crypt32_CertOpenSystemStoreW(
	HCRYPTPROV_LEGACY      hProv,
	LPCWSTR            szSubsystemProtocol
	);
typedef HCERTSTORE WINAPI api_crypt32_PFXImportCertStore(
	CRYPT_DATA_BLOB* pPFX,
	LPCWSTR szPassword,
	DWORD   dwFlags);
typedef PCCERT_CONTEXT WINAPI api_crypt32_CertEnumCertificatesInStore(
	HCERTSTORE hCertStore,
	PCCERT_CONTEXT pPrevCertContext
	);
typedef PCCERT_CONTEXT WINAPI api_crypt32_CertDuplicateCertificateContext(
	PCCERT_CONTEXT pCertContext
	);
typedef BOOL __stdcall api_crypt32_PFXExportCertStoreEx(
	HCERTSTORE hStore,
	CRYPT_DATA_BLOB* pPFX,
	LPCWSTR szPassword,
	void* pvPara,
	DWORD dwFlags);
typedef BOOL __stdcall api_crypt32_CertCloseStore(
	HCERTSTORE hCertStore,
	DWORD dwFlags
	);
typedef BOOL __stdcall api_crypt32_CryptUnprotectData(
            DATA_BLOB*      pDataIn,             // in encr blob
  __deref_opt_out_opt LPWSTR*     ppszDataDescr,       // out
         DATA_BLOB*      pOptionalEntropy,
  __reserved      PVOID           pvReserved,
         CRYPTPROTECT_PROMPTSTRUCT*  pPromptStruct,
            DWORD           dwFlags,
            DATA_BLOB*      pDataOut
  );

//////////////////////////////////////////////////////////////////////////
// WinSock 2
//////////////////////////////////////////////////////////////////////////
typedef int	WSAAPI api_ws2_32_WSAStartup(
	WORD wVersionRequested,
	LPWSADATA lpWSAData
	);
typedef int	WSAAPI api_ws2_32_listen(
	SOCKET s,
	int backlog
	);
typedef int	WSAAPI api_ws2_32_send(
	SOCKET s,
	const char FAR * buf,
	int len,
	int flags
	);
typedef int	WSAAPI api_ws2_32_closesocket(
	SOCKET s
	);
typedef void WSAAPI api_ws2_32_WSASetLastError(
	int iError
	);
typedef void WSAAPI api_ws2_32_freeaddrinfo(
	       PADDRINFOA      pAddrInfo
	);
typedef SOCKET WSAAPI api_ws2_32_socket (
	int af,
	int type,
	int protocol);
typedef int	WSAAPI api_ws2_32_bind(
	SOCKET s,
	const struct sockaddr FAR * name,
	int namelen
	);
typedef int	WSAAPI api_ws2_32_recv(
	SOCKET s,
	char FAR * buf,
	int len,
	int flags
	);
typedef int	WSAAPI api_ws2_32_setsockopt (
	SOCKET s,
	int level,
	int optname,
	const char FAR * optval,
	int optlen);
typedef int	WSAAPI api_ws2_32_shutdown (
	SOCKET s,
	int how);
typedef int	WSAAPI api_ws2_32_getsockname(
	SOCKET s,
	struct sockaddr FAR * name,
	int FAR * namelen
	);
typedef int	WSAAPI api_ws2_32_WSAGetLastError(
	void
	);
typedef int	WSAAPI api_ws2_32_select(
	int nfds,
	fd_set FAR * readfds,
	fd_set FAR * writefds,
	fd_set FAR * exceptfds,
	const struct timeval FAR * timeout
	);
typedef int	WSAAPI api_ws2_32_getaddrinfo(
	       PCSTR               pNodeName,
	       PCSTR               pServiceName,
	       const ADDRINFOA *   pHints,
	PADDRINFOA *        ppResult
	);
typedef int	WSAAPI api_ws2_32_WSAStartup(
	WORD wVersionRequested,
	LPWSADATA lpWSAData
	);
typedef int	WSAAPI api_ws2_32_connect(
	SOCKET s,
	const struct sockaddr FAR * name,
	int namelen
	);
typedef int	WSAAPI api_ws2_32_WSAIoctl(
	SOCKET s,
	DWORD dwIoControlCode,
	LPVOID lpvInBuffer,
	DWORD cbInBuffer,
	LPVOID lpvOutBuffer,
	DWORD cbOutBuffer,
	LPDWORD lpcbBytesReturned,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);
typedef int	WSAAPI api_ws2_32_sendto(
	SOCKET s,
	const char FAR * buf,
	int len,
	int flags,
	const struct sockaddr FAR * to,
	int tolen
	);
typedef int	WSAAPI api_ws2_32_recvfrom(
	SOCKET s,
	char FAR * buf,
	int len,
	int flags,
	struct sockaddr FAR * from,
	int FAR * fromlen
	);
typedef int	WSAAPI api_ws2_32_getpeername(
	SOCKET s,
	struct sockaddr FAR * name,
	int FAR * namelen
	);
typedef int	WSAAPI api_ws2_32_WSAAddressToStringW(
	LPSOCKADDR lpsaAddress,
	    DWORD               dwAddressLength,
	LPWSAPROTOCOL_INFOW lpProtocolInfo,
	LPWSTR lpszAddressString,
	 LPDWORD             lpdwAddressStringLength
	);
typedef int	WSAAPI api_ws2_32_WSAEventSelect(
	SOCKET s,
	WSAEVENT hEventObject,
	long lNetworkEvents
	);
typedef SOCKET	WSAAPI api_ws2_32_accept(
	SOCKET s,
	struct sockaddr FAR * addr,
	int FAR * addrlen
	);
typedef int	WSAAPI api_ws2_32_WSASend(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);

//////////////////////////////////////////////////////////////////////////
// USERENV
//////////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI *api_userenv_CreateEnvironmentBlock)(LPVOID *environment, HANDLE token, BOOL inherit);
typedef BOOL (WINAPI *api_userenv_DestroyEnvironmentBlock)(LPVOID environment);
typedef BOOL (WINAPI *api_userenv_GetDefaultUserProfileDirectoryW)(LPWSTR lpProfileDir, LPDWORD lpcchSize);

//////////////////////////////////////////////////////////////////////////
// GDI32
//////////////////////////////////////////////////////////////////////////
typedef HBITMAP WINAPI api_gdi32_CreateDIBSection(
	       HDC               hdc, 
	           CONST BITMAPINFO *pbmi, 
	           UINT              usage, 
	VOID            **ppvBits, 
	       HANDLE            hSection, 
	           DWORD             offset);
typedef HDC     WINAPI api_gdi32_CreateCompatibleDC( HDC hdc);
typedef HGDIOBJ WINAPI api_gdi32_SelectObject(HDC hdc, HGDIOBJ h);
typedef BOOL WINAPI api_gdi32_DeleteDC( HDC hdc);
typedef int   WINAPI api_gdi32_GetDeviceCaps( HDC hdc, int index);
typedef int  WINAPI api_gdi32_SaveDC(HDC hdc);
typedef BOOL   WINAPI api_gdi32_SetRectRgn(HRGN hrgn, int left, int top, int right, int bottom);
typedef BOOL  WINAPI api_gdi32_GdiFlush(void);
typedef BOOL  WINAPI api_gdi32_SetViewportOrgEx( HDC hdc, int x, int y, LPPOINT lppt);
typedef int   WINAPI api_gdi32_GetDIBits( HDC hdc, HBITMAP hbm, UINT start, UINT cLines,
							   LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT usage);
typedef BOOL WINAPI api_gdi32_DeleteObject( HGDIOBJ ho);
typedef HBITMAP WINAPI api_gdi32_CreateCompatibleBitmap( HDC hdc, int cx, int cy);
typedef BOOL WINAPI api_gdi32_RestoreDC(HDC hdc, int nSavedDC);

//////////////////////////////////////////////////////////////////////////
// KERNEL32
//////////////////////////////////////////////////////////////////////////
typedef BOOL WINAPI	api_kernel32_IsWow64Process(
	HANDLE hProcess,
	PBOOL Wow64Process
	);
typedef VOID WINAPI	api_kernel32_Sleep(
  DWORD dwMilliseconds
  );
typedef DWORD WINAPI api_kernel32_GetTickCount(VOID);
typedef LPVOID WINAPI api_kernel32_MapViewOfFile(
  HANDLE hFileMappingObject,
  DWORD dwDesiredAccess,
  DWORD dwFileOffsetHigh,
  DWORD dwFileOffsetLow,
  SIZE_T dwNumberOfBytesToMap
  );
typedef BOOL WINAPI api_kernel32_UnmapViewOfFile(
  LPCVOID lpBaseAddress
  );
typedef BOOL WINAPI api_kernel32_Process32NextW(
  HANDLE hSnapshot,
  LPPROCESSENTRY32W lppe
  );
typedef BOOL WINAPI api_kernel32_Process32FirstW(
  HANDLE hSnapshot,
  LPPROCESSENTRY32W lppe
  );
typedef BOOL WINAPI api_kernel32_Thread32Next(
  HANDLE hSnapshot,
  LPTHREADENTRY32 lpte
  );
typedef BOOL WINAPI api_kernel32_Thread32First(
  HANDLE hSnapshot,
  LPTHREADENTRY32 lpte
  );
typedef HANDLE WINAPI api_kernel32_CreateToolhelp32Snapshot(
  DWORD dwFlags,
  DWORD th32ProcessID
  );
typedef HANDLE WINAPI api_kernel32_GetCurrentThread(
  VOID
  );
typedef DWORD WINAPI api_kernel32_GetCurrentThreadId(
  VOID
  );
typedef UINT WINAPI api_kernel32_GetPrivateProfileIntW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  INT nDefault,
  LPCWSTR lpFileName
  );
typedef DWORD WINAPI api_kernel32_GetPrivateProfileStringW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  LPCWSTR lpDefault,
  LPWSTR lpReturnedString,
  DWORD nSize,
  LPCWSTR lpFileName
  );
typedef BOOL WINAPI api_kernel32_TerminateProcess(
  HANDLE hProcess,
  UINT uExitCode
  );
typedef HANDLE WINAPI api_kernel32_CreateRemoteThread(
      HANDLE hProcess,
   LPSECURITY_ATTRIBUTES lpThreadAttributes,
      SIZE_T dwStackSize,
      LPTHREAD_START_ROUTINE lpStartAddress,
   LPVOID lpParameter,
      DWORD dwCreationFlags,
  LPDWORD lpThreadId
  );
typedef HANDLE WINAPI api_kernel32_OpenProcess(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  DWORD dwProcessId
  );
typedef BOOL WINAPI api_kernel32_WriteProcessMemory(
  HANDLE hProcess,
  LPVOID lpBaseAddress,
  LPCVOID lpBuffer,
  SIZE_T nSize,
  SIZE_T * lpNumberOfBytesWritten
  );
typedef LPVOID WINAPI api_kernel32_VirtualAllocEx(
  HANDLE hProcess,
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD flAllocationType,
  DWORD flProtect
  );
typedef BOOL WINAPI api_kernel32_VirtualFreeEx(
  HANDLE hProcess,
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD  dwFreeType
  );
typedef BOOL WINAPI api_kernel32_VirtualProtectEx(
  HANDLE hProcess,
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD flNewProtect,
  PDWORD lpflOldProtect
  );
typedef BOOL WINAPI api_kernel32_VirtualProtect(
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD flNewProtect,
  PDWORD lpflOldProtect
  );
typedef BOOL WINAPI api_kernel32_TlsSetValue(
     DWORD dwTlsIndex,
  LPVOID lpTlsValue
  );
typedef BOOL WINAPI api_kernel32_TlsGetValue(
  DWORD dwTlsIndex
  );
typedef HANDLE WINAPI api_kernel32_CreateEventW(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
     BOOL bManualReset,
     BOOL bInitialState,
  LPCWSTR lpName
  );
typedef BOOL WINAPI api_kernel32_GetComputerNameW (
  LPWSTR lpBuffer,
  LPDWORD nSize
  );
typedef BOOL WINAPI api_kernel32_GetThreadContext(
    HANDLE hThread,
  LPCONTEXT lpContext
  );
typedef BOOL WINAPI api_kernel32_SetThreadContext(
  HANDLE hThread,
  CONST CONTEXT *lpContext
  );
typedef HANDLE WINAPI api_kernel32_CreateThread(
   LPSECURITY_ATTRIBUTES lpThreadAttributes,
      SIZE_T dwStackSize,
      LPTHREAD_START_ROUTINE lpStartAddress,
  LPVOID lpParameter,
      DWORD dwCreationFlags,
  LPDWORD lpThreadId
  );
typedef DWORD WINAPI api_kernel32_GetCurrentProcessId(
  VOID
  );
typedef FARPROC WINAPI api_kernel32_GetProcAddress(
  HMODULE hModule,
  LPCSTR lpProcName
  );
typedef HMODULE WINAPI api_kernel32_LoadLibraryA(
  LPCSTR lpLibFileName
  );
typedef HMODULE WINAPI api_kernel32_LoadLibraryW(
  LPCWSTR lpLibFileName
  );
typedef LPVOID WINAPI api_kernel32_VirtualAlloc(
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD flAllocationType,
  DWORD flProtect
  );
typedef BOOL WINAPI api_kernel32_VirtualFree(
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD dwFreeType
  );
typedef BOOL WINAPI api_kernel32_ReadProcessMemory(
  HANDLE hProcess,
  LPCVOID lpBaseAddress,
  LPVOID lpBuffer,
  SIZE_T nSize,
  SIZE_T * lpNumberOfBytesRead
  );
typedef SIZE_T WINAPI api_kernel32_VirtualQueryEx(
  HANDLE hProcess,
  LPCVOID lpAddress,
  PMEMORY_BASIC_INFORMATION lpBuffer,
  SIZE_T dwLength
  );
typedef BOOL WINAPI api_kernel32_FreeLibrary(
  HMODULE hLibModule
  );
typedef HMODULE WINAPI api_kernel32_GetModuleHandleW(
  LPCWSTR lpModuleName
  );
typedef BOOL WINAPI api_kernel32_CreateProcessW(
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
  );
typedef BOOL WINAPI api_kernel32_SetFileTime(
  HANDLE hFile,
  CONST FILETIME *lpCreationTime,
  CONST FILETIME *lpLastAccessTime,
  CONST FILETIME *lpLastWriteTime
  );
typedef LPTOP_LEVEL_EXCEPTION_FILTER WINAPI api_kernel32_SetUnhandledExceptionFilter(
  LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
  );
typedef BOOL WINAPI api_kernel32_CreateDirectoryW(
  LPCWSTR lpPathName,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );
typedef HANDLE WINAPI api_kernel32_GetProcessHeap( VOID );
typedef BOOL WINAPI api_kernel32_WriteFile(
  HANDLE hFile,
  LPCVOID lpBuffer,
  DWORD nNumberOfBytesToWrite,
  LPDWORD lpNumberOfBytesWritten,
  LPOVERLAPPED lpOverlapped
  );
typedef HANDLE WINAPI api_kernel32_HeapCreate(
  DWORD flOptions,
  SIZE_T dwInitialSize,
  SIZE_T dwMaximumSize
  );
typedef HANDLE WINAPI api_kernel32_CreateFileW(
  LPCWSTR lpFileName,
  DWORD dwDesiredAccess,
  DWORD dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD dwCreationDisposition,
  DWORD dwFlagsAndAttributes,
  HANDLE hTemplateFile
  );
typedef DWORD WINAPI api_kernel32_GetTempPathW(
  DWORD nBufferLength,
  LPWSTR lpBuffer
  );
typedef BOOL WINAPI api_kernel32_RemoveDirectoryW(
  LPCWSTR lpPathName
  );
typedef BOOL WINAPI api_kernel32_FindNextFileW(
  HANDLE hFindFile,
  LPWIN32_FIND_DATAW lpFindFileData
  );
typedef BOOL WINAPI api_kernel32_DeleteFileW(
  LPCWSTR lpFileName
  );
typedef LPWSTR WINAPI api_kernel32_GetCommandLineW(
  VOID
  );
typedef UINT WINAPI api_kernel32_SetErrorMode(
  UINT uMode
  );
typedef HANDLE WINAPI api_kernel32_FindFirstFileW(
  LPCWSTR lpFileName,
  LPWIN32_FIND_DATAW lpFindFileData
  );
typedef DWORD WINAPI api_kernel32_WTSGetActiveConsoleSessionId(
  VOID
  );
typedef BOOL WINAPI api_kernel32_GetVersionExW(
  LPOSVERSIONINFOW lpVersionInformation
  );

typedef DWORD WINAPI api_kernel32_GetModuleFileNameW(
  HMODULE hModule,
  LPWSTR lpFilename,
  DWORD nSize
  );
typedef HANDLE WINAPI api_kernel32_CreateMutexW(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,
  BOOL bInitialOwner,
  LPCWSTR lpName
  );

typedef HANDLE WINAPI api_kernel32_OpenMutexW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );
typedef BOOL WINAPI api_kernel32_ReleaseMutex(
  HANDLE hMutex
  );
typedef DWORD WINAPI api_kernel32_WaitForSingleObject(
  HANDLE hHandle,
  DWORD dwMilliseconds
  );
typedef DWORD WINAPI api_kernel32_WaitForMultipleObjects(
  DWORD nCount,
  CONST HANDLE *lpHandles,
  BOOL bWaitAll,
  DWORD dwMilliseconds
  );
typedef LPVOID WINAPI api_kernel32_GlobalLock (
  HGLOBAL hMem
  );
typedef BOOL WINAPI api_kernel32_GlobalUnlock(
  HGLOBAL hMem
  );
typedef DWORD WINAPI api_kernel32_GetProcessId(
  HANDLE Process
  );
typedef VOID WINAPI api_kernel32_GetNativeSystemInfo(
  LPSYSTEM_INFO lpSystemInfo
  );
typedef BOOL WINAPI api_kernel32_SetThreadPriority(
  HANDLE hThread,
  int nPriority
  );
typedef VOID WINAPI api_kernel32_ExitProcess(
  UINT uExitCode
  );
typedef DWORD WINAPI api_kernel32_ExpandEnvironmentStringsW(
  LPCWSTR lpSrc,
  LPWSTR lpDst,
  DWORD nSize
  );
typedef DWORD WINAPI api_kernel32_GetEnvironmentVariableW(
  LPCWSTR lpName,
  LPWSTR lpBuffer,
  DWORD nSize
  );
typedef VOID WINAPI api_kernel32_GetSystemTime(
  LPSYSTEMTIME lpSystemTime
  );
typedef BOOL WINAPI api_kernel32_SetEndOfFile(
  HANDLE hFile
  );
typedef BOOL WINAPI api_kernel32_SystemTimeToFileTime(
  CONST SYSTEMTIME *lpSystemTime,
  LPFILETIME lpFileTime
  );
typedef BOOL WINAPI api_kernel32_SetFilePointerEx(
  HANDLE hFile,
  LARGE_INTEGER liDistanceToMove,
  PLARGE_INTEGER lpNewFilePointer,
  DWORD dwMoveMethod
  );
typedef DWORD WINAPI api_kernel32_SetFilePointer(
  HANDLE hFile,
  LONG lDistanceToMove,
  PLONG lpDistanceToMoveHigh,
  DWORD dwMoveMethod
  );
typedef BOOL WINAPI api_kernel32_IsBadReadPtr(
  CONST VOID *lp,
  UINT_PTR ucb
  );
typedef DWORD WINAPI api_kernel32_GetFileAttributesW(
  LPCWSTR lpFileName
  );
typedef BOOL WINAPI api_kernel32_ReadFile(
  HANDLE hFile,
  LPVOID lpBuffer,
  DWORD nNumberOfBytesToRead,
  LPDWORD lpNumberOfBytesRead,
  LPOVERLAPPED lpOverlapped
  );
typedef DWORD WINAPI api_kernel32_GetTimeZoneInformation(
  LPTIME_ZONE_INFORMATION lpTimeZoneInformation
  );
typedef BOOL WINAPI api_kernel32_FlushFileBuffers(
  HANDLE hFile
  );
typedef BOOL WINAPI api_kernel32_GetFileSizeEx(
  HANDLE hFile,
  PLARGE_INTEGER lpFileSize
  );
typedef BOOL WINAPI api_kernel32_FindClose(
  HANDLE hFindFile
  );
typedef UINT WINAPI api_kernel32_GetTempFileNameW(
  LPCWSTR lpPathName,
  LPCWSTR lpPrefixString,
  UINT uUnique,
  LPWSTR lpTempFileName
  );
typedef BOOL WINAPI api_kernel32_FileTimeToLocalFileTime(
  CONST FILETIME *lpFileTime,
  LPFILETIME lpLocalFileTime
  );
typedef BOOL WINAPI api_kernel32_GetFileTime(
  HANDLE hFile,
  LPFILETIME lpCreationTime,
  LPFILETIME lpLastAccessTime,
  LPFILETIME lpLastWriteTime
  );
typedef BOOL WINAPI api_kernel32_GetVolumeNameForVolumeMountPointW(
  LPCWSTR lpszVolumeMountPoint,
  LPWSTR lpszVolumeName,
  DWORD cchBufferLength
  );
typedef BOOL WINAPI api_kernel32_GetFileInformationByHandle(
  HANDLE hFile,
  LPBY_HANDLE_FILE_INFORMATION lpFileInformation
  );
typedef BOOL WINAPI api_kernel32_SetFileAttributesW(
  LPCWSTR lpFileName,
  DWORD dwFileAttributes
  );
typedef BOOL WINAPI api_kernel32_MoveFileExW(
  LPCWSTR lpExistingFileName,
  LPCWSTR lpNewFileName,
  DWORD    dwFlags
  );
typedef LANGID WINAPI api_kernel32_GetUserDefaultUILanguage(void);
typedef BOOL WINAPI api_kernel32_FileTimeToDosDateTime(
  CONST FILETIME *lpFileTime,
  LPWORD lpFatDate,
  LPWORD lpFatTime
  );
typedef BOOL WINAPI api_kernel32_DuplicateHandle(
  HANDLE hSourceProcessHandle,
  HANDLE hSourceHandle,
  HANDLE hTargetProcessHandle,
  LPHANDLE lpTargetHandle,
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  DWORD dwOptions
  );
typedef HANDLE WINAPI api_kernel32_CreateFileMappingW(
  HANDLE hFile,
  LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
  DWORD flProtect,
  DWORD dwMaximumSizeHigh,
  DWORD dwMaximumSizeLow,
  LPCWSTR lpName
  );
typedef DWORD WINAPI api_kernel32_TlsAlloc(
  VOID
  );
typedef BOOL WINAPI api_kernel32_TlsFree(
  DWORD dwTlsIndex
  );
typedef HANDLE WINAPI api_kernel32_OpenEventW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );
typedef BOOL WINAPI api_kernel32_SetEvent(
  HANDLE hEvent
  );
typedef BOOL WINAPI api_kernel32_ResetEvent(
  HANDLE hEvent
  );
typedef VOID WINAPI api_kernel32_InitializeCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection
  );
typedef VOID WINAPI api_kernel32_LeaveCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection
  );
typedef VOID WINAPI api_kernel32_EnterCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection
  );
typedef VOID WINAPI api_kernel32_DeleteCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection
  );

//////////////////////////////////////////////////////////////////////////
// Miscellaneous functions from user hooks
//////////////////////////////////////////////////////////////////////////
typedef BOOL WINAPI api_kernel32_GetFileAttributesExW(
   LPCWSTR lpFileName,
   GET_FILEEX_INFO_LEVELS fInfoLevelId,
  LPVOID lpFileInformation
  );
typedef HDESK WINAPI api_user32_OpenInputDesktop(
  DWORD dwFlags,
  BOOL fInherit,
  ACCESS_MASK dwDesiredAccess);

typedef BOOL WINAPI api_user32_SwitchDesktop(
  HDESK hDesktop);

typedef LRESULT WINAPI api_user32_DefWindowProcW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefWindowProcA(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefDlgProcW(
  HWND hDlg,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefDlgProcA(
  HWND hDlg,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefFrameProcW(
  HWND hWnd,
  HWND hWndMDIClient,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefFrameProcA(
  HWND hWnd,
  HWND hWndMDIClient,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefMDIChildProcW(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_DefMDIChildProcA(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_CallWindowProcW(
  WNDPROC lpPrevWndFunc,
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef LRESULT WINAPI api_user32_CallWindowProcA(
  WNDPROC lpPrevWndFunc,
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

typedef ATOM WINAPI api_user32_RegisterClassW(
  CONST WNDCLASSW *lpWndClass);

typedef ATOM WINAPI api_user32_RegisterClassA(
  CONST WNDCLASSA *lpWndClass);

typedef ATOM WINAPI api_user32_RegisterClassExW(
  CONST WNDCLASSEXW *);

typedef ATOM WINAPI api_user32_RegisterClassExA(
  CONST WNDCLASSEXA *);

typedef HDC WINAPI api_user32_BeginPaint(
  HWND hWnd,
  LPPAINTSTRUCT lpPaint);

typedef BOOL WINAPI api_user32_EndPaint(
  HWND hWnd,
  CONST PAINTSTRUCT *lpPaint);

typedef HDC WINAPI api_user32_GetDCEx(
  HWND hWnd,
  HRGN hrgnClip,
  DWORD flags);

typedef HDC WINAPI api_user32_GetDC(
  HWND hWnd);

typedef HDC WINAPI api_user32_GetWindowDC(
  HWND hWnd);

typedef int WINAPI api_user32_ReleaseDC(
  HWND hWnd,
  HDC hDC);

typedef BOOL WINAPI api_user32_GetUpdateRect(
  HWND hWnd,
  LPRECT lpRect,
  BOOL bErase);

typedef int WINAPI api_user32_GetUpdateRgn(
  HWND hWnd,
  HRGN hRgn,
  BOOL bErase);

typedef DWORD WINAPI api_user32_GetMessagePos(
  VOID);

typedef BOOL WINAPI api_user32_GetCursorPos(
  LPPOINT lpPoint);

typedef BOOL WINAPI api_user32_SetCursorPos(
  int X,
  int Y);

typedef BOOL WINAPI api_user32_ReleaseCapture(
  VOID);

typedef BOOL WINAPI api_user32_GetMessageW(
  LPMSG lpMsg,
  HWND hWnd,
  UINT wMsgFilterMin,
  UINT wMsgFilterMax);

typedef BOOL WINAPI api_user32_GetMessageA(
  LPMSG lpMsg,
  HWND hWnd,
  UINT wMsgFilterMin,
  UINT wMsgFilterMax);

typedef BOOL WINAPI api_user32_TranslateMessage(
  CONST MSG *lpMsg);

typedef BOOL WINAPI api_user32_SetWindowTextW(
  HWND hWnd,
  LPCWSTR lpString);

typedef HRESULT WINAPI api_shell32_SHGetFolderPathW(
  HWND hwnd, 
  int csidl, 
  HANDLE hToken, 
  DWORD dwFlags, 
  LPWSTR pszPath);

//////////////////////////////////////////////////////////////////////////
// SHLWAPI.DLL
//////////////////////////////////////////////////////////////////////////
typedef LSTATUS WINAPI api_shlwapi_SHDeleteKeyW(HKEY hkey, LPCWSTR pszSubKey);
typedef LSTATUS WINAPI api_shlwapi_SHDeleteValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue);
typedef LPWSTR WINAPI api_shlwapi_PathSkipRootW(LPCWSTR pszPath);
typedef LPWSTR WINAPI api_shlwapi_PathCombineW(LPWSTR pszDest, LPCWSTR pszDir, LPCWSTR pszFile);
typedef BOOL WINAPI api_shlwapi_PathAddExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
typedef BOOL WINAPI api_shlwapi_PathUnquoteSpacesW(LPWSTR lpsz);
typedef LPWSTR WINAPI api_shlwapi_PathRemoveBackslashW(LPWSTR pszPath);
typedef LPWSTR WINAPI api_shlwapi_PathFindFileNameW(LPCWSTR pszPath);
typedef LPWSTR WINAPI api_shlwapi_PathAddBackslashW(LPWSTR pszPath);
typedef BOOL WINAPI api_shlwapi_PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
typedef BOOL WINAPI api_shlwapi_PathQuoteSpacesW(LPWSTR lpsz);
typedef BOOL WINAPI api_shlwapi_PathRemoveFileSpecW(LPWSTR pszPath);
typedef BOOL WINAPI api_shlwapi_PathIsDirectoryW(LPCWSTR pszPath);
typedef HRESULT WINAPI api_shlwapi_UrlUnescapeA(LPSTR pszUrl, LPSTR pszUnescaped, LPDWORD pcchUnescaped, DWORD dwFlags);
typedef BOOL WINAPI api_shlwapi_PathMatchSpecW(LPCWSTR pszFile, LPCWSTR pszSpec);
typedef BOOL WINAPI api_shlwapi_PathIsURLW(LPCWSTR pszPath);

//////////////////////////////////////////////////////////////////////////
// OLEAUT32.DLL
//////////////////////////////////////////////////////////////////////////
typedef BSTR WINAPI api_oleaut32_SysAllocString(const OLECHAR * psz);
typedef void WINAPI api_oleaut32_SysFreeString(BSTR bstrString);
typedef void WINAPI api_oleaut32_VariantInit(VARIANTARG * pvarg);
typedef HRESULT WINAPI api_oleaut32_VariantClear(VARIANTARG * pvarg);
