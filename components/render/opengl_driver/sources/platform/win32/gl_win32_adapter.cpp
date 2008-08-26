#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    Описание реализации адаптера
*/

typedef WINGDIAPI HGLRC (WINAPI *CreateContextFn)       (HDC dc);
typedef WINGDIAPI BOOL  (WINAPI *DeleteContextFn)       (HGLRC context);
typedef WINGDIAPI int   (WINAPI *DescribePixelFormatFn) (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd);
typedef WINGDIAPI HGLRC (WINAPI *GetCurrentContextFn)   ();
typedef WINGDIAPI HDC   (WINAPI *GetCurrentDCFn)        ();
typedef WINGDIAPI int   (WINAPI *GetPixelFormatFn)      (HDC dc);
typedef WINGDIAPI PROC  (WINAPI *GetProcAddressFn)      (LPCSTR name);
typedef WINGDIAPI BOOL  (WINAPI *MakeCurrentFn)         (HDC device_context, HGLRC context);
typedef WINGDIAPI BOOL  (WINAPI *SetPixelFormatFn)      (HDC dc, int pixel_format, const PIXELFORMATDESCRIPTOR* pfd);
typedef WINGDIAPI BOOL  (WINAPI *ShareListsFn)          (HGLRC src, HGLRC dst);
typedef WINGDIAPI BOOL  (WINAPI *SwapBuffersFn)         (HDC dc);
typedef WINGDIAPI void  (APIENTRY *glFlushFn)           (void);

/*
    Таблица функций, используемых для работы OpenGL
*/

struct WglEntries
{
  CreateContextFn       CreateContext;
  DeleteContextFn       DeleteContext;
  DescribePixelFormatFn DescribePixelFormat;
  GetCurrentContextFn   GetCurrentContext;
  GetCurrentDCFn        GetCurrentDC;
  GetPixelFormatFn      GetPixelFormat;
  GetProcAddressFn      GetProcAddress;
  MakeCurrentFn         MakeCurrent;
  SetPixelFormatFn      SetPixelFormat;
  ShareListsFn          ShareLists;
  SwapBuffersFn         SwapBuffers;
  glFlushFn             Flush;

///Конструктор
  WglEntries (syslib::DynamicLibrary& dll)
  {
      //инициализация базовых точек входа

    get (dll, "wglCreateContext",       CreateContext);
    get (dll, "wglDeleteContext",       DeleteContext);
    get (dll, "wglDescribePixelFormat", DescribePixelFormat);
    get (dll, "wglGetCurrentContext",   GetCurrentContext);
    get (dll, "wglGetCurrentDC",        GetCurrentDC);
    get (dll, "wglGetPixelFormat",      GetPixelFormat);
    get (dll, "wglGetProcAddress",      GetProcAddress);
    get (dll, "wglMakeCurrent",         MakeCurrent);
    get (dll, "wglSetPixelFormat",      SetPixelFormat);
    get (dll, "wglShareLists",          ShareLists);
    get (dll, "wglSwapBuffers",         SwapBuffers);
    get (dll, "glFlush",                Flush);        

    if (!xtl::xstricmp (dll.Name (), "opengl32.dll"))
    {
        //подмена функций диспетчеризации

      SetPixelFormat = &::SetPixelFormat;
      GetPixelFormat = &::GetPixelFormat;
    }
  }

///Приведение типа с проверкой корректности возврата
  template <class Fn> static void get (syslib::DynamicLibrary& dll, const char* symbol, Fn& fn)
  {
    fn = reinterpret_cast<Fn> (dll.GetSymbol (symbol));

    if (!fn)
      throw xtl::format_operation_exception ("render::low_level::opengl::windows::WglEntries::WglEntries",
        "Symbol '%s' not found in library '%s'", symbol, dll.Name ());
  }
  
///Инициализация функции расширения
  template <class Fn> void getext (const char* symbol, Fn& fn)
  {
    fn = reinterpret_cast<Fn> (GetProcAddress (symbol));    
  }
};

