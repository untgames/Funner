#include <cfloat>
#include <cstdio>
#include <direct.h>
#include <exception>
#include <process.h>

#include <stl/list>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parse_iterator.h>
#include <common/parser.h>
#include <common/strlib.h>
#include <common/var_registry.h>

#include <syslib/application.h>
#include <syslib/timer.h>
#include <syslib/window.h>

#include <sg/scene.h>
#include <sg/visual_model.h>
#include <sg/camera.h>

#include <media/rms/manager.h>

#include <render/screen.h>
#include <render/scene_render.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/shell.h>

#include <tools/ui/main_window.h>
#include <tools/ui/custom_window_system.h>

#include <engine/subsystem_manager.h>

#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace tools::ui;
using namespace scene_graph;
using namespace engine;
using namespace render;

extern "C"
{

extern double _HUGE = DBL_MAX;

}

namespace
{

/*
    Константы
*/

const char* TRAJECTORIES_REGISTRY_NAME          = "Configuration.Trajectories";
const char* APPLICATION_CONFIGURATION_FILE_NAME = "media/conf/application_config.xml";
const char* CONFIGURATION_FILE_NAME             = "media/conf/form_config.xml";
const char* SCREEN_ATTACHMENT_NAME              = "MainScreen";
const char* INTERPRETER_NAME                    = "lua";
const char* DESC_FILE_SUFFIX                    = ".desc";

//тестовое пользовательское дочернее окно
class MyChildWindow: public ICustomChildWindow, public xtl::reference_counter
{
  public:
///Конструктор
    MyChildWindow ()
      : window (syslib::WindowStyle_PopUp, 1024, 768)
    {
//      if (AttachmentRegistry::Find<syslib::Window> ("MainWindow"))
//        throw xtl::format_operation_exception ("MyChildWindow::MyChildWindow", "Window with attachment name 'MainWindow' already registered");

      AttachmentRegistry::Register ("MainWindow", xtl::ref (window));

      manager.Start (CONFIGURATION_FILE_NAME);
    }

///Изменение положения окна
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

///Изменение размеров окна
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

///Установка родительского окна
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

///Управление видимостью окна
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

///Подсчёт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    void OnTime ()
    {
      window.Invalidate ();
    }

  private:
    SubsystemManager     manager;                    //менеджер подсистем
    OrthoCamera::Pointer camera;
    syslib::Window       window;
};

//тестовый сервер приложения
class MyApplicationServer: public IApplicationServer, public xtl::reference_counter
{
  public:
    MyApplicationServer ()
      : shell_environment (new script::Environment),
        shell (INTERPRETER_NAME, shell_environment),
        project_path ("projects\\project1\\"),
        wait_trajectory_timer (xtl::bind (&MyApplicationServer::CheckNewTrajectories, this), 1000, syslib::TimerState_Paused),
        wait_envelope_timer (xtl::bind (&MyApplicationServer::CheckNewEnvelope, this), 1000, syslib::TimerState_Paused),
        envelope (VisualModel::Create ())
    {
      static const char* METHOD_NAME = "MyApplicationServer::MyApplicationServer";

      AttachmentRegistry::Register ("MeshResourceManager", resource_manager);

        //получение текущей папки

      char* buffer;

      if ((buffer = _getcwd (0, 0)) == 0)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get current directory, _getcwd error");
      else
      {
        working_directory = buffer;

        free (buffer);
      }
      
        //регистрация шлюзов

      shell_environment->BindLibraries ("*");

        //создание глобальной точки входа

      script::InvokerRegistry& lib = shell_environment->Library ("Modeler");

        //регистрация функций

      lib.Register ("CalculateEnvelope", script::make_invoker<void (size_t)> (xtl::bind (&MyApplicationServer::CalculateEnvelope, this, _1)));
      lib.Register ("CalculateTrajectory", script::make_invoker<void ()> (xtl::bind (&MyApplicationServer::CalculateTrajectory, this)));

        //чтение конфигурации

      common::Parser   parser     (APPLICATION_CONFIGURATION_FILE_NAME);
      common::ParseLog parser_log (parser.Log ());

      for (size_t i = 0; i < parser_log.MessagesCount (); i++)
      {
        common::ParseLogMessageType message_type = parser_log.MessageType (i);

        if (message_type == common::ParseLogMessageType_Error || message_type == common::ParseLogMessageType_FatalError)
          throw xtl::format_operation_exception (METHOD_NAME, "Configuration parse error: '%s'", parser_log.Message (i));
      }

      common::ParseIterator iter = parser.Root ().First ("Configuration");

      LoadEnvelope ();

      plugin_path = common::get<const char*> (*iter, "PluginPath");

      camera = OrthoCamera::Create ();

      camera->SetLeft   (-3);
      camera->SetRight  (3);
      camera->SetBottom (-3);
      camera->SetTop    (3);
      camera->SetZNear  (0);
      camera->SetZFar   (100);

      camera->SetPosition (0, 0, 4);
      camera->LookTo      (math::vec3f (0.0f), NodeOrt_Z, NodeOrt_Y, NodeTransformSpace_World);

      camera->BindToScene (scene);

      screen.SetBackgroundColor (0.f, 0.05f, 0.2f, 0);

      Viewport viewport;

      viewport.SetArea       (0, 0, 100, 100);
      viewport.SetCamera     (&*camera);
      viewport.SetRenderPath ("ModelerRender");

      screen.Attach (viewport);

      AttachmentRegistry::Register<Screen> (SCREEN_ATTACHMENT_NAME, screen);
    }

