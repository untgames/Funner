#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

const char* LOG_NAME = "android.syslib.window";

}

/*
    Окно
*/

enum { ScreenKeyboardType_Num = ScreenKeyboardType_PlatformSpecific };

typedef xtl::trackable_ptr<screen_keyboard_handle> screen_keyboard_ptr;

struct syslib::window_handle: public MessageQueue::Handler
{
  global_ref<jobject>  controller;                                //контроллер android окна
  global_ref<jobject>  view;                                      //android окно
  screen_keyboard_ptr  screen_keyboards [ScreenKeyboardType_Num]; //экранные клавиатуры
  WindowMessageHandler message_handler;                           //обработчик сообщений
  void*                user_data;                                 //пользовательские данные окна
  unsigned int         background_color;                          //цвет заднего плана
  unsigned int         background_state;                          //наличие заднего фона
  jmethodID            get_view_method;                           //метод получения окна
  jmethodID            get_top_method;                            //метод получения верхнего угла окна
  jmethodID            get_left_method;                           //метод получения левого угла окна
  jmethodID            get_width_method;                          //метод получения ширины окна
  jmethodID            get_height_method;                         //метод получения высоты окна
  jmethodID            layout_method;                             //метод установки размеров и положения окна
  jmethodID            set_visibility_method;                     //метод установки видимости окна
  jmethodID            get_visibility_method;                     //метод получения видимости окна
  jmethodID            clear_focus_method;                        //метод отмены фокуса ввода
  jmethodID            request_focus_method;                      //метод запроса фокуса ввода
  jmethodID            set_background_color_method;               //метод установки цвета заднего плана окна
  jmethodID            maximize_method;                           //метод максимизации окна
  jmethodID            get_surface_method;                        //метод получения поверхности
  jmethodID            post_invalidate_method;                    //метод оповещения о необходимости перерисовки окна
  jmethodID            bring_to_front_method;                     //метод перемещения окна на передний план
  jmethodID            remove_from_parent_window_method;          //метод удаления окна
  bool                 is_multitouch_enabled;                     //включен ли multitouch
  int                  current_touch_id;                          //текущий идентификатор касания
  bool                 is_surface_created;                        //состояние поверхности
  volatile bool        is_native_handle_received;                 //получен ли android window handle
  common::Log          log;                                       //поток протоколирования
  
///Конструктор
  window_handle ()
    : message_handler (0)
    , user_data (0)
    , background_color (0)
    , background_state (0)
    , is_multitouch_enabled (false)
    , current_touch_id (-1)
    , is_surface_created (false)
    , is_native_handle_received (false)
    , log (LOG_NAME)
  {    
    MessageQueueSingleton::Instance ()->RegisterHandler (*this);
  }

///Деструктор
  ~window_handle ()
  {
    try
    {
      JNIEnv& env = get_env ();
      
      env.CallVoidMethod (controller.get (), remove_from_parent_window_method);

      check_errors ();      
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::android::window_handle::~window_handle", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at syslib::android::window_handle::~window_handle");
    }
    
