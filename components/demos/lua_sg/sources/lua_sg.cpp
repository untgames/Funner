#include "shared.h"

static clock_t MOVE_PERIOD = CLOCKS_PER_SEC / 100;

const char* SCRIPT_FILE_NAME = "script/sg.lua";
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

void load_script (IInterpreter& interpreter, const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], buffer.size ());        

  interpreter.DoCommands (file_name, buffer.c_str (), buffer.size (), &log_print);    
}

void input_event_handler (const char* event, IInterpreter* interpreter)
{
  interpreter->DoCommands ("input_event", event, strlen (event), &log_print);
}

int main ()
{
  try
  {    
      //инициализация приложения

    TestApplication test;
    
      //инициализация рендера
      
    SceneRender& render = test.Render ();

      //создание сцены
      
    Scene scene;  
      
    OrthoCamera::Pointer camera;

    camera = OrthoCamera::Create ();    
    
      //создание областей вывода
      
    Viewport vp;
    Desktop  desktop;    
    
    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (camera.get ());
    
    vp.SetArea (0, 0, 100, 100);

    desktop.SetBackgroundColor (math::vec4f (1, 1, 1, 1));    
    desktop.Attach (vp);

    RenderTarget& render_target = test.RenderTarget ();

    render_target.SetDesktop (&desktop);        

    xtl::shared_ptr<Environment> env (new Environment);

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());                
  
    bind_math_library (*env);
    bind_scene_graph_library (*env);

    load_script (*script, SCRIPT_FILE_NAME);

    invoke<void> (*script, "set_root_node", xtl::ref (scene.Root ()));
    invoke<void> (*script, "set_camera", camera);

    input::TranslationMap translation_map (TRANSLATION_MAP_FILE_NAME);

    input::TranslationMap::EventHandler event_handler = xtl::bind (&input_event_handler, _1, script.get ());

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
