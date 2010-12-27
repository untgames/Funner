#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Описание реализации адаптера
*/

typedef stl::vector<Output::Pointer> OutputArray;

struct Adapter::Impl
{
  Log               log;               //протокол работы OpenGL
  OutputManager     output_manager;    //менеджер устройств вывода
  AdapterLibraryPtr library;           //библиотека адаптера
  stl::string       name;              //имя адаптера
  
///Конструктор
  Impl (const char* in_name, const char* in_dll_path, const char* init_string)  
    : library (AdapterLibrary::LoadLibrary (in_dll_path))
    , name (in_name)
  {
  }  
};

/*
    Конструктор / деструктор
*/

Adapter::Adapter (const char* name, const char* dll_path, const char* init_string)
{
  try
  {
      //проверка корректности аргументов    
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");
      
    if (!init_string)
      init_string = "";

      //создание реализации
    
    impl = new Impl (name, dll_path, init_string);
    
    impl->log.Printf ("...adapter successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::Adapter");
    
    throw;
  }
}

Adapter::~Adapter ()
{
  Log log = impl->log;
  
  log.Printf ("Destroy adapter '%s'", impl->name.c_str ());
  
  impl = 0;
  
  log.Printf ("...adapter successfully destroyed");
}

/*
    Имя адаптера / путь к модулю / описание
*/

const char* Adapter::GetName ()
{
  return impl->name.c_str ();
}

const char* Adapter::GetPath ()
{
  return impl->library->GetName ();
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::glx::Adapter::Adapter";
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->output_manager.GetOutputsCount ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  return impl->output_manager.GetOutput (index);
}

/*
    Запрос устройства вывода
*/

Output::Pointer Adapter::GetOutput (Window window)
{
  return impl->output_manager.FindContainingOutput (window);
}

/*
    Библиотека
*/

AdapterLibrary& Adapter::GetLibrary ()
{
  return *impl->library;
}

/*
    Перечисление доступных форматов пикселей
*/

int get_fb_config_attrib (AdapterLibrary& library, Display *display, GLXFBConfig config, int attribute)
{
  int value = 0;
  
  Status result = library.GetFBConfigAttrib (display, config, attribute, &value);
  
  if (result < Success)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Adapter::EnumPixelFormats",
      "glXGetFBConfigAttrib failed");
  
  return value;
}

void Adapter::EnumPixelFormats (Display *display, int screen, PixelFormatArray& pixel_formats, GlxExtensionEntriesArray& entries)
{
    // блокировка дисплея

  DisplayLock lock (display);
  
  int nelems = 0;
  
  GLXFBConfig *configs = impl->library->GetFBConfigs (display, screen, &nelems);

  if (!nelems)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::Adapter::EnumPixelFormats", "glXGetFBConfigs failed");
      
  const char *extensions = impl->library->GetExtensionsString (display, screen);
  
  if (!extensions)
    extensions = "";
  
  bool ext_swap_method_supported = false, ext_ARB_multisample_supported = false;  
  
  if (strstr (extensions, "GLX_OML_swap_method")) ext_swap_method_supported     = true;    
  if (strstr (extensions, "GLX_ARB_multisample")) ext_ARB_multisample_supported = true;

  AdapterLibrary& lib = GetLibrary ();

  for (int i=0; i<nelems; i++)
  {
    GLXFBConfig config = configs [i];
    
      // исключаем все конфигурации с индексным цветом
    
    if (!(get_fb_config_attrib (lib, display, config, GLX_RENDER_TYPE) & GLX_RGBA_BIT))
      continue;
    
    PixelFormatDesc desc;

    desc.adapter                 = this;
    desc.glx_extension_entries   = 0;
    desc.pixel_format_index      = get_fb_config_attrib (lib, display, config, GLX_FBCONFIG_ID);
    desc.color_bits              = get_fb_config_attrib (lib, display, config, GLX_BUFFER_SIZE);
    desc.alpha_bits              = get_fb_config_attrib (lib, display, config, GLX_ALPHA_SIZE);
    desc.depth_bits              = get_fb_config_attrib (lib, display, config, GLX_DEPTH_SIZE);
    desc.stencil_bits            = get_fb_config_attrib (lib, display, config, GLX_STENCIL_SIZE);
    
    if (ext_ARB_multisample_supported) desc.samples_count = get_fb_config_attrib (lib, display, config, GLX_SAMPLE_BUFFERS_ARB);
    else                               desc.samples_count = 0;

    if (get_fb_config_attrib (lib, display, config, GLX_DOUBLEBUFFER)) desc.buffers_count = 2;
    else                                                               desc.buffers_count = 1;

    desc.aux_buffers             = get_fb_config_attrib (lib, display, config, GLX_AUX_BUFFERS);
    desc.max_pbuffer_width       = get_fb_config_attrib (lib, display, config, GLX_MAX_PBUFFER_WIDTH);
    desc.max_pbuffer_height      = get_fb_config_attrib (lib, display, config, GLX_MAX_PBUFFER_HEIGHT);
    desc.max_pbuffer_pixels      = get_fb_config_attrib (lib, display, config, GLX_MAX_PBUFFER_PIXELS);
    desc.support_stereo          = get_fb_config_attrib (lib, display, config, GLX_STEREO) != 0;
    desc.support_draw_to_window  = (get_fb_config_attrib (lib, display, config, GLX_DRAWABLE_TYPE) & GLX_WINDOW_BIT) != 0;
    desc.support_draw_to_pbuffer = (get_fb_config_attrib (lib, display, config, GLX_DRAWABLE_TYPE) & GLX_PBUFFER_BIT) != 0;

    if (ext_swap_method_supported)
    {
      int swap_method = get_fb_config_attrib (lib, display, config, GLX_SWAP_METHOD_OML);

      switch (swap_method)
      {
        case GLX_SWAP_EXCHANGE_OML:
          desc.swap_method = SwapMethod_Flip;
          break;
        default:
        case GLX_SWAP_COPY_OML:
          desc.swap_method = SwapMethod_Copy;
          break;
        case GLX_SWAP_UNDEFINED_OML:
          desc.swap_method = SwapMethod_Discard;
          break;
      }      
    }
    else
    {
      desc.swap_method = SwapMethod_Copy;
    }
    
    pixel_formats.push_back (desc);
  }
}
