#include "shared.h"

using namespace syslib;
using namespace syslib::android;

//TODO: обработка исключений в CreateWindow и удаление окна
//TODO: wait for surface created

/*
    Окно
*/

struct Platform::window_handle
{
  global_ref<jobject>  view;                           //android окно
  ANativeWindow*       native_handle;                  //native android window hanle
  WindowMessageHandler message_handler;                //обработчик сообщений
  void*                user_data;                      //пользовательские данные окна
  unsigned int         background_color;               //цвет заднего плана
  unsigned int         background_state;               //наличие заднего фона
  jmethodID            get_top_method;                 //метод получения верхнего угла окна
  jmethodID            get_left_method;                //метод получения левого угла окна
  jmethodID            get_width_method;               //метод получения ширины окна
  jmethodID            get_height_method;              //метод получения высоты окна
  jmethodID            layout_method;                  //метод установки размеров и положения окна
  jmethodID            set_visibility_method;          //метод установки видимости окна
  jmethodID            get_visibility_method;          //метод получения видимости окна
  jmethodID            request_focus_method;           //метод запроса фокуса ввода  
  jmethodID            set_background_color_method;    //метод установки цвета заднего плана окна
  jmethodID            maximize_method;                //метод максимизации окна
  jmethodID            get_surface_method;             //метод получения поверхности
  jmethodID            post_invalidate_method;         //метод оповещения о необходимости перерисовки окна
  jmethodID            bring_to_front_method;          //метод перемещения окна на передний план
  
///Конструктор
  window_handle ()
    : native_handle (0)
    , message_handler (0)
    , user_data (0)
    , background_color (0)
    , background_state (0)
  {
    MessageQueueSingleton::Instance ()->RegisterHandler (this);
  }  

///Деструктор
  ~window_handle ()
  {
    MessageQueueSingleton::Instance ()->UnregisterHandler (this);
    
    if (native_handle)
      ANativeWindow_release (native_handle);
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
    printf ("on_display_hint_callback(%d)\n", hint);
    fflush (stdout);
  }

