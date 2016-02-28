#include <windows.h>
#include <shlwapi.h>

#include "defines.h"
#include "defines.h"
#include "str.h"
#include "mem.h"

//The maximum line size for _sprintfExX
#define MAX_SPRINTF_STRING_SIZE (10 * 1024 * 1024)

static int unicodeToX(DWORD codePage, const LPWSTR source, int sourceSize, LPSTR dest, int destSize)
{
  if(sourceSize == -1)sourceSize = Str::_LengthW(source);
  int size = CWA(kernel32, WideCharToMultiByte)(codePage, 0, source, sourceSize, dest, destSize, NULL, NULL);
  if(destSize > 0)
  {
    if(size >= destSize)size = 0; //No place for the null character.
    dest[size] = 0;
  }
  return size;
}

static LPSTR unicodeToXEx(DWORD codePage, const LPWSTR source, int size)
{
  if(size == -1)size = Str::_LengthW(source);
  int destSize = unicodeToX(codePage, source, size, NULL, 0);
  if(destSize > 0)
  {
    destSize += sizeof(BYTE);
    LPSTR dest = (LPSTR)Mem::alloc(destSize * sizeof(BYTE));
    if(dest != NULL)
    {
      unicodeToX(codePage, source, size, dest, destSize);
      return dest;
    }
  }
  return NULL;
}

/*
  General function to format the Unicode-string with memory allocation.
  
  IN OUT buffer   - buffer for writing, must be either valid or point to NULL. In case of
					error buffer will not return to its original state.
  IN bufferOffset - the current offset in characters in *buffer, from which you want to start making changes.
  IN format       - line format.
  IN arguments    - arguments for format.
  
  Return          - new full-size string, or (DWORD)-1 in case of error.
*/
static DWORD vsprintfHelperExW(LPWSTR *buffer, DWORD bufferOffset, const LPWSTR format, va_list arguments)
{
  DWORD bufferSize = Str::_LengthW(format);
  int realSize;

  do
  {
    bufferSize += 512;
    if(bufferSize > MAX_SPRINTF_STRING_SIZE || !Mem::reallocEx(buffer, (bufferOffset + bufferSize) * sizeof(WCHAR) + sizeof(WCHAR)))return (DWORD)-1;
  }
  while((realSize = CWA(shlwapi, wvnsprintfW)((*buffer) + bufferOffset, bufferSize, format, arguments)) < 0 || realSize >= (int)(bufferSize - 1));

  bufferOffset += (DWORD)realSize;
  (*buffer)[bufferOffset] = 0;

  return bufferOffset;
}

/*
  General function to format the ANSI-line with memory allocation.
  
  IN OUT buffer   - buffer for writing, must be either valid or point to NULL. In case of
					error buffer will not return to its original state.
  IN bufferOffset - the current offset in characters in *buffer, from which you want to start making changes.
  IN format       - line format.
  IN arguments    - arguments for format.

  Return          - new full-size string, or (DWORD)-1 in case of error.
*/
static DWORD vsprintfHelperExA(LPSTR *buffer, DWORD bufferOffset, const LPSTR format, va_list arguments)
{
  DWORD bufferSize = Str::_LengthA(format);
  int realSize;

  do
  {
    bufferSize += 512;
    if(bufferSize > MAX_SPRINTF_STRING_SIZE || !Mem::reallocEx(buffer, (bufferOffset + bufferSize) * sizeof(char) + sizeof(char)))return (DWORD)-1;
  }
  while((realSize = CWA(shlwapi, wvnsprintfA)((*buffer) + bufferOffset, bufferSize, format, arguments)) < 0 || realSize >= (int)(bufferSize - 1));

  bufferOffset += (DWORD)realSize;
  (*buffer)[bufferOffset] = 0;
  return bufferOffset;
}

void Str::_CopyW(LPWSTR pstrDest, LPCWSTR pstrSource, int iSize)
{
  if(iSize == -1)iSize = _LengthW(pstrSource);
  Mem::_copy(pstrDest, pstrSource, iSize * sizeof(WCHAR));
  pstrDest[iSize] = 0;
}

LPWSTR Str::_CopyExW(LPWSTR pstrSource, int iSize)
{
  if(pstrSource == NULL)return NULL;
  if(iSize == -1)iSize = _LengthW(pstrSource);
  iSize *= sizeof(WCHAR);
  LPWSTR p = (LPWSTR)Mem::alloc(iSize + sizeof(WCHAR));
  if(p != NULL)
  {
    Mem::_copy(p, pstrSource, iSize);
    //p[iSize] = 0;
  }
  return p;
}

void Str::_CopyA(LPSTR pstrDest, LPCSTR pstrSource, int iSize)
{
  if(iSize == -1)iSize = _LengthA(pstrSource);
  Mem::_copy(pstrDest, pstrSource, iSize);
  pstrDest[iSize] = 0;
}

LPSTR Str::_CopyExA(LPSTR pstrSource, int iSize)
{
  if(pstrSource == NULL)return NULL;
  if(iSize == -1)iSize = _LengthA(pstrSource);
  LPSTR p = (LPSTR)Mem::alloc(iSize + 1);
  if(p != NULL)
  {
    Mem::_copy(p, pstrSource, iSize);
    //p[iSize] = 0;
  }
  return p;
}


