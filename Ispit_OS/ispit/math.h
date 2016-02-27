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
    The division of the 64-bit signed number (Only available in Win32).

    IN dwA - dividend.
    IN dwB - divisor.

    Return - result.
  */
  __int64 ASM_INTERNAL_DEF _divI64(__int64 dwA, __int64 dwB);

  /*
    The division of the 64-bit unsigned number (Only available in Win32).

	IN dwA - dividend.
	IN dwB - divisor.

	Return - result.
  */
  DWORD64 ASM_INTERNAL_DEF _divU64(DWORD64 dwA, DWORD64 dwB);

  /*
    Retrieve a division reminder of signed 64-bit integers

	IN dwA - dividend.
	IN dwB - divisor.

	Return - result.
  */
  __int64 ASM_INTERNAL_DEF _modI64(__int64 dwA, __int64 dwB);

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

  /*
    Convert float to int32 (Available in Win32).

    IN f   - float.
    Return - int32.
  */
  int ASM_INTERNAL_DEF _ftoi(float f);
#endif

  /*
    Returns the absolute value of a 32 bit number.

    IN iNumber - number.

    Return - absolute value.
  */
  int _abs(int iNumber);

  /*
    Returns the absolute value of a 64 bit number.

    IN iNumber - number.

    Return - absolute value.
  */
  __int64 _abs64(__int64 iNumber);

  /*
    Divide a fraction by a whole and fractional part.

    IN d   - starting number.
    OUT ip - integral part of.

    Return - fractional part.
  */
  double _modf(double d, double *ip);

  /*
    Rounds a shot to the lower.

    IN d - number.

    Return - result.
  */
  double _floor(double d);

  /*
    Round the number towards bigger.

    IN d - number.

    Return - result.
  */
  double _ceil(double d);

  /*
    Function to compare two elements in the array.

    IN p1  - first element.
    IN p2  - second element.

    Return -  > 0 - element p1 greater than p2.
              = 0 - elements are equal.
              < 0 - element p1 lesser than p2.
  */
  typedef int (*qsortProc)(const void *p1, const void *p2);

  /*
    Quick sort algorithm for an array of x.

    IN pBase   - the base address of the array.
    IN dwCount - the number of elements in the array.
    IN dwWidth - width of single element.
    IN pComp   - compare function.
  */
  void _qsort(void *pBase, DWORD dwCount, DWORD dwWidth, qsortProc pComp);
};
