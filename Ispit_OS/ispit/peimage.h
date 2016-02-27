/*
  PE parser. 

  FIXME: Lost connection between the functions and logic.


*/
#pragma once

#include "defines.h"

#if !defined PEIMAGE_32
#  error PEIMAGE_32 not defined!
#endif

#if !defined PEIMAGE_64
#  error PEIMAGE_64 not defined!
#endif

namespace PeImage
{
  enum
  {
    MAX_FILE_ALIGMENT    = (64 * 1024), //Max. alignment for FileAligment.
    MIN_FILE_ALIGMENT    = 512,         //Min. alignment for FileAligment.
    MAX_VIRTUAL_ALIGMENT = (64 * 1024), //Max. alignment for SectionAligment.
    MIN_VIRTUAL_ALIGMENT = 512,         //Min. alignment for SectionAligment.
  };
  
  //Header of VS_VERSION_INFO. Based on VS_VERSIONINFO.
  typedef struct
  {
    WORD length;
    WORD valueLength;
    WORD type;
    WCHAR key[16];
    VS_FIXEDFILEINFO vsf;
  }VERSIONINFO_HEADER;

  typedef struct
  {
    WORD length; 
    WORD valueLength; 
    WORD type; 
    WCHAR key[1]; 
  }VERSIONINFO_VAR;

  //The structure for storing information about a file.
  typedef struct
  {
    WORD machine;                     //File type. IMAGE_FILE_MACHINE_I386/IMAGE_FILE_MACHINE_AMD64.
    IMAGE_DOS_HEADER *dosHeader;      //DOS-header.
    union
    {
#     if(PEIMAGE_32 > 0)
      IMAGE_NT_HEADERS32 *p32;         //NT32-header.
#     endif
#     if(PEIMAGE_64 > 0)
      IMAGE_NT_HEADERS64 *p64;         //NT64-header.
#     endif
    }ntHeader;

    IMAGE_DATA_DIRECTORY dataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; //..
    
    WORD sectionsCount;              //number of sections
    IMAGE_SECTION_HEADER *sections;  //Sections.
    LPBYTE *sectionsRawData;         //Content of sections.
    DWORD *sectionsFlags;            //Custom flags sections, not used by PeImage.
	DWORD minimalRawOffsetOfSection; //Minimum RAW position of the first section in the image. 0 or
									 //multiple FileAlignment.
  }PEDATA;

  //Baseline data for NT-header. All variables correspond to the variables in IMAGE_NT_HEADERS
  typedef struct
  {
    DWORD   timeDateStamp;
    WORD    characteristics;
    BYTE    majorLinkerVersion;
    BYTE    minorLinkerVersion;
    DWORD64 imageBase;
    DWORD   sectionAlignment;
    DWORD   fileAlignment;
    WORD    majorOperatingSystemVersion;
    WORD    minorOperatingSystemVersion;
    WORD    majorImageVersion;
    WORD    minorImageVersion;
    WORD    majorSubsystemVersion;
    WORD    minorSubsystemVersion;
    WORD    subsystem;
    WORD    dllCharacteristics;
    DWORD64 sizeOfStackReserve;
    DWORD64 sizeOfStackCommit;
    DWORD64 sizeOfHeapReserve;
    DWORD64 sizeOfHeapCommit;
  }PENTBASEDATA;
  
  enum
  {
    BIF_CHECKSUM      = 0x1, //calculate checksum.
    BIF_NO_RECALC_RVA = 0x2, //Do not count the virtual address, and all data associated with them.
							 //Use of this flag can cause damage to the image, if
							 //added new sections and data in existing section.
  };

  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Create empty image.

    OUT pedata - structure for storing image.
    IN machine - platform type. IMAGE_FILE_MACHINE_I386 or IMAGE_FILE_MACHINE_AMD64.
    
    Return     - true  - in case of success,
                 false - in case or error.
  */
  bool _createEmpty(PEDATA *pedata, WORD machine);

