#pragma once
/*
  Cryptography.
*/

namespace Crypt
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
    The pseudo-random number generator according to the method Mersenne Twister.

    Return - number of 0 to 0x7FFFFFFF.
  */
  DWORD mtRand(void);

  /*
    Expanding MTRand, you can specify the interval.

    IN minVal - minimum number.
    IN maxVal - maximum number.
              
    Return    - number from minVal to maxVal.
  */
  DWORD __inline mtRandRange(DWORD minVal, DWORD maxVal);
};
