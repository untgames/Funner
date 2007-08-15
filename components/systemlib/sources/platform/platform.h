#ifndef SYSLIB_PLATFORM_HEADER
#define SYSLIB_PLATFORM_HEADER

#include <syslib/window.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги окна
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowFlag
{
  WindowFlag_Visible, //видимость окна
  WindowFlag_Active,  //активность окна
  WindowFlag_Focus    //фокус ввода окна
};

#ifdef _WIN32
  #include <platform/win32/platform.h>
  
  typedef Win32Platform Platform;
#else
  #include <platform/default/platform.h>
  
  typedef DefaultPlatform Platform;
#endif

}

#endif
