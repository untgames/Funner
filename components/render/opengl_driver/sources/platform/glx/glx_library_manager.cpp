#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
===================================================================================================
    Точки входа GLX драйвера
===================================================================================================
*/

typedef GLXContext  (*glXCreateNewContextFn)       (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
typedef void        (*glXDestroyContextFn)         (Display *dpy, GLXContext ctx);
typedef Bool        (*glXMakeContextCurrentFn)     (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef GLXPbuffer  (*glXCreatePbufferFn)          (Display *dpy, GLXFBConfig config, const int *attrib_list);
typedef void        (*glXDestroyPbufferFn)         (Display *dpy, GLXPbuffer pbuf);
typedef void        (*glXSwapBuffersFn)            (Display *dpy, GLXDrawable drawable);
typedef GLXContext  (*glXGetCurrentContextFn)      (void);
typedef Display*    (*glXGetCurrentDisplayFn)      (void);
typedef GLXDrawable (*glXGetCurrentDrawableFn)     (void);
typedef GLXDrawable (*glXGetCurrentReadDrawableFn) (void);
typedef Bool        (*glXIsDirectFn)               (Display *dpy, GLXContext ctx);
typedef const char* (*glXGetClientStringFn)        (Display *dpy, int name);
typedef Bool        (*glXQueryVersionFn)           (Display *dpy, int *major, int *minor);
typedef const char* (*glXQueryServerStringFn)      (Display *dpy, int screen, int name);

/*
===================================================================================================
    Динамическая библиотека
===================================================================================================
*/

class DynamicLibrary
{
  public:
///Конструктор
    DynamicLibrary (const char* in_path)
      : library (0)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::glx::DynamicLibrary::DynamicLibrary";

        //проверка корректности аргументов

      if (!in_path)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path");
        
      path = in_path;
      
        //загрузка библиотеки
        
      size_t dlopen_flags = RTLD_NOW | RTLD_GLOBAL;

      library = dlopen (in_path, dlopen_flags);

      if (!library)
        raise_error (common::format ("::dlopen('%S', %lu)", in_path, dlopen_flags).c_str ());
    }
    
///Деструктор
    ~DynamicLibrary ()
    {
      if (dlclose (library))
        raise_error ("::dlclose");
    }
    
///Получение дескриптора библиотеки
    void* GetLibrary ()
    {
      return library;
    }
    
///Получение пути к библиотеке
    const char* GetPath ()
    {
      return path.c_str ();
    }
    
///Получение точки входа
    void* GetSymbol (const char* symbol_name)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::glx::DynamicLibrary::GetSymbol";
      
      if (!symbol_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

      void* address = dlsym (library, symbol_name);

      return address;
    }

  private:
    void*       library; //дескриптор библиотеки
    stl::string path;    //путь к библиотеке
};

typedef stl::auto_ptr<DynamicLibrary> DynamicLibraryPtr;

}

/*
===================================================================================================
    GLX библиотека
===================================================================================================
*/

struct AdapterLibrary::Impl
{
  Log                    log;             //протокол
  DynamicLibraryPtr      dll;             //динамическая библиотека
  IContextLostListener  *listener;        //слушатель события потери контекста
  AdapterLibrary        *prev;            //предыдущая библиотека
  AdapterLibrary        *next;            //следующая библиотека
  static AdapterLibrary *first;           //первая библиотека
  
  glXCreateNewContextFn       fglXCreateNewContext;
  glXDestroyContextFn         fglXDestroyContext;
  glXMakeContextCurrentFn     fglXMakeContextCurrent;
  glXCreatePbufferFn          fglXCreatePbuffer;
  glXDestroyPbufferFn         fglXDestroyPbuffer;
  glXSwapBuffersFn            fglXSwapBuffers;
  glXGetCurrentContextFn      fglXGetCurrentContext;
  glXGetCurrentDisplayFn      fglXGetCurrentDisplay;
  glXGetCurrentDrawableFn     fglXGetCurrentDrawable;
  glXGetCurrentReadDrawableFn fglXGetCurrentReadDrawable;
  glXIsDirectFn               fglXIsDirect;
  glXGetClientStringFn        fglXGetClientString;
  glXQueryVersionFn           fglXQueryVersion;
  glXQueryServerStringFn      fglXQueryServerString;

///Конструктор
  Impl (DynamicLibraryPtr& in_dll)
    : dll (in_dll)
    , listener (0)
  {
    try
    {
        //регистрация библиотеки
      
      log.Printf ("...get default GLX-entries");
      
        //инициализация точек входа
        
      GetSymbol ("glXCreateNewContext",       fglXCreateNewContext);
      GetSymbol ("glXDestroyContext",         fglXDestroyContext);
      GetSymbol ("glXMakeContextCurrent",     fglXMakeContextCurrent);
      GetSymbol ("glXCreatePbuffer",          fglXCreatePbuffer);
      GetSymbol ("glXDestroyPbuffer",         fglXDestroyPbuffer);
      GetSymbol ("glXSwapBuffers",            fglXSwapBuffers);
      GetSymbol ("glXGetCurrentDisplay",      fglXGetCurrentDisplay);
      GetSymbol ("glXGetCurrentDrawable",     fglXGetCurrentDrawable);
      GetSymbol ("glXGetCurrentReadDrawable", fglXGetCurrentReadDrawable);
      GetSymbol ("glXIsDirect",               fglXIsDirect);
      GetSymbol ("glXGetClientString",        fglXGetClientString);
      GetSymbol ("glXQueryVersion",           fglXQueryVersion);
      GetSymbol ("glXQueryServerString",      fglXQueryServerString);

        //вывод общей информации
      
      Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
      
      DisplayLock lock (display);

      int major = 0, minor = 0;

      if (!fglXQueryVersion (display, &major, &minor))
        return;

      log.Printf ("...GLX %d.%d library successfully loaded", major, minor);
      log.Printf ("...client vendor: '%s'",     fglXGetClientString (display, GLX_VENDOR));
      log.Printf ("...client version: '%s'",    fglXGetClientString (display, GLX_VERSION));
      log.Printf ("...client extensions: '%s'", fglXGetClientString (display, GLX_EXTENSIONS));
    }
    catch (xtl::exception& exception)
    {
      log.Printf ("...GLX library load failed");
      exception.touch ("render::low_level::opengl::glx::AdapterLibrary::Impl::Impl");
      throw;
    }
  }
  