/*
    Макетный контекст OpenGL, необходимый для корректной инициализации WGL
*/

class DummyContext
{
  public:
///Конструктор
    DummyContext (HWND parent_window, const WglEntries& in_entries)
      : entries (in_entries),
        window (parent_window),
        dc (GetDC (window.GetHandle ())),
        context (0)
    {
      try
      {      
        if (!dc)
          raise_error ("GetDC");

        PIXELFORMATDESCRIPTOR pfd;

        static const size_t REQUIRE_FLAGS = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

        for (int pixel_format=1; entries.DescribePixelFormat (dc, pixel_format, sizeof pfd, &pfd); pixel_format++)
          if ((pfd.dwFlags & REQUIRE_FLAGS) == REQUIRE_FLAGS)
          {
            if (!entries.SetPixelFormat (dc, pixel_format, &pfd))
              raise_error ("wglSetPixelFormat");                           

            context = entries.CreateContext (dc);

            if (!context)
              raise_error ("wglCreateContext");

            prev_dc      = entries.GetCurrentDC ();
            prev_context = entries.GetCurrentContext ();

            if (!entries.MakeCurrent (dc, context))
              raise_error ("wglMakeCurrent");                            

            return;
          }

        throw xtl::format_operation_exception ("render::low_level::opengl::windows::DummyContext", "No pixel formats with OpenGL support");
      }
      catch (...)      
      {
        if (context) entries.DeleteContext (context);
        if (dc)      ReleaseDC (window.GetHandle (), dc);
        throw;
      }              
    }

///Деструктор
    ~DummyContext ()
    {      
      if (!entries.MakeCurrent (prev_dc, prev_context))
        entries.MakeCurrent (0, 0);

      entries.DeleteContext (context);
      ReleaseDC             (window.GetHandle (), dc);
    }

  private:
    const WglEntries& entries;      //точки входа в wgl-функции
    DummyWindow       window;       //макетное окно
    HDC               dc;           //контекст устройства отрисовки
    HGLRC             context;      //контекст OpenGL
    HDC               prev_dc;      //предыдущий контекст устройства отрисовки
    HGLRC             prev_context; //предыдущий контекст OpenGL
};

/*
    Контекст перечисления форматов пикселей
*/

struct PixelFormatEnumContext
{  
  Adapter::PixelFormatArray& pixel_formats;         //массив форматов пикселей
  HDC                        device_context;        //контекст устройства отрисовки
  const WglEntries&          wgl_entries;           //точки входа wgl-функций
  WglExtensionEntries*       wgl_extension_entries; //таблица WGL-расширений  
  Adapter*                   adapter;               //адаптер
  int                        current_pixel_format;  //текущий номер формата пикселей
  bool                       has_pbuffer;           //наличие WGL_ARB_pbuffer
  bool                       has_multisample;       //наличие WGL_ARB_multisample
  
///Конструктор
  PixelFormatEnumContext
   (HWND                               window,
    HDC                                in_device_context,
    Adapter*                           in_adapter,
    const WglEntries&                  in_entries,
    Adapter::PixelFormatArray&         in_pixel_formats,
    Adapter::WglExtensionEntriesArray& wgl_extension_entries_array)
      : pixel_formats (in_pixel_formats),
        device_context (in_device_context),
        wgl_entries (in_entries),
        adapter (in_adapter),
        current_pixel_format (0),
        wgl_extension_entries (0),
        has_pbuffer (false),
        has_multisample (false)
  {
      //попытка получить точки входа для перечисления форматов с использованием расширений

    try
    {
        //создание макетного контекста

      DummyContext context (window, wgl_entries);

        //инициализация расширений

      wgl_extension_entries_array.push_back ();

      try
      {
        wgl_extension_entries = &wgl_extension_entries_array.back ();        

        wgl_extension_entries->Init (adapter->GetLibrary ());
        
          //проверка наличия расширений WGL_ARB_pbuffer и WGL_ARB_multisample
          
        const char* extensions = 0;
          
        if (wgl_extension_entries->GetExtensionsStringARB)
          extensions = wgl_extension_entries->GetExtensionsStringARB (device_context);
          
        if (!extensions && wgl_extension_entries->GetExtensionsStringEXT)
          extensions = wgl_extension_entries->GetExtensionsStringEXT ();

        if (extensions)
        {
          has_pbuffer     = strstr (extensions, "WGL_ARB_pbuffer") != 0;
          has_multisample = strstr (extensions, "WGL_ARB_multisample") != 0;
        }
      }
      catch (...)
      {
        wgl_extension_entries_array.pop_back ();
        throw;
      }
    }
    catch (...)
    {
      //!!!сделать протоколирование!!
      //подавление всех исключений
    }
  }

///Получение атрибута формата пикселей
  bool GetPixelFormatAttribute (int attribute_name, int& attribute_value)
  {
    return wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, 1, &attribute_name, &attribute_value) != 0;
  }
  
