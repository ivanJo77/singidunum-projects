#pragma once
/*
  Mathematic functions.
*/

namespace Math
{
  /*
    Init.
  */
  void Init(void);

  /*
    Uninit.
  */
  void Uninit(void);

#if !defined _WIN64

  /*
    The division of the 64-bit unsigned number (Only available in Win32).

	IN dwA - dividend.
	IN dwB - divisor.

	Return - result.
  */
  DWORD64 ASM_INTERNAL_DEF _divU64(DWORD64 dwA, DWORD64 dwB);

  /*
    Retrieve a division reminder of unsigned 64-bit integers

	IN dwA - dividend.
	IN dwB - divisor.

	Return - result.
  */
  DWORD64 ASM_INTERNAL_DEF _modU64(DWORD64 dwA, DWORD64 dwB);

  /*
    Multiply 64-bit signed numbers (Only available in Win32).

    IN dwA - multiplicand.
    IN dwB - factor.

    Return - result.
  */
  DWORD64 ASM_INTERNAL_DEF _mul64(DWORD64 dwA, DWORD64 dwB);

  /*
    The right shift of 64-bit number (Only available in Win32).

    IN number - number.
    IN bits   - bits to shift.

    Return    - result.
  */
  DWORD64 ASM_INTERNAL_DEF _shr64(DWORD64 number, BYTE bits);

#endif

  /*
    Divide a fraction by a whole and fractional part.

    IN d   - starting number.
    OUT ip - integral part of.

    Return - fractional part.
  */
  double _modf(double d, double *ip);

  
};