int Str::_LengthA(LPCSTR pstrStr)
{
  int c = 0;
  if(pstrStr != NULL)while(pstrStr[c] != 0)c++;
  return c;
}

int Str::_LengthW(LPCWSTR pstrStr)
{
  int c = 0;
  if(pstrStr != NULL)while(pstrStr[c] != 0)c++;
  return c;
}

int Str::_CompareA(LPCSTR pstrStr1, LPCSTR pstrStr2, int iSize1, int iSize2)
{
  //pointer check
  if(pstrStr1 == NULL && pstrStr2 != NULL)return -1;
  if(pstrStr1 != NULL && pstrStr2 == NULL)return 1;
  if(pstrStr1 == NULL && pstrStr2 == NULL)return 0;

  //If both sizes are not defined.
  if(iSize1 == -1 && iSize2 == -1)
  {
    while((iSize1 = *pstrStr1 - *pstrStr2) == 0 && *pstrStr2 != 0)
    {
      pstrStr1++;
      pstrStr2++;
    }
  }
  else
  {
    if(iSize1 == -1)iSize1 = _LengthA(pstrStr1);
    if(iSize2 == -1)iSize2 = _LengthA(pstrStr2);

    //If the sizes are not equal, or at least one of them is equal to 0.
    if(iSize1 != iSize2 || iSize1 == 0 || iSize2 == 0)iSize1 -= iSize2;
    //If the dimensions are equal.
    else for(int c = 0; c < iSize2; c++)
    {
      if((iSize1 = *pstrStr1 - *pstrStr2) != 0)break;
      pstrStr1++;
      pstrStr2++;
    }
  }

  return (iSize1 == 0 ? 0 : (iSize1 > 0 ? 1 : -1));
}

LPWSTR Str::_findSubStringW(const LPWSTR string, const LPWSTR subString)
{
  if(*subString == 0)return (LPWSTR)string;

  LPWSTR cp = (LPWSTR)string;
  LPWSTR s1, s2;

  while(*cp)
  {
    s1 = cp;
    s2 = (LPWSTR)subString;

    while(*s1 && *s2 && !(*s1-*s2))s1++, s2++;
    if(*s2 == 0)return cp;
    cp++;
  }

  return NULL;
}

int Str::_sprintfW(LPWSTR pBuf, int iBufSize, LPCWSTR pstrFormat, ...)
{
  va_list list;
  va_start(list, pstrFormat);
  int iSize = _vsprintfW(pBuf, iBufSize, pstrFormat, list);
  va_end(list);
  return iSize;
}

int Str::_sprintfA(LPSTR pBuf, int iBufSize, LPCSTR pstrFormat, ...)
{
  va_list list;
  va_start(list, pstrFormat);
  int iSize = _vsprintfA(pBuf, iBufSize, pstrFormat, list);
  va_end(list);
  return iSize;
}

int Str::_vsprintfW(LPWSTR pBuf, int iBufSize, LPCWSTR pstrFormat, va_list arglist)
{
  if(iBufSize <= 0)return -1;

  Mem::_zero(pBuf, iBufSize * sizeof(WCHAR));
  int iSize = CWA(shlwapi, wvnsprintfW)(pBuf, iBufSize, pstrFormat, arglist);
  pBuf[iBufSize - 1] = 0;

  if(iSize == -1)
  {
    //Geniuses Indians decided to change the return value to -1 in Vista, if not enough 
	//space on the entire line, but the buffer is full.. Mind set LastError not enough...
    iSize = _LengthW(pBuf);
  }
  else pBuf[iSize] = 0;

  return iSize;
}

int Str::_vsprintfA(LPSTR pBuf, int iBufSize, LPCSTR pstrFormat, va_list arglist)
{
  if(iBufSize <= 0)return -1;

  Mem::_zero(pBuf, iBufSize);
  int iSize = CWA(shlwapi, wvnsprintfA)(pBuf, iBufSize, pstrFormat, arglist);
  pBuf[iBufSize - 1] = 0;

  if(iSize == -1)
  {
    //Geniuses Indians decided to change the return value to -1 in Vista, if not enough
    //space on the entire line, but the buffer is full. Mind set LastError not enough...
    iSize = _LengthA(pBuf);
  }
  else pBuf[iSize] = 0;

  return iSize;
}

void Str::_utf8Free(UTF8STRING *string)
{
  Mem::free(string->data);
  Mem::_zero(string, sizeof(UTF8STRING));
}

bool Str::_utf8FromUnicode(const LPWSTR source, int sourceSize, UTF8STRING *dest)
{
  if(sourceSize == -1)sourceSize = _LengthW(source);
  if(sourceSize > 0)
  {
    dest->data = (void *)unicodeToXEx(CP_UTF8, source, sourceSize);
    if(dest->data != NULL)
    {
      dest->length = sourceSize;
      dest->size   = _LengthA((LPSTR)dest->data);
      return true;
    }
  }
  return false;
}