///Перечисление форматов
  void EnumFormats ()
  {
    PixelFormatDesc out_desc;
    
      //попытка перечисления с использованием расширеня WGL_ARB_pixel_format
      
    if (wgl_extension_entries->GetPixelFormatAttribivARB)
    {
        //определение максимального формата пикселей      

      int name = WGL_NUMBER_PIXEL_FORMATS_ARB, max_pixel_format = 0;

      if (wgl_extension_entries->GetPixelFormatAttribivARB (device_context, 1, 0, 1, &name, &max_pixel_format))
      {
          //перечисление форматов

        for (current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
        {
          static int basic_names [] = {
            WGL_SUPPORT_OPENGL_ARB,
            WGL_DRAW_TO_WINDOW_ARB,
            WGL_PIXEL_TYPE_ARB,
            WGL_COLOR_BITS_ARB,
            WGL_ALPHA_BITS_ARB,
            WGL_DEPTH_BITS_ARB,
            WGL_STENCIL_BITS_ARB,
            WGL_DOUBLE_BUFFER_ARB,
            WGL_AUX_BUFFERS_ARB,
            WGL_STEREO_ARB,
            WGL_SWAP_METHOD_ARB,
            WGL_ACCELERATION_ARB,
          };

          static const int BASIC_NAMES_COUNT = sizeof (basic_names) / sizeof (GLenum);

          int value_buffer [BASIC_NAMES_COUNT], *iter = value_buffer;

          if (!wgl_extension_entries->GetPixelFormatAttribivARB (device_context, current_pixel_format, 0, BASIC_NAMES_COUNT, basic_names, value_buffer))
            continue;

          if (!*iter++) //форматы без поддержки OpenGL игнорируются
            continue;

          if (!*iter++) //форматы без поддержки рисования в окно игнорируются
            continue;

          if (*iter++ == PFD_TYPE_COLORINDEX) //палитровые форматы игнорируются
            continue;

          out_desc.adapter                 = adapter;
          out_desc.wgl_extension_entries   = wgl_extension_entries;
          out_desc.pixel_format_index      = current_pixel_format;
          out_desc.color_bits              = *iter++;
          out_desc.alpha_bits              = *iter++;
          out_desc.depth_bits              = *iter++;
          out_desc.stencil_bits            = *iter++;
          out_desc.samples_count           = 0;      
          out_desc.buffers_count           = *iter++ ? 2 : 1;
          out_desc.aux_buffers             = *iter++;
          out_desc.support_draw_to_pbuffer = false;
          out_desc.support_stereo          = *iter++ != 0;
          
          switch (*iter++)
          {
            case WGL_SWAP_EXCHANGE_ARB:
              out_desc.swap_method = SwapMethod_Flip;
              break;
            case WGL_SWAP_COPY_ARB:
              out_desc.swap_method = SwapMethod_Copy;
              break;
            default:            
            case WGL_SWAP_UNDEFINED_ARB:
              out_desc.swap_method = SwapMethod_Discard;
              break;
          }

          switch (*iter++)
          {
            default:
            case WGL_NO_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_No;
              break;
            case WGL_GENERIC_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_MCD;
              break;
            case WGL_FULL_ACCELERATION_ARB:
              out_desc.acceleration = Acceleration_ICD;
              break;
          }

          int value;

          if (has_pbuffer && GetPixelFormatAttribute (WGL_DRAW_TO_PBUFFER_ARB, value))
          {
            out_desc.support_draw_to_pbuffer = value != 0;
          }
          
          if (has_multisample && GetPixelFormatAttribute (WGL_SAMPLES_ARB, value))
          {
            out_desc.samples_count = size_t (value);
          }
          
            //добавление дескриптора в массив

          pixel_formats.push_back (out_desc);          
        }

        return;        
      }
    }
    
      //перечисление без использования расширений

    PIXELFORMATDESCRIPTOR pfd;
    
      //определение максимального формата пикселей

    int max_pixel_format = wgl_entries.DescribePixelFormat (device_context, 1, sizeof pfd, &pfd);
    
    for (current_pixel_format=1; current_pixel_format<=max_pixel_format; ++current_pixel_format)
    {
      if (!wgl_entries.DescribePixelFormat (device_context, current_pixel_format, sizeof (pfd), &pfd))
        continue;

      if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL)) //форматы без поддержки OpenGL игнорируются
        continue;

      if (pfd.iPixelType == PFD_TYPE_COLORINDEX) //палитровые форматы игнорируются
        continue;
        
      if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) //форматы без рисования в окно игнорируются
        continue;

      out_desc.adapter                 = adapter;
      out_desc.wgl_extension_entries   = wgl_extension_entries;
      out_desc.pixel_format_index      = current_pixel_format;
      out_desc.color_bits              = pfd.cColorBits;
      out_desc.alpha_bits              = pfd.cAlphaBits;
      out_desc.depth_bits              = pfd.cDepthBits;
      out_desc.stencil_bits            = pfd.cStencilBits;
      out_desc.samples_count           = 0;
      out_desc.buffers_count           = pfd.dwFlags & PFD_DOUBLEBUFFER ? 2 : 1;
      out_desc.aux_buffers             = pfd.cAuxBuffers;
      out_desc.support_draw_to_pbuffer = false;
      out_desc.support_stereo          = (pfd.dwFlags & PFD_STEREO) != 0;

      if      (pfd.dwFlags & PFD_SWAP_COPY)     out_desc.swap_method = SwapMethod_Copy;
      else if (pfd.dwFlags & PFD_SWAP_EXCHANGE) out_desc.swap_method = SwapMethod_Flip;
      else                                      out_desc.swap_method = SwapMethod_Discard;

      if (pfd.dwFlags & PFD_GENERIC_FORMAT)
      {
        out_desc.acceleration = Acceleration_No;
      }
      else
      {
        out_desc.acceleration = (pfd.dwFlags & PFD_GENERIC_ACCELERATED) ? Acceleration_MCD : Acceleration_ICD;
      }      

        //добавление дескриптора в массив

      pixel_formats.push_back (out_desc);
    }
  }
};

}