  /*
    Creating an image from memory. The data are checked for errors.

    OUT pedata  - structure for storing image.
    IN mem      - pointer to a source memory block.
    IN memSize  - memory size, you can specify MAXDWORD, but if the image is wrong, 
				  it can lead to the exclusion.
    IN isModule - the contents of memory is module.

    Return      - end address of the image - in case of success,
                  NULL - in case or error.
  */
  void *_createFromMemory(PEDATA *pedata, void *mem, DWORD memSize, bool isModule);

  /*
    Release image.

    IN pedata - image to release.
  */
  void _freeImage(PEDATA *pedata);

  /*
    Sets the DOS-header for the image.

    IN OUT pedata - image to process.
    IN dosHeader  - DOS-header, correct parameters must be specified and e_magic and e_lfanew.

    Return        - true  - in case of success,
                    false - in case or error.
  */
  bool _setDosHeader(PEDATA *pedata, void *dosHeader);

  /*
    Sets NT-header and sets the basic parameters.

    IN OUT pedata - image to process.
    IN basedata   - base data.

	Return        - true  - in case of success,
					false - in case or error.
  */
  bool _setNtHeader(PEDATA *pedata, PENTBASEDATA *basedata);

  /*
    Sets NT-header completely copying the original header. Also fills PEDATA.DataDirectory.

    IN OUT pedata - image to process.
	IN ntHeader   - pointer to IMAGE_NT_HEADERS32 or IMAGE_NT_HEADERS64. The header should be correct.

	Return        - true  - in case of success,
					false - in case or error.
  */
  bool _setNtHeaderFromNtHeader(PEDATA *pedata, void *ntHeader);

  /*
    Add section.

    IN OUT pedata      - image to process.
    IN name            - name of the section, not more characters than IMAGE_SIZEOF_SHORT_NAME.
    IN characteristics - analog to IMAGE_SECTION_HEADER.Characteristics.
    IN data            - contents of section.
    IN dataOffset      - position data in RAW (PointerToRawData). It makes no sense for _buildImage().
    IN dataSize        - size of pData. Automatically aligned FileAlignment during the build.
    IN virtualAddress  - RVA section, during the assembly does not make sense as it would be recalculated.
	IN virtualDataSize - virtual data size of section. Any number, may not be aligned to
                         SectionAligment. It is determined SizeOfInitializedData and 
						 SizeOfUninitializedData.
    IN flags           - custom flags section.
    
	Return			   - true  - in case of success,
						 false - in case or error.
  */
  bool _addSection(PEDATA *pedata, LPSTR name, DWORD characteristics, void *data, DWORD dataOffset, DWORD dataSize, DWORD virtualAddress, DWORD virtualDataSize, DWORD flags);

  /*
    Retrieve section index by it's name.

    IN pedata - image.
    IN name   - section name is case sensitive.

    Return    - -1 - if the section is not found, or an index in case of success.
  */
  int _getSectionIndexByName(PEDATA *pedata, LPSTR name);
  
  /*
    Setting data for DataDirectory.

    IN OUT pedata     - image to process.
    IN index          - one of the values ​​of IMAGE_DIRECTORY_ENTRY_*.
    IN virtualAddress - RVA-address.
    IN size           - virtual size.

	Return			  - true  - in case of success,
						false - in case or error.
  */
  bool _setDataDirectory(PEDATA *pedata, BYTE index, DWORD virtualAddress, DWORD size);

  /*
    The build of image. A method of assembling as close to the MS Linker 9.0.

    IN pedata          - image to process.
    IN flags           - flags BIF_*.
    IN rvaOfEntryPoint - RVA entry point. If 0, the value is taken from the pedata.
    OUT output         - buffer for build image. If is NULL, updated all the internal 
						 data pedata (size, RVA sections, etc.).

    Return             - 0 - If an error occurs, or the size of the image in case of success.
  */
  DWORD _buildImage(PEDATA *pedata, DWORD flags, DWORD rvaOfEntryPoint, LPBYTE *output);
  
  /*
    Counts RVA next section, excluding  IMAGE_SECTION_HEADER.PointerToRawData.

    IN pedata        - image to process.
    IN sectionsCount - the estimated number of sections in the image. 
					   it is important to ACCURETLY indicate this value.

    Return           - RVA sections if successful,
                       0 - in case of error.
  */
  DWORD _calcNextSectionRva(PEDATA *pedata, DWORD sectionsCount);

