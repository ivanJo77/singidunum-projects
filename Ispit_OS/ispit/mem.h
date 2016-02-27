/*
  Working with memory Windows 32/64 based on the Heap.

  Note: These functions are primarily designed for stability, so have the following
		properties:
			1. Allocates a block of memory required is filled with zeros.
*/

#pragma once

#if !defined MEM_PERSONAL_HEAP
#  error MEM_PERSONAL_HEAP not defined!
#endif

#if !defined MEM_ALLOC_SAFE_BYTES
#  error MEM_ALLOC_SAFE_BYTES not defined!
#endif

//Structure to specify the parameters of the memory storage
typedef struct
{
  void *data;
  SIZE_T size;
}MEMDATA;

//structure offset.
#define OFFSETOF(v, m)  ((DWORD_PTR)(&((v *)(NULL))->m))

//alignment.
#define ALIGN_DOWN(x, align)  ((x) & ~(align - 1))
#define ALIGN_UP(x, align) (((x) & (align - 1)) ? ALIGN_DOWN(x, align) + align : (x))

//The size of the virtual memory pages.
#define VM_PAGE_SIZE 4096

#define VM_STEP_MASK (~0xFFFF)
#define VM_STEP      0x10000

namespace Mem
{
  /*
    Initialization, the creation of the heap.

    IN initialSize - initial heap size.
  */
#if(MEM_PERSONAL_HEAP == 1)
  void init(SIZE_T initialSize);
#else
  void init(void);
#endif

  /*
    Uninit.
  */
  void uninit(void);
  
  /*
    Change memory heap size.

    IN OUT old - a memory unit for to change, or NULL to create a new (equivalent to alloc).
    IN size    - new block size.

    Return     - true - If successful, the memory allocation,
                 false - in the case of an error, and the old block of memory is not freed.
  */
  bool reallocEx(void *old, SIZE_T size);

  /*
    Change memory heap size.

	IN old  - a memory unit for to change, or NULL to create a new (equivalent to alloc).
	IN size    - new block size.

    Return  - address of new block, or NULL in case of error.
  */
  void *realloc(void *old, SIZE_T size);

  /*
    Allocate memory block.

    IN size - block size.

    Return  - address of new block, or NULL in case of error.
  */
  void *alloc(SIZE_T size);

  /*
    Allocate memory, without reset of the block.

    IN size - block size.

    Return  - address of new block, or NULL in case of error.
  */
  void *quickAlloc(SIZE_T size);

  /*
    Release of memory block.

    IN mem - memory block to release.
  */
  void free(void *mem);

  /*
    Freeing a block of memory with pre-zeroing.

    IN mem  - a memory block for liberation, can not be NULL.
    IN size - the number of bytes to be cleared.
  */
  void zeroAndFree(void *mem, SIZE_T size);

  /*
    Freeing memory at the addresses listed in the array. and the release of the memory array itself

    IN mem   - array to free.
    IN count - number of items in mem
  */
  void freeArrayOfPointers(void *mem, SIZE_T count);

  /*
    Copy a block of memory.

    OUT dest  - memory address in which to copy bytes.
    IN source - source memory address.
    IN size   - number of bytes to copy.
  */
  void ASM_INTERNAL_DEF _copy(void *dest, const void *source, SIZE_T size);

  /*
    Copy a block of memory from the end of the memory block.

	OUT dest  - memory address in which to copy bytes.
	IN source - source memory address.
	IN size   - number of bytes to copy.
  */
  void _copyFromEnd(void *dest, const void *source, SIZE_T size);

  /*
    Copy a block of memory, wrapper for Copy.

	OUT dest  - memory address in which to copy bytes.
	IN source - source memory address.
	IN size   - number of bytes to copy.

    Return    - returns dest + size.
  */
  void *_copy2(void *dest, const void *source, SIZE_T size);

  /*
    Copy a block of memory with automatic memory allocation for a copy.

	IN source - source memory address.
	IN size   - number of bytes to copy.

    Return    - address of copied memory, or NULL in case of errors.
  */
  void *copyEx(const void *source, SIZE_T size);

  /*
    Comparison of memory blocks.

    IN mem1  - first memory block address.
    IN mem2  - second memory block address.
    IN size  - the number of bytes to compare.

    Return    -  <0 - mem1 less than mem2
                 =0 - mem1 equals to mem2
                 >0 - mem1 greater than mem2
  */
  int _compare(const void *mem1, const void *mem2, SIZE_T size);

  /*
    Reset to 0 the memory block.

    IN mem  - address of memory block to erase.
    IN size - the number of bytes to erase.
  */
  void _zero(void *mem, SIZE_T size);

  /*
    Filling a block of memory with character.

    IN mem  - address of memory block to fill.
    IN size - the number of bytes to fill.
  */
  void ASM_INTERNAL_DEF _set(void *mem, char c, SIZE_T size);

  /*
    Look for a character in a block of memory from left to right.

    IN mem  - memory block to be searched.
    IN c    - searching character.
    IN size - the number of bytes to search.

    Return  - If successful, a pointer to a character,
              in the case of an error NULL.
  */
  void *_getL(void *mem, char c, SIZE_T size);

  /*
    Look for a character in a block of memory from right to left.

    IN mem  - memory block to be searched.
    IN c    - searching character.
    IN size - the number of bytes to search.

    Return  - If successful, a pointer to a character,
			  in the case of an error NULL.
  */
  void *_getR(void *mem, char c, SIZE_T size);

  /*
    Substitutes one byte to another memory block.
  
    IN mem     - the memory block to be searched.
    IN size    - the number of bytes in the block mem.
    IN oldChar - char to search for.
    IN newChar - replacement char.
  */
  void _replace(void *mem, SIZE_T size, char oldChar, char newChar);

  /*
    Search for data in the memory block.

    IN mem      - pointer to the memory block.
    IN memSize  - size of mem block.
    IN data     - data block to search for.
    IN dataSize - size of data block.

    Return       - a pointer to the first occurrence of data block, or NULL if not found.
  */
  void *_findData(const void *mem, SIZE_T memSize, void *data, SIZE_T dataSize);

  /*
    Data exchange of two addresses.

    IN mem1 - first address.
    IN mem2 - second address.
    IN size - the number of bytes to swap.
  */
  void _swap(void *mem1, void *mem2, SIZE_T size);

  /*
    Replacement of DWORD values.

    IN originalValue - original value.
    IN newValue      - new value.
    IN OUT mem       - memory block to process.
    IN memSize       - size of mem block.

    Return           - number of changes performed.
  */
  SIZE_T _replaceDword(DWORD originalValue, DWORD newValue, void *mem, SIZE_T memSize);

  /*
    Replacement of QDWORD (DWORD64) values.

	IN originalValue - original value.
	IN newValue      - new value.
	IN OUT mem       - memory block to process.
	IN memSize       - size of mem block.

	Return           - number of changes performed.
  */
  SIZE_T _replaceQword(DWORD64 originalValue, DWORD64 newValue, void *mem, SIZE_T memSize);
};