    void ExecuteCommand (const char* command)
    {
      if (!xtl::xstrcmp (command, "close"))
      {
        syslib::Application::Exit (0);
        return;
      }

      try
      {        
        shell.Execute (command);
      }
      catch (std::exception& exception)
      {
        common::Console::Print (exception.what ());
      }
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
      return new MyChildWindow ();
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    void CheckNewTrajectories ()
    {
      for (WaitedMeshes::iterator iter = waited_trajectories.begin (), end= waited_trajectories.end (); iter != end;)
      {
        stl::string file_path = project_path + iter->file_name;

        if (!common::FileSystem::IsFileExist (file_path.c_str ()))
        {
          ++iter;
          continue;
        }

        size_t file_size = common::FileSystem::GetFileSize (file_path.c_str ());

        if (!file_size)
        {
          ++iter;
          continue;
        }

        if (file_size != iter->current_file_size)
        {
          iter->current_file_size = file_size;
          ++iter;
          continue;
        }

        WaitedMeshes::iterator next = iter;

        ++next;

        VisualModel::Pointer trajectory = VisualModel::Create ();

        trajectory->SetMeshName (file_path.c_str ());
        trajectory->SetName     ("Trajectory");
        trajectory->BindToScene (scene);

        trajectory->Scale (-2.f, 2.f, 2.f);

        waited_trajectories.erase (iter);

        iter = next;
      }

      if (waited_trajectories.empty ())
        wait_trajectory_timer.Pause ();
    }

    void CalculateTrajectory ()
    {
      waited_trajectories.push_back (WaitedMesh ("trajectory.xmesh"));

      wait_trajectory_timer.Run ();
    }

    void LoadEnvelope ()
    {
      stl::string mesh_name = project_path + "envelope.binmesh";

      media::rms::Group resource_group;

      resource_group.Add (mesh_name.c_str ());

      resource_manager.CreateBinding (resource_group).Load ();

      if (!common::FileSystem::IsFileExist (mesh_name.c_str ()))
      {
        common::Console::Printf ("Can't load envelope '%s', no such file\n", mesh_name.c_str ());
        return;
      }

      envelope->SetMeshName (mesh_name.c_str ());
      envelope->BindToScene (scene);
      envelope->SetName     ("Envelope");
    }

    void CheckNewEnvelope ()
    {
      if (!common::FileSystem::IsFileExist (waited_envelope.file_name.c_str ()))
        return;

      if (waited_envelope.waiting_for_xmesh)
      {
        common::InputFile desc_file (waited_envelope.file_name.c_str ());

        size_t desc_file_data;

        desc_file.Read (&desc_file_data, sizeof (desc_file_data));

        waited_envelope.file_name.resize (waited_envelope.file_name.size () - xtl::xstrlen (DESC_FILE_SUFFIX));

        if (!desc_file_data || !common::FileSystem::IsFileExist (waited_envelope.file_name.c_str ()) ||
            (common::FileSystem::GetFileSize (waited_envelope.file_name.c_str ()) != desc_file_data))
        {
          common::Console::Print ("There was an error while calculating envelope\n");
          wait_envelope_timer.Pause ();
          return;
        }

        stl::string application_name  = common::format ("%s\\%smesh-converter.exe", working_directory.c_str (), plugin_path.c_str ()),
                    xmesh_file_name   = waited_envelope.file_name,
                    binmesh_file_name = common::format ("%senvelope.binmesh", project_path.c_str ());

        waited_envelope.file_name         = binmesh_file_name;
        waited_envelope.waiting_for_xmesh = false;
        waited_envelope.current_file_size = 0;

        _spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), xmesh_file_name.c_str (), binmesh_file_name.c_str (), 0);

        return;
      }

