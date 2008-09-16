#include <cstdio>
#include <cfloat>
#include <exception>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/log.h>

#include <syslib/application.h>
#include <syslib/timer.h>
#include <syslib/window.h>

#include <sg/scene.h>
#include <sg/visual_model.h>
#include <sg/camera.h>

#include <render/screen.h>
#include <render/scene_render.h>

#include <tools/ui/main_window.h>
#include <tools/ui/custom_window_system.h>

#include <client/engine.h>
#include <client/window_manager.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace tools::ui;
using namespace scene_graph;
using namespace client;
using namespace render;

extern "C"
{

extern double _HUGE = DBL_MAX;

}

namespace
{

/*
     онстанты
*/

const char* MODEL_MESH_NAME             = "media/mesh/trajectory.xmesh";
const char* MODEL2_MESH_NAME            = "media/mesh/envelope.xmesh";
const char* ENGINE_CONFIGURATION_BRANCH = "Configuration";
const char* WINDOW_CONFIGURATION_BRANCH = "Configuration.Window";
const char* SCREEN_ATTACHMENT_NAME      = "MainScreen";

//тестовое пользовательское дочернее окно
class MyChildWindow: public ICustomChildWindow, public xtl::reference_counter
{
  public:
/// онструктор
    MyChildWindow (const EngineAttachments& attachments)
      : window (syslib::WindowStyle_PopUp),
        engine (ENGINE_CONFIGURATION_BRANCH, StartupGroup_Level2),
        invalidate_timer (xtl::bind (&MyChildWindow::OnTime, this), 10)
    {
        //установка точек прив€зки

      engine.Attach (attachments);            
      
        //регистраци€ окна

      common::VarRegistry window_config (WINDOW_CONFIGURATION_BRANCH);

      WindowManager::InitWindow (window, window_config, engine);            

        //запуск систем движка

      engine.Start (StartupGroup_LevelMax);      
    }

///»зменение положени€ окна
    void SetPosition (size_t x, size_t y)
    {
      try
      {
        window.SetPosition (x, y);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("MyChildWindow::SetPosition");
        throw;
      }
    }

///»зменение размеров окна
    void SetSize (size_t width, size_t height)
    {
      try
      {
        window.SetSize (width, height);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("MyChildWindow::SetSize");
        throw;
      }
    }
    
///”становка родительского окна
    void SetParent (const void* parent_window_handle)
    {
      try
      {
        window.SetParentHandle (parent_window_handle);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("MyChildWindow::SetParent");
        throw;
      }
    }    

///”правление видимостью окна
    void Show (bool state)
    {
      try
      {
        window.SetVisible (state);

        if (state)
          window.Invalidate ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("MyChildWindow::Show");
        throw;
      }
    }

///ѕодсчЄт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    void OnTime ()
    {
      window.Invalidate ();
    }

  private:
    OrthoCamera::Pointer camera;
    syslib::Window       window;
    Engine               engine;
    syslib::Timer        invalidate_timer;
};

//тестовый сервер приложени€
class MyApplicationServer: public IApplicationServer, public xtl::reference_counter
{
  public:
    MyApplicationServer ()
    {
      VisualModel::Pointer model = VisualModel::Create ();
      VisualModel::Pointer envelope = VisualModel::Create ();

      model->SetMeshName (MODEL_MESH_NAME);
      model->SetName     ("Trajectory");
      model->BindToScene (scene);

//      model->Scale (-2.01f, 2.01f, 2.01f);
      model->Scale (-2.f, 2.f, 2.f);

      envelope->SetMeshName (MODEL2_MESH_NAME);
      envelope->BindToScene (scene);
      envelope->SetName     ("Envelope");

      camera = OrthoCamera::Create ();

      camera->SetLeft   (-3);
      camera->SetRight  (3);
      camera->SetBottom (-3);
      camera->SetTop    (3);
      camera->SetZNear  (0);
      camera->SetZFar   (100);
      
      camera->SetPosition (0, 0, 4);
      camera->LookTo      (math::vec3f (0.0f), NodeOrt_Z, NodeOrt_Y, NodeTransformSpace_World);
      
      math::vec3f dir = camera->WorldOrtZ (),
                  p   = invert (camera->WorldTM ()) * math::vec3f (0, 0, 4),
                  p1  = invert (camera->WorldTM ()) * math::vec4f (0, 0, 1, 0),
                  p3  = invert (camera->WorldTM ()) * math::vec3f (0, 0, 0);
      
      printf ("Camera dir: %g %g %g\n", dir.x, dir.y, dir.z);
      printf ("Light:      %g %g %g\n", p.x, p.y, p.z);
      printf ("Point:      %g %g %g\n", p3.x, p3.y, p3.z);
      printf ("Normal:     %g %g %g\n", p1.x, p1.y, p1.z);

      camera->BindToScene (scene);
      
      screen.SetBackgroundColor (0.f, 0.05f, 0.2f, 0);
      
      Viewport viewport;
      
      viewport.SetArea       (0, 0, 100, 100);
      viewport.SetCamera     (&*camera);
      viewport.SetRenderPath ("ModelerRender");

      screen.Attach (viewport);

      attachments.SetScreen (SCREEN_ATTACHMENT_NAME, &screen);
    }

    void ExecuteCommand (const char* command)
    {
      if (!xtl::xstrcmp (command, "close"))
        syslib::Application::Exit (0);
      
      printf ("MyApplicationServer::ExecuteCommand(%s)\n", command);
    }

    void SetProperty (const char* name, const stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::SetProperty");
    } 
    
    void GetProperty (const char* name, stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::GetProperty");
    }
    
    bool IsPropertyPresent (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }

    bool IsPropertyReadOnly (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }    

    void RegisterPropertyListener (const char* name_wc_mask, IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::RegisterPropertyListener");
    }

    void UnregisterPropertyListener (const char* name_wc_mask, IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterPropertyListener");
    }
    
    void UnregisterAllPropertyListeners (const char* name_wc_mask)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");
    }
    
    void UnregisterAllPropertyListeners ()
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");    
    }
    
    ICustomChildWindow* CreateChildWindow (const char* init_string)
    {
      return new MyChildWindow (attachments);
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }
    
  private:
    Scene                scene;
    OrthoCamera::Pointer camera;
    Screen               screen;
    EngineAttachments    attachments;
};

//класс тестового приложени€
struct Test
{
  Test () : log ("modeler.test")
  {
      //регистраци€ сервера приложени€

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer",
      xtl::com_ptr<MyApplicationServer> (new MyApplicationServer, false).get ());

      //создание главного окна

    MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);

      //подписка на событи€ протоколировани€

    main_window.SetLogHandler (xtl::bind (&Test::LogPrint, this, _1));
  }
  
  void LogPrint (const char* message)
  {
    log.Print (message);
  }

  common::Log  log;
  MainWindow   main_window;
};

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

}

int main ()
{
  try
  {
    common::LogFilter log_filter ("*", &log_print);
    
    Test test;

    test.main_window.ExecuteFile ("media/ui.lua");

    syslib::Application::Run ();
    
    return syslib::Application::GetExitCode ();    
  }
  catch (xtl::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
