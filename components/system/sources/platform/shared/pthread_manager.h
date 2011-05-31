#ifndef SYSLIB_PTHREAD_MANAGER_HEADER
#define SYSLIB_PTHREAD_MANAGER_HEADER

#include <shared/default_thread_manager.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� PThread
///////////////////////////////////////////////////////////////////////////////////////////////////
class PThreadManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static thread_t CreateThread  (IThreadCallback*);
    static void     DestroyThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void JoinThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetThreadId        (thread_t thread);
    static size_t GetCurrentThreadId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static tls_t CreateTls  (IThreadCleanupCallback* cleanup);
    static void  DestroyTls (tls_t tls);
    static void  SetTls     (tls_t tls, void* data);
    static void* GetTls     (tls_t tls);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static mutex_t CreateMutex  ();
    static void    DestroyMutex (mutex_t);
    static void    LockMutex    (mutex_t);
    static bool    LockMutex    (mutex_t, size_t wait_in_milliseconds);
    static bool    TryLockMutex (mutex_t);
    static void    UnlockMutex  (mutex_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static semaphore_t CreateSemaphore   (size_t initial_count);
    static void        DestroySemaphore  (semaphore_t);
    static void        WaitSemaphore     (semaphore_t);
    static bool        WaitSemaphore     (semaphore_t, size_t wait_in_milliseconds);
    static bool        TryWaitSemaphore  (semaphore_t);
    static void        PostSemaphore     (semaphore_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static condition_t CreateCondition    ();
    static void        DestroyCondition   (condition_t);
    static void        WaitCondition      (condition_t, mutex_t);
    static bool        WaitCondition      (condition_t, mutex_t, size_t wait_in_milliseconds);
    static void        NotifyCondition    (condition_t, bool broadcast);  
};

}

#endif