      size_t file_size = common::FileSystem::GetFileSize (waited_envelope.file_name.c_str ());

      if (!file_size)
        return;

      if (file_size != waited_envelope.current_file_size)
      {
        waited_envelope.current_file_size = file_size;
        return;
      }

      LoadEnvelope ();

      wait_envelope_timer.Pause ();
    }
    
    void CalculateEnvelope (size_t lod)
    {
      stl::string application_name      = common::format ("%s\\%smodeler-envelope.exe", working_directory.c_str (), plugin_path.c_str ()),
                  model_name            = common::format ("%smodel.dat", project_path.c_str ()),
                  envelope_name         = common::format ("%senvelope.xmesh", project_path.c_str ()),
                  binmesh_envelope_name = common::format ("%senvelope.binmesh", project_path.c_str ()),
                  lod_string            = common::format ("%u", lod);

      waited_envelope.file_name         = envelope_name + DESC_FILE_SUFFIX;
      waited_envelope.current_file_size = 0;
      waited_envelope.waiting_for_xmesh = true;

      common::FileSystem::Remove (envelope_name.c_str ());
      common::FileSystem::Remove (binmesh_envelope_name.c_str ());
      common::FileSystem::Remove (waited_envelope.file_name.c_str ());
      
      _spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), model_name.c_str (), envelope_name.c_str (), lod_string.c_str (), 0);

      wait_envelope_timer.Run ();
    }

  private:
    typedef xtl::shared_ptr<script::Environment> ShellEnvironmentPtr;

    struct WaitedMesh
    {
      stl::string file_name;
      size_t      current_file_size;
      bool        waiting_for_xmesh; //если true - то ожидается появление начальной текстовой модели, если false - то ожидается окончание конвертирование xmesh в binmesh

      WaitedMesh () {}
      WaitedMesh (const char* in_file_name) : file_name (in_file_name), current_file_size (0), waiting_for_xmesh (true) {}
    };

    typedef stl::list<WaitedMesh> WaitedMeshes;

  private:
    ShellEnvironmentPtr         shell_environment;     //окружение скриптовой среды
    script::Shell               shell;                 //скриптовый интерпретатор
    Scene                       scene;
    OrthoCamera::Pointer        camera;
    Screen                      screen;
    stl::string                 plugin_path;
    stl::string                 project_path;
    WaitedMeshes                waited_trajectories;
    syslib::Timer               wait_trajectory_timer;
    syslib::Timer               wait_envelope_timer;
    stl::string                 working_directory;
    VisualModel::Pointer        envelope;
    WaitedMesh                  waited_envelope;
    media::rms::ResourceManager resource_manager;
};

//класс тестового приложения
struct Test
{
  Test () : log ("modeler.test")
  {
      //регистрация сервера приложения

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer",
      xtl::com_ptr<MyApplicationServer> (new MyApplicationServer, false).get ());

      //создание главного окна

    MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);

      //подписка на события протоколирования

    main_window.SetLogHandler (xtl::bind (&Test::LogPrint, this, _1));
  }

  void LogPrint (const char* message)
  {
    log.Print (message);
  }

  common::Log  log;
  MainWindow   main_window;
};

void print (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

}

int main ()
{
  try
  {
    Test test;

    test.main_window.SetLogHandler (&print);
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
