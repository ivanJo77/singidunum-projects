#pragma once
#include "defines.h"
/*
  Tools to work with file system.
*/

#if !defined FS_ALLOW_FILEMAPPING
#  error FS_ALLOW_FILEMAPPING not defined!
#endif

namespace Fs
{
  //Flags for _fileToMem
  enum
  {
#   if(FS_ALLOW_FILEMAPPING > 0)
    FTOMF_WRITE_ACCESS = 0x1, //Open a file for writing.
#   endif
    FTOMF_SHARE_WRITE  = 0x2, //Open the file, even if it is open for shared write.
  };

  enum
  {
    FFFLAG_RECURSIVE      = 0x1, //Recursive search.
    FFFLAG_SEARCH_FOLDERS = 0x2, //Directory search.
    FFFLAG_SEARCH_FILES   = 0x4, //File search.
  };

  //Data on the image file in the memory limit of 4Gb.
  typedef struct
  {
    LPBYTE data; //The contents of the file.
    SIZE_T size; //data size.
    HANDLE file; //file handle.
#   if(FS_ALLOW_FILEMAPPING > 0)
    HANDLE map;  //image handle.
#   endif
  }MEMFILE;

  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Saving data to a file.

	IN fileName - the file for writing.
    IN data     - data to be written, can be NULL.
    IN dataSize - size of the data, may be 0.

	Return      - true, if was successful,
				  false, in a case of error. 
  */
  bool _saveToFile(const LPWSTR fileName, const void *data, DWORD dataSize);

  /*
    Reading the file to the buffer.

    IN fileName   - file to open.
    OUT buffer    - buffer.
    IN bufferSize - buffer size.

    Return        - count of bytes read, or (DWORD) -1 on error.
  */
  DWORD _readFileToBuffer(const LPWSTR fileName, void *buffer, DWORD bufferSize);

  /*
    Creates an image file in memory.

    IN fileName - file to open.
    OUT mem     - data about the image file, the structure must be emptied CloseMemFile.
    IN flags    - flags FTOMF_*.

    Return      - true - in case image was created
                  false - in the case of an error, or the file size more than 4Gb.
  */
  bool _fileToMem(LPWSTR fileName, MEMFILE *mem, DWORD flags);

  /*
    Closes an open file FileToMem.

    IN mem - file to close.
  */
  void _closeMemFile(MEMFILE *mem);

  /*
    The add-on SetFilePointerEx to change the position in the file.
    
    IN file           - file handle.
    IN distanceToMove - distance to move, can be a negative number.
    IN moveMethod     - way to move: FILE_BEGIN, FILE_CURRENT, FILE_END.

	Return			  - true, if was successful,
						false, in a case of error. 
  */
  bool _setFilePointer(HANDLE file, DWORD64 distanceToMove, DWORD moveMethod);

  /*
    Retrieve current file pointer position.

    IN file - file handle.

    Return - position in the file, or (DWORD64) (-1) in the case of an error.
  */
  DWORD64 _getFilePointer(HANDLE file);

  /*
    Retrieve file size.

    IN file - file handle.

    Return  - position in the file, or (DWORD64) (-1) in the case of an error.
  */
  DWORD64 _getFileSize(HANDLE file);

  /*
    Retrieve file size.

    IN file - file path.

    Return  - position in the file, or (DWORD64) (-1) in the case of an error.
  */
  DWORD64 _getFileSizeEx(LPWSTR file);

  /*
    Deleting a file with a preliminary removal of attributes.

    IN file - file name.

	Return	- true, if was successful,
			  false, in a case of error. 
  */
  bool _removeFile(LPWSTR file);

  /*
    Creating a temporary file in the% TEMP%.

	IN prefix  - prefix in the file name (no more than 3-x characters), or NULL for default values.
    OUT buffer - buffer for the entire file name, size of the buffer must be at least MAX_PATH.

	Return		- true, if was successful,
				  false, in a case of error. 
  */
  bool _createTempFile(const LPWSTR prefix, LPWSTR buffer);

  /*
    Creating a temporary file in the% TEMP% (heavier function is also required initialization Crypt).

    IN prefix    - prefix in the file name (no more than 3-x characters), or NULL for default values.
    IN extension - file extension (without the dot) can not be NULL.
    OUT buffer   - buffer for the entire file name, size of the buffer must be at least MAX_PATH.

	Return		- true, if was successful,
				  false, in a case of error. 
  */
  bool _createTempFileEx(const LPWSTR prefix, const LPWSTR extension, LPWSTR buffer);

  /*
    Create a temporary folder to the %TEMP%.

	IN prefix    - prefix in the name of the folder (no more than 3-x characters), or NULL for 
				   default values .
    OUT buffer   - buffer to complete the folder name, the size of the buffer must be at least MAX_PATH.

	Return		- true, if was successful,
				  false, in a case of error. 
	*/
  bool _createTempDirectory(const LPWSTR prefix, LPWSTR buffer);
    
  /*
    Determines whether the name '..' or '.'.

    IN name - name for verification.

    Return  - true - if it is,
              false - if it isn't.
  */
  bool _isDotsName(LPWSTR name);

  /*
    Write file format data [DWORD: dwDataSize] [BYTE [dwDataSize]].

    IN file     - file that will be written.
    IN data     - data to write.
    IN dataSize - data size.

    Return      - true - if data write was successful,
				  false - If an error occurs when a file-pointer will be returned to the place, but
						  if the file size will not be restored if the data record as it affected
						  to change it.
  */
  bool _writeSizeAndData(HANDLE file, void *data, DWORD dataSize);

