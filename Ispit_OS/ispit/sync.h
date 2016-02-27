#pragma once
/*
  The feature set for synchronization between threads and processes.
*/

namespace Sync
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
    Waiting for mutex and its capture.

    IN mutexAttributes - SECURITY_ATTRIBUTES of mutex or NULL.
    IN name            - mutex name.

    Return             - mutex handle.

    Note: To release the mutex, you must call _freeMutex().
  */
  HANDLE _waitForMutex(SECURITY_ATTRIBUTES *mutexAttributes, LPWSTR name);

  /*
    Waiting for the event, and all the window message processing threads.

    IN count        - number of handles.
    IN handles      - handles.
    IN waitAll      - wait for all objects.
    IN milliseconds - timeout in milliseconds, INFINITE for endless waiting.

    Return          - according to WaitForMultipleObjects().
  */
  DWORD _waitForMultipleObjectsAndDispatchMessages(DWORD count, const HANDLE* handles, bool waitAll, DWORD milliseconds);

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

  /*
    Checks whether the mutex exists.

    IN name - mutex name.

    Return  - true - exists,
              false - does not exists.
  */
  bool _mutexExists(LPWSTR name);
};