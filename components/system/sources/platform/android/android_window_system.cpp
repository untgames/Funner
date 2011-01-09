#include "shared.h"

using namespace syslib;
using namespace syslib::android;

/*
    Окно
*/

struct Platform::window_handle
{
  global_ref<jobject>  view;                           //android окно
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
  jmethodID            set_background_color_method;    //метод установки цвета заднего плана окна
  jmethodID            maximize_method;                //метод максимизации окна
  
  window_handle ()
    : message_handler (0)
    , user_data (0)
    , background_color (0)
    , background_state (0)
  {
  }
};

namespace
{

///Поддержка класса окна
class JniWindowClass
{
  public:
///Конструктор
    JniWindowClass ()
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
        e.touch ("syslib::android::JniWindowClass::JniWindowClass");
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

  private:
    global_ref<jclass> activity_class;
    jmethodID          create_view_method;
};

typedef common::Singleton<JniWindowClass> JniWindowClassSingleton;

}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    stl::auto_ptr<window_handle> window (new window_handle);
    
    window->view            = JniWindowClassSingleton::Instance ()->CreateView (init_string);
    window->message_handler = handler;
    window->user_data       = user_data;
    
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
    window->set_background_color_method = find_method (&env, view_class.get (), "setBackgroundColorThreadSafe", "(I)V");
    window->maximize_method             = find_method (&env, view_class.get (), "maximizeThreadSafe", "()V");

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

const void* Platform::GetNativeWindowHandle (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::GetNativeWindowHandle");
}

const void* Platform::GetNativeDisplayHandle (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::GetNativeDisplayHandle");
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
    JNIEnv& env = get_env ();    
    
    switch (flag)
    {
      case WindowFlag_Visible:
        env.CallVoidMethod (window->view.get (), window->set_visibility_method, state);        
        check_errors ();
        break;
      case WindowFlag_Maximized:
        env.CallVoidMethod (window->view.get (), window->maximize_method);        
        check_errors ();
        break;      
      case WindowFlag_Active:
      case WindowFlag_Focus:
      case WindowFlag_Minimized:
        throw xtl::make_not_implemented_exception ("SetWindowFlag");
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
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::SetParentWindow");
}

const void* Platform::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::GetParentWindow");

  return 0;
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::AndroidPlatform::InvalidateWindow");
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

void Platform::SetCursorVisible (window_t, bool state)
{
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
    return window->background_state != 0;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidPlatform::GetBackgroundState");
    throw;
  }
}
