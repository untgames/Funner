#ifndef SYSLIB_WINDOWS_PLATFORM_HEADER
#define SYSLIB_WINDOWS_PLATFORM_HEADER

#include <shared/default_window_manager.h>
#include <shared/default_library_manager.h>
#include <shared/default_timer_manager.h>
#include <shared/default_application_manager.h>
#include <shared/default_thread_manager.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsWindowManager: public DefaultWindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������/��������/����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef void (*WindowMessageHandler)(window_t, WindowEvent, const WindowEventContext&, void* user_data);

    static window_t CreateWindow  (WindowStyle, WindowMessageHandler, const void* parent_handle, const char* init_string, void* user_data);
    static void     CloseWindow   (window_t);
    static void     DestroyWindow (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ����� ���� (����� ���� ���������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool ChangeWindowStyle (window_t, WindowStyle);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-���������� ����������� ���� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeWindowHandle  (window_t);
    static const void* GetNativeDisplayHandle (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowTitle (window_t, const wchar_t* title);
    static void GetWindowTitle (window_t, size_t buffer_size_in_chars, wchar_t* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowRect (window_t, const Rect&);
    static void SetClientRect (window_t, const Rect&);
    static void GetWindowRect (window_t, Rect&);
    static void GetClientRect (window_t, Rect&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowFlag (window_t, WindowFlag flag, bool state);
    static bool GetWindowFlag (window_t, WindowFlag flag);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        SetParentWindowHandle (window_t child, const void* parent_handle);
    static const void* GetParentWindowHandle (window_t child);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InvalidateWindow (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetCursorPosition (const Point& position);
    static Point GetCursorPosition ();
    static void  SetCursorPosition (window_t, const Point& client_position); //� ���������� ����������� ����
    static Point GetCursorPosition (window_t);                               //� ���������� ����������� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetCursorVisible (window_t, bool state);
    static bool GetCursorVisible (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static cursor_t CreateCursor  (const char* name, int hotspot_x, int hotspot_y); //hotspot_x/hotspot_y = -1 - default value
    static void     DestroyCursor (cursor_t);
    static void     SetCursor     (window_t window, cursor_t cursor);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� multitouch ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetMultitouchEnabled (window_t window, bool state);
    static bool IsMultitouchEnabled  (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetBackgroundColor (window_t window, const Color& color);
    static void  SetBackgroundState (window_t window, bool state);
    static Color GetBackgroundColor (window_t window);
    static bool  GetBackgroundState (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer); //������������ ����� ������ ��� ����� '\0'
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsThreadManager: public DefaultThreadManager
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
///��������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetThreadPriority (thread_t thread, ThreadPriority thread_priority);

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsLibraryManager: public DefaultLibraryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static dll_t LoadLibrary   (const wchar_t* name);
    static void  UnloadLibrary (dll_t);
    static void* GetSymbol     (dll_t, const char* symbol_name);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsTimerManager: public DefaultTimerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler, void* user_data);
    static void    KillTimer   (timer_t handle);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsApplicationManager: public DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� ���� �� miliseconds ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� URL �� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetEnvironmentVariable (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenSaverState (bool state);
    static bool GetScreenSaverState ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� Windows
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsScreenManager: public DefaultScreenManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreensCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t CreateScreen  (size_t screen_index);
    static void     DestroyScreen (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetScreenName (screen_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����-������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreenModesCount (screen_t);
    static void   GetScreenMode       (screen_t, size_t mode_index, ScreenModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����-������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenCurrentMode (screen_t, const ScreenModeDesc&);
    static void GetScreenCurrentMode (screen_t, ScreenModeDesc&);
    static void GetScreenDefaultMode (screen_t, ScreenModeDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����-���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenGammaRamp (screen_t, const Color3f table [256]);
    static void GetScreenGammaRamp (screen_t, Color3f table [256]);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t FindContainingScreen (const void* native_window_handle);    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-���������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeScreenHandle (screen_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetScreenProperties (screen_t, common::PropertyMap& properties);    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsPlatform
 : public WindowsWindowManager
 , public WindowsThreadManager
 , public WindowsLibraryManager
 , public WindowsTimerManager
 , public WindowsApplicationManager
 , public WindowsScreenManager
 , public DefaultSensorManager
{
};

}

#endif