  void OnDrawCallback ()
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    Notify (WindowEvent_OnPaint, context);
  }

  void OnTouchCallback (int pointer_id, int action, float x, float y)
  {
//    printf ("on_touch_callback(%d, %d, %g, %g)\n", pointer_id, action, x, y);
//    fflush (stdout);
  }

  void OnTrackballCallback (int pointer_id, int action, float x, float y)
  {
//    printf ("on_trackball_callback(%d, %d, %g, %g)\n", pointer_id, action, x, y);
//    fflush (stdout);
  }

  void OnKeyCallback (int key, int action, bool is_alt_pressed, bool is_shift_pressed, bool is_sym_pressed)
  {
    WindowEventContext context;

    memset (&context, 0, sizeof (context));

    context.keyboard_alt_pressed     = is_alt_pressed;
    context.keyboard_shift_pressed   = is_shift_pressed;
    context.keyboard_control_pressed = is_sym_pressed;
    context.key_scan_code            = (ScanCode)key;
    context.key                      = Key_Unknown; ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    static const int ACTION_DOWN = 0, ACTION_UP = 1;
    
    switch (action)
    {
      case ACTION_DOWN:
        Notify (WindowEvent_OnKeyDown, context);
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

  void OnSurfaceCreatedCallback ()
  {
    printf ("on_surface_created_callback()\n");
    fflush (stdout);
  }

  void OnSurfaceDestroyedCallback ()
  {
    printf ("on_surface_destroyed_callback()\n");
    fflush (stdout);
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

        activity_class = get_env ().GetObjectClass (get_context ().activity.get ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
          
        create_view_method = find_method (&get_env (), activity_class.get (), "createEngineView", "(Ljava/lang/String;)Landroid/view/View;");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowManager::JniWindowManager");
        throw;
      }
    }
    
///Создание окна
    local_ref<jobject> CreateView (const char* init_string)
    {
      const ApplicationContext& context = get_context ();     
      
      local_ref<jobject> view = check_errors (get_env ().CallObjectMethod (context.activity.get (), create_view_method, tojstring (init_string)));
      
      if (!view)
        throw xtl::format_operation_exception ("", "EngineActivity::createEngineView failed");
        
      return view;
    }
    
///Регистрация дескриптора окна
    void RegisterWindow (jobject view, Platform::window_t window)
    {
      if (FindWindow (view))
        throw xtl::format_operation_exception ("syslib::android::JniWindowManager::RegisterWindow", "Window have already registered");
        
      windows.push_back (WindowEntry (view, window));
    }
    
    void UnregisterWindow (jobject view)
    {
      if (!view)
        return;
      
      JNIEnv& env = get_env ();
      
      for (WindowEntryList::iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
        if (env.IsSameObject (view, iter->view.get ()))
        {
          windows.erase (iter);
          
          return;
        }
    }
    
///Поиск дескриптора окна
    Platform::window_t FindWindow (jobject view) const
    {
      if (!view)
        return 0;
      
      JNIEnv& env = get_env ();
      
      for (WindowEntryList::const_iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
        if (env.IsSameObject (view, iter->view.get ()))
          return iter->window;
          
      return 0;
    }
    
  private:
    struct WindowEntry
    {
      global_ref<jobject> view;
      Platform::window_t  window;
      
      WindowEntry (jobject in_view, Platform::window_t in_window)
        : view (in_view)
        , window (in_window)
      {
      }
    };
  
    typedef stl::list<WindowEntry> WindowEntryList;

  private:
    global_ref<jclass> activity_class;
    jmethodID          create_view_method;
    WindowEntryList    windows;
};

typedef common::Singleton<JniWindowManager> JniWindowManagerSingleton;

/*
    Функции обратного вызова
*/

template <class Fn> class WindowMessage: public MessageQueue::Message
{
  public:
    WindowMessage (Platform::window_t in_window, const Fn& in_fn)
      : window (in_window)
      , fn (in_fn) {}

    void Dispatch ()
    {
      fn (window);
    }

  private:
    Platform::window_t window;
    Fn                 fn;
};

template <class Fn> void push_message (jobject view, const Fn& fn)
{
  try
  {
    Platform::window_t window = JniWindowManagerSingleton::Instance ()->FindWindow (view);
    
    if (!window)
      return;
    
    MessageQueueSingleton::Instance ()->PushMessage (window, MessageQueue::MessagePtr (new WindowMessage<Fn> (window, fn), false));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void on_layout_callback (JNIEnv& env, jobject view, int left, int top, int right, int bottom)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnLayoutCallback, _1, left, top, right, bottom));
}

void on_display_hint_callback (JNIEnv& env, jobject view, int hint)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnDisplayHintCallback, _1, hint));
}

void on_draw_callback (JNIEnv& env, jobject view)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnDrawCallback, _1));
}

void on_touch_callback (JNIEnv& env, jobject view, int pointer_id, int action, float x, float y)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnTouchCallback, _1, pointer_id, action, x, y));
}

void on_trackball_callback (JNIEnv& env, jobject view, int pointer_id, int action, float x, float y)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnTrackballCallback, _1, pointer_id, action, x, y));
}

void on_key_callback (JNIEnv& env, jobject view, int key, int action, jboolean is_alt_pressed, jboolean is_shift_pressed, jboolean is_sym_pressed)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnKeyCallback, _1, key, action, is_alt_pressed != 0, is_shift_pressed != 0, is_sym_pressed != 0));
}

void on_focus_callback (JNIEnv& env, jobject view, jboolean gained)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnFocusCallback, _1, gained != 0));
}

void on_surface_created_callback (JNIEnv& env, jobject view)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnSurfaceCreatedCallback, _1));
}

