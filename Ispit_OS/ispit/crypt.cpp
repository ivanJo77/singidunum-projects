#include<WinSock2.h>
#include <windows.h>
#include <wincrypt.h>

#include"defines.h"

#include "crypt.h"
#include "mem.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
///////////////////////////////////////////////////////////////////////////////////////////////////
#if(1)
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
#define MT_RAND_MAX ((long)(0x7FFFFFFF)) /* (1 << 31) - 1 */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti;
static DWORD _last_rand_tickcount;

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
  mt[0]= s & 0xffffffffUL;
  for (mti = 1; mti < N; mti++)
  {
    mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
  unsigned long y;
  static unsigned long mag01[2] = {0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if(mti >= N)/* generate N words at one time */
  {
    int kk;

    for(kk = 0; kk < N - M; kk++)
    {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for(; kk < N - 1; kk++)
    {
      y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
      mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
  }

  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
static bool crc32Intalized;

void Crypt::init(void)
{
  _last_rand_tickcount = 0;
  crc32Intalized = false;
}

void Crypt::uninit(void)
{

}

DWORD Crypt::mtRand(void)
{
	DWORD c = CWA(kernel32, GetTickCount)();
	if(c != _last_rand_tickcount)
	{
		_last_rand_tickcount = c;
		init_genrand(c);
	}

	return genrand_int32();
}

DWORD Crypt::mtRandRange(DWORD minVal, DWORD maxVal)
{
  if(maxVal == minVal)return maxVal;
  register DWORD r = mtRand();
  register DWORD x = maxVal - minVal + 1;
  return minVal + (r - (r / x) * x);
}
