#pragma once

#define CWA(dll, api)                 ::api
#define MEM_PERSONAL_HEAP             1
#define MEM_ALLOC_SAFE_BYTES          1
#define PEIMAGE_32                    1
#define PEIMAGE_64                    0
#define NTDLL_IMPORT                  0

//Prefixes for functions that are entirely written in asm.
#if defined _WIN64
#  define ASM_INTERNAL_DEF
#  define ASM_INTERNAL
#else
#  define ASM_INTERNAL_DEF __stdcall
#  define ASM_INTERNAL     __declspec(naked) __stdcall
#endif

