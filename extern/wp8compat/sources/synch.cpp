#include <windows.h>

extern "C"
{

HANDLE WINAPI CreateMutexA (LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
{
  return CreateMutexExA (lpMutexAttributes, lpName, bInitialOwner ? CREATE_MUTEX_INITIAL_OWNER : 0, MUTEX_ALL_ACCESS);
}

DWORD WINAPI WaitForSingleObject (HANDLE hHandle, DWORD dwMilliseconds)
{
  return WaitForSingleObjectEx (hHandle, dwMilliseconds, FALSE);
}

DWORD WINAPI WaitForMultipleObjects (DWORD nCount, const HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
{
  return WaitForMultipleObjectsEx (nCount, lpHandles, bWaitAll, dwMilliseconds, FALSE);
}

HANDLE WINAPI CreateEventA (LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
  DWORD flags = 0;

  if (bManualReset)  flags |= CREATE_EVENT_MANUAL_RESET;
  if (bInitialState) flags |= CREATE_EVENT_INITIAL_SET;

  return CreateEventExA (lpEventAttributes, lpName, flags, EVENT_ALL_ACCESS);
}

BOOL WINAPI PulseEvent (HANDLE hEvent)
{
  if (!SetEvent (hEvent))
    return FALSE;

  if (!ResetEvent (hEvent))
    return FALSE;

  return TRUE;
}

}
