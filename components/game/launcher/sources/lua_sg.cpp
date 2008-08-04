#include "shared.h"

static clock_t MOVE_PERIOD = CLOCKS_PER_SEC / 100;

const char* SCRIPT_FILE_NAME = "media/scripts/sg.lua";
const char* TRANSLATION_MAP_FILE_NAME = "media/translation_table.keymap";

void idle (TestApplication& app, Shell& shell)
{
  try
  {
    static clock_t last_time = clock ();
    
    if (clock () - last_time > MOVE_PERIOD)
    {
      invoke<void> (*shell.Interpreter (), "idle", float (clock () - last_time) / CLOCKS_PER_SEC);

      last_time = clock ();
    }        

    app.PostRedraw ();
  }
  catch (std::exception& exception)
  {
    printf ("exception at idle: %s\n", exception.what ());
  }
}

/*
    Отладочное протоколирование
*/

inline void log_print (const char* message)
{
  printf ("%s\n", message);
}

/*
    Выполнение скрипта    
*/

void input_event_handler (const char* event, Shell& shell)
{
  shell.Execute (event, &log_print);
}

void set_camera (Camera* camera, Viewport& vp)
{
  vp.SetCamera (camera);
}

void app_exit ()
{
  syslib::Application::Exit (0);
}

int main ()
{
  try
  {    
      //инициализация приложения

    TestApplication test;
    
      //инициализация рендера
      
    SceneRender& render = test.Render ();

      //создание областей вывода
      
    Viewport vp;
    Desktop  desktop;    
    
    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    
    vp.SetArea (0, 0, 100, 100);

    desktop.SetBackgroundColor (math::vec4f (1, 1, 1, 1));    
    desktop.Attach (vp);

    RenderTarget& render_target = test.RenderTarget ();

    render_target.SetDesktop (&desktop);        

      //инициализация LUA

    xtl::shared_ptr<Environment> env (new Environment);

    Shell shell ("lua", env);

    bind_math_library (*env);
    bind_scene_graph_library (*env);

    InvokerRegistry& lib = env->Library ("global");

    lib.Register ("set_camera", make_invoker<void (Camera*)> (xtl::bind (&set_camera, _1, xtl::ref (vp))));
    lib.Register ("exit", make_invoker (&app_exit));

    shell.ExecuteFile (SCRIPT_FILE_NAME, &log_print);    

      //инициализация системы ввода

    input::TranslationMap translation_map (TRANSLATION_MAP_FILE_NAME);

    input::TranslationMap::EventHandler event_handler = xtl::bind (&input_event_handler, _1, xtl::ref (shell));

    test.InputDevice ().SetEventHandler (xtl::bind (&input::TranslationMap::ProcessEvent, &translation_map, _1, event_handler));
    
      //установка idle-функции

    test.SetIdleHandler (xtl::bind (&idle, _1, xtl::ref (shell)));    

      //запуск приложения

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