    MessageQueueSingleton::Instance ()->UnregisterHandler (*this);
  }
 
  void Notify (WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (this, event, context, user_data);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  
  void OnLayoutCallback (int left, int top, int right, int bottom)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    Notify (WindowEvent_OnMove, context);
    Notify (WindowEvent_OnSize, context);
  }

  void OnDisplayHintCallback (int hint)
  {
  }

  void OnDrawCallback ()
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    Notify (WindowEvent_OnPaint, context);
  }

  void FillTouchEventContext (int pointer_id, float x, float y, WindowEventContext& context)
  {
    memset (&context, 0, sizeof (context));

    context.touches_count = 1;

    Touch& touch = context.touches [0];

    touch.touch_id   = pointer_id;
    touch.position.x = x;
    touch.position.y = y;
  }

  void OnTouchCallback (int pointer_id, int action, float x, float y)
  {    
    WindowEventContext context;

    FillTouchEventContext (pointer_id, x, y, context);

    switch (action)
    {
      case AMOTION_EVENT_ACTION_DOWN:
        if (is_multitouch_enabled || pointer_id == current_touch_id || current_touch_id == -1)      
        {
          Notify (WindowEvent_OnTouchesBegan, context);
          current_touch_id = pointer_id;
        }

        break;
      case AMOTION_EVENT_ACTION_MOVE:
        if (is_multitouch_enabled || pointer_id == current_touch_id)
          Notify (WindowEvent_OnTouchesMoved, context);

        break;
      case AMOTION_EVENT_ACTION_UP:
      case AMOTION_EVENT_ACTION_CANCEL:
        if (is_multitouch_enabled || pointer_id == current_touch_id)            
          Notify (WindowEvent_OnTouchesEnded, context);
          
        current_touch_id = -1;
          
        break;
      default:
        log.Printf ("Unhandled touch action %d received\n", action);
        break;
    }
  }

  void OnDoubletapCallback (int pointer_id, float x, float y)
  {
    WindowEventContext context;

    FillTouchEventContext (pointer_id, x, y, context);

    Notify (WindowEvent_OnTouchesDoubletap, context);
  }

  void OnTrackballCallback (int pointer_id, int action, float x, float y)
  {
//    printf ("on_trackball_callback(%d, %d, %g, %g)\n", pointer_id, action, x, y);
//    fflush (stdout);
  }

  void OnKeyCallback (int key, int action, bool is_alt_pressed, bool is_shift_pressed, bool is_sym_pressed, int unicode_char)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    context.keyboard_alt_pressed     = is_alt_pressed;
    context.keyboard_shift_pressed   = is_shift_pressed;
    context.keyboard_control_pressed = is_sym_pressed;
    context.key_scan_code            = (ScanCode)key;

    Key syslib_key = Key_Unknown;

    switch (key)
    {
      case AKEYCODE_0:             syslib_key = Key_0;            break;
      case AKEYCODE_1:             syslib_key = Key_1;            break;
      case AKEYCODE_2:             syslib_key = Key_2;            break;
      case AKEYCODE_3:             syslib_key = Key_3;            break;
      case AKEYCODE_4:             syslib_key = Key_4;            break;
      case AKEYCODE_5:             syslib_key = Key_5;            break;
      case AKEYCODE_6:             syslib_key = Key_6;            break;
      case AKEYCODE_7:             syslib_key = Key_7;            break;
      case AKEYCODE_8:             syslib_key = Key_8;            break;
      case AKEYCODE_9:             syslib_key = Key_9;            break;
      case AKEYCODE_A:             syslib_key = Key_A;            break;
      case AKEYCODE_B:             syslib_key = Key_B;            break;
      case AKEYCODE_C:             syslib_key = Key_C;            break;
      case AKEYCODE_D:             syslib_key = Key_D;            break;
      case AKEYCODE_E:             syslib_key = Key_E;            break;
      case AKEYCODE_F:             syslib_key = Key_F;            break;
      case AKEYCODE_G:             syslib_key = Key_G;            break;
      case AKEYCODE_H:             syslib_key = Key_H;            break;
      case AKEYCODE_I:             syslib_key = Key_I;            break;
      case AKEYCODE_J:             syslib_key = Key_J;            break;
      case AKEYCODE_K:             syslib_key = Key_K;            break;
      case AKEYCODE_L:             syslib_key = Key_L;            break;
      case AKEYCODE_M:             syslib_key = Key_M;            break;
      case AKEYCODE_N:             syslib_key = Key_N;            break;
      case AKEYCODE_O:             syslib_key = Key_O;            break;
      case AKEYCODE_P:             syslib_key = Key_P;            break;
      case AKEYCODE_Q:             syslib_key = Key_Q;            break;
      case AKEYCODE_R:             syslib_key = Key_R;            break;
      case AKEYCODE_S:             syslib_key = Key_S;            break;
      case AKEYCODE_T:             syslib_key = Key_T;            break;
      case AKEYCODE_U:             syslib_key = Key_U;            break;
      case AKEYCODE_V:             syslib_key = Key_V;            break;
      case AKEYCODE_W:             syslib_key = Key_W;            break;
      case AKEYCODE_X:             syslib_key = Key_X;            break;
      case AKEYCODE_Y:             syslib_key = Key_Y;            break;
      case AKEYCODE_Z:             syslib_key = Key_Z;            break;
      case AKEYCODE_COMMA:         syslib_key = Key_Comma;        break;
      case AKEYCODE_PERIOD:        syslib_key = Key_Dot;          break;
      case AKEYCODE_ALT_LEFT:
      case AKEYCODE_ALT_RIGHT:     syslib_key = Key_Alt;          break;
      case AKEYCODE_SHIFT_LEFT:
      case AKEYCODE_SHIFT_RIGHT:   syslib_key = Key_Shift;        break;
      case AKEYCODE_TAB:           syslib_key = Key_Tab;          break;
      case AKEYCODE_SPACE:         syslib_key = Key_Space;        break;
      case AKEYCODE_ENTER:         syslib_key = Key_Enter;        break;
      case AKEYCODE_DEL:           syslib_key = Key_BackSpace;    break;
      case AKEYCODE_MINUS:         syslib_key = Key_Minus;        break;
      case AKEYCODE_LEFT_BRACKET:  syslib_key = Key_LeftBracket;  break;
      case AKEYCODE_RIGHT_BRACKET: syslib_key = Key_RightBracket; break;
      case AKEYCODE_BACKSLASH:     syslib_key = Key_BackSlash;    break;
      case AKEYCODE_SEMICOLON:     syslib_key = Key_Semicolon;    break;
      case AKEYCODE_APOSTROPHE:    syslib_key = Key_Apostrophe;   break;
      case AKEYCODE_SLASH:         syslib_key = Key_Slash;        break;
      case AKEYCODE_PLUS:          syslib_key = Key_Plus;         break;
      case AKEYCODE_PAGE_UP:       syslib_key = Key_PageUp;       break;
      case AKEYCODE_PAGE_DOWN:     syslib_key = Key_PageDown;     break;
      case AKEYCODE_BACK:          syslib_key = Key_Back;         break;
      case AKEYCODE_HOME:          syslib_key = Key_HomeScreen;   break;
      case AKEYCODE_MENU:          syslib_key = Key_Menu;         break;
      case AKEYCODE_SEARCH:        syslib_key = Key_Search;       break;
    }

    context.key = syslib_key;
    
    static const int ACTION_DOWN = 0, ACTION_UP = 1, ACTION_MULTIPLE = 2;

    switch (action)
    {
      case ACTION_DOWN:
        Notify (WindowEvent_OnKeyDown, context);

        context.char_code = (wchar_t)unicode_char;

        Notify (WindowEvent_OnChar, context);
        break;
      case ACTION_MULTIPLE:
        context.char_code = (wchar_t)unicode_char;

        Notify (WindowEvent_OnChar, context);
        break;
      case ACTION_UP:
        Notify (WindowEvent_OnKeyUp, context);
        break;
      default:
        break;
    }
  }

  void OnFocusCallback (bool gained)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));
        
    if (gained) Notify (WindowEvent_OnSetFocus, context);
    else        Notify (WindowEvent_OnLostFocus, context);
  }

  void OnVisibilityCallback (bool is_visible)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    if (is_visible) Notify (WindowEvent_OnShow, context);
    else            Notify (WindowEvent_OnHide, context);
  }

  void OnSurfaceCreatedCallback ()
  {
    if (is_surface_created)
      return;
      
    log.Printf ("Surface created for window %p", view.get ());

      //получение поверхности

    local_ref<jobject> surface = check_errors (get_env ().CallObjectMethod (controller.get (), get_surface_method));

    if (!surface)
      throw xtl::format_operation_exception ("", "EngineViewController::getSurfaceThreadSafe failed");                  

    is_surface_created = true;

      //оповещение об изменении дескриптора
    
    WindowEventContext context;

    memset (&context, 0, sizeof (context));    
    
    context.handle = (void*)view.get ();

    Notify (WindowEvent_OnChangeHandle, context); 
  }

  void OnSurfaceDestroyedCallback ()
  {
    if (!is_surface_created)
      return;    
      
    log.Printf ("Surface destroyed for window %p", view.get ());

    is_surface_created = false;
    
      //оповещение об изменении дескриптора

    WindowEventContext context;

    memset (&context, 0, sizeof (context));    

    Notify (WindowEvent_OnChangeHandle, context);
  }  
};