void on_surface_destroyed_callback (JNIEnv& env, jobject view)
{
  push_message (view, xtl::bind (&Platform::window_handle::OnSurfaceDestroyedCallback, _1));
}

}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    if (!handler)
      throw xtl::make_null_argument_exception ("", "handler");
      
    stl::auto_ptr<window_handle> window (new window_handle);
    
    window->view            = JniWindowManagerSingleton::Instance ()->CreateView (init_string);
    window->message_handler = handler;
    window->user_data       = user_data;
    
      //получение методов
    
    JNIEnv& env = get_env ();
    
    local_ref<jclass> view_class = env.GetObjectClass (window->view.get ());
        
    if (!view_class)
      throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
      
    window->get_left_method             = find_method (&env, view_class.get (), "getLeftThreadSafe", "()I");
    window->get_top_method              = find_method (&env, view_class.get (), "getTopThreadSafe", "()I");
    window->get_width_method            = find_method (&env, view_class.get (), "getWidthThreadSafe", "()I");
    window->get_height_method           = find_method (&env, view_class.get (), "getHeightThreadSafe", "()I");
    window->layout_method               = find_method (&env, view_class.get (), "layoutThreadSafe", "(IIII)V");
    window->set_visibility_method       = find_method (&env, view_class.get (), "setVisibilityThreadSafe", "(I)V");
    window->get_visibility_method       = find_method (&env, view_class.get (), "getVisibilityThreadSafe", "()I");
    window->request_focus_method        = find_method (&env, view_class.get (), "requestFocusThreadSafe", "()Z");
    window->bring_to_front_method       = find_method (&env, view_class.get (), "bringToFrontThreadSafe", "()V");
    window->set_background_color_method = find_method (&env, view_class.get (), "setBackgroundColorThreadSafe", "(I)V");
    window->maximize_method             = find_method (&env, view_class.get (), "maximizeThreadSafe", "()V");
    window->get_surface_method          = find_method (&env, view_class.get (), "getSurfaceThreadSafe", "()Landroid/view/Surface;");
    window->post_invalidate_method      = find_method (&env, view_class.get (), "postInvalidate", "()V");
    
      //получение нативного дескриптора окна
      
    local_ref<jobject> surface = check_errors (env.CallObjectMethod (window->view.get (), window->get_surface_method));
    
    if (!surface)
      throw xtl::format_operation_exception ("", "EngineView::getSurfaceThreadSafe failed");

    window->native_handle = ANativeWindow_fromSurface (&env, surface.get ());
    
    JniWindowManagerSingleton::Instance ()->RegisterWindow (window->view.get (), window.get ());

//    if (!window->native_handle) //callback processing
//      throw xtl::format_operation_exception ("", "ANativeWindow_fromSurface failed");

    return window.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::CreateWindow");
    throw;
  }
}

void Platform::CloseWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
    
    return window->native_handle;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::GetNativeWindowHandle");
    throw;
  }
}

const void* Platform::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
}

void Platform::GetWindowTitle (window_t, size_t size, wchar_t* buffer)
{
  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception ("syslib::AndroidPlatform::GetWindowTitle", "buffer");

  *buffer = L'\0';
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t window, const Rect& rect)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();
    
    env.CallVoidMethod (window->view.get (), window->layout_method, rect.left, rect.top, rect.right, rect.bottom);
    
    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::SetWindowRect");
    throw;
  }
}

void Platform::SetClientRect (window_t window, const Rect& rect)
{
  try
  {
    SetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::SetClientRect");
    throw;
  }
}

void Platform::GetWindowRect (window_t window, Rect& out_result)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    Rect result;
    
    JNIEnv& env = get_env ();
    
    result.left   = check_errors (env.CallIntMethod (window->view.get (), window->get_left_method));
    result.top    = check_errors (env.CallIntMethod (window->view.get (), window->get_top_method));
    result.right  = result.left + check_errors (env.CallIntMethod (window->view.get (), window->get_width_method));
    result.bottom = result.top + check_errors (env.CallIntMethod (window->view.get (), window->get_height_method));
    
    out_result = result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::GetWindowRect");
    throw;
  }
}

void Platform::GetClientRect (window_t window, Rect& rect)
{
  try
  {
    GetWindowRect (window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t window, WindowFlag flag, bool state)
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
        env.CallVoidMethod (window->view.get (), window->set_visibility_method, state ? VIEW_VISIBLE : VIEW_INVISIBLE);
        check_errors ();
        break;
      }
      case WindowFlag_Active:
        env.CallVoidMethod (window->view.get (), window->bring_to_front_method);
        check_errors ();
        break;
      case WindowFlag_Focus:
        if (!check_errors (env.CallBooleanMethod (window->view.get (), window->request_focus_method)))
          throw xtl::format_operation_exception ("", "EngineView::requestFocusThreadSafe failed");

        break;
      case WindowFlag_Maximized:
        env.CallVoidMethod (window->view.get (), window->maximize_method);        
        check_errors ();
        break;        
      case WindowFlag_Minimized:
      {
        env.CallVoidMethod (window->view.get (), window->set_visibility_method, VIEW_GONE);  
        check_errors ();      
        
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::SetWindowFlag");
    throw;
  }
}

