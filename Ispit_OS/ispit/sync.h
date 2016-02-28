#pragma once
/*
  The feature set for synchronization between threads and processes.
*/

namespace Sync
{
  /*
    Waiting for mutex and its capture.

    IN mutexAttributes - SECURITY_ATTRIBUTES of mutex or NULL.
    IN name            - mutex name.

    Return             - mutex handle.

    Note: To release the mutex, you must call _freeMutex().
  */
  HANDLE _waitForMutex(SECURITY_ATTRIBUTES *mutexAttributes, LPWSTR name);

  /*
    The release of the allocated mutex through the WaitForMutex.

    IN mutex - mutex handle.
  */
  void _freeMutex(HANDLE mutex);

  /*
    Create a unique mutex.

    IN mutexAttributes - SECURITY_ATTRIBUTES of mutex or NULL.
    IN name            - mutex name.

    Return             - handle of the mutex, or NULL on error or if the mutex already exists.
  */
  HANDLE _createUniqueMutex(SECURITY_ATTRIBUTES *mutexAttributes, LPWSTR name);

};