namespace
{

///Константы
const int VIEW_VISIBLE   = 0; //окно видимо
const int VIEW_INVISIBLE = 4; //окно невидимо
const int VIEW_GONE      = 8; //окно невидимо и исключено из расчёта лэйаута

///Поддержка класса окна
class JniWindowManager
{
  public:
///Конструктор
    JniWindowManager ()
    {
      try
      {          
          //получение класса Activity

        activity_class = get_env ().GetObjectClass (get_activity ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
          
        create_surface_view_controller_method = find_method (&get_env (), activity_class.get (), "createSurfaceViewController", "(Ljava/lang/String;J)Lcom/untgames/funner/application/EngineViewController;");
        create_web_view_controller_method     = find_method (&get_env (), activity_class.get (), "createWebViewController", "(Ljava/lang/String;J)Lcom/untgames/funner/application/EngineViewController;");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowManager::JniWindowManager");
        throw;
      }
    }
    
///Создание окна
    local_ref<jobject> CreateSurfaceViewController (const char* init_string, void* window_ref)
    {
      local_ref<jobject> controller = check_errors (get_env ().CallObjectMethod (get_activity (), create_surface_view_controller_method, tojstring (init_string).get (), window_ref));

      if (!controller)
        throw xtl::format_operation_exception ("", "EngineActivity::createSurfaceViewController failed");
        
      return controller;
    }
    
    local_ref<jobject> CreateWebViewController (const char* init_string, void* window_ref)
    {
      local_ref<jobject> controller = check_errors (get_env ().CallObjectMethod (get_activity (), create_web_view_controller_method, tojstring (init_string).get (), window_ref));

      if (!controller)
        throw xtl::format_operation_exception ("", "EngineActivity::createWebViewController failed");
        
      return controller;
    }    
    
///Поиск дескриптора окна
    static window_t FindWindow (jobject controller)
    {
      try
      {
        if (!controller)
          return 0;

        JNIEnv& env = get_env ();

        local_ref<jclass> controller_class = env.GetObjectClass (controller);

        if (!controller_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for EngineViewController)");
        
        jmethodID get_window_ref = env.GetMethodID (controller_class.get (), "getWindowRef", "()J");

        if (!get_window_ref)
          return 0;

        return (window_t)check_errors (env.CallLongMethod (controller, get_window_ref));
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::AndroidWindowManager::FindWindow");
        throw;
      }
    }
    
  private:
    global_ref<jclass> activity_class;
    jmethodID          create_surface_view_controller_method;
    jmethodID          create_web_view_controller_method;    
};

typedef common::Singleton<JniWindowManager> JniWindowManagerSingleton;

/*
    Функции обратного вызова
*/

template <class Fn> class WindowMessage: public MessageQueue::Message
{
  public:
    WindowMessage (window_t in_window, const Fn& in_fn)
      : window (in_window)
      , fn (in_fn) {}

    void Dispatch ()
    {
      fn (window);
    }

  private:
    window_t window;
    Fn       fn;
};

template <class Fn> void push_message (jobject controller, const Fn& fn)
{
  try
  {
    window_t window = JniWindowManager::FindWindow (controller);

    if (!window)
      return;

    MessageQueueSingleton::Instance ()->PushMessage (*window, MessageQueue::MessagePtr (new WindowMessage<Fn> (window, fn), false));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void on_layout_callback (JNIEnv& env, jobject controller, int left, int top, int right, int bottom)
{
  push_message (controller, xtl::bind (&window_handle::OnLayoutCallback, _1, left, top, right, bottom));
}

void on_display_hint_callback (JNIEnv& env, jobject controller, int hint)
{
  push_message (controller, xtl::bind (&window_handle::OnDisplayHintCallback, _1, hint));
}

void on_draw_callback (JNIEnv& env, jobject controller)
{
  push_message (controller, xtl::bind (&window_handle::OnDrawCallback, _1));
}

void on_touch_callback (JNIEnv& env, jobject controller, int pointer_id, int action, float x, float y)
{
  push_message (controller, xtl::bind (&window_handle::OnTouchCallback, _1, pointer_id, action, x, y));
}

void on_doubletap_callback (JNIEnv& env, jobject controller, int pointer_id, float x, float y)
{
  push_message (controller, xtl::bind (&window_handle::OnDoubletapCallback, _1, pointer_id, x, y));
}

void on_trackball_callback (JNIEnv& env, jobject controller, int pointer_id, int action, float x, float y)
{
  push_message (controller, xtl::bind (&window_handle::OnTrackballCallback, _1, pointer_id, action, x, y));
}

void on_key_callback (JNIEnv& env, jobject controller, int key, int action, jboolean is_alt_pressed, jboolean is_shift_pressed, jboolean is_sym_pressed, jint unicode_char)
{
  push_message (controller, xtl::bind (&window_handle::OnKeyCallback, _1, key, action, is_alt_pressed != 0, is_shift_pressed != 0, is_sym_pressed != 0, unicode_char));
}

void on_focus_callback (JNIEnv& env, jobject controller, jboolean gained)
{
  push_message (controller, xtl::bind (&window_handle::OnFocusCallback, _1, gained != 0));
}

void on_visibility_callback (JNIEnv& env, jobject controller, jboolean is_visible)
{
  push_message (controller, xtl::bind (&window_handle::OnVisibilityCallback, _1, is_visible != 0));
}

void on_surface_created_callback (JNIEnv& env, jobject controller)
{
  try
  {
    push_message (controller, xtl::bind (&window_handle::OnSurfaceCreatedCallback, _1));
    push_message (controller, xtl::bind (&window_handle::OnDrawCallback, _1));

    window_t window = JniWindowManager::FindWindow (controller);

    if (!window)
      return;    

    window->is_native_handle_received = true;
  }
  catch (...)
  {
  }
}

void on_surface_changed_callback (JNIEnv& env, jobject controller, jint format, jint width, jint height)
{
  push_message (controller, xtl::bind (&window_handle::OnDrawCallback, _1));    
}

void on_surface_destroyed_callback (JNIEnv& env, jobject controller)
{
  push_message (controller, xtl::bind (&window_handle::OnSurfaceDestroyedCallback, _1));
}

}

/*
    Создание/закрытие/уничтожение окна
*/

window_t AndroidWindowManager::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data, WindowType type, void** out_view_controller)
{
  try
  {
    if (!handler)
      throw xtl::make_null_argument_exception ("", "handler");
      
    stl::auto_ptr<window_handle> window (new window_handle);

    switch (type)
    {
      case WindowType_Surface:
        window->controller = JniWindowManagerSingleton::Instance ()->CreateSurfaceViewController (init_string, window.get ());
        break;
      case WindowType_WebView:
        window->controller = JniWindowManagerSingleton::Instance ()->CreateWebViewController (init_string, window.get ());
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }

    window->message_handler = handler;
    window->user_data       = user_data;

      //получение методов
    
    JNIEnv& env = get_env ();

    local_ref<jclass> controller_class = env.GetObjectClass (window->controller.get ());
        
    if (!controller_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed (for View)");      

    window->get_view_method                  = find_method (&env, controller_class.get (), "getView", "()Landroid/view/View;");
    window->get_left_method                  = find_method (&env, controller_class.get (), "getLeftThreadSafe", "()I");
    window->get_top_method                   = find_method (&env, controller_class.get (), "getTopThreadSafe", "()I");
    window->get_width_method                 = find_method (&env, controller_class.get (), "getWidthThreadSafe", "()I");
    window->get_height_method                = find_method (&env, controller_class.get (), "getHeightThreadSafe", "()I");
    window->layout_method                    = find_method (&env, controller_class.get (), "layoutThreadSafe", "(IIII)V");
    window->set_visibility_method            = find_method (&env, controller_class.get (), "setVisibilityThreadSafe", "(I)V");
    window->get_visibility_method            = find_method (&env, controller_class.get (), "getVisibilityThreadSafe", "()I");
    window->clear_focus_method               = find_method (&env, controller_class.get (), "clearFocusThreadSafe", "(Landroid/content/Context;)V");
    window->request_focus_method             = find_method (&env, controller_class.get (), "requestFocusThreadSafe", "()Z");
    window->bring_to_front_method            = find_method (&env, controller_class.get (), "bringToFrontThreadSafe", "()V");
    window->set_background_color_method      = find_method (&env, controller_class.get (), "setBackgroundColorThreadSafe", "(I)V");
    window->maximize_method                  = find_method (&env, controller_class.get (), "maximizeThreadSafe", "()V");
    window->get_surface_method               = find_method (&env, controller_class.get (), "getSurfaceThreadSafe", "()Landroid/view/Surface;");
    window->post_invalidate_method           = find_method (&env, controller_class.get (), "postInvalidate", "()V");
    window->remove_from_parent_window_method = find_method (&env, controller_class.get (), "removeFromParentWindowThreadSafe", "()V");
    
      //получение объекта окна

    window->view = check_errors (env.CallObjectMethod (window->controller.get (), window->get_view_method));

    if (!window->view)
      throw xtl::format_operation_exception ("", "EngineViewController::getView failed");

      //ожидание создания поверхности
    
    if (type == WindowType_Surface)
    {
      size_t start_wait = common::milliseconds ();

      for (;;)
      {
        if (window->is_native_handle_received)
          break;

        static const size_t WAIT_TIME_IN_MICROSECONDS = 100*1000; //100 milliseconds

        usleep (WAIT_TIME_IN_MICROSECONDS);
        
        static const size_t MAX_TIMEOUT_IN_MILLISECONDS = 2000;
        
        if (common::milliseconds () - start_wait > MAX_TIMEOUT_IN_MILLISECONDS)
          throw xtl::format_operation_exception ("", "Can't create window because Surface has not been created for %u milliseconds", MAX_TIMEOUT_IN_MILLISECONDS);
      }

        //получение дескриптора поверхности

      local_ref<jobject> surface = check_errors (env.CallObjectMethod (window->controller.get (), window->get_surface_method));

      if (!surface)
        throw xtl::format_operation_exception ("", "EngineViewController::getSurfaceThreadSafe failed");                  

      window->is_surface_created = true;
    }
    
    if (out_view_controller)
      *out_view_controller = window->controller.get ();
    
    return window.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CreateWindow");
    throw;
  }
}

void AndroidWindowManager::CloseWindow (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    WindowEventContext context;

    memset (&context, 0, sizeof (context));          

    window->Notify (WindowEvent_OnClose, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CloseWindow");
    throw;
  }
}

void AndroidWindowManager::DestroyWindow (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    WindowEventContext context;

    memset (&context, 0, sizeof (context));          

    window->Notify (WindowEvent_OnDestroy, context);
    
    delete window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::CloseWindow");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* AndroidWindowManager::GetNativeWindowHandle (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    if (!window->is_surface_created)
      return 0;
      
    return (void*)window->view.get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetNativeWindowHandle");
    throw;
  }
}

const void* AndroidWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    Заголовок окна
*/

void AndroidWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
}

void AndroidWindowManager::GetWindowTitle (window_t, size_t size, wchar_t* buffer)
{
  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception ("syslib::AndroidWindowManager::GetWindowTitle", "buffer");

  *buffer = L'\0';
}

/*
    Область окна / клиентская область
*/

void AndroidWindowManager::SetWindowRect (window_t window, const Rect& rect)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();
    
