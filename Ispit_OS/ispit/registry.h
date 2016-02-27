/*
  Work with Windows registry.
*/
#pragma once

#define REGISTRY_MAX_VALUE_DATA_SIZE (1 * 1024 * 1204)

namespace Registry
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
    Retrieves the value of the registry as a string. Also handles the REG_EXPAND_SZ.

    IN key        - key.
	IN subKey     - sub key, if the value is NULL, the value is requested for
					the current key.
    IN value      - name of the value.
    IN buffer     - buffer.
    IN bufferSize - buffer size, in characters, including the null.

	Return        - size of the string, in characters, excluding the null character,
				   (DWORD)-1 - in the case of an error.
  */
  DWORD _getValueAsString(HKEY key, const LPWSTR subKey, const LPWSTR value, LPWSTR buffer, DWORD bufferSize);
  
  /*
    Setting the value of the registry as a string.

	IN key        - key.
	IN subKey     - sub key, if the value is NULL, the value is set for	the current key.
	IN value      - name of the value.
	IN buffer     - buffer.
	IN bufferSize - buffer size, in characters, including the null,

    Return        - true - in case of success,
                    false - in case of error.
  */
  bool _setValueAsString(HKEY key, const LPWSTR subKey, const LPWSTR value, const LPWSTR buffer, DWORD bufferSize);

  /*
    Retrieve the value of the registry as a DWORD.

	IN key		- key.
	IN subKey   - sub key, if the value is NULL, the value is retrieved for the current key.
	IN value    - name of the value.

    Return		- value, or 0 in case of error.
  */
  DWORD _getValueAsDword(HKEY key, const LPWSTR subKey, LPWSTR value);

  /*
    Set the value of the registry as a DWORD.
  
    IN key      - key.
    IN subKey   - sub key, if the value is NULL, the value is set for	the current key.
    IN value    - name of the value.
    IN data     - value to set.

	Return      - true - in case of success,
				  false - in case of error.
  */
  bool _setValueAsDword(HKEY key, const LPWSTR subKey, const LPWSTR value, DWORD data);

  /*
    Retrieve the value of the registry as binary data.

	IN key        - key.
	IN subKey     - sub key, if the value is NULL, the value is retrieved for the current key.
	IN value      - name of the value.
    OUT type      - the type of value REG_*, may be NULL.
    OUT buffer    - buffer.
    IN bufferSize - buffer size.

    Return        - data size, or (DWORD)-1 - in case of error.
  */
  DWORD _getValueAsBinary(HKEY key, const LPWSTR subKey, const LPWSTR value, LPDWORD type, void *buffer, DWORD bufferSize);

  /*
    Set the value of the registry as binary data.
  
    IN key        - key.
    IN subKey     - sub key, if the value is NULL, the value is set for the current key.
    IN value      - name of the value.
    IN type       - the type of value REG_*, may be NULL.
    IN buffer     - buffer.
    IN bufferSize - buffer size.

	Return        - true - in case of success,
					false - in case of error.
  */
  bool _setValueAsBinary(HKEY key, const LPWSTR subKey, const LPWSTR value, DWORD type, const void *buffer, DWORD bufferSize);
    
  /*
    Retrieve the value of the registry as a data with memory allocation.

	IN key     - key.
	IN subKey  - sub key, if the value is NULL, the value is retrieved for the current key.
	IN value   - name of the value.
    OUT type   - the type of value REG_*, may be NULL.
    OUT buffer - buffer for the value, to be released through the Mem.

    Return     - data size, or (DWORD)-1 - in case of error.
  */
  DWORD _getValueAsBinaryEx(HKEY key, const LPWSTR subKey, const LPWSTR value, LPDWORD type, void **buffer);
  
  /*
    Retrieve the value of the registry as a CRC32. requires Crypt.

	IN key    - key.
	IN subKey - sub key, if the value is NULL, the value is retrieved for the current key.
	IN value  - name of the value.

    Return   - CRC32, or 0 in the case of an error or an empty string.
  */
  DWORD _getsCrc32OfValue(HKEY key, const LPWSTR subKey, const LPWSTR value);

  /*
    Delete key with subkeys.
    
    IN key    - key.
    IN subKey - subkey to remove (can not be NULL).

	Return    - true - in case of success,
				false - in case of error.
  */
  bool _deleteKey(HKEY key, const LPWSTR subKey);

  /*
    Delete value from the registry.
    
	IN key    - key.
	IN subKey - sub key, if the value is NULL, the value is under the current key.
	IN value  - name of the value to delete.

	Return    - true - in case of success,
				false - in case of error.
  */
  bool _deleteValue(HKEY key, const LPWSTR subKey, const LPWSTR value);

  /*
    Checks whether the value exists

	IN key    - key.
	IN subKey - sub key, if the value is NULL, the value is under the current key.
	IN value  - name of the value to check.

	Return    - true - in case of success,
				false - in case of error.
  */
  bool _valueExists(HKEY key, const LPWSTR subKey, const LPWSTR value);
};