struct Adapter::Impl: public ILibrary
{
  OutputManager          output_manager;   //менеджер устройств вывода
  syslib::DynamicLibrary dll;              //динамическая библиотека модуля адаптера отрисовки
  WglEntries             entries;          //точки входа wgl-функций
  stl::string            name;             //имя адаптера
  stl::string            path;             //путь к адаптеру
  IContextListener*      context_listener; //слушатель событий контекста

///Конструктор
  Impl (const char* in_name, const char* in_dll_path)
    : dll (in_dll_path),
      entries (dll),
      name (in_name),
      path (in_dll_path),
      context_listener (0) {}
      
///Получение адреса точки входа OpenGL
  const void* GetProcAddress (const char* name, size_t search_flags)
  {
    try
    {
      static const char* METHOD_NAME = "render::low_level::opengl::windows::Adapter::Impl::GetProcAddress";

      if (!name)
      {
        if (search_flags & EntrySearch_NoThrow)
          return 0;

        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      }

      const void* address = 0;

      if (!address && (search_flags & EntrySearch_Context))
        address = entries.GetProcAddress (name);    
        
      if (address)
        return address;

      if (search_flags & EntrySearch_Library)
        address = dll.GetSymbol (name);

      if (address)
        return address;

      if (search_flags & EntrySearch_NoThrow)
        return 0;

      throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in library '%s'",
        name, dll.Name ());
    }
    catch (...)
    {
      if (search_flags & EntrySearch_NoThrow)
        return 0;

      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

Adapter::Adapter (const char* name, const char* dll_path)
{
  try
  {
      //проверка корректности аргументов    
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");
      
      //создание реализации

    impl = new Impl (name, dll_path);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
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
  return impl->path.c_str ();
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::windows::Adapter::Adapter";
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

Output* Adapter::FindContainingOutput (HWND window)
{
  return impl->output_manager.FindContainingOutput (window);
}

/*
    Установка слушателя событий контекста
*/

void Adapter::SetContextListener (IContextListener* listener)
{
  impl->context_listener = listener;
}

IContextListener* Adapter::GetContextListener ()
{
  return impl->context_listener;
}

/*
    Получение интерфейса библиотеки OpenGL
*/

ILibrary& Adapter::GetLibrary ()
{
  return *impl;
}

/*
    Перечисление доступных форматов пикселей
*/

void Adapter::EnumPixelFormats
 (HWND                      window,
  HDC                       device_context,
  PixelFormatArray&         pixel_formats,
  WglExtensionEntriesArray& wgl_extension_entries_array)
{
  try
  {
      //оповещение о возможной потере контекста в процессе перечисления форматов

    if (impl->context_listener)
      impl->context_listener->OnLostCurrent ();

      //перечисление доступных форматов

    PixelFormatEnumContext (window, device_context, this, impl->entries, pixel_formats, wgl_extension_entries_array).EnumFormats ();
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Диспетчеры вызовов WGL
*/

HGLRC Adapter::CreateContext (HDC dc)
{
  HGLRC context = impl->entries.CreateContext (dc);
  
  if (!context)
    raise_error ("wglCreateContext");
    
  return context;
}

void Adapter::DeleteContext (HGLRC context)
{
  if (!impl->entries.DeleteContext (context))
    raise_error ("wglDeleteContext");
}

HGLRC Adapter::GetCurrentContext ()
{
  return impl->entries.GetCurrentContext ();
}

HDC Adapter::GetCurrentDC ()
{
  return impl->entries.GetCurrentDC ();
}

int Adapter::GetPixelFormat (HDC dc)
{
  return impl->entries.GetPixelFormat (dc);
}

PROC Adapter::GetProcAddress (LPCSTR name)
{
  return impl->entries.GetProcAddress (name);
}

void Adapter::MakeCurrent (HDC device_context, HGLRC context)
{
  if (impl->context_listener)
    impl->context_listener->OnLostCurrent ();

  if (!impl->entries.MakeCurrent (device_context, context))
    raise_error ("wglMakeCurrent");

  if (device_context && context && impl->context_listener)
    impl->context_listener->OnSetCurrent ();
}

void Adapter::SetPixelFormat (HDC dc, int pixel_format)
{
  PIXELFORMATDESCRIPTOR pfd;

  if (!impl->entries.DescribePixelFormat (dc, pixel_format, sizeof pfd, &pfd))
    raise_error ("wglDescribePixelFormat");

  if (!impl->entries.SetPixelFormat (dc, pixel_format, &pfd))
    raise_error ("wglSetPixelFormat");
}

void Adapter::ShareLists (HGLRC src, HGLRC dst)
{
  if (!impl->entries.ShareLists (src, dst))
    raise_error ("wglShareLists");
}

void Adapter::SwapBuffers (HDC dc)
{
    //сброс буфера команд OpenGL

  if (impl->entries.GetCurrentDC () == dc)
    impl->entries.Flush ();

    //обмен буферов

  if (!impl->entries.SwapBuffers (dc))
    raise_error ("wglSwapBuffers");
}
