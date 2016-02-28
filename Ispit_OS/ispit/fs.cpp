//#include <windows.h>
#include<WinSock2.h>
#include <WTypes.h>
#include <aclapi.h>
#include <shlwapi.h>

#include "defines.h"
#include "fs.h"
#include "mem.h"
#include "str.h"

DWORD64 Fs::_getFileSize(HANDLE file)
{
  LARGE_INTEGER li;
  if(CWA(kernel32, GetFileSizeEx)(file, &li))return li.QuadPart;
  return (DWORD64)(-1);
}

DWORD64 Fs::_getFileSizeEx(LPWSTR file)
{
  DWORD64 size = (DWORD64)(-1);
  HANDLE handle = CWA(kernel32, CreateFileW)(file, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

  if(handle != INVALID_HANDLE_VALUE)
  {
    size = _getFileSize(handle);
    CWA(kernel32, CloseHandle)(handle);
  }

  return size;
}

bool Fs::_WriteFile(HANDLE hFile, void *pBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD pdwNumberOfBytesWritten, HANDLE hOverlappedEvent)
{
  bool bRetVal = false;
  OVERLAPPED ov;

  Mem::_zero(&ov, sizeof(OVERLAPPED));
  ov.hEvent = hOverlappedEvent; 

  if(CWA(kernel32, WriteFile)(hFile, pBuffer, dwNumberOfBytesToWrite, pdwNumberOfBytesWritten, &ov))bRetVal = true;
  else if(CWA(kernel32, GetLastError)() == ERROR_IO_PENDING)
  {
    if(CWA(kernel32, GetOverlappedResult)(hFile, &ov, pdwNumberOfBytesWritten, TRUE))bRetVal = true;
  }

  return bRetVal;
}

bool Fs::_pathCombine(LPWSTR dest, const LPWSTR dir, const LPWSTR file)
{
  LPWSTR p = (LPWSTR)file;
  if(p != NULL)while(*p == '\\' || *p == '/')p++;
  return CWA(shlwapi, PathCombineW)(dest, dir, p) == NULL ? false : true;
}

