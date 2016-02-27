#pragma once
/*
  A system for creating and sending debug logs on the server (BO_DEBUG == 2) or locally
  (BO_DEBUG == 1).
*/

#if(BO_DEBUG > 0)

//Macros for fast access.
#define WDEBUG_PREFIX(t, f) DebugClient::WriteString(__FUNCTION__, __FILE__, __LINE__, DebugClient::t, L##f
#define WDEBUG0(t, f)                                                WDEBUG_PREFIX(t, f))
#define WDEBUG1(t, f, c1)                                            WDEBUG_PREFIX(t, f), c1)
#define WDEBUG2(t, f, c1, c2)                                        WDEBUG_PREFIX(t, f), c1, c2)
#define WDEBUG3(t, f, c1, c2, c3)                                    WDEBUG_PREFIX(t, f), c1, c2, c3)
#define WDEBUG4(t, f, c1, c2, c3, c4)                                WDEBUG_PREFIX(t, f), c1, c2, c3, c4)
#define WDEBUG5(t, f, c1, c2, c3, c4, c5)                            WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5)
#define WDEBUG6(t, f, c1, c2, c3, c4, c5, c6)                        WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6)
#define WDEBUG7(t, f, c1, c2, c3, c4, c5, c6, c7)                    WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6, c7)
#define WDEBUG8(t, f, c1, c2, c3, c4, c5, c6, c7, c8)                WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6, c7, c8)
#define WDEBUG9(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9)            WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6, c7, c8, c9)
#define WDEBUG10(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10)      WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6, c7, c8, c9, c10)
#define WDEBUG11(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11) WDEBUG_PREFIX(t, f), c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11)

namespace DebugClient
{
  //Log Type
  enum
  {
    WDDT_INFO,    //Information.
    WDDT_WARNING, //Warning.
    WDDT_ERROR    //Error.
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
    String entry in the log report.

    IN pstrFuncName   - current function name.
    IN pstrSourceFile - current source file.
    IN dwLineNumber   - the current line number in the current file.
    IN bType          - one of the values WDDT_*.
    IN pstrFormat     - format analogue to sprintf.
    IN ...            - arguments for pstrFormat.
  */
  void WriteString(LPSTR pstrFuncName, LPSTR pstrSourceFile, DWORD dwLineNumber, BYTE bType, LPWSTR pstrFormat, ...);

  /*
  Running a local server to receive logs from WriteString all processes, and send them to
  server.
  */
#if(BO_DEBUG == 2)
  void StartLogServer(void);
#endif
  
  /*
    Setting the hook on the exception for records in the log.
  */
  void RegisterExceptionFilter(void);
};
#else
#  define WDEBUG0(t, f)
#  define WDEBUG1(t, f, c1)
#  define WDEBUG2(t, f, c1, c2)
#  define WDEBUG3(t, f, c1, c2, c3)
#  define WDEBUG4(t, f, c1, c2, c3, c4)
#  define WDEBUG5(t, f, c1, c2, c3, c4, c5)
#  define WDEBUG6(t, f, c1, c2, c3, c4, c5, c6)
#  define WDEBUG7(t, f, c1, c2, c3, c4, c5, c6, c7)
#  define WDEBUG8(t, f, c1, c2, c3, c4, c5, c6, c7, c8)
#  define WDEBUG9(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9)
#  define WDEBUG10(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10)
#  define WDEBUG11(t, f, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11)
#endif
