#pragma once
/*
  Cryptography.
*/

//MD5 hash size.
#define MD5HASH_SIZE 16

namespace Crypt
{
  //Data RC4.
  typedef struct
  {      
    BYTE state[256];       
    BYTE x;        
    BYTE y;
  }RC4KEY;
  
  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Create MD5 hash of the data.

    OUT output   - buffer for hash size MD5HASH_SIZE.
    IN inputData - data, which need to calculate the hash.
    IN dataSize  - number of bytes in inputData.

	Return            - true, if was successful,
						false, in a case of error. 
  */
  bool _md5Hash(LPBYTE output, void *inputData, DWORD dataSize);

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

  /*
    Calculate CRC32.

    IN data - data, which need to calculate the hash.
    IN size - number of bytes in data.

    Return  - hash.

	Note: The first time you call the function creates a table crc32, the creation of which is not
			protected of multithreading.
  */
  DWORD crc32Hash(const void *data, DWORD size);

  /*
    Initialize the current RC4 key.

    IN key        - input binary key.
    IN binKeySize - binary key size in bytes.
    OUT key       - data for RC4.
  */
  void _rc4Init(const void *binKey, WORD binKeySize, RC4KEY *key);
  
  /*
    Encrypt/Decrypt RC4.

    IN OUT buffer - data to process
    IN size       - data size.
    IN key        - structure initialized by RC4Init.
  */
  void _rc4(void *buffer, DWORD size, RC4KEY *key);

  /*
    Combining rc4Init() and rc4() into a single function.

    IN binKey     - input binary key.
    IN binKeySize - binary key size in bytes.
    IN OUT buffer - data to process
    IN size       - data size.
  */
  void _rc4Full(const void *binKey, WORD binKeySize, void *buffer, DWORD size);

  /*
    Visual encryption buffer. The key for graphical first byte.

    IN OUT buffer - data to process
    IN size       - data size.
  */
  void _visualEncrypt(void *buffer, DWORD size);

  /*
    Visual decryption buffer. The key for graphical first byte.

	IN OUT buffer - data to process
	IN size       - data size.
  */
  void _visualDecrypt(void *buffer, DWORD size);

  /*
    Encoding data in base64.

    IN source     - data to process
    IN sourceSize - data size.
    OUT destSize  - size of the resulting base64 string without the null character. Can be NULL.

	Return        - string with zero at the end, or NULL on failure. The string must
					be destroyed by Mem.
  */
  LPSTR _base64Encode(LPBYTE source, SIZE_T sourceSize, SIZE_T *destSize);

  /*
    decoding base64 data into string.
    
	IN source     - data to process
	IN sourceSize - data size.
    OUT destSize  - the size of the data obtained without the zero at the end. Can be NULL.

	Return        - data with a null character at the end, or NULL on failure. The string must
					be destroyed by Mem.
  */
  LPBYTE _base64Decode(LPSTR source, SIZE_T sourceSize, SIZE_T *destSize);

  /*
    Generating data of predetermined length.

    OUT buffer  - a data buffer.
    IN size     - the number of bytes to generate.
    IN minValue - the minimum value of the byte.
    IN maxValue - the maximum value of a byte.
	OUT extMode - true - Detailed mode to use, improves the algorithm, but slows down to
						~ (dwSize * 20) ms.
				  false - the standard mode.
  */
  void _generateBinaryData(void *buffer, DWORD size, BYTE minValue, BYTE maxValue, bool extMode);

  /*
    RC4 key generation.

    OUT key - key.
  */
  void _generateRc4Key(Crypt::RC4KEY *key);

  /*
	apply ROT47 algorithm for character encoding/decoding.

	IN chr      - character to encode/decode.
	Returns encoded/decoded character.
  */
  BYTE Rot47(BYTE chr);

  /*
	apply ROR13 hashing algorithm .

	IN input		- byte array to hash.
	IN size			- size of array
	Returns hash of requested buffer.
  */
  DWORD32 Ror13(BYTE input[],int size);
};
