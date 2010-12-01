#ifndef COMMONLIB_PLATFORM_HEADER
#define COMMONLIB_PLATFORM_HEADER

#include <cstddef>

namespace common
{

//implementation forwards
class ICustomAllocator;
class ICustomFileSystem;

/*
    ���������-��������� ����������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultPlatform
{
  public:
    typedef void* lockable_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomAllocator* GetSystemAllocator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();
    static ICustomFileSystem* GetIOSystem   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetMilliseconds ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef size_t threadid_t;
    
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� Win32
///////////////////////////////////////////////////////////////////////////////////////////////////
class Win32Platform: public DefaultPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� ������������
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
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitLockable    (lockable_t&);
    static void DestroyLockable (lockable_t&);
    static void Lock            (lockable_t&);
    static void Unlock          (lockable_t&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetMilliseconds ();    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cocoa
///////////////////////////////////////////////////////////////////////////////////////////////////
class CocoaPlatform: public UnistdPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ICustomFileSystem* GetFileSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �������
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
///����������� ����� ������������
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
///����������� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void MountSystemSpecificPaths ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef BADA_DEVICE
  typedef UnistdPlatform Platform;
#elif defined _WIN32
  typedef Win32Platform Platform;
#elif defined __IPHONE_3_0
  typedef CocoaIPhonePlatform Platform;
#elif defined __APPLE__
  typedef CocoaDesktopPlatform Platform;
#elif defined BEAGLEBOARD
  typedef UnistdPlatform Platform;
#elif defined __GNUC__
  typedef UnistdPlatform Platform;
#else
  #error Unknown compiler
#endif

}

#endif
