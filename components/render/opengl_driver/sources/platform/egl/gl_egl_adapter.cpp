#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
     онстанты
*/

const size_t OUTPUT_ARRAY_RESERVE = 8;    //резервируемое количество устройств вывода
const size_t MAX_EGL_CONFIGS      = 1024; //максимальное количество конфигураций

}

/*
    ќписание реализации адаптера
*/

typedef stl::vector<Output*> OutputArray;

struct Adapter::Impl
{
  Log         log;     //протокол работы OpenGL
  OutputArray outputs; //зарегистрированные устройства вывода
  Library     library; //библиотека точек входа OpenGL
  
/// онструктор
  Impl ()
  {
    outputs.reserve (OUTPUT_ARRAY_RESERVE);
  }  
};

/*
     онструктор / деструктор
*/

Adapter::Adapter ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
    »м€ адаптера / путь к модулю / описание
*/

const char* Adapter::GetName ()
{
  return "EGL";
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::egl::Adapter";
}

/*
    ѕеречисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->outputs.size ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::egl::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return impl->outputs [index];
}

/*
    –егистраци€ устройств вывода
*/

void Adapter::RegisterOutput (Output* output)
{
  if (!output)
    return;

  impl->outputs.push_back (output);
}

void Adapter::UnregisterOutput (Output* output)
{
  if (!output)
    return;
    
  for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
    if (*iter == output)
    {
      impl->outputs.erase (iter);

      return;
    }
}

/*
    «апрос устройства вывода
*/

Output::Pointer Adapter::GetOutput (const void* window_handle)
{
  try
  {
    if (!window_handle)
      throw xtl::make_null_argument_exception ("", "window_handle");
      
      //поиск устройства вывода среди уже созданных
      
    for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
      if ((*iter)->GetWindowHandle () == window_handle)
        return *iter;

      //создание нового устройства вывода

    return Output::Pointer (new Output (this, window_handle), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Adapter::GetOutput");
    throw;
  }  
}

/*
    Ѕиблиотека
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}

/*
    ѕеречисление доступных форматов пикселей
*/

namespace
{

inline EGLint get_config_attribute (EGLDisplay egl_display, EGLConfig egl_config, EGLint attribute)
{
  EGLint value = 0;
  
  if (!eglGetConfigAttrib (egl_display, egl_config, attribute, &value))
    raise_error (common::format ("::eglGetConfigAttrib(attribute=0x%04x)", attribute).c_str ());
    
  return value;
}

}

void Adapter::EnumPixelFormats (EGLDisplay display, PixelFormatArray& pixel_formats)
{
  try
  {
    EGLConfig configs [MAX_EGL_CONFIGS];

    EGLint num_configs = 0;

    if (!eglGetConfigs (display, configs, sizeof (configs) / sizeof (*configs), &num_configs))
      raise_error ("::eglGetConfigs");

    pixel_formats.reserve (pixel_formats.size () + num_configs);

    for (EGLint i=0; i<num_configs; i++)
    {
      EGLConfig config = configs [i];

#ifndef OPENGL_ES2_SUPPORT
      if (get_config_attribute (display, config, EGL_COLOR_BUFFER_TYPE) != EGL_RGB_BUFFER)
        continue;
#endif

      if (!(get_config_attribute (display, config, EGL_SURFACE_TYPE) & EGL_WINDOW_BIT))
        continue;

#ifdef OPENGL_ES_SUPPORT
      if (!(get_config_attribute (display, config, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES_BIT))
        continue;
#endif

#ifdef OPENGL_ES2_SUPPORT
      if (!(get_config_attribute (display, config, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES2_BIT))
        continue;
#endif

#ifndef OPENGL_ES2_SUPPORT
      if (get_config_attribute (display, config, EGL_LEVEL) != 0)
        continue;
#endif

#ifdef OPENGL_ES_SUPPORT
      if (!(get_config_attribute (display, config, EGL_CONFORMANT) & EGL_OPENGL_ES_BIT))
        continue;
#endif

#ifdef OPENGL_ES2_SUPPORT
      if (!(get_config_attribute (display, config, EGL_CONFORMANT) & EGL_OPENGL_ES2_BIT))
        continue;
#endif

      PixelFormatDesc desc;

      memset (&desc, 0, sizeof (desc));

      desc.egl_config    = config;
      desc.alpha_bits    = get_config_attribute (display, config, EGL_ALPHA_SIZE);
      desc.color_bits    = get_config_attribute (display, config, EGL_BUFFER_SIZE) - desc.alpha_bits;
      desc.depth_bits    = get_config_attribute (display, config, EGL_DEPTH_SIZE);
      desc.stencil_bits  = get_config_attribute (display, config, EGL_STENCIL_SIZE);
      desc.samples_count = get_config_attribute (display, config, EGL_SAMPLES);

      switch (get_config_attribute (display, config, EGL_CONFIG_CAVEAT))
      {
        default:
        case EGL_NONE:
          desc.acceleration = Acceleration_Full;
        case EGL_SLOW_CONFIG:
          desc.acceleration = Acceleration_Slow;
          break;
        case EGL_NON_CONFORMANT_CONFIG:
         continue;
      }

      pixel_formats.push_back (desc);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::egl::Adapter::EnumPixelFormats");
    throw;
  }
}
