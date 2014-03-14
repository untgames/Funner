#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    Константы
*/

const size_t OUTPUT_MAX_NAME_SIZE = 128; //максимальный размер имени устройства вывода

}

/*
    Описание реализации устройства вывода
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct Output::Impl
{
  Log               log;                         //протокол графического драйвера
  AdapterPtr        adapter;                     //адаптер отрисовки
  NativeWindowType  native_window;               //окно
  NativeDisplayType native_display;              //целевое платформо-зависимое устройство вывода
  EGLDisplay        egl_display;                 //целевое устройство вывода
  char              name [OUTPUT_MAX_NAME_SIZE]; //имя цепочки обмена  
  
///Конструктор
  Impl (Adapter* in_adapter, const void* window_handle)
    : adapter (in_adapter)
    , native_window ((NativeWindowType)window_handle)
    , native_display (0)
    , egl_display (0)  
  {
    *name = 0;

    try
    {
        //платформо-зависимая инициализация

      PlatformInitialize ();

        //создание дисплея

      log.Printf ("...create display");

      egl_display = eglGetDisplay (native_display);

      if (!egl_display)
        log.Printf ("...using default display");

        //инициализация EGL

      log.Printf ("...initialize EGL");            

      EGLint major_version = 0, minor_version = 0;

      if (!eglInitialize (egl_display, &major_version, &minor_version))
        raise_error ("::eglInitialize");        

      log.Printf ("...EGL intialized successfull (version %d.%d)", major_version, minor_version);      
    }
    catch (...)
    {
      PlatformDone ();
      throw;
    }
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
        //закрытие сессии с целевым устройством вывода
      
      eglTerminate (egl_display);
      
        //платформо-зависимое освобождение ресурсов
      
      PlatformDone ();
    }
    catch (...)
    {
    }
  }
  
#if defined ( _WIN32 ) && !defined ( BADA )

///Платформо-зависимая инициализация
  void PlatformInitialize ()
  {
    try
    {
      log.Printf ("...get device context");

#ifndef OPENGL_ES2_SUPPORT
      native_display = ::GetDC (native_window);      

      if (!native_display)
        throw xtl::format_operation_exception ("::GetDC", "Operation failed"); //сделать через raise_error!!!
#endif

      log.Printf ("...get window name");

      if (!GetWindowTextA (native_window, name, sizeof (name)))
        throw xtl::format_operation_exception ("::GetWindowTextA", "Operation failed"); //сделать через raise_error!!!
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::egl::Adapter::Impl::PlatformInitializeWin32");
      throw;
    }
  }
  
///Платформо-зависимое освобождение ресурсов
  void PlatformDone ()
  {    
    if (native_display && native_window)
    {
      log.Printf ("...release device context");
      
      ::ReleaseDC (native_window, native_display);
    }
  }

#elif defined BADA

///Платформо-зависимая инициализация
  void PlatformInitialize ()
  {
    native_display = EGL_DEFAULT_DISPLAY;

    log.Printf ("...get control name");
        
    strncpy (name, common::tostring (reinterpret_cast<Osp::Ui::Control*> (native_window)->GetName ().GetPointer ()).c_str (), sizeof (name));
  }
  
///Платформо-зависимое освобождение ресурсов
  void PlatformDone ()
  {    
  }
  
#elif defined BEAGLEBOARD

///Платформо-зависимая инициализация
  void PlatformInitialize ()
  {
    native_display = (NativeDisplayType)syslib::x11::DisplayManager::DisplayHandle ();

    log.Printf ("...get window name");
    
    char* window_name = 0;

    if (!XFetchName ((Display*)native_display, (Window)native_window, &window_name))
      window_name = (char*)"default";
      
    if (!window_name)
      window_name = (char*)"default";

    strncpy (name, window_name, sizeof (name));
  }
  
///Платформо-зависимое освобождение ресурсов
  void PlatformDone ()
  {
  }
  
#elif defined ANDROID  

///Платформо-зависимая инициализация
  void PlatformInitialize ()
  {
    native_display = EGL_DEFAULT_DISPLAY;
  }
  
///Платформо-зависимое освобождение ресурсов
  void PlatformDone ()
  {
  }
  
#elif defined TABLETOS

///Платформо-зависимая инициализация
  void PlatformInitialize ()
  {
    native_display = EGL_DEFAULT_DISPLAY;    
  }
  
///Платформо-зависимое освобождение ресурсов
  void PlatformDone ()
  {
  }

#else
  #error Unknown platform!
#endif  
};

/*
    Конструктор / деструктор
*/

Output::Output (Adapter* adapter, const void* window_handle)
{
  try
  {
    if (!window_handle)
      throw xtl::make_null_argument_exception ("", "window_handle");
      
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

    impl = new Impl (adapter, window_handle);
    
    impl->adapter->RegisterOutput (this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Output::Output");
    throw;
  }
}

Output::~Output ()
{
  impl->adapter->UnregisterOutput (this);
}

/*
    Получение имени
*/

const char* Output::GetName ()
{
  return impl->name;
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::Output::GetModesCount");
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::Output::GetModeDesc");
}

/*
    Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::Output::SetCurrentMode");
}

void Output::GetCurrentMode (OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::Output::GetCurrentMode");
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::egl::Output::SetGammaRamp", "Gamma ramp not supported in EGL");
}

void Output::GetGammaRamp (Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::egl::Output::GetGammaRamp", "Gamma ramp not supported in EGL");
}

/*
    Получение параметров EGL
*/

EGLDisplay Output::GetEglDisplay ()
{
  return impl->egl_display;
}

const void* Output::GetWindowHandle ()
{
  return impl->native_window;
}

NativeDisplayType Output::GetDisplay ()
{
  return impl->native_display;
}
