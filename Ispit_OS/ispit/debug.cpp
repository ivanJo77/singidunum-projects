#include <WTypes.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <sddl.h>
#include <accCtrl.h>

#include"defines.h"
#include"process.h"
#include"winsecurity.h"
#include"fs.h"
#include"str.h"
#include"debug.h"
#include"mem.h"
#include"sync.h"

#define BO_DEBUG 1

#if(BO_DEBUG > 0)

#define MUTEX_WRITEFILE L"{8EEEA37C-5CEF-11DD-9810-2A4256D89593}"
#define LOG_FILE        L"debug.txt"
#define LOG_BUFFER_SIZE 2048
#define PIPE_NAME       L"\\\\.\\pipe\\4273890-9343434"
#define PIPE_MUTEX      L"4273890-9343434"

static SECURITY_ATTRIBUTES saFullAccess;
static SECURITY_DESCRIPTOR sdFullAccess;
static bool SecurityOK;
static WCHAR __strDebugReportFile[MAX_PATH];
#if(BO_DEBUG == 1)
static DWORD integrityLevel;
#endif

#pragma pack(push, 1)
typedef struct
{
	BYTE bDataType;     //report Type.

	DWORD dwLastError;  //The value of GetLastError () at the time of the call.
	DWORD dwPID;        //PID
	DWORD dwTID;        //TID
	DWORD dwTickCount;  //GetTickCount()

	char strFunctionName[50]; //UTF-8. The function name, I think 50 bytes will be enough.
	char strSourceFile[50];   //UTF-8. The source file name.
	DWORD dwLineNumber;       //The line number in the source file.
}DEBUGDATA;
#pragma pack(pop)

