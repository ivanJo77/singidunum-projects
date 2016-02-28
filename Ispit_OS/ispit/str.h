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
  enum
  {
    STS_TRIM          = 0x1, //Trim automatically call for each line.
    STS_USE_SEPARATOR = 0x2, //Use a separator instead of a standard method \r\n.
  };
  
  enum
  {
    STA_FORMAT_C = 0x1, //Arguments are located in the C-format strings.
  };

  //describing UTF-8 strings.
  typedef struct
  {
    void *data;   //string.
    DWORD length; //The size of the string in characters, excluding the null character.
    DWORD size;   //The size of the string in bytes, excluding the null character.
  }UTF8STRING;

  typedef struct
  {
    char anyCharSymbol;  //Symbol of any one character, usually '?'.
    char anyCharsSymbol; //Symbol count of any character, usually '*'.
    LPSTR mask;          //Mask.
    DWORD maskSize;      //Mask size.
    LPSTR string;        //String.
    DWORD stringSize;    //string size.
	DWORD beginOfMatch;  //OUT index string, which begins comparison. Always 0 if flag is not
						 //set to MATCH_SEARCH_SUBSSTRING.
    DWORD endOfMatch;    //OUT index string, which ended the comparison.
    DWORD flags;         //flags MATCH_*.
  }MATCHDATAA;

  //flags MATCHDATAX.flags.
  enum
  {
    MATCH_FULL_EQUAL            = 0x01, //The mask should be extended to end of string.    
    MATCH_UNIVERSAL_NEWLINE     = 0x02, //does not distinguish between \r\n and \n.
    MATCH_CASE_INSENSITIVE      = 0x04, //Insensitive.
    MATCH_CASE_INSENSITIVE_FAST = 0x08, //Insensitive to characters A-Z, a-z.
    MATCH_SEARCH_SUBSSTRING     = 0x10  //Find the string in the string. (Similarly, the mask "*mask").
  };
  
  /*
    Init.
  */
  void Init(void);

  /*
    Uninit.
  */
  void Uninit(void);

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
    Converting binary data in Unicode HEX-string.

    IN buffer     - buffer. May be a pointer to a string.
    IN bufferSize - buffer size.
    OUT string    - string. It's size should be at least (bufferSize * 2 + 1) characters.
  */
  void _toHexW(const void *binary, DWORD binarySize, LPWSTR string);

  /*
	Converting binary data in ANSI HEX-string.

	IN buffer     - buffer. May be a pointer to a string.
	IN bufferSize - buffer size.
	OUT string    - string. It's size should be at least (bufferSize * 2 + 1) characters.                                                      
  */
  void _toHexA(const void *binary, DWORD binarySize, LPSTR string);

  /*
    Converts a string with special characters into normal bytes.
    
    IN OUT str - string to process.
  */
  void _fromCFormatA(LPSTR str);

  /*
	Converts a string with special characters into normal bytes.

	IN OUT str - string to process.
  */
  void _fromCFormatW(LPWSTR str);

  /*
    Splitting a string into lines based on a character \n or \r. Universal for Win, Mac, Unix.

    IN input     - source string.
    IN inputSize - source string size.
	OUT strings  - array of pointers to strings, to be released by Mem::freeArrayOfPointers.
				   Strings which will have zero size may be marked as
				   NULL-pointer also as blank lines.
    IN flags     - flags STS_*.
    IN separator - string separator, is relevant only when the flag STS_USE_SEPARATOR is set.
                  
    Return       - the number of lines parsed, or (DWORD)-1 if an error occurs.

  */
  DWORD _splitToStringsA(LPSTR input, DWORD inputSize, LPSTR **strings, DWORD flags, char separator);

  /*
	Splitting a string into lines based on a character \n or \r. Universal for Win, Mac, Unix.

	IN input     - source string.
	IN inputSize - source string size.
	OUT strings  - array of pointers to strings, to be released by Mem::freeArrayOfPointers.
				   Strings which will have zero size may be marked as
				   NULL-pointer also as blank lines.
	IN flags     - flags STS_*.
	IN separator - string separator, is relevant only when the flag STS_USE_SEPARATOR is set.

	Return       - the number of lines parsed, or (DWORD)-1 if an error occurs.

  */
  DWORD _splitToStringsW(LPWSTR input, DWORD inputSize, LPWSTR **strings, DWORD flags, WCHAR separator);

  /*
    Splitting a string of arguments, supporting quotes.

	IN input     - source string.
	IN inputSize - source string size.
    OUT args     - array of pointers to strings, to be released by Mem::freeArrayOfPointers.
                   Lines that will have zero size will be marked as "\0".
    IN flags     - flags STA_*.

    Return       - the number of arguments parsed, or (DWORD)-1 if an error occurs.
  */
  DWORD _getArgumentsA(const LPSTR input, DWORD inputSize, LPSTR **args, DWORD flags);

  /*
	Splitting a string of arguments, supporting quotes.

	IN input     - source string.
	IN inputSize - source string size.
	OUT args     - array of pointers to strings, to be released by Mem::freeArrayOfPointers.
				   Lines that will have zero size will be marked as "\0".
	IN flags     - flags STA_*.

	Return       - the number of arguments parsed, or (DWORD)-1 if an error occurs.
  */
  DWORD _getArgumentsW(const LPWSTR input, DWORD inputSize, LPWSTR **args, DWORD flags);

  /*
    Join arguments into string.

    IN args      - arguments.
    IN argsCount - number of arguments.

	Return       - string in case of success (must be freed by Mem),
				   NULL - in case of failure.
  */
  LPWSTR _joinArgumentsW(const LPWSTR *args, DWORD argsCount);
  
  /*
    Create ANSI string of Unicode.

    IN source     - source Unicode-string.
    IN sourceSize - source size in characters, or -1 to auto-detect.
    OUT dest      - new ANSI-line, will always end in 0. Can be NULL if destSize == 0.
	IN destSize   - dest buffer size, in bytes, including the null character. If 0, then the function
					returns the requested buffer size in bytes, excluding the zero character.

	Return        - 0 - error (dest [0] is equal to zero)
					or number of bytes copied or required to dest, excluding the 
					terminating null character.
  */
  int _unicodeToAnsi(const LPWSTR source, int sourceSize, LPSTR dest, int destSize);

  /*
    Create ANSI string from Unicode, with memory allocation.

	IN source - source Unicode-string.
	IN size   - source size in characters, or -1 to auto-detect.

    Return    - ANSI-string (always ended with 0), or NULL if error.
  */
  LPSTR _unicodeToAnsiEx(const LPWSTR source, int size);

  /*
    Creating Unicode strings from ANSI.

    IN source     - source ANSI-string.
    IN sourceSize - source size in characters, or -1 to auto-detect.
    OUT dest      - new Unicode-line, will always end in 0. Can be NULL if destSize == 0.
	IN destSize   - dest buffer size, in bytes, including the null character. If 0, then the function
					returns the requested buffer size in bytes, excluding the zero character.

	Return        - 0 - error (dest [0] is equal to zero)
					or number of bytes copied or required to dest, excluding the 
					terminating null character.
  */
  int _ansiToUnicode(LPSTR source, int sourceSize, LPWSTR dest, int destSize);

  /*
    Create Unicode string from ANSI, with memory allocation.

    IN source - source ANSI-string.
	IN size   - source size in characters, or -1 to auto-detect.

	Return    - Unicode-string (always ended with 0), or NULL if error.
  */
  LPWSTR _ansiToUnicodeEx(LPSTR source, int size);

  /*
	Creating Unicode strings from OEM.

	IN source     - source OEM-string.
	IN sourceSize - source size in characters, or -1 to auto-detect.
	OUT dest      - new Unicode-line, will always end in 0. Can be NULL if destSize == 0.
	IN destSize   - dest buffer size, in bytes, including the null character. If 0, then the function
					returns the requested buffer size in bytes, excluding the zero character.

	Return        - 0 - error (dest [0] is equal to zero)
					or number of bytes copied or required to dest, excluding the 
					terminating null character.
  */
  int _oemToUnicode(LPSTR source, int sourceSize, LPWSTR dest, int destSize);

  /*
	Create Unicode string from OEM, with memory allocation.

	IN source - source OEM-string.
	IN size   - source size in characters, or -1 to auto-detect.

	Return    - Unicode-string (always ended with 0), or NULL if error.
  */
  LPWSTR _oemToUnicodeEx(LPSTR source, int size);

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
	Compares two Unicode strings.

	IN pstrStr1 - first string. May be NULL.
	IN pstrStr2 - second string. May be NULL.
	IN iSize1   - length of the first string or -1.
	IN iSize2   - length of the second string or -1.

	Return      -  0 - strings are equal,
				  -1 - first string is less than second.
				   1 - first string is greater than second.
  */
  int _CompareW(LPCWSTR pstrStr1, LPCWSTR pstrStr2, int iSize1, int iSize2);

  /*
    Search for a character in ANSI string.

    IN string - string in which to search.
    IN c      - character to find.

    Return    - pointer to the first character found, or NULL if no match.
  */
  LPSTR _findCharA(const LPSTR string, char c);

  /*
	Search for a character in ANSI string, from the right.

	IN string - string in which to search.
	IN c      - character to find.

	Return    - pointer to the first character found, or NULL if no match.
  */
  LPSTR _findRCharA(const LPSTR string, char c);

  /*
	Search for a character in Unicode string.

	IN string - string in which to search.
	IN c      - character to find.

	Return    - pointer to the first character found, or NULL if no match.
  */
  LPWSTR _findCharW(const LPWSTR string, WCHAR c);
  
  /*
	Search for a character in Unicode string, from the right.

	IN string - string in which to search.
	IN c      - character to find.

	Return    - pointer to the first character found, or NULL if no match.
  */
  LPWSTR _findRCharW(const LPWSTR string, WCHAR c);

  /*
    Search for substring in ANSI string.

    IN string    - string in which to search. Can not be NULL.
    IN subString - string to search for. Can not be NULL.

    Return       - a pointer to the first occurrence of substring, or NULL if not found.
  */
  LPSTR _findSubStringA(const LPSTR string, const LPSTR subString);

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
    Extension of wsprintfW, with automatic memory allocation.

    OUT ppBuf     - pointer to the destination string buffer. To be released by Mem.
	IN pstrFormat - format string.
	IN ...        - format arguments.

    Return        - size of the string without the null character, or -1 on error.

    Note: The maximum size of the output string ~MAX_SPRINTF_STRING_SIZE;
  */
  int _sprintfExW(LPWSTR *ppBuf, LPCWSTR pstrFormat, ...);

  /*
    Extension of _sprintfExW, which adds a new row at the end of the existing.

	IN OUT buffer - buffer. May point to NULL. In the case of an error, and if not specified NULL,
					will be reduced to offset characters ending in the null character.
    IN offset     - Offset in characters in *buffer, from which to start making changes.
	IN format     - format string.
	IN ...        - format arguments.

    Return        - full size of the string without the null character, or (DWORD)-1 in case of error.
  */
  DWORD _sprintfCatExW(LPWSTR *buffer, DWORD offset, const LPWSTR format, ...);

  /*
    Extension of wsprintfA, with automatic memory allocation.

	OUT ppBuf     - pointer to the destination string buffer. To be released by Mem.
	IN pstrFormat - format string.
	IN ...        - format arguments.

	Return        - size of the string without the null character, or -1 on error.

    Note: the maximum size of the input string ~MAX_SPRINTF_STRING_SIZE;
  */
  int _sprintfExA(LPSTR *ppBuf, LPCSTR pstrFormat, ...);

  /*
	Extension of wsprintfW, with automatic memory allocation.

	OUT ppBuf     - pointer to the destination string buffer. To be released by Mem.
	IN pstrFormat - format string.
	IN ...        - format arguments.

	Return        - size of the string without the null character, or -1 on error.

	Note: The maximum size of the output string ~MAX_SPRINTF_STRING_SIZE;
  */
  int _vsprintfExW(LPWSTR *ppBuf, LPCWSTR pstrFormat, va_list arglist);
  
  /*
	Extension of _sprintfExA, which adds a new row at the end of the existing.

	IN OUT buffer - buffer. May point to NULL. In the case of an error, and if not specified NULL,
					will be reduced to offset characters ending in the null character.
	IN offset     - Offset in characters in *buffer, from which to start making changes.
	IN format     - format string.
	IN ...        - format arguments.

	Return        - full size of the string without the null character, or (DWORD)-1 in case of error.
  */
  DWORD _sprintfCatExA(LPSTR *buffer, DWORD offset, const LPSTR format, ...);

  /*
    Extension of _vsprintfExW, which adds a new row at the end of the existing.

	IN OUT buffer - buffer. May point to NULL. In the case of an error, and if not specified NULL,
					will be reduced to offset characters ending in the null character.
	IN offset     - Offset in characters in *buffer, from which to start making changes.
	IN format     - format string.
	IN ...        - format arguments.

	Return        - full size of the string without the null character, or (DWORD)-1 in case of error.
  */
  DWORD _vsprintfCatExW(LPWSTR *buffer, DWORD offset, const LPWSTR format, va_list arguments);

  /*
    Extension of wvsprintfA, with automatic memory allocation.

	OUT ppBuf     - pointer to the destination string buffer. To be released by Mem.
	IN pstrFormat - format string.
    IN arglist    - string arguments.

    Return        - size of the string without the null character, or -1 on error.

    Note: The maximum size of the output string ~MAX_SPRINTF_STRING_SIZE;
  */
  int _vsprintfExA(LPSTR *ppBuf, LPCSTR pstrFormat, va_list arglist);
  
  /*
    Extension of _vsprintfExA, which adds a new row at the end of the existing.

	IN OUT buffer - buffer. May point to NULL. In the case of an error, and if not specified NULL,
	will be reduced to offset characters ending in the null character.
	IN offset     - Offset in characters in *buffer, from which to start making changes.
	IN format     - format string.
    IN arguments  - string arguments.

    Return        - size of the string without the null character, or -1 on error.
  */
  DWORD _vsprintfCatExA(LPSTR *buffer, DWORD offset, const LPSTR format, va_list arguments);

  /*
    Checks for the byte UTF-8 BOM.

    IN data - bytes to check.
  
    Return  - true - in case of success,
              false - in case of error.
  */
  bool _utf8IsBom(const void *data);

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

  /*
    Creating a UTF-8 string from ANSI.

	IN source     - source string.
	IN sourceSize - size of pstrSource, or -1 to auto-detect.
	OUT dest      - UTF-8 string.

    Return        - true - in case of success,
                    false - in case of error.
  */
  bool _utf8FromAnsi(const LPSTR source, int sourceSize, UTF8STRING *dest);

  /*
    Convert UTF-8 string to Unicode.

    IN rawString - UTF-8 string.
    IN size      - string size in bytes, or -1 to auto-detect.

    Return       - Unicode-string in case of success (must be freed by Mem),
                   or NULL in case of error.
  */
  LPWSTR _utf8ToUnicode(const void *rawString, const int size);

  /*
    Checks whether a string is multistring string.

    IN string - multistring string to check.
    IN size   - size of multistring.
    
    Return    - true - in case of success,
                false - in case of error.
  */
  bool _isValidMultiStringA(const LPSTR string, DWORD size);

  /*
	Checks whether a string is multistring string.

	IN string - multistring string to check.
	IN size   - size of multistring.
    
    Return    - true - in case of success,
                false - in case of error.
  */
  bool _isValidMultiStringW(const LPWSTR string, DWORD size);

  /*
    Retrieve a string from multistring by index.

    IN string - multistring string.
    IN index  - index.

    Return    - pointer to the string, or NULL in case of error.
  */
  LPSTR _multiStringGetIndexA(LPSTR string, DWORD index);

  /*
	Retrieve a string from multistring by index.

	IN string - multistring string.
	IN index  - index.

	Return    - pointer to the string, or NULL in case of error.
  */
  LPWSTR _multiStringGetIndexW(LPWSTR string, DWORD index);

  /*
    Retrieve row count in multistring string.

    IN string - multistring string.

    Return    - number of lines.
  */
  DWORD _multiStringGetCountA(const LPSTR string);
  
  /*
	Retrieve row count in multistring string.

	IN string - multistring string.

	Return    - number of lines.
  */
  DWORD _multiStringGetCountW(const LPWSTR string);

  /*
    Creates a multistring with one row of string.

    IN string  - source string.
    OUT length - full size of the string in characters, including null characters.

    Return     - multistring (must be freed by Mem), or NULL on error.
  */
  LPSTR _multiStringCreateA(const LPSTR string, LPDWORD length);

  /*
  Creates a multistring with one row of string.

  IN string  - source string.
  OUT length - full size of the string in characters, including null characters.

  Return     - multistring (must be freed by Mem), or NULL on error.
  */
  LPWSTR _multiStringCreateW(const LPWSTR string, LPDWORD length);

  /*
    Returns formatted string from a system error.

    IN dwErrorCode - error code.

	Return         - error string, or NULL in case of error. The string must be emptied by Mem.
  */
  LPWSTR _GetErrorString(DWORD dwErrorCode);

  /*
	Quote string with slashes by adding characters \, ', ", \0 - slash. Analogue to PHP addslashes.

    IN source - source string.
    IN dest   - destination string.
  */
  void _addSlashesW(LPWSTR source, LPWSTR dest);

  /*
    Quote string with slashes by adding characters \, ', ", \0 - slash, with memory allocation.

    IN source - source string.
    IN size   - the size of the string in characters. May be -1.

    Return    - NULL - in case of error, address of new string in case of success.
  */
  LPWSTR _addSlashesExW(const LPWSTR source, SIZE_T size);

  /*
	Quote string with slashes by adding characters \, ', ", \0 - slash. Analogue to PHP addslashes.

	IN source - source string.
	IN dest   - destination string.
  */
  void _addSlashesA(LPSTR source, LPSTR dest);

  /*
	Quote string with slashes by adding characters \, ', ", \0 - slash, with memory allocation.

	IN source - source string.
	IN size   - the size of the string in characters. May be -1.

	Return    - NULL - in case of error, address of new string in case of success.
  */
  LPSTR _addSlashesExA(const LPSTR source, SIZE_T size);

  /*
    Get the size of the current line in the list of strings separated by \r\n or \n.

    IN string - beginning of the line. Subsequent call should be next.
    IN memEnd - End of memory. Usually string + size.
    OUT next  - a pointer to the next line, or the end of the memory.

    Return    - string size.
  */
  SIZE_T __inline _getCurrentStringSizeW(const LPWSTR string, const LPWSTR memEnd, LPWSTR *next);

  /*
	Get the size of the current line in the list of strings separated by \r\n or \n.

	IN string - beginning of the line. Subsequent call should be next.
	IN memEnd - End of memory. Usually string + size.
	OUT next  - a pointer to the next line, or the end of the memory.

	Return    - string size.
  */
  SIZE_T __inline _getCurrentStringSizeA(const LPSTR string, const LPSTR memEnd, LPSTR *next);

  /*
    Comparing strings with a mask.

    IN md - comparison data.

    Return - true  - a match is found,
             false - no match.
  */
  bool _matchA(MATCHDATAA *md);

  /*
    Lightweight version of matchA(), for comparison with the characters '*', '?', and also insensitive.

    IN mask   - mask.
    IN string - string.

    Return - true  - a match is found,
             false - no match.
  */
  bool _matchExA(const LPSTR mask, const LPSTR string);
};