  /*
    The add-on ReadFile with built-waiting asynchronous operation.

    IN hFile                 - file handle.
    OUT pBuffer              - buffer.
    IN dwNumberOfBytesToRead - number of bytes to read.
    OUT pdwNumberOfBytesRead - the number of bytes read, or 0 if all the data is read.
    IN hOverlappedEvent      - event under the rules of the asynchronous I/O.

    Return                   - true - if read was successful,
                               false - in a case of errors.
  */
  bool _ReadFile(HANDLE hFile, void *pBuffer, DWORD dwNumberOfBytesToRead, LPDWORD pdwNumberOfBytesRead, HANDLE hOverlappedEvent);

  /*
    The add-on WriteFile with built-waiting asynchronous operation.

    IN hFile                    - file handle.
    IN pBuffer                  - buffer.
    IN dwNumberOfBytesToWrite   - number of bytes to write.
    OUT pdwNumberOfBytesWritten - the number of bytes written, or 0 if all the data is read.
    IN hOverlappedEvent         - event under the rules of the asynchronous I/O.

	Return                      - true - if write was successful,
								  false - in a case of errors.
  */
  bool _WriteFile(HANDLE hFile, void *pBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD pdwNumberOfBytesWritten, HANDLE hOverlappedEvent);

  /*
    Output a formatted string to a file.

    IN hFile      - file handle.
    IN pstrFormat - output line-format.
    IN ...        - line-format arguments.
    
	Return        - true - if successful,
					false - in a case of errors.
  */
  bool _WriteFileFormatA(HANDLE hFile, LPSTR pstrFormat, ...);

  /*
    Set the time for a folder or file.

    IN isDir          - true - if file - is a directory, false - if the file.
    IN file           - file or folder for processing.
    IN creationTime   - time of creation, can be NULL.
    IN lastAccessTime - time of last access, can be NULL.
    IN lastWriteTime  - time of last modification, may be NULL.

	Return			  - true - if successful,
						false - in a case of errors.
  */
  bool _setFileTime(bool isDir, LPWSTR file, FILETIME *creationTime, FILETIME *lastAccessTime, FILETIME *lastWriteTime);

  /*
    Generate random time to file and its parents from startTime to the current 
	time minus reservedSeconds.

    IN startTime       - time as to which will be generated.
    IN reservedSeconds - reserved seconds of the current time that is impossible to generate.
    IN file            - file or folder for processing. The string should not end with a slash.
    IN upLevel         - maximum level for processing. 0 - only file.

    Return             - true - if successful,
                         false - in the case of an error, or if at least one object could 
						 not set time
  */
  bool _setRandomFileTime(DWORD startTime, DWORD reservedSeconds, LPWSTR fileName, DWORD upLevel);

  /*
    Retrieve time for a folder or file.

    IN isDir          - true - if file - is a directory, false - if the file.
    IN file           - file or folder for processing.
	IN creationTime   - time of creation, can be NULL.
	IN lastAccessTime - time of last access, can be NULL.
	IN lastWriteTime  - time of last modification, may be NULL.

	Return			  - true - if successful,
						false - in a case of errors.
  */
  bool _getFileTime(bool isDir, LPWSTR file, FILETIME *creationTime, FILETIME *lastAccessTime, FILETIME *lastWriteTime);

  /*
    Copy the file time.
    
    IN sourceIsDir - true - if sourceFile - is a directory, false - if the file.
    IN sourceFile  - source file or folder for processing.
    IN destIsDir   - true - if destFile - is a directory, false - if the file.
    IN destFile    - destination file or folder for processing.

	Return		   - true - if successful,
					 false - in a case of errors.
  */
  bool _copyFileTime(bool sourceIsDir, LPWSTR sourceFile, bool destIsDir, LPWSTR destFile);

  /*
    Creating a directory tree.

    IN path               - the full path to create. According to this address must be writable.
    IN securityAttributes - see CreateDirectory, for each newly created directory is specified, 
							this structure can be NULL.

	Return				  - true - if successful,
							false - in a case of errors.
  */
  bool _createDirectoryTree(LPWSTR path, LPSECURITY_ATTRIBUTES securityAttributes);

  /*
    Remove the directory tree.

    IN path - path to remove.

	Return	- true - if successful,
			  false - in a case of errors.
  */
  bool _removeDirectoryTree(LPWSTR path);

  /*
    Callback function called by _findFiles when file is found.

    IN path     - the path that the file was found. May end in a slash!
    IN fileInfo - file data.
    IN data     - arbitrary data.

    Return      - true - continue search,
                  false - stop search.
  */
  typedef bool (FINDFILEPROC)(const LPWSTR path, const WIN32_FIND_DATAW *fileInfo, void *data);
  
  /*
    Removes quotation marks and replace the environment variables with their values.

    IN path    - path to process.
    OUT buffer - processed buffer. The buffer must be at least MAX_PATH

	Return	   - true - if successful,
				 false - in a case of errors.
  */
  bool _unquoteAndExpandPath(LPWSTR path, LPWSTR buffer);

  /*
    Add-on over PathCombibeW, which eliminates one of the features of the function.

    OUT dest - buffer.
    IN dir   - The first part of the path.
    IN file  - the second part of the path.

	Return	 - true - if successful,
			   false - in a case of errors.
  */
  bool _pathCombine(LPWSTR dest, const LPWSTR dir, const LPWSTR file);

  /*
    Converts / in \.

    IN OUT path - path.
  */
  void _normalizeSlashes(LPWSTR path);

  /*
    Replacing slashes on a character.

    IN OUT string - line for processing.
    IN c          - new symbol.
  */
  void _replaceSlashes(LPWSTR string, WCHAR c);
};
