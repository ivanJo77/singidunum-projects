#pragma once
/*
  tools for working wit ANSI and Unicode strings.

  Note: The functions are not designed to work with strings larger than the symbols of 0x7FFFFFFF.
*/

//Determines whether it's a whitespace character.
#define IS_SPACE_CHAR(c) ((c) == 0x20 || ((c) >= 0x9 && (c) <= 0xD))

//UTF8 BOM
#define UTF8_BOM      0xBFBBEF
#define UTF8_BOM_SIZE 3

//UTF16 BOM
#define UTF16_BOM      0xFEFF
#define UTF16_BOM_SIZE 2

namespace Str
{  
  //describing UTF-8 strings.
  typedef struct
  {
    void *data;   //string.
    DWORD length; //The size of the string in characters, excluding the null character.
    DWORD size;   //The size of the string in bytes, excluding the null character.
  }UTF8STRING;

  /*
    Copying Unicode strings.
    
    OUT pstrDest  - destination string, can not be NULL.
    IN pstrSource - source string, may be NULL.
    IN iSize      - number of characters to copy, or -1 to copy complete string.
  */
  void _CopyW(LPWSTR pstrDest, LPCWSTR pstrSource, int iSize);

  /*
    Copy a Unicode string with memory allocation.

	IN pstrSource - source string, may be NULL.
	IN iSize      - number of characters to copy, or -1 to copy complete string.

    Return        - pointer to the new copy of the string.
  */
  LPWSTR _CopyExW(LPWSTR pstrSource, int iSize);

  /*
    Copy ANSI strings.

	OUT pstrDest  - destination string, can not be NULL.
	IN pstrSource - source string, may be NULL.
	IN iSize      - number of characters to copy, or -1 to copy complete string.
  */
  void _CopyA(LPSTR pstrDest, LPCSTR pstrSource, int iSize);

  /*
    Copy ANSI string with memory allocation.

	IN pstrSource - source string, may be NULL.
	IN iSize      - number of characters to copy, or -1 to copy complete string.

    Return        - pointer to the new copy of the string.
  */
  LPSTR _CopyExA(LPSTR pstrSource, int iSize);

  /*
    Calculates the size of ANSI strings.

    IN pstrStr - string to process. May be NULL.

    Return     - string length.
  */
  int _LengthA(LPCSTR pstrStr);

  /*
    Calculates the size of Unicode strings.

	IN pstrStr - string to process. May be NULL.

	Return     - string length.
  */
  int _LengthW(LPCWSTR pstrStr);

  /*
    Compares two ANSI strings.

    IN pstrStr1 - first string. May be NULL.
    IN pstrStr2 - second string. May be NULL.
    IN iSize1   - length of the first string or -1.
    IN iSize2   - length of the second string or -1.

    Return      -  0 - strings are equal,
                  -1 - first string is less than second.
                   1 - first string is greater than second.
  */
  int _CompareA(LPCSTR pstrStr1, LPCSTR pstrStr2, int iSize1, int iSize2);

  /*
	Search for substring in Unicode string.

	IN string    - string in which to search. Can not be NULL.
	IN subString - string to search for. Can not be NULL.

	Return       - a pointer to the first occurrence of substring, or NULL if not found.
  */
  LPWSTR _findSubStringW(const LPWSTR string, const LPWSTR subString);
  
  /*
    Extension of wnsprintfW..

    OUT pBuf      - pointer to the destination string buffer.
    IN iBufSize   - buffer size (including the null terminator).
    IN pstrFormat - format string.
    IN ...        - format arguments.

    Return        - size of the string without the null character, or -1 on error.
  */
  int _sprintfW(LPWSTR pBuf, int iBufSize, LPCWSTR pstrFormat, ...);
  
  /*
	Extension of wnsprintfA..

	OUT pBuf      - pointer to the destination string buffer.
	IN iBufSize   - buffer size (including the null terminator).
	IN pstrFormat - format string.
	IN ...        - format arguments.

	Return        - size of the string without the null character, or -1 on error.
  */
  int _sprintfA(LPSTR pBuf, int iBufSize, LPCSTR pstrFormat, ...);

  /*
    Extension of wvnsprintfW..

	OUT pBuf      - pointer to the destination string buffer.
	IN iBufSize   - buffer size (including the null terminator).
	IN pstrFormat - format string.
	IN ...        - format arguments.

	Return        - size of the string without the null character, or -1 on error.
  */
  int _vsprintfW(LPWSTR pBuf, int iBufSize, LPCWSTR pstrFormat, va_list arglist);

  /*
    Extension of wvnsprintfA..

	OUT pBuf      - pointer to the destination string buffer.
	IN iBufSize   - buffer size (including the null terminator).
	IN pstrFormat - format string.
	IN ...        - format arguments.

	Return        - size of the string without the null character, or -1 on error.
  */
  int _vsprintfA(LPSTR pBuf, int iBufSize, LPCSTR pstrFormat, va_list arglist);

  /*
    Free UTF-8 string.

    IN string  - string in UTF-8.
  */
  void _utf8Free(UTF8STRING *string);

  /*
    Creating a UTF-8 string from Unicode.

    IN source     - source string.
    IN sourceSize - size of pstrSource, or -1 to auto-detect.
    OUT dest      - UTF-8 string.

    Return        - true - in case of success,
                    false - in case of error.
  */
  bool _utf8FromUnicode(const LPWSTR source, int sourceSize, UTF8STRING *dest);

};