//Open log file.
static HANDLE OpenLogFile(void)
{
#if(BO_DEBUG == 1)
	WinSecurity::_setLowIntegrityLevelLabel(__strDebugReportFile, SE_FILE_OBJECT, false);
	HANDLE file = CWA(kernel32, CreateFileW)(__strDebugReportFile, GENERIC_WRITE | WRITE_DAC, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//alternative path
	WCHAR path[MAX_PATH];
	if(file == INVALID_HANDLE_VALUE && (integrityLevel == Process::INTEGRITY_UNKNOWN || integrityLevel == Process::INTEGRITY_HIGH))
	{
		path[0] = 0;
		CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path);
		Fs::_pathCombine(path, path, LOG_FILE);
		WinSecurity::_setLowIntegrityLevelLabel(path, SE_FILE_OBJECT, false);
		file = CWA(kernel32, CreateFileW)(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	//Write BOM
	if(file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER li;
		if(CWA(kernel32, GetFileSizeEx)(file, &li) && li.HighPart == 0 && li.LowPart == 0)
		{
			DWORD dws = UTF8_BOM;
			CWA(kernel32, WriteFile)(file, &dws, UTF8_BOM_SIZE, &li.LowPart, NULL);
		}
	}
#elif(BO_DEBUG == 2)
	HANDLE file = CWA(kernel32, CreateFileW)(__strDebugReportFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
	return file;
}
static BOOL _debugClientInit=false;
void DebugClient::Init(void)
{
	if(!_debugClientInit)
		_debugClientInit=true;

#if(BO_DEBUG == 1)
	SecurityOK = WinSecurity::_getFullAccessDescriptors(&saFullAccess, &sdFullAccess);
	integrityLevel = Process::_getIntegrityLevel(CURRENT_PROCESS);
	if(integrityLevel == Process::INTEGRITY_UNKNOWN || integrityLevel == Process::INTEGRITY_HIGH)
	{
		Str::_CopyW(__strDebugReportFile, L"C:\\" LOG_FILE, -1);
	}
	else
	{
		__strDebugReportFile[0] = 0;
		CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, __strDebugReportFile);
		Fs::_pathCombine(__strDebugReportFile, __strDebugReportFile, LOG_FILE);
	}
#elif(BO_DEBUG == 2)
	Str::_CopyW(__strDebugReportFile, PIPE_NAME, -1);
#endif
}

void DebugClient::Uninit(void)
{

}

void DebugClient::WriteString(LPSTR pstrFuncName, LPSTR pstrSourceFile, DWORD dwLineNumber, BYTE bType, LPWSTR pstrFormat, ...)
{
	if(!_debugClientInit)
		return;
	static const LPSTR WDDType[] = {"INFO", "WARNING", "ERROR"};

	DEBUGDATA Data;
	SYSTEMTIME lt;

	//fill in the structure.
	Mem::_zero(&Data, sizeof(DEBUGDATA));
	Data.dwLastError   = CWA(kernel32, GetLastError)();
	Data.bDataType     = bType;
	Data.dwPID         = CWA(kernel32, GetCurrentProcessId)();
	Data.dwTID         = CWA(kernel32, GetCurrentThreadId)();
	Data.dwTickCount   = CWA(kernel32, GetTickCount)();
	Data.dwLineNumber  = dwLineNumber;

	CWA(kernel32, GetLocalTime)(&lt);

	//function name.
	DWORD dwSize = pstrFuncName ? Str::_LengthA(pstrFuncName) : 0;
	if(dwSize > 0)
	{
		dwSize++;
		Str::_CopyA(Data.strFunctionName, pstrFuncName, min((sizeof(Data.strFunctionName) / sizeof(char)), dwSize));
	}

	//The original file.
	dwSize = pstrSourceFile ? Str::_LengthA(pstrSourceFile) : 0;
	if(dwSize > 0)
	{
		dwSize++;
		Str::_CopyA(Data.strSourceFile, pstrSourceFile, min((sizeof(Data.strSourceFile) / sizeof(char)), dwSize));
	}

	//create a report.
	Str::UTF8STRING u8str;
	bool utf8_ok = false;
	{
		LPWSTR pTmpBuffer = (LPWSTR)Mem::alloc(LOG_BUFFER_SIZE * sizeof(WCHAR));

		//Default string.
#define ERROR_STR "FAILED TO ALLOCATE STRING."

		char def_str[sizeof(ERROR_STR)];
		Mem::_copy(def_str, ERROR_STR, sizeof(ERROR_STR) * sizeof(char));

		u8str.data   = (void *)def_str;
		u8str.length = sizeof(ERROR_STR) - 1;
		u8str.size   = sizeof(ERROR_STR) - 1;

#undef ERROR_STR

		if(pTmpBuffer)
		{
			//Heading.
			int iSysSize = Str::_sprintfW(pTmpBuffer, 250,
				L"[%02u:%02u:%02u] TC=%010u, PID=%04u(0x%04X), TID=%04u(0x%04X), LE=%u(0x%X), F=%S, FL=%S (%u)\r\n%S: ",
				lt.wHour, lt.wMinute, lt.wSecond, Data.dwTickCount, Data.dwPID, Data.dwPID, Data.dwTID, Data.dwTID,
				Data.dwLastError, Data.dwLastError,
				Data.strFunctionName, Data.strSourceFile, Data.dwLineNumber,
				(bType < sizeof(WDDType) / sizeof(LPSTR) ? WDDType[bType] : "-"));

			//Log.
			if(iSysSize > 0)
			{
				va_list list;
				va_start(list, pstrFormat);
				int iLogSize =  Str::_vsprintfW(pTmpBuffer + iSysSize, LOG_BUFFER_SIZE - iSysSize, pstrFormat, list);
				va_end(list);

				//Construct in UTF-8.
				if(iLogSize > 0)utf8_ok = Str::_utf8FromUnicode(pTmpBuffer, iSysSize + iLogSize, &u8str);
			}

			Mem::free(pTmpBuffer);
		}
	}

	HANDLE hMutex = Sync::_waitForMutex(SecurityOK ? &saFullAccess : NULL, MUTEX_WRITEFILE);
	if(hMutex)
	{
		//Write into file.
		HANDLE hFile = OpenLogFile();
		if(hFile != INVALID_HANDLE_VALUE)
		{
#if(BO_DEBUG == 1)
			if(CWA(kernel32, SetFilePointer)(hFile, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER)
			{
				CWA(kernel32, WriteFile)(hFile, u8str.data, u8str.size, &dwSize, 0);
				CWA(kernel32, WriteFile)(hFile, "\r\n\r\n", 4, &dwSize, 0);
			}
#elif(BO_DEBUG == 2)
			DWORD dwTmp = PIPE_READMODE_MESSAGE | PIPE_WAIT;
			CWA(kernel32, SetNamedPipeHandleState)(hFile, &dwTmp, NULL, NULL);
			CWA(kernel32, WriteFile)(hFile, u8str.data, u8str.size, &dwSize, 0);      
#endif
			//CWA(kernel32, FlushFileBuffers)(hFile);
			CWA(kernel32, CloseHandle)(hFile);
		}
		Sync::_freeMutex(hMutex);
	}

	if(utf8_ok)Str::_utf8Free(&u8str);
	CWA(kernel32, SetLastError(Data.dwLastError));
}

#endif