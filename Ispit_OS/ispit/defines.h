#pragma once

#define CWA(dll, api)                 ::api
#define MEM_PERSONAL_HEAP             1
#define MEM_ALLOC_SAFE_BYTES          1
#define XLIB_UCL_ONLY_MAX_COMPRESSION 1
#define XLIB_UCL_ENABLE_NRV2B         1
#define XLIB_UCL_ENABLE_NRV2D         0
#define XLIB_UCL_ENABLE_NRV2E         0
#define XLIB_DEBUG_SERVER_URL         "http://someserver/report.php"
#define XLIB_DEBUG_SERVER_CRYPTKEY    "{0ECEEC85-264F-49FF-836F-0F7D3A7C5559}"
#define PEIMAGE_32                    1
#define PEIMAGE_64                    0
#define XLIB_PECRYPT_LITE             0
#define XLIB_MSCAB_FCI                1
#define XLIB_MSCAB_FDI                0
#define NTDLL_IMPORT                  0
#define FS_ALLOW_FILEMAPPING          0

#include "config.h"

//Making and version control.
#define MAKE_VERSION(a, b, c, d) (((((DWORD)(a)) & 0xFF) << 24) | ((((DWORD)(b)) & 0xFF) << 16) | ((((DWORD)(c)) & 0xFF) << 8) | ((((DWORD)(d)) & 0xFF)))
#define VERSION_MAJOR(a)         ((BYTE)(((a) >> 24) & 0xFF))
#define VERSION_MINOR(b)         ((BYTE)(((b) >> 16) & 0xFF))
#define VERSION_SUBMINOR(c)      ((BYTE)(((c) >> 8) & 0xFF))
#define VERSION_BUILD(d)         ((BYTE)((d) & 0xFF))

//Prefixes for functions that are entirely written in asm.
#if defined _WIN64
#  define ASM_INTERNAL_DEF
#  define ASM_INTERNAL
#else
#  define ASM_INTERNAL_DEF __stdcall
#  define ASM_INTERNAL     __declspec(naked) __stdcall
#endif

//Conversion BIG_ENDIAN <=> LITTLE_ENDIAN 
#define SWAP_WORD(s) (((((WORD)(s)) >> 8) & 0x00FF) | ((((WORD)(s)) << 8) & 0xFF00))
#define SWAP_DWORD(l) (((((DWORD)(l)) >> 24) & 0x000000FFL) | ((((DWORD)(l)) >>  8) & 0x0000FF00L) | ((((DWORD)(l)) <<  8) & 0x00FF0000L) | ((((DWORD)(l)) << 24) & 0xFF000000L))

//Create QWORD from two DWORD
#define MAKEDWORD64(l, h) ((DWORD64)(((DWORD)((DWORD64)(l) & MAXDWORD)) | ((DWORD64)((DWORD)((DWORD64)(h) & MAXDWORD))) << 32))

//The number of connection attempts.
#define WININET_CONNECT_RETRY_COUNT 5

//The delay between connections.
#define WININET_CONNECT_RETRY_DELAY 5000

//Storage settings in the registry.
#define PATH_REGKEY L"SOFTWARE\\Microsoft"

//Wrapper for PE executable.
#define FILEEXTENSION_EXECUTABLE L".exe"

//The Wrapper for a temporary file.
#define FILEEXTENSION_TEMP L".tmp"

//The Wrapper for a text file.
#define FILEEXTENSION_TXT L".txt"

//No. PE-section, which is a key basic configuration.
#define PESECTION_OF_BASECONFIG_KEY 3

//include music lyrics into final .EXE file
#define INCLUDE_LYRICS 1

//The format of screenshots for UserHook.
#define USERCLICK2IMAGE_LIMIT  20
#define USERCLICK2IMAGE_SIZE   500

//Min/Max ports for TCP-server.
#define TCPSERVER_PORT_FIRST 10000
#define TCPSERVER_PORT_LAST  40000

//The font used in the dialogues
#define FONT_DIALOG "MS Shell Dlg 2"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Various options, depending on the option BO_*.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if(0)
#  define HOOKER_SETWINDOWTEXT
#endif

#if(0)
#  define HOOKER_NTCREATEFILE
#endif

#if(0)
#  define HOOKER_SETCHILDPROCESSFLAGS
#endif

