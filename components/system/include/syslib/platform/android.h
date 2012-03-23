#ifndef SYSLIB_PLATFORM_ANDROID_HEADER
#define SYSLIB_PLATFORM_ANDROID_HEADER

#include <syslib/window.h>

///forward declarations
struct ANativeWindow;
class _jobject;

namespace syslib
{

namespace android
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор нативного окна
///////////////////////////////////////////////////////////////////////////////////////////////////
struct NativeWindow
{
  ANativeWindow* native_window; //указатель на нативное окно
  _jobject*      view;          //указатель на view
};

}

}

#endif