    env.CallVoidMethod (window->controller.get (), window->layout_method, rect.left, rect.top, rect.right, rect.bottom);
    
    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetWindowRect");
    throw;
  }
}

void AndroidWindowManager::SetClientRect (window_t window, const Rect& rect)
{
  try
  {
    SetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetClientRect");
    throw;
  }
}

void AndroidWindowManager::GetWindowRect (window_t window, Rect& out_result)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    Rect result;
    
    JNIEnv& env = get_env ();
    
    result.left   = check_errors (env.CallIntMethod (window->controller.get (), window->get_left_method));
    result.top    = check_errors (env.CallIntMethod (window->controller.get (), window->get_top_method));
    result.right  = result.left + check_errors (env.CallIntMethod (window->controller.get (), window->get_width_method));
    result.bottom = result.top + check_errors (env.CallIntMethod (window->controller.get (), window->get_height_method));
    
    out_result = result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetWindowRect");
    throw;
  }
}

void AndroidWindowManager::GetClientRect (window_t window, Rect& rect)
{
  try
  {
    GetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void AndroidWindowManager::SetWindowFlag (window_t window, WindowFlag flag, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        env.CallVoidMethod (window->controller.get (), window->set_visibility_method, state ? VIEW_VISIBLE : VIEW_INVISIBLE);
        check_errors ();
        break;
      }
      case WindowFlag_Active:
        env.CallVoidMethod (window->controller.get (), window->bring_to_front_method);
        check_errors ();
        break;
      case WindowFlag_Focus:
        if (state)
        {
          if (!check_errors (env.CallBooleanMethod (window->controller.get (), window->request_focus_method)))
            throw xtl::format_operation_exception ("", "EngineViewController::requestFocusThreadSafe failed");
        }
        else
        {
          env.CallVoidMethod (window->controller.get (), window->clear_focus_method, get_activity ());
          check_errors ();
        }

        break;
      case WindowFlag_Maximized:
        env.CallVoidMethod (window->controller.get (), window->maximize_method);        
        check_errors ();
        break;        
      case WindowFlag_Minimized:
      {
        env.CallVoidMethod (window->controller.get (), window->set_visibility_method, VIEW_GONE);  
        check_errors ();      
        
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetWindowFlag");
    throw;
  }
}

bool AndroidWindowManager::GetWindowFlag (window_t window, WindowFlag flag)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
        return check_errors (env.CallIntMethod (window->controller.get (), window->get_visibility_method)) != 0;
      case WindowFlag_Maximized:
      case WindowFlag_Active:
      case WindowFlag_Focus:
      case WindowFlag_Minimized:
        throw xtl::make_not_implemented_exception ("GetWindowFlag");
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    Обновление окна
*/

void AndroidWindowManager::InvalidateWindow (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    get_env ().CallVoidMethod (window->controller.get (), window->post_invalidate_method);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::InvalidateWindow");
    throw;
  }
}

/*
   Установка multitouch режима для окна
*/

void AndroidWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    window->is_multitouch_enabled = enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetMultitouchEnabled");
    throw;
  }
}

bool AndroidWindowManager::IsMultitouchEnabled (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    return window->is_multitouch_enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::IsMultitouchEnabled");
    throw;
  }
}

/*
    Цвет фона
*/

void AndroidWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
        
    unsigned int int_color = window->background_state + (unsigned int)color.red * 0x10000 + (unsigned int)color.green * 0x100 +
      color.blue;
                             
    get_env ().CallVoidMethod (window->controller.get (), window->set_background_color_method, int_color);
    
    check_errors ();
    
    window->background_color = int_color & 0xffffffu;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetBackgroundColor");
    throw;
  }
}

void AndroidWindowManager::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    

    window->background_state = state ? 0xff000000u : 0u;

    get_env ().CallVoidMethod (window->controller.get (), window->set_background_color_method, window->background_color | window->background_state);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::SetBackgroundState");
    throw;
  }
}

Color AndroidWindowManager::GetBackgroundColor (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");        
    
    unsigned int int_color = window->background_color;
    
    Color result;
    
    result.blue  = int_color % 0x100;
    result.green = int_color / 0x100 % 0x100;
    result.red   = int_color / 0x10000 % 0x100;
    
    return result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetBackgroundColor");
    throw;
  }
}

