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
typedef const char* (*glXGetClientStringFn)        (Display *dpy, int name);
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

/*
===================================================================================================
    GLX библиотека
===================================================================================================
*/

class GlxAdapterLibrary: public IAdapterLibrary, public xtl::reference_counter
{
  public:
///Конструктор
    GlxAdapterLibrary (DynamicLibraryPtr& in_dll)
      : dll (in_dll)
      , listener (0)
    {
      try
      {
          //регистрация библиотеки

        next = first;
        prev = 0;

        if (first) first->prev = this;

        first = this;
        
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
        GetSymbol ("glXGetClientString",        fglXGetClientString);
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
        exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::GlxAdapterLibrary");
        throw;
      }
    }
    
    ~GlxAdapterLibrary ()
    {
      log.Printf ("...unload dll '%s'", GetName ());
      
        //отмена регистрации библиотеки
        
      if (prev) prev->next = next;
      else      first      = next;
      
      if (next) next->prev = prev;
    }
    
///Получение имени библиотеки
    const char* GetName ()
    {
      return dll->GetPath ();
    }
    
//создание внеэкранной области рендеринга
    GLXPbuffer CreatePbuffer (Display *dpy, GLXFBConfig config, const int *attrib_list)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::CreatePbuffer");
    }
    
//уничтожение внеэкранной области рендеринга
    void DestroyPbuffer (Display *dpy, GLXPbuffer pbuf)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::DestroyPbuffer");
    }

//создание нового контекста GLX-рендеринга
    GLXContext CreateContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::CreateContext");
    }
    
//создание контекста GLX
    void DeleteContext (Display *dpy, GLXContext ctx)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::DeleteContext");
    }

//установка текущего контекста
    void MakeCurrent (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx, IContextLostListener* listener)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::MakeCurrent");
    }
    
//получение текущего контекста
    GLXContext GetCurrentContext ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentContext");
    }
    
//получение текущего устройства отображения
    Display* GetCurrentDisplay ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentDisplay");
    }
    
//получение текущей области drawable
    GLXDrawable GetCurrentDrawable ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentDrawable");
    }
    
//получение текущей области drawable для чтения
    GLXDrawable GetCurrentReadDrawable ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentReadDrawable");
    }

//обменивает содержимое рабочего и фонового буферов
    void SwapBuffers (Display *dpy, GLXDrawable drawable)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::SwapBuffers");
    }
    
///Подсчёт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }
    
///Поиск библиотеки
    static GlxAdapterLibrary* FindLibrary (const DynamicLibraryPtr& dll)
    {
      void *library = dll->GetLibrary ();

      for (GlxAdapterLibrary* i=first; i; i=i->next)
        if (i->dll->GetLibrary () == library)
          return i;

      return 0;
    }

///Получение адреса точки входа
    const void* GetProcAddress (const char* name, size_t search_flags)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetProcAddress");
    }
    
  protected:
///Определение адреса точки входа с приведением типов и проверкой корректности аргументов
    template <class Fn> void GetSymbol (const char* symbol, Fn& fn, bool check = true)
    {
      fn = (Fn)dll->GetSymbol (symbol);

      if (!fn && check)
        throw xtl::format_operation_exception ("render::low_level::opengl::glx::AdapterLibrary::GetSymbol",
          "Symbol '%s' not found in library '%s'", symbol, dll->GetPath ());
    }        
    
  protected:
    Log log; //протокол
    
  private:
    DynamicLibraryPtr         dll;             //динамическая библиотека
    IContextLostListener     *listener;        //слушатель события потери контекста
    GlxAdapterLibrary        *prev;            //предыдущая библиотека
    GlxAdapterLibrary        *next;            //следующая библиотека
    static GlxAdapterLibrary *first;           //первая библиотека

  private:
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
    glXGetClientStringFn        fglXGetClientString;
    glXQueryServerStringFn      fglXQueryServerString;
};

GlxAdapterLibrary* GlxAdapterLibrary::first = 0;

}

/*
===================================================================================================
    Менеджер загружаемых библиотеки
===================================================================================================
*/

/*
    Загрузка библиотеки
*/

AdapterLibraryPtr LibraryManager::LoadLibrary (const char* name)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::LibraryManager::LoadLibrary";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  try
  {
    Log log;        
    
      //загрузка динамической библиотеки    
      
    log.Printf ("...load library '%s'", name);
      
    DynamicLibraryPtr dll (new DynamicLibrary (name));
    
      //попытка найти уже загруженную библиотеку

    GlxAdapterLibrary* library = GlxAdapterLibrary::FindLibrary (dll);

    if (library)
    {    
      log.Printf ("...library '%s' already loaded", name);
      return library;
    }

      //создание новой библиотеки
      
    return AdapterLibraryPtr (new GlxAdapterLibrary (dll), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("%s('%s')", METHOD_NAME, name);
    throw;
  }
}
