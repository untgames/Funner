#ifndef COMMONLIB_PLATFORM_HEADER
#define COMMONLIB_PLATFORM_HEADER

#include <cstddef>

namespace common
{

//implementation forwards
class ICustomAllocator;
class ICustomFileSystem;

/*
    Платформо-зависимые интерфейсы
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultPlatform
{
  public:
    typedef void* lockable_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системного распределителя памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomAllocator* GetSystemAllocator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение файловых систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();
    static ICustomFileSystem* GetIOSystem   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системного времени
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetMilliseconds ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с блокировками
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора текущей нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef size_t threadid_t;
    
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа Win32
///////////////////////////////////////////////////////////////////////////////////////////////////
class Win32Platform: public DefaultPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение файловых систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с блокировками
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора текущей нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unistd
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdPlatform: public DefaultPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение файловых систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с блокировками
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора текущей нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системного времени
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetMilliseconds ();    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Android
///////////////////////////////////////////////////////////////////////////////////////////////////
class AndroidPlatform: public UnistdPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cocoa
///////////////////////////////////////////////////////////////////////////////////////////////////
class CocoaPlatform: public UnistdPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение файловых систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системного времени
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetMilliseconds ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cocoa Desktop
///////////////////////////////////////////////////////////////////////////////////////////////////
class CocoaDesktopPlatform: public CocoaPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cocoa iOS
///////////////////////////////////////////////////////////////////////////////////////////////////
class CocoaIPhonePlatform: public CocoaPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///TabletOS
///////////////////////////////////////////////////////////////////////////////////////////////////
class TabletOsPlatform: public UnistdPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация путей монтирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Указание текущей платформы
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined _WIN32
  typedef Win32Platform Platform;
#elif defined IPHONE
  typedef CocoaIPhonePlatform Platform;
#elif defined __APPLE__
  typedef CocoaDesktopPlatform Platform;
#elif defined TABLETOS
  typedef TabletOsPlatform Platform;
#elif defined BEAGLEBOARD
  typedef UnistdPlatform Platform;
#elif defined ANDROID
  typedef AndroidPlatform Platform;  
#elif defined __GNUC__
  typedef UnistdPlatform Platform;
#else
  #error Unknown compiler
#endif

}

#endif
