#include <windows.h>
#include <shlwapi.h>

#include "defines.h"
#include "defines.h"
#include "str.h"
#include "math.h"
#include "mem.h"

//FIXME: Redefine the size of an int to DWORD/SIZE_T.


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

static int xToUnicode(DWORD codePage, const LPSTR source, int sourceSize, LPWSTR dest, int destSize)
{
  if(sourceSize == -1)sourceSize = Str::_LengthA(source);
  int size = CWA(kernel32, MultiByteToWideChar)(codePage, 0, source, sourceSize, dest, destSize);
  if(destSize > 0)
  {
    if(size >= destSize)size = 0; //No place for the null character.
    dest[size] = 0;
  }
  return size;
}

static LPWSTR xToUnicodeEx(DWORD codePage, LPSTR source, int size)
{
  if(size == -1)size = Str::_LengthA(source);
  int destSize = xToUnicode(codePage, source, size, NULL, 0);
  if(destSize > 0)
  {
    destSize += sizeof(BYTE);
    LPWSTR dest = (LPWSTR)Mem::alloc(destSize * sizeof(WCHAR));
    if(dest != NULL)
    {
      xToUnicode(codePage, source, size, dest, destSize);
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

////////////////////////////////////////////////////////////////////////////////////////////////////

void Str::Init(void)
{

}

void Str::Uninit(void)
{

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

LPWSTR Str::_joinArgumentsW(const LPWSTR *args, DWORD argsCount)
{
  DWORD outputSize = 0;
  LPWSTR output    = NULL;
  
  for(DWORD i = 0; i < argsCount; i++)
  {
    bool quote = false;;

    //Check whether you need to enter into an argument in quotes.
    {
      LPWSTR p = args[i];
      while(*p != 0)
      {
        if(IS_SPACE_CHAR(*p))
        {
          quote = true;
          break;
        }
        p++;
      }
    }

    //allocate.
    int len = _LengthW(args[i]);
    if(!Mem::reallocEx(&output, (outputSize + 1/*space*/ + 1/*"*/ + len + 1/*"*/ + 1/*\0*/) * sizeof(WCHAR)))
    {
      Mem::free(output);
      return NULL;
    }

    //Copy.
    if(i > 0)output[outputSize++] = ' ';
    if(quote)output[outputSize++] = '\"';
    
    Str::_CopyW(output + outputSize, args[i], len);
    outputSize += len;
    
    if(quote)
    {
      output[outputSize++] = '\"';
      output[outputSize]   = 0;
    }
  }
  return output;
}

int Str::_unicodeToAnsi(const LPWSTR source, int sourceSize, LPSTR dest, int destSize)
{
  return unicodeToX(CP_ACP, source, sourceSize, dest, destSize);
}

LPSTR Str::_unicodeToAnsiEx(const LPWSTR source, int size)
{
  return unicodeToXEx(CP_ACP, source, size);
}

int Str::_ansiToUnicode(LPSTR source, int sourceSize, LPWSTR dest, int destSize)
{
  return ::xToUnicode(CP_ACP, source, sourceSize, dest, destSize);
}

LPWSTR Str::_ansiToUnicodeEx(LPSTR source, int size)
{
  return xToUnicodeEx(CP_ACP, source, size);
}

int Str::_oemToUnicode(LPSTR source, int sourceSize, LPWSTR dest, int destSize)
{
  return ::xToUnicode(CP_OEMCP, source, sourceSize, dest, destSize);
}

LPWSTR Str::_oemToUnicodeEx(LPSTR source, int size)
{
  return xToUnicodeEx(CP_OEMCP, source, size);
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

int Str::_CompareW(LPCWSTR pstrStr1, LPCWSTR pstrStr2, int iSize1, int iSize2)
{
  //pointers check
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
    if(iSize1 == -1)iSize1 = _LengthW(pstrStr1);
    if(iSize2 == -1)iSize2 = _LengthW(pstrStr2);

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

LPSTR Str::_findCharA(const LPSTR string, char c)
{
  LPSTR p = string;
  
  while(*p != 0 && *p != c)p++;
  if(*p == c)return (LPSTR)p;
  return NULL;
}

LPSTR Str::_findRCharA(const LPSTR string, char c)
{
  LPSTR p = string;
  LPSTR start = p;
  
  while(*p++ != 0);
  while(--p != start && *p != c);
  if(*p == c)return (LPSTR)p;
  return NULL;
}

LPWSTR Str::_findCharW(const LPWSTR string, WCHAR c)
{
  LPWSTR p = string;
  
  while(*p != 0 && *p != c)p++;
  if(*p == c)return (LPWSTR)p;
  return NULL;
}

LPWSTR Str::_findRCharW(const LPWSTR string, WCHAR c)
{
  LPWSTR p = string;
  LPWSTR start = p;
  
  while(*p++ != 0);
  while(--p != start && *p != c);
  if(*p == c)return (LPWSTR)p;
  return NULL;
}

LPSTR Str::_findSubStringA(const LPSTR string, const LPSTR subString)
{
  if(*subString == 0)return (LPSTR)string;

  LPSTR cp = (LPSTR)string;
  LPSTR s1, s2;

  while(*cp)
  {
    s1 = cp;
    s2 = (LPSTR)subString;

    while(*s1 && *s2 && !(*s1-*s2))s1++, s2++;
    if(*s2 == 0)return cp;
    cp++;
  }

  return NULL;
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

int Str::_sprintfExW(LPWSTR *ppBuf, LPCWSTR pstrFormat, ...)
{
  va_list list;
  va_start(list, pstrFormat);
  int iSize = _vsprintfExW(ppBuf, pstrFormat, list);
  va_end(list);
  return iSize;
}

DWORD Str::_sprintfCatExW(LPWSTR *buffer, DWORD offset, const LPWSTR format, ...)
{
  va_list list;
  va_start(list, format);
  DWORD size = _vsprintfCatExW(buffer, offset, format, list);
  va_end(list);
  return size;
}

int Str::_sprintfExA(LPSTR *ppBuf, LPCSTR pstrFormat, ...)
{
  va_list list;
  va_start(list, pstrFormat);
  int iSize = _vsprintfExA(ppBuf, pstrFormat, list);
  va_end(list);
  return iSize;
}

DWORD Str::_sprintfCatExA(LPSTR *buffer, DWORD offset, const LPSTR format, ...)
{
  va_list list;
  va_start(list, format);
  DWORD size = _vsprintfCatExA(buffer, offset, format, list);
  va_end(list);
  return size;
}

int Str::_vsprintfExW(LPWSTR *ppBuf, LPCWSTR pstrFormat, va_list arglist)
{
  LPWSTR pBuf  = NULL;
  int realSize = (int)vsprintfHelperExW(&pBuf, 0, (LPWSTR)pstrFormat, arglist);

  if(realSize < 0)Mem::free(pBuf);
  else *ppBuf = pBuf;
  
  return realSize;
}

DWORD Str::_vsprintfCatExW(LPWSTR *buffer, DWORD offset, const LPWSTR format, va_list arguments)
{
  LPWSTR source = *buffer;
  DWORD realSize = vsprintfHelperExW(buffer, offset, format, arguments);
  
  if(realSize == (DWORD)-1)
  {
    if(offset == 0 && source == NULL)
    {
      Mem::free(*buffer);
      *buffer = NULL;
    }
    Mem::reallocEx(buffer, (offset + 1/*safe byte*/) * sizeof(WCHAR));
  }
  return realSize;
}

int Str::_vsprintfExA(LPSTR *ppBuf, LPCSTR pstrFormat, va_list arglist)
{
  LPSTR pBuf   = NULL;
  int realSize = (int)vsprintfHelperExA(&pBuf, 0, (LPSTR)pstrFormat, arglist);

  if(realSize < 0)Mem::free(pBuf);
  else *ppBuf = pBuf;
  
  return realSize;
}

DWORD Str::_vsprintfCatExA(LPSTR *buffer, DWORD offset, const LPSTR format, va_list arguments)
{
  LPSTR source = *buffer;
  DWORD realSize = vsprintfHelperExA(buffer, offset, format, arguments);
  
  if(realSize == (DWORD)-1)
  {
    if(offset == 0 && source == NULL)
    {
      Mem::free(*buffer);
      *buffer = NULL;
    }
    Mem::reallocEx(buffer, (offset + 1/*safe byte*/) * sizeof(char));
  }
  return realSize;
}

bool Str::_utf8IsBom(const void *data)
{
  DWORD bom = UTF8_BOM;
  return (Mem::_compare(data, &bom, UTF8_BOM_SIZE) == 0);
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

bool Str::_utf8FromAnsi(const LPSTR source, int sourceSize, UTF8STRING *dest)
{
  if(sourceSize == -1)sourceSize = _LengthA(source);

  bool r = false;
  if(sourceSize > 0)
  {
    LPWSTR unicodeString = xToUnicodeEx(CP_ACP, source, sourceSize);
    if(unicodeString != NULL)
    {
      r = Str::_utf8FromUnicode(unicodeString, -1, dest);
      Mem::free(unicodeString);
    }
  }
  return r;
}

LPWSTR Str::_utf8ToUnicode(const void *rawString, const int size)
{
  return xToUnicodeEx(CP_UTF8, (LPSTR)rawString, size);
}

bool Str::_isValidMultiStringA(const LPSTR string, DWORD size)
{
  return (string != NULL && size >= 2 && string[size - 1] == 0 && (string)[size - 2] == 0);
}

bool Str::_isValidMultiStringW(const LPWSTR string, DWORD size)
{
  return (string != NULL && size >= 2 && string[size - 1] == 0 && (string)[size - 2] == 0);
}

LPSTR Str::_multiStringGetIndexA(LPSTR string, DWORD index)
{
  if(index == 0)return string;
  for(DWORD i = 0; ; string++)if(*string == 0)
  {
    LPSTR c = string + 1;
    if(*c == 0)break; //end of multiline.
    if(++i == index)return c;
  }
  return NULL;
}

LPWSTR Str::_multiStringGetIndexW(LPWSTR string, DWORD index)
{
  if(index == 0)return string;
  for(DWORD i = 0; ; string++)if(*string == 0)
  {
    LPWSTR c = string + 1;
    if(*c == 0)break; //end of multiline.
    if(++i == index)return c;
  }
  return NULL;
}

DWORD Str::_multiStringGetCountA(const LPSTR string)
{
  LPSTR p = string;
  DWORD count = 0;
  for(DWORD i = 0; ; p++)if(p[0] == 0)
  {
    count++;
    if(p[1] == 0)break;
  }
  return count;
}

DWORD Str::_multiStringGetCountW(const LPWSTR string)
{
  LPWSTR p = string;
  DWORD count = 0;
  for(DWORD i = 0; ; p++)if(p[0] == 0)
  {
    count++;
    if(p[1] == 0)break;
  }
  return count;
}

LPSTR Str::_multiStringCreateA(const LPSTR string, LPDWORD length)
{
  *length = 0;
  LPSTR ret = NULL;
  int len   = _LengthA(string);

  if(len > 0 && (ret = (LPSTR)Mem::alloc((len + 2) * sizeof(char))) != NULL)
  {
    Mem::_copy(ret, string, len * sizeof(char));
    ret[len]     = 0;
    ret[len + 1] = 0;
    *length      = len + 2;
  }
  return ret;
}

LPWSTR Str::_multiStringCreateW(const LPWSTR string, LPDWORD length)
{
  *length = 0;
  LPWSTR ret = NULL;
  int len   = _LengthW(string);

  if(len > 0 && (ret = (LPWSTR)Mem::alloc((len + 2) * sizeof(WCHAR))) != NULL)
  {
    Mem::_copy(ret, string, len * sizeof(WCHAR));
    ret[len]     = 0;
    ret[len + 1] = 0;
    *length      = len + 2;
  }
  return ret;
}

LPWSTR Str::_GetErrorString(DWORD dwErrorCode)
{
  LPWSTR pMsgBuf;
  LPWSTR pRet = NULL;
  DWORD dwSize = CWA(kernel32, FormatMessageW)(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                                               NULL,
                                               dwErrorCode,
                                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                               (LPWSTR)&pMsgBuf,
                                               0,
                                               NULL);
  if(dwSize > 0)
  {
    pRet = _CopyExW(pMsgBuf, dwSize);
    CWA(kernel32, LocalFree)(pMsgBuf);
  }

  return pRet;
}

void Str::_addSlashesW(LPWSTR source, LPWSTR dest)
{
  while(*source != 0)
  {
    if(*source == '\"' || *source == '\'' || *source == '\\' || *source == '\0')*dest++ = '\\';
    *dest++ = *source++;
  }
  *dest = 0;
}

LPWSTR Str::_addSlashesExW(const LPWSTR source, SIZE_T size)
{
  if(size == -1)size = _LengthW(source);
  LPWSTR dest = (LPWSTR)Mem::quickAlloc((size * 2 + 1) * sizeof(WCHAR));
  if(dest != NULL)_addSlashesW(source, dest);
  return dest;
}

void Str::_addSlashesA(LPSTR source, LPSTR dest)
{
  while(*source != 0)
  {
    if(*source == '\"' || *source == '\'' || *source == '\\' || *source == '\0')*dest++ = '\\';
    *dest++ = *source++;
  }
  *dest = 0;
}

LPSTR Str::_addSlashesExA(const LPSTR source, SIZE_T size)
{
  if(size == -1)size = _LengthA(source);
  LPSTR dest = (LPSTR)Mem::quickAlloc((size * 2 + 1) * sizeof(char));
  if(dest != NULL)_addSlashesA(source, dest);
  return dest;
}

SIZE_T Str::_getCurrentStringSizeW(const LPWSTR string, const LPWSTR memEnd, LPWSTR *next)
{
  LPWSTR curEnd = string; //A pointer to the end of line (\n character or the end of the memory).
  SIZE_T curSize;         //The size of the current line without characters \n and \r

  //Match the end of the current line.
  while(curEnd < memEnd && *curEnd != '\n' && *curEnd != '\r')curEnd++;
  curSize = curEnd - string;

  //remaining lines.
  if(curEnd + 1 < memEnd && curEnd[0] == '\r' && curEnd[1] == '\n')curEnd++;

  *next = curEnd + 1;
  return curSize;
}

SIZE_T Str::_getCurrentStringSizeA(const LPSTR string, const LPSTR memEnd, LPSTR *next)
{
	LPSTR curEnd = string; //A pointer to the end of line (\n character or the end of the memory).
	SIZE_T curSize;         //The size of the current line without characters \n and \r

  //Match the end of the current line.
  while(curEnd < memEnd && *curEnd != '\n' && *curEnd != '\r')curEnd++;
  curSize = curEnd - string;

  //remaining lines.
  if(curEnd + 1 < memEnd && curEnd[0] == '\r' && curEnd[1] == '\n')curEnd++;

  *next = curEnd + 1;
  return curSize;
}

#define engCharToLower(c) (c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c)
bool Str::_matchA(MATCHDATAA *md)
{
  if(md->flags & MATCH_SEARCH_SUBSSTRING)
  {
    MATCHDATAA subMd;
    Mem::_copy(&subMd, md, sizeof(MATCHDATAA));

    subMd.flags &= ~MATCH_SEARCH_SUBSSTRING;
    for(DWORD stringOffset = 0; stringOffset < md->stringSize; stringOffset++)
    {
      subMd.string     = md->string + stringOffset;
      subMd.stringSize = md->stringSize - stringOffset;

      if(_matchA(&subMd))
      {
        md->endOfMatch   = stringOffset + subMd.endOfMatch;
        md->beginOfMatch = stringOffset;
        return true;
      }
    }

    md->beginOfMatch = 0;
    md->endOfMatch   = 0;
    return false;
  }
  
  md->beginOfMatch = 0;
  for(DWORD stringOffset = 0, maskOffset = 0;; stringOffset++, maskOffset++)
  {
    //Reached the end of the masks.
    if(maskOffset == md->maskSize)
    {
      md->endOfMatch = stringOffset;
      return (md->flags & MATCH_FULL_EQUAL ? (stringOffset == md->stringSize) : true);
    }

    char maskChar = md->mask[maskOffset];

    //Just skip character.
    if(maskChar == md->anyCharSymbol)
    {
      //Reached the end of the line. failure...
      if(stringOffset == md->stringSize)
      {
        md->endOfMatch = stringOffset; 
        return false;
      }
    }
    else if(maskChar == md->anyCharsSymbol)
    {
      while(++maskOffset < md->maskSize && md->mask[maskOffset] == md->anyCharsSymbol);
      
      //Mask extend to end of line.
      if(maskOffset == md->maskSize)
      {
        md->endOfMatch = md->stringSize; 
        return true;
      }
      
      //Create a copy of the structure with the shift.
      MATCHDATAA subMd;
      bool r;

      Mem::_copy(&subMd, md, sizeof(MATCHDATAA));
      subMd.mask     += maskOffset;
      subMd.maskSize -= maskOffset;
      md->endOfMatch  = stringOffset;

      for(; stringOffset < md->stringSize; stringOffset++)
      {
        subMd.string     = md->string     + stringOffset;
        subMd.stringSize = md->stringSize - stringOffset;
        
        r = _matchA(&subMd);
        md->endOfMatch = stringOffset + subMd.endOfMatch;
        if(r)return true;
      }
      return false;
    }
    else
    {
      if(stringOffset == md->stringSize)
      {
        md->endOfMatch = stringOffset;
        return false;
      }
      
      char stringChar = md->string[stringOffset];
      
      //change casing.
      if(md->flags & (MATCH_CASE_INSENSITIVE_FAST | MATCH_CASE_INSENSITIVE))
      {
        if(md->flags & MATCH_CASE_INSENSITIVE_FAST)
        {
          maskChar   = engCharToLower(maskChar);
          stringChar = engCharToLower(stringChar);
        }
        else
        {
          maskChar   = ((DWORD_PTR)(CWA(user32, CharLowerA)((LPSTR)maskChar)))   & 0xFF;
          stringChar = ((DWORD_PTR)(CWA(user32, CharLowerA)((LPSTR)stringChar))) & 0xFF;
        }
      }
      
      //The characters are not equal.
      if(maskChar != stringChar)
      {
        if(md->flags & MATCH_UNIVERSAL_NEWLINE)
        {
          if(maskChar == '\n' && stringChar == '\r' && stringOffset + 1 < md->stringSize && md->string[stringOffset + 1] == '\n')
          {
            stringOffset++;
            continue;
          }

          if(stringChar == '\n' && maskChar == '\r' && maskOffset + 1 < md->maskSize && md->mask[maskOffset + 1] == '\n')
          {
            maskOffset++;
            continue;
          }
        }

        md->endOfMatch  = stringOffset;
        return false;
      }
    }
  }
}
#undef engCharToLower

bool Str::_matchExA(const LPSTR mask, const LPSTR string)
{
  MATCHDATAA md;

  md.anyCharsSymbol = '*';
  md.anyCharSymbol  = '?';
  md.mask           = mask;
  md.maskSize       = Str::_LengthA(mask);
  md.string         = string;
  md.stringSize     = Str::_LengthA(string);
  md.flags          = MATCH_FULL_EQUAL | MATCH_CASE_INSENSITIVE | MATCH_UNIVERSAL_NEWLINE;

  return _matchA(&md);
}
