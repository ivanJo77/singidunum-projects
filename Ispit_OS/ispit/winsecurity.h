/*
  Working with Security Descriptors of Windows.
*/
#pragma once

namespace WinSecurity
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
    Create SECURITY_ATTRIBUTES and SECURITY_DESCRIPTOR with full access.

    OUT sa - pointer to SECURITY_ATTRIBUTES, can be NULL, if meaning is not necessary.
    OUT sd - pointer to SECURITY_DESCRIPTOR.

	Return - In case of success: the address of the memory to be cleared through LocalFree, or (void *) -1.
			 in the case of an error: NULL.

    Note: sa and sd are linked.
  */
  void *_getFullAccessDescriptors(SECURITY_ATTRIBUTES *sa, SECURITY_DESCRIPTOR *sd);

  /*
    Setting the rights of object to full access to Low Integrity process.

    IN name       - object name.
    IN objectType - object type SE_OBJECT.
    IN inherit    - inheritance of child objects.
    
    Return        - true - in case of success,
                    false - in case of error.
  */
  bool _setLowIntegrityLevelLabel(LPWSTR name, DWORD objectType, bool inherit);

  /*
    Obtaining token of TOKEN_USER.

    IN token - token, requested permission TOKEN_QUERY.

    Return   - NULL - in case of error,
               address of the structure in case of success, must be released by Mem.
  */
  TOKEN_USER *_getUserByToken(HANDLE token);
};