  ~Impl ()
  {

  }
    
///Определение адреса точки входа с приведением типов и проверкой корректности аргументов
  template <class Fn> void GetSymbol (const char* symbol, Fn& fn, bool check = true)
  {
    fn = (Fn)dll->GetSymbol (symbol);

    if (!fn && check)
      throw xtl::format_operation_exception ("render::low_level::opengl::glx::AdapterLibrary::Impl::GetSymbol",
        "Symbol '%s' not found in library '%s'", symbol, dll->GetPath ());
  }        
    
///Поиск библиотеки
  static AdapterLibrary* FindLibrary (const DynamicLibraryPtr& dll)
  {
    void *library = dll->GetLibrary ();

    for (AdapterLibrary* i=first; i; i=i->next)
      if (i->dll->GetLibrary () == library)
        return i;

    return 0;
  }
};

AdapterLibrary* AdapterLibrary::Impl::first = 0;

/*
    Конструктор / деструктор
*/

AdapterLibrary::AdapterLibrary (DynamicLibraryPtr& in_dll)
  : impl (new Impl (in_dll))
{
  impl->next = impl->first;
  impl->prev = 0;

  if (impl->first) impl->first->prev = this;

  impl->first = this;
}

~AdapterLibrary::AdapterLibrary ()
{ 
    //отмена регистрации библиотеки
    
  if (impl->prev) impl->prev->next = impl->next;
  else            impl->first      = impl->next;
  
  if (impl->next) impl->next->prev = impl->prev;
}

/*
    Загрузка библиотеки
*/

AdapterLibraryPtr AdapterLibrary::LoadLibrary (const char* path)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::LibraryManager::LoadLibrary";
  
  if (!path)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path");

  try
  {
    Log log;        
    
      //загрузка динамической библиотеки    
      
    log.Printf ("...load library '%s'", path);
      
    DynamicLibraryPtr dll (new DynamicLibrary (path));
    
      //попытка найти уже загруженную библиотеку

    AdapterLibrary* library = AdapterLibrary::Impl::FindLibrary (dll);

    if (library)
    {
      log.Printf ("...library '%s' already loaded", path);
      return library;
    }

      //создание новой библиотеки
      
    return AdapterLibraryPtr (new GlxAdapterLibrary (dll), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("%s('%s')", METHOD_NAME, path);
    throw;
  }
}

/*
    Получение имени библиотеки
*/
    
const char* AdapterLibrary::GetName ()
{
  return impl->dll->GetPath ();
}

/*
    Создание внеэкранной области рендеринга
*/

GLXPbuffer AdapterLibrary::CreatePbuffer (Display *dpy, GLXFBConfig config, const int *attrib_list)
{
  try
  {
    GLXPbuffer buf = impl->fglXCreatePbuffer (dpy, config, attrib_list);
    
    if (!buf)
      raise_error ("glXCreatePbuffer failed");
      
    return buf;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::CreatePbuffer");
    throw;
  }
}

/*
    Уничтожение внеэкранной области рендеринга
*/
    
void AdapterLibrary::DestroyPbuffer (Display *dpy, GLXPbuffer pbuf)
{
  impl->fglXDestroyPbuffer (dpy, pbuf);
}

/*
    Создание нового контекста GLX-рендеринга
*/

GLXContext AdapterLibrary::CreateContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
{
  try
  {
    GLXContext ctx = impl->fglXCreateNewContext (dpy, config, render_type, share_list, direct);
    
    if (!ctx)
      raise_error ("glXCreateNewContext failed");
      
    return ctx;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::CreateContext");
    throw;
  }
}

/*
    Создание контекста GLX
*/

void AdapterLibrary::DestroyContext (Display *dpy, GLXContext ctx)
{
  impl->fglXDestroyContext (dpy, ctx);
}

/*
    Установка текущего контекста
*/

void AdapterLibrary::MakeCurrent (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx, IContextLostListener* listener)
{
    //сброс текущего контекста

  if (impl->listener)
  {
    try
    {
      listener->OnLostCurrent ();
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

  impl->fglXMakeCurrent (dpy, draw, read, ctx);
  
  impl->listener = new_listener;
}

/*
    Получение текущего контекста
*/
    
GLXContext AdapterLibrary::GetCurrentContext ()
{
  return impl->fglXGetCurrentContext ();
}

/*
    Получение текущей области drawable
*/
    
GLXDrawable AdapterLibrary::GetCurrentDrawable ()
{
  return impl->fglXGetCurrentDrawable ();
}

/*
    Получение текущей области drawable для чтения
*/
    
GLXDrawable AdapterLibrary::GetCurrentReadDrawable ()
{
  return impl->fglXGetCurrentReadDrawable ();
}

/*
    Обменивает содержимое рабочего и фонового буферов
*/

void AdapterLibrary::SwapBuffers (Display *dpy, GLXDrawable drawable)
{
  impl->fglXSwapBuffers (dpy, drawable);
}
    
/*
    Получение адреса точки входа
*/

const void* GetProcAddress (const char* name, size_t search_flags)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetProcAddress");
}