bool AndroidWindowManager::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");        
    
    return window->background_state != 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidWindowManager::GetBackgroundState");
    throw;
  }
}

/*
    Создание и уничтожение экранной клавиатуры
*/

struct syslib::screen_keyboard_handle: public xtl::reference_counter, public xtl::trackable
{
  global_ref<jobject> view;                       //android окно
  global_ref<jobject> controller;                 //контроллер
  global_ref<jclass>  input_method_manager_class; //класс менеджера ввода
  global_ref<jobject> input_method_manager;       //менеджер ввода
  jmethodID           request_focus_method;       //метод запроса фокуса
  jmethodID           show_keyboard_method;       //метод показа клавиатуры
  jmethodID           hide_keyboard_method;       //метод скрытия клавиатуры

/// Конструктор
  screen_keyboard_handle (window_t window)
    : view (window->view)
    , controller (window->controller)
    , request_focus_method (window->request_focus_method)
  {
    JNIEnv& env = syslib::android::get_env ();

    jobject   activity           = get_activity ();
    jclass    activity_class     = env.GetObjectClass (activity);
    jmethodID get_system_service = find_method (&env, activity_class, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    input_method_manager = check_errors (env.CallObjectMethod (activity, get_system_service, tojstring ("input_method").get ()));

    if (!input_method_manager)
      throw xtl::format_operation_exception ("", "getSystemService for InputMethodManager failed");

    input_method_manager_class = env.GetObjectClass (input_method_manager.get ());

    if (!input_method_manager_class)
      throw xtl::format_operation_exception ("", "Can't get input manager class");

    show_keyboard_method = find_method (&env, input_method_manager_class.get (), "showSoftInput", "(Landroid/view/View;I)Z");
    hide_keyboard_method = find_method (&env, input_method_manager_class.get (), "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
  }
};

screen_keyboard_t AndroidScreenKeyboardManager::CreateScreenKeyboard (window_t window, ScreenKeyboardType type, const char*)
{  
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    if (type < 0 || type >= (ScreenKeyboardType)ScreenKeyboardType_Num)
      throw xtl::make_argument_exception ("", "type", type);

    screen_keyboard_ptr& keyboard = window->screen_keyboards [type];

    if (keyboard)
    {
      addref (&*keyboard);

      return keyboard.get ();
    }

    stl::auto_ptr<screen_keyboard_handle> new_keyboard (new screen_keyboard_handle (window));

    keyboard = new_keyboard.get ();

    return new_keyboard.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::AndroidScreenKeyboardManager::CreateScreenKeyboard");
    throw;
  }
}

void AndroidScreenKeyboardManager::DestroyScreenKeyboard (screen_keyboard_t keyboard)
{
  if (!keyboard)
    return;

  release (keyboard);
}

/*
    Показ и скрытие клавиатуры
*/

void AndroidScreenKeyboardManager::ShowScreenKeyboard (screen_keyboard_t keyboard)
{
  try
  {
    if (!keyboard)
      throw xtl::make_null_argument_exception ("", "keyboard");

    JNIEnv& env = syslib::android::get_env ();

    if (!check_errors (env.CallBooleanMethod (keyboard->controller.get (), keyboard->request_focus_method)))
      throw xtl::format_operation_exception ("", "EngineViewController::requestFocusThreadSafe failed");

    static const int SHOW_FORCED = 2;

    bool result = check_errors (env.CallBooleanMethod (keyboard->input_method_manager.get (), keyboard->show_keyboard_method, keyboard->view.get (), SHOW_FORCED));

    if (!result)
      throw xtl::format_operation_exception ("", "ShowScreenKeyboard failed");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::AndroidScreenKeyboardManager::ShowScreenKeyboard");
    throw;
  }
}

void AndroidScreenKeyboardManager::HideScreenKeyboard (screen_keyboard_t keyboard)
{
  try
  {
    if (!keyboard)
      throw xtl::make_null_argument_exception ("", "keyboard");

    JNIEnv& env = syslib::android::get_env ();

    jclass    view_class       = env.GetObjectClass (keyboard->view.get ());
    jmethodID get_window_token = find_method (&env, view_class, "getWindowToken", "()Landroid/os/IBinder;");
    jobject   window_token     = check_errors (env.CallObjectMethod (keyboard->view.get (), get_window_token));

    bool result = check_errors (env.CallBooleanMethod (keyboard->input_method_manager.get (), keyboard->hide_keyboard_method, window_token, 0));

    if (!result)
      throw xtl::format_operation_exception ("", "HideScreenKeyboard failed");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::AndroidScreenKeyboardManager::HideScreenKeyboard");
    throw;
  }
}

/*
    Проверка поддержки клавиатуры
*/

bool AndroidScreenKeyboardManager::IsScreenKeyboardSupported (ScreenKeyboardType type)
{
  switch (type)
  {
    case ScreenKeyboardType_Ascii:
      return true;
    default:
      return false;
  }
}

bool AndroidScreenKeyboardManager::IsScreenKeyboardSupported (const char*)
{
  return false;
}

namespace syslib
{

namespace android
{

/// регистрация методов обратного вызова окна
void register_window_callbacks (JNIEnv* env)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    jclass controller_class = env->FindClass ("com/untgames/funner/application/EngineViewController");

