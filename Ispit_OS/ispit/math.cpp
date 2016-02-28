#include <WTypes.h>

#include "defines.h"
#include "defines.h"
#include "mem.h"
#include "math.h"

#if BO_DEBUG>0
extern "C" int _fltused=0;
#endif
typedef union
{
  double  d;
  struct
  {
    long ls;
    long ms;
  };
}DOUBLEDATA;

void Math::Init(void)
{

}

void Math::Uninit(void)
{

}

#if !defined _WIN64

DWORD64 ASM_INTERNAL Math::_divU64(DWORD64 dwA, DWORD64 dwB)
{
  __asm
  {
    push ebx
    push esi

    mov  eax, dword ptr[esp + 0x18]
    or   eax, eax
    jnz  L1
    mov  ecx, dword ptr[esp + 0x14]
    mov  eax, dword ptr[esp + 0x10]
    xor  edx, edx
    div  ecx
    mov  ebx, eax
    mov  eax, dword ptr[esp + 0x0C]
    div  ecx
    mov  edx, ebx
    jmp  L2
L1:
    mov  ecx, eax
    mov  ebx, dword ptr[esp + 0x14]
    mov  edx, dword ptr[esp + 0x10]
    mov  eax, dword ptr[esp + 0x0C]
L3:
    shr  ecx, 1
    rcr  ebx, 1
    shr  edx, 1
    rcr  eax, 1
    or   ecx, ecx
    jnz  L3
    div  ebx
    mov  esi, eax

    mul  dword ptr[esp + 0x18]
    mov  ecx, eax
    mov  eax, dword ptr[esp + 0x14]
    mul  esi
    add  edx, ecx
    jc   L4

    cmp  edx, dword ptr[esp + 0x10]
    ja   L4
    jb   L5
    cmp  eax, dword ptr[esp + 0x0C]
    jbe  L5
L4:
    dec  esi
L5:
    xor  edx, edx
    mov  eax, esi
L2:
    pop  esi
    pop  ebx
    ret  0x10
  }
}

DWORD64 ASM_INTERNAL Math::_modU64(DWORD64 dwA, DWORD64 dwB)
{
  __asm
  {
    push ebx
    
    mov  eax, dword ptr[esp + 0x14]
    or   eax, eax
    jnz  L1
    mov  ecx, dword ptr[esp + 0x10]
    mov  eax, dword ptr[esp + 0x0C]
    xor  edx, edx
    div  ecx
    mov  eax, dword ptr[esp + 0x08]
    div  ecx
    mov  eax, edx
    xor  edx, edx
    jmp  L2

  L1:
    mov  ecx, eax
    mov  ebx, dword ptr[esp + 0x10]
    mov  edx, dword ptr[esp + 0x0C]
    mov  eax, dword ptr[esp + 0x08]

  L3:
    shr  ecx, 1
    rcr  ebx, 1
    shr  edx, 1
    rcr  eax, 1
    or   ecx, ecx
    jnz  L3
    div  ebx

    mov  ecx, eax
    mul  dword ptr[esp + 0x14]
    xchg ecx, eax
    mul  dword ptr[esp + 0x10]
    add  edx, ecx
    jc   L4

    cmp  edx, dword ptr[esp + 0x0C]
    ja   L4
    jb   L5
    cmp  eax, dword ptr[esp + 0x08]
    jbe  L5

  L4:
    sub  eax, dword ptr[esp + 0x10]
    sbb  edx, dword ptr[esp + 0x14]

  L5:
    sub  eax, dword ptr[esp + 0x08]
    sbb  edx, dword ptr[esp + 0x0C]
    neg  edx
    neg  eax
    sbb  edx, 0

  L2:
    pop  ebx
    ret  0x10
  }
}

DWORD64 ASM_INTERNAL Math::_mul64(DWORD64 dwA, DWORD64 dwB)
{
  __asm
  {
    mov  eax, dword ptr[esp + 0x08]
    mov  ecx, dword ptr[esp + 0x10]
    or   ecx, eax
    mov  ecx, dword ptr[esp + 0x0C]
    
    jnz  hard

    mov  eax, dword ptr[esp + 0x04]
    mul  ecx

    ret  0x10

  hard:
    push ebx

    mul  ecx
    mov  ebx, eax

    mov  eax, dword ptr[esp + 0x08]
    mul  dword ptr[esp + 0x14]
    add  ebx, eax

    mov  eax, dword ptr[esp + 0x08]
    mul  ecx
    add  edx, ebx

    pop  ebx
    ret  0x10
  }
}

#endif

double Math::_modf(double d, double *ip)
{
  DOUBLEDATA x;
  int e;

  if(d < 1)
  {
    if(d < 0)
    {
      x.d = _modf(-d, ip);
      *ip = -*ip;
      return -x.d;
    }

    *ip = 0;
    return d;
  }
  
  x.d = d;
  e = ((x.ms >> 20) & 0x7FFL) - 1022L;
  if(e <= 20 + 1)
  {
    x.ms &= ~(0x1FFFFFL >> e);
    x.ls = 0;
  }
  else if(e <= 20 + 33)x.ls &= ~(0x7FFFFFFFL >> (e - 20 - 2));
  
  *ip = x.d;
  return d - x.d;
}



