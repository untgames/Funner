/**********************************************************\

  Auto-generated FunnerEngine.cpp

  This file contains the auto-generated main plugin object
  implementation for the Funner Engine project

\**********************************************************/

#include <wchar.h>
#include <string.h>

#include "NpapiTypes.h"
#include "FunnerEngineAPI.h"

#ifdef _WIN32
  #include "Win/PluginWindowWin.h"
#endif

#include "FunnerEngine.h"

#include <engine/engine.h>

namespace
{

/*
    Константы
*/

const char*    WINDOW_NAME          = "FunnerWebWindow";
const wchar_t* ENGINE_LIBRARY_NAME  = L"funner.dll";

/*
    Синглтон для хранения объекта движка
*/

class EngineHolder
{
  public:
    static engine::IEngine& Instance ()
    {
      static EngineHolder holder;
      
      return *holder.instance;
    }
  
  private:
    EngineHolder ()
    {
      LoadPlugin ();    
    }  

#ifdef _WIN32
    void LoadPlugin ()
    {
      static wchar_t module_path [1024];      
      
      extern HINSTANCE gInstance;
      
      *module_path = L'\0';
      
      if (!::GetModuleFileNameW (gInstance, module_path, sizeof (module_path)))
      {
//        log ("Can't get module path");
        abort ();
      }
      
      wchar_t* s = module_path + wcslen (module_path);
      
      for (;s!=module_path && *s != L'\\';s--);
      
      *s = L'\0';
      
      if (!SetDllDirectory (module_path))
      {
//        log ("Can't set DLL search directory\n");
        abort ();
      }

      *s++ = L'\\';

      wcscpy (s, ENGINE_LIBRARY_NAME);
      
      HMODULE library = LoadLibraryW (module_path);
      
      if (!library)
      {
//        log ("Can't load engine library '%S'\n", module_path);
        abort ();
      }
      
      FunnerInitProc FunnerInit = (FunnerInitProc)GetProcAddress (library, "FunnerInit");  
      
      if (!FunnerInit)
      {
//        log ("Bad library (entries not found)\n");
        abort ();
      }

      instance = std::auto_ptr<engine::IEngine> (FunnerInit ());

      if (!&*instance)
      {
//        log ("Engine startup failed!");
        abort ();
      }
    }

#else
  #error Unsupported platform
#endif    

  private:
    std::auto_ptr<engine::IEngine> instance;
};

/*
    Нить
*/

class PlatformThread
{
  public:
    PlatformThread ()
    {
#ifdef _WIN32
      thread = 0;
#endif      
    }

    ~PlatformThread ()
    {
      CloseThread ();
    }    
    
    void Start ()
    {
      CreateThread ();
    }    

#ifdef _WIN32
    void Terminate ()
    {
      TerminateThread (thread, 0);
    }

#endif

  private:
    virtual void Run () = 0;

    PlatformThread (const PlatformThread&);
    PlatformThread& operator = (const PlatformThread&);

  private:
#ifdef _WIN32
    void CreateThread ()
    {
      thread = (HANDLE)_beginthread (&PlatformThread::ThreadRoutine, 0, this);
    }

    void CloseThread ()
    {
      CloseHandle (thread);
    }

    static void ThreadRoutine (void* this_thread)
    {
      if (!this_thread)
        return;

      reinterpret_cast<PlatformThread*> (this_thread)->Run ();
    }

    HANDLE thread;    
#endif
};

template <class Fn> class ThreadTemplate: public PlatformThread
{
  public:
    ThreadTemplate (Fn in_fn) : fn (in_fn) {}

  private:
    void Run ()
    {
      fn ();
    }

  private:
    Fn fn;
};

template <class Fn>
PlatformThread* make_thread (Fn fn)
{
  return new ThreadTemplate<Fn> (fn);
}

}

/*
    Описание реализации FunnerEngine
*/

struct FunnerEngine::Impl
{
  FB::PluginWindow*              plugin_window;
  std::auto_ptr<engine::IWindow> engine_window;
  std::auto_ptr<PlatformThread>  thread;
  
  Impl ()
    : plugin_window (0)
  {
  }
  
  ~Impl ()
  {
    if (&*thread)
      thread->Terminate ();
  }
  
  void ThreadRoutine (void* wnd, size_t width, size_t height)
  {
    engine::IEngine& engine = EngineHolder::Instance ();
    
    engine.SetBaseDir ("/std/d:/---");

    std::auto_ptr<engine::IWindow> window (engine.CreateWindow (WINDOW_NAME));

    window->SetParentHandle (wnd);
    window->SetSize         (width, height);
    window->Show            (true);
    
    engine_window = window;

    engine.Run ();
  }

  bool CreateWindow (FB::PluginWindow *new_window)
  {
    if (&*engine_window)
      return false;
    
    if (!new_window)
      return false;                

#ifdef _WIN32
    if (FB::PluginWindowWin* window_win = dynamic_cast<FB::PluginWindowWin*> (new_window))
    {
      RECT window_rect;    

      if (!::GetClientRect (window_win->getHWND (), &window_rect))
        return false;

      thread = std::auto_ptr<PlatformThread> (make_thread (boost::bind (&Impl::ThreadRoutine, this, reinterpret_cast<void*> (window_win->getHWND ()),
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top)));
        
      thread->Start ();
    }

#endif

    plugin_window = new_window;
    
    return true;
  }  
  
  void DestroyWindow (FB::PluginWindow *window)
  {
    if (window != plugin_window)
      return;

    plugin_window = 0;

    engine_window.reset ();

    thread->Terminate ();

    thread.reset ();
  }
};

void FunnerEngine::StaticInitialize()
{
  // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
  // this will only execute once per process, just a guarantee that it won't execute again until
  // after StaticDeinitialize is called
}

void FunnerEngine::StaticDeinitialize()
{
  // Place one-time deinitialization stuff here
}

FunnerEngine::FunnerEngine()
  : impl (new Impl)
{
}

FunnerEngine::~FunnerEngine()
{
}

FB::JSAPI* FunnerEngine::createJSAPI()
{
  // m_host is the BrowserHostWrapper
  return new FunnerEngineAPI(m_host);
}

bool FunnerEngine::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
  return false;
}

bool FunnerEngine::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
  return false;
}

bool FunnerEngine::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
  return false;
}

bool FunnerEngine::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
{
  return impl->CreateWindow (window);
}

bool FunnerEngine::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow* window)
{
  impl->DestroyWindow (window);  

  return true;
}
