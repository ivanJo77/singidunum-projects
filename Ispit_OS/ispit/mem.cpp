#include <windows.h>

#include "defines.h"
#include "mem.h"

#if defined _WIN64
#  define ADVANCED_ALLOC_BYTES 8
#else
#  define ADVANCED_ALLOC_BYTES 4
#endif

static HANDLE mainHeap;

#if(MEM_PERSONAL_HEAP == 1)
static bool heapCreated;
#endif

#if(MEM_PERSONAL_HEAP == 1)
void Mem::init(SIZE_T initialSize)
{
  mainHeap = CWA(kernel32, HeapCreate)(0, initialSize, 0);
  if(mainHeap == NULL)
  {
    mainHeap = CWA(kernel32, GetProcessHeap)();
    heapCreated = false;
  }
  else heapCreated = true;
}
#else
void Mem::init(void)
{
  mainHeap = CWA(kernel32, GetProcessHeap)();
}
#endif

void Mem::uninit(void)
{
# if(MEM_PERSONAL_HEAP == 1)
  if(heapCreated)CWA(kernel32, HeapDestroy)(mainHeap);
# endif
}

bool Mem::reallocEx(void *old, SIZE_T size)
{
  if(size == 0)
  {
    free(*(LPBYTE *)old);
    *(LPBYTE *)old = NULL;
  }
  else
  {
    register void *p = realloc(*(LPBYTE *)old, size);
    if(p == NULL)return false;
    *(LPBYTE *)old = (LPBYTE)p;
  }

  return true;
}

void *Mem::realloc(void *old, SIZE_T size)
{
  if(size == 0)return NULL;

# if(MEM_ALLOC_SAFE_BYTES == 1)
  size += ADVANCED_ALLOC_BYTES;
# endif

  if(old == NULL)old = CWA(kernel32, HeapAlloc)(mainHeap, HEAP_ZERO_MEMORY, size);
  else old = CWA(kernel32, HeapReAlloc)(mainHeap, HEAP_ZERO_MEMORY, old, size);

  return old;
}

void *Mem::alloc(SIZE_T size)
{
  register void *p;

  if(size == 0)p = NULL;
  else
  {
#   if(MEM_ALLOC_SAFE_BYTES == 1)
    size += ADVANCED_ALLOC_BYTES;
#   endif

    p = CWA(kernel32, HeapAlloc)(mainHeap, HEAP_ZERO_MEMORY, size);
  }
  return p;
}

void *Mem::quickAlloc(SIZE_T size)
{
  register void *p;

  if(size == 0)p = NULL;
  else
  {
#   if(MEM_ALLOC_SAFE_BYTES == 1)
    size += ADVANCED_ALLOC_BYTES;
#   endif

    p = CWA(kernel32, HeapAlloc)(mainHeap, 0, size);
  }
  return p;
}

void Mem::free(void *mem)
{
  if(mem)CWA(kernel32, HeapFree)(mainHeap, 0, mem);
}

void Mem::zeroAndFree(void *mem, SIZE_T size)
{
  _zero(mem, size);
  free(mem);
}

void Mem::freeArrayOfPointers(void *mem, SIZE_T count)
{
  if(mem && count)
  {
    LPBYTE *p = (LPBYTE *)mem;
    while(count--)free(p[count]);
    free(p);
  }
}

void ASM_INTERNAL Mem::_copy(void *dest, const void *source, SIZE_T size)
{
#if defined _WIN64
  for(register SIZE_T i = 0; i < size; i++)
  {
    ((LPBYTE)dest)[i] = ((LPBYTE)source)[i];
    if(i == 0)i = 0; //Dumb compiler tries to use memcpy.
  }
#else
  __asm
  {
    push esi
    push edi
    push ecx

    mov esi, dword ptr[esp + 0x8 + 0xC]
    mov edi, dword ptr[esp + 0x4 + 0xC]
    mov ecx, dword ptr[esp + 0xC + 0xC]
    rep movsb

    pop ecx
    pop edi
    pop esi
    ret 0xC
  };
#endif
}

void *Mem::copyEx(const void *source, SIZE_T size)
{
  void *p = quickAlloc(size);
  if(p != NULL)_copy(p, source, size);
  return p;
}

int Mem::_compare(const void *mem1, const void *mem2, SIZE_T size)
{
  register BYTE m1, m2;
  for(register SIZE_T i = 0; i < size; i++)
  {
    m1 = ((LPBYTE)mem1)[i];
    m2 = ((LPBYTE)mem2)[i];
    if(m1 != m2)return (int)(m1 - m2);
  }
  return 0;
}

void Mem::_zero(void *mem, SIZE_T size)
{
  _set(mem, 0, size);
}

void ASM_INTERNAL Mem::_set(void *mem, char c, SIZE_T size)
{
#if defined _WIN64
  register SIZE_T i = size;
  while(i--)((char *)mem)[i] = c;
#else
  __asm
  {
    push eax
    push edi
    push ecx

    mov al,  byte ptr[esp + 0x8 + 0xC]
    mov edi, dword ptr[esp + 0x4 + 0xC]
    mov ecx, dword ptr[esp + 0xC + 0xC]
    rep stosb

    pop ecx
    pop edi
    pop eax
    ret 0xC
  };
#endif
}

void Mem::_replace(void *mem, SIZE_T size, char oldChar, char newChar)
{
  for(register SIZE_T i = 0; i < size; i++)if(((char *)mem)[i] == oldChar)((char *)mem)[i] = newChar;
}

void *Mem::_findData(const void *mem, SIZE_T memSize, void *data, SIZE_T dataSize)
{
  if(memSize >= dataSize)
  {
    memSize -= dataSize;

    for(register SIZE_T i = 0; i <= memSize; i++)
    {
      register LPBYTE p = (LPBYTE)mem + i;
      if(_compare(p, data, dataSize) == 0)return (void *)p;
    }
  }
  return NULL;
}

void Mem::_swap(void *mem1, void *mem2, SIZE_T size)
{
  register BYTE tmp;
  register LPBYTE b1 = (LPBYTE)mem1;
  register LPBYTE b2 = (LPBYTE)mem2;

  if(mem1 != mem2)while(size--)
  {
    tmp   = *b1;
    *b1++ = *b2;
    *b2++ = tmp;
  }
}

