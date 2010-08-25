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

namespace
{

void log (const char* msg, ...)
{
  va_list list;
  
  va_start (list, msg);    
  
  FILE* f = fopen ("D:/work/unt/funner/components/engine/wrappers/web/gen/log.txt", "a");
  
  if (f)
  {  
    vfprintf (f, msg, list);  
    fclose (f);
  }  
}

const char*    WINDOW_NAME          = "FunnerWebWindow";
const wchar_t* ENGINE_LIBRARY_NAME  = L"funner.dll";

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
        log ("Can't get module path");
        abort ();
      }
      
      wchar_t* s = module_path + wcslen (module_path);
      
      for (;s!=module_path && *s != L'\\';s--);
      
      *s = L'\0';
      
      if (!SetDllDirectory (module_path))
      {
        log ("Can't set DLL search directory\n");
        abort ();
      }

      *s++ = L'\\';

      wcscpy (s, ENGINE_LIBRARY_NAME);
      
      HMODULE library = LoadLibraryW (module_path);
      
      if (!library)
      {
        log ("Can't load engine library '%S'\n", module_path);
        abort ();
      }
      
      FunnerInitProc FunnerInit = (FunnerInitProc)GetProcAddress (library, "FunnerInit");  
      
      if (!FunnerInit)
      {
        log ("Bad library (entries not found)\n");
        abort ();
      }

      instance = std::auto_ptr<engine::IEngine> (FunnerInit ());

      if (!&*instance)
      {
        log ("Engine startup failed!");
        abort ();
      }
    }

#else
  #error Unsupported platform
#endif    

  private:
    std::auto_ptr<engine::IEngine> instance;
};

}

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
  //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
  return false;
}

bool FunnerEngine::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
  //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
  return false;
}

bool FunnerEngine::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
  //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
  return false;
}

bool FunnerEngine::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *new_window)
{
  // The window is attached; act appropriately
  
  if (!new_window)
    return false;

  std::auto_ptr<engine::IWindow> engine_window (EngineHolder::Instance ().CreateWindow (WINDOW_NAME));

#ifdef _WIN32
  if (FB::PluginWindowWin* window_win = dynamic_cast<FB::PluginWindowWin*> (new_window))
  {
    RECT window_rect;    

    if (!::GetClientRect (window_win->getHWND (), &window_rect))
      return false;

    engine_window->SetParentHandle (reinterpret_cast<void*> (window_win->getHWND ()));    
    engine_window->SetSize         (window_rect.right - window_rect.left, window_rect.bottom - window_rect.top);        
    
    wchar_t path[512];

    *path = 0;

    ::GetCurrentDirectoryW (sizeof (path), path);

    log ("path='%S'\n", path);

    log ("width=%u height=%u\n", engine_window->GetWidth (), engine_window->GetHeight ());    

    SetCurrentDirectoryW (L"d:/---");
    
    const char* args [] = {"npFunnerEngine.dll", "--no-main-loop"};
    
    EngineHolder::Instance ().ParseCommandLine (1, args);
    
    EngineHolder::Instance ().Run ();
  }
  
#endif

  window = engine_window;

  return true;
}

bool FunnerEngine::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow*)
{
  // The window is about to be detached; act appropriately
  
  window.reset ();
  
  return true;
}