  /*
    Calculation of the current size of the RAW PE image.
    
    IN pedata - image to process.

    Return   - image size.
  */
  DWORD _getCurrentRawSize(PEDATA *pedata);

  /*
    Size calculation of the RAW image.
    
    IN image - image to process. The correctness of the image is not checked.

    Return  - image size.
  */
  DWORD _getRawSize(const void *image);
  
  /*
    Calculation of the current virtual image size, without IMAGE_SECTION_HEADER.VirtualAddress.

    IN pedata - image to process.

    Return   - image size.
  */
  DWORD _getCurrentVirtualSize(PEDATA *pedata);

  /*
    Retrieve a section of the loaded module.
  
    IN hModule      - module.
    IN wIndex       - section index.
    OUT virtualSize - virtual size of a section, may be NULL.

    Return          - NULL - in the case of an error, or the address of the section in case of success.

    Note: The correctness and availability of the module is not checked.
  */
  void *_getSectionOfModule(HMODULE module, WORD index, LPDWORD virtualSize);

  /*
    Retrieve section header by it's name.

    IN image       - image (RAW/VA).
    IN sectionName - name of section to search for.

    Return         - address of the section header, or NULL - in the case of an error.
  */
  IMAGE_SECTION_HEADER *_getSectionByName(const void *image, const LPSTR sectionName);

  /*
    Copy the image in another process with application of relocations. Function facilitates and ignores
    correctness of pages, as all the original pages should be readable, otherwise the function
    will return an error.

    IN process - process into which will be copied. Requested rights
                 PROCESS_VM_OPERATION | PROCESS_VM_WRITE
    IN image   - image to copy, in the current process.

    Return     - address of the image in the process, or NULL on error.
  */
  void *_copyModuleToProcess(HANDLE process, void *image);

	  /*
    Copy the image in another process with application of relocations. Function facilitates and ignores
    correctness of pages, as all the original pages should be readable, otherwise the function
    will return an error.
		Permits to have relocation table (PE .reloc) section elsewhere in RAM

		IN process		- process into which will be copied. Requested rights
										PROCESS_VM_OPERATION | PROCESS_VM_WRITE
		IN image			- image to copy, in the current process.
		IN relocsDir	- pointer to relocs data directory
		IN relHdrSrc	- pointer to base relocations structure
		IN iatDir			- pointer to IAT data directory
		IN iatDetail	- pointer to IAT detail table
		
		Return				- address of the image in the process, or NULL on error.
  */
  void *_copyModuleToProcessEx(HANDLE process, void *image, IMAGE_DATA_DIRECTORY *relocsDir, DWORD relocDelta, IMAGE_BASE_RELOCATION *relHdrSrc);

  /*
    Load import table.

    IN image          - image, in which the changes will be made.
    IN loadLibraryA   - pointer to the function LoadLibraryA.
    IN getProcAddress - pointer to the function GetProcAddress.

	Return			  - true  - in case of success,
						false - in case or error.
  */
  bool _loadImport(void *image, void *loadLibraryA, void *getProcAddress);

  /*
    Change addresses in import table.

    IN image       - image, in which the changes will be made.
    IN oldFunction - address of old function.
    IN newFunction - address of new function.

	Return		   - true  - in case of success,
					 false - in case or error.
  */
  bool _repalceImportFunction(void *image, const void *oldFunction, const void *newFunction);

  /*
    Conversion RVA to RSA.

    IN pedata       - image to process.
    IN rva          - RVA.
	IN sectionIndex - index of section which owns the address. Or NULL if the section isn't	required.

    Return          - address relative to section sectionIndex, or 0 if an error occurs.
  */
  DWORD _rvaToRsa(PEDATA *pedata, DWORD rva, LPWORD sectionIndex);

  /*
    Checks if the PE image is correct.

    IN mem     - image.
    IN memSize - image size.

	Return	   - true  - in case of success,
				 false - in case or error.
  */
  bool _isPeImage(void *mem, DWORD memSize);
};
