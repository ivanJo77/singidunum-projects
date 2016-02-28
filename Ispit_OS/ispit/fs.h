#pragma once
#include "defines.h"
/*
  file system tools.
*/

namespace Fs
{
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
    Add-on over PathCombibeW, which eliminates one of the features of the function.

    OUT dest - buffer.
    IN dir   - The first part of the path.
    IN file  - the second part of the path.

	Return	 - true - if successful,
			   false - in a case of errors.
  */
  bool _pathCombine(LPWSTR dest, const LPWSTR dir, const LPWSTR file);

};
