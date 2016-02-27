/*
  A set of functions to work with time as in Unix.
*/
#pragma once

namespace Time
{
  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Retrieve current time.

    Return - current time in seconds.
  */
  DWORD _getTime(void);

  /*
    Retrieve current local time.

    Return - current time in seconds.
  */
  DWORD _getLocalTime(void);

  /*
    Retrieve current time in GMT.

    Return - current GMT time in seconds.
  */
  int _getLocalGmt(void);

  /*
    Converting FILETIME in Unix time.

    IN ft  - pointer to FILETIME.

    Return - Unix time.
  */
  DWORD _fileTimeToTime(const FILETIME *ft);

  /*
    Convert Unix time to FILETIME.

    IN time - time.
    OUT ft  - pointer to FILETIME.

    Return  - Windows time.
  */
  void _timeToFileTime(const DWORD time, FILETIME *ft);
  
  /*
    Converting SYSTEMTIME in Unix time.

    IN st  - pointer to SYSTEMTIME.

    Return - Unix time.
  */
  DWORD _systemTimeToTime(const SYSTEMTIME *st);
};