    if (!controller_class)
      throw xtl::format_operation_exception ("", "Can't find EngineViewController class\n");

    static const JNINativeMethod methods [] = {
      {"onLayoutCallback", "(IIII)V", (void*)&on_layout_callback},
      {"onDisplayHintCallback", "(I)V", (void*)&on_display_hint_callback},
      {"onDrawCallback", "()V", (void*)&on_draw_callback},
      {"onTouchCallback", "(IIFF)V", (void*)&on_touch_callback},
      {"onDoubletapCallback", "(IFF)V", (void*)&on_doubletap_callback},
      {"onTrackballCallback", "(IIFF)V", (void*)&on_trackball_callback},
      {"onKeyCallback", "(IIZZZI)V", (void*)&on_key_callback},
      {"onFocusCallback", "(Z)V", (void*)&on_focus_callback},
      {"onSurfaceCreatedCallback", "()V", (void*)&on_surface_created_callback},
      {"onSurfaceDestroyedCallback", "()V", (void*)&on_surface_destroyed_callback},
      {"onSurfaceChangedCallback", "(III)V", (void*)&on_surface_changed_callback},
      {"onVisibilityCallback", "(Z)V", (void*)&on_visibility_callback},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);
    
    jint status = env->RegisterNatives (controller_class, methods, methods_count);
    
    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_window_callbacks");
    throw;
  }
}

}

}
