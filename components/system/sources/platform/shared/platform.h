#ifndef SYSLIB_PLATFORM_HEADER
#define SYSLIB_PLATFORM_HEADER

#include <shared/default_application_manager.h>
#include <shared/default_cookie_manager.h>
#include <shared/default_library_manager.h>
#include <shared/default_screen_manager.h>
#include <shared/default_screen_keyboard_manager.h>
#include <shared/default_sensor_manager.h>
#include <shared/default_thread_manager.h>
#include <shared/default_timer_manager.h>
#include <shared/default_window_manager.h>

#include <shared/android.h>
#include <shared/bada.h>
#include <shared/iphone.h>
#include <shared/linux.h>
#include <shared/macosx.h>
#include <shared/tabletos.h>
#include <shared/windows.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///”казание текущей платформы
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef BADA
  typedef BadaPlatform Platform;
#elif defined _WIN32
  typedef WindowsPlatform Platform;
#elif defined IPHONE
  typedef IPhonePlatform Platform;
#elif defined __APPLE__
  typedef MacOsPlatform Platform;
#elif defined ANDROID
  typedef AndroidPlatform Platform;
#elif defined BEAGLEBOARD
  typedef LinuxPlatform Platform;
#elif defined MEEGO
  typedef LinuxPlatform Platform;  
#elif defined TABLETOS
  typedef TabletOsPlatform Platform;
#elif defined LINUX
  typedef LinuxPlatform Platform;  
#elif defined __GNUC__
  typedef UnistdPlatform Platform;
#else
  #error Unknown compiler
#endif

}

#endif