bool Platform::GetWindowFlag (window_t window, WindowFlag flag)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
    
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
        return check_errors (env.CallIntMethod (window->view.get (), window->get_visibility_method)) != 0;
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
    e.touch ("syslib::AndroidPlatform::GetWindowFlag");
    throw;
  }
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::SetParentWindowHandle", "Parent windows not supported for Android platform");
}

const void* Platform::GetParentWindowHandle (window_t child)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::GetParentWindowHandle", "Parent windows not supported for Android platform");
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    get_env ().CallVoidMethod (window->view.get (), window->post_invalidate_method);

    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::InvalidateWindow");
    throw;
  }
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point&)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::SetCursorPosition(const Point&)", "No cursor for Android platform");
}

Point Platform::GetCursorPosition ()
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::GetCursorPosition()", "No cursor for Android platform");
}

void Platform::SetCursorPosition (window_t, const Point& client_position)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::SetCursorPosition(window_t,const Point&)", "No cursor for Android platform");
}

Point Platform::GetCursorPosition (window_t)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::GetCursorPosition(window_t)", "No cursor for Android platform");
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t window, bool state)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::AndroidPlatform::SetCursorVisible", "window");

  if (state)
    throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::SetCursorVisible", "No cursor for Android platform");
}

bool Platform::GetCursorVisible (window_t)
{
  return false;
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char*, int, int)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::CreateCursor", "No cursor for Android platform");
}

void Platform::DestroyCursor (cursor_t)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::DestroyCursor", "No cursor for Android platform");
}

void Platform::SetCursor (window_t, cursor_t)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidPlatform::SetCursor", "No cursor for Android platform");
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
        
    unsigned int int_color = window->background_state + (unsigned int)color.red * 0x10000 + (unsigned int)color.green * 0x100 +
      color.blue;
                             
    get_env ().CallVoidMethod (window->view.get (), window->set_background_color_method, int_color);
    
    check_errors ();
    
    window->background_color = int_color & 0xffffffu;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::SetBackgroundColor");
    throw;
  }
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");    
        
    window->background_state = state ? 0xff000000u : 0u;
    
    get_env ().CallVoidMethod (window->view.get (), window->set_background_color_method, window->background_color | window->background_state);
    
    check_errors ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::SetBackgroundState");
    throw;
  }
}

Color Platform::GetBackgroundColor (window_t window)
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
    e.touch ("syslib::AndroidPlatform::GetBackgroundColor");
    throw;
  }
}

bool Platform::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");        
    
    return window->background_state != 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::GetBackgroundState");
    throw;
  }
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
      
    jclass view_class = env->FindClass ("com/untgames/funner/application/EngineView");
    
    if (!view_class)
      throw xtl::format_operation_exception ("", "Can't find EngineView class\n");
    
    static const JNINativeMethod methods [] = {
      {"onLayoutCallback", "(IIII)V", (void*)&on_layout_callback},
      {"onDisplayHintCallback", "(I)V", (void*)&on_display_hint_callback},
      {"onDrawCallback", "()V", (void*)&on_draw_callback},
      {"onTouchCallback", "(IIFF)V", (void*)&on_touch_callback},
      {"onTrackballCallback", "(IIFF)V", (void*)&on_trackball_callback},
      {"onKeyCallback", "(IIZZZ)V", (void*)&on_key_callback},
      {"onFocusCallback", "(Z)V", (void*)&on_focus_callback},
      {"onSurfaceCreatedCallback", "()V", (void*)&on_surface_created_callback},
      {"onSurfaceDestroyedCallback", "()V", (void*)&on_surface_destroyed_callback},
    };
    
    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (view_class, methods, methods_count);

    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (staus=%d)", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_window_callbacks");
    throw;
  }
}

}

}
