#include "shared.h"

using namespace syslib;
using namespace syslib::android;

/*
    Окно
*/

struct Platform::window_handle
{
  global_ref<jobject>  android_window;   //android окно
  WindowMessageHandler message_handler;  //обработчик сообщений
  void*                user_data;        //пользовательские данные окна
  
  window_handle ()
    : message_handler (0)
    , user_data (0)
  {
  }
};

namespace
{

///обработка java исключений
void check_errors ()
{
}

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
          
         //получение класса view
         
       view_class = get_env ().FindClass ("android/view/View");
       
       if (!view_class)
         throw xtl::format_operation_exception ("", "android.view.View class not found");

         //получение методов

        GetMethod (activity_class, "createCustomView", "(Ljava/lang/String;)Landroid/view/View;", create_window_method);
        GetMethod (view_class, "getLeft", "()I", get_left_method);
        GetMethod (view_class, "getTop", "()I", get_top_method);
        GetMethod (view_class, "getWidth", "()I", get_width_method);
        GetMethod (view_class, "getHeight", "()I", get_height_method);         
        GetMethod (view_class, "layout", "(IIII)V", layout_method);
        GetMethod (view_class, "setVisibility", "(I)V", set_visibility_method);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowMetaClass::JniWindowMetaClass");
        throw;
      }
    }
    
///Создание окна
    local_ref<jobject> CreateWindow (const char* init_string)
    {
      const ApplicationContext& context = get_context ();     
      
      local_ref<jobject> window = get_env ().CallObjectMethod (context.activity.get (), create_window_method, tojstring (init_string));
      
      if (!window)
        throw xtl::format_operation_exception ("", "EngineActivity::CreateView failed");
      
      //check exceptions!!!

      return window;
    }
    
///Получение размеров окна
    Rect GetWindowRect (const local_ref<jobject>& window)
    {
      Rect result;
      
      JNIEnv& env = get_env ();
      
      result.left   = env.CallIntMethod (window.get (), get_left_method);
      result.top    = env.CallIntMethod (window.get (), get_top_method);
      result.right  = result.left + env.CallIntMethod (window.get (), get_width_method);
      result.bottom = result.top + env.CallIntMethod (window.get (), get_height_method);
      
      //check exceptions!!!
      
      return result;
    }
    
///Установка размеров окна
    void SetWindowRect (const local_ref<jobject>& window, const Rect& rect)
    {
      JNIEnv& env = get_env ();
      
      env.CallVoidMethod (window.get (), layout_method, rect.left, rect.top, rect.right, rect.bottom);
      
      //check exceptions!!!            
    }
    
///Установка видимости окна
    void SetVisibility (const local_ref<jobject>& window, bool state)
    {
      JNIEnv& env = get_env ();
      
      env.CallVoidMethod (window.get (), set_visibility_method, state);
      
      //check exceptions!!!                  
    }

  private:
///Запрос метода
    void GetMethod (const global_ref<jclass>& class_, const char* name, const char* signature, jmethodID& method)
    {
      try
      {
        if (!class_)
          throw xtl::make_null_argument_exception ("", "class_");
          
        method = find_method (&get_env (), class_.get (), name, signature);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowClass::GetMethod");
        throw;
      }
    }
    
  private:
    global_ref<jclass> activity_class;
    global_ref<jclass> view_class;
    jmethodID          create_window_method;
    jmethodID          get_top_method;
    jmethodID          get_left_method;
    jmethodID          get_width_method;
    jmethodID          get_height_method;
    jmethodID          layout_method;
    jmethodID          set_visibility_method;
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
    
    window->android_window  = JniWindowClassSingleton::Instance ()->CreateWindow (init_string);
    window->message_handler = handler;
    window->user_data       = user_data;

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
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetNativeWindowHandle");
}

const void* Platform::GetNativeDisplayHandle (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetNativeDisplayHandle");
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
    JniWindowClassSingleton::Instance ()->SetWindowRect (window->android_window, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::DefaultPlatform::SetWindowRect");
    throw;
  }
}

void Platform::SetClientRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetClientRect");
}

void Platform::GetWindowRect (window_t window, Rect& result)
{
  try
  {
    result = JniWindowClassSingleton::Instance ()->GetWindowRect (window->android_window);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::DefaultPlatform::GetWindowRect");
    throw;
  }
}

void Platform::GetClientRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t window, WindowFlag flag, bool state)
{
  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        JniWindowClassSingleton::Instance ()->SetVisibility (window->android_window, state);
        break;
      case WindowFlag_Active: //активность окна
      case WindowFlag_Focus:
      case WindowFlag_Maximized:
      case WindowFlag_Minimized:
        throw xtl::make_not_implemented_exception ("SetWindowFlag");
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::DefaultPlatform::SetWindowFlag");
    throw;
  }
}

bool Platform::GetWindowFlag (window_t, WindowFlag)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetWindowFlag");

  return false;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetParentWindow");
}

const void* Platform::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetParentWindow");

  return 0;
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::InvalidateWindow");
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetCursorPosition");

  return Point ();
}

void Platform::SetCursorPosition (window_t, const Point& client_position)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::SetCursorPosition (window_t, const Point&)");
}

Point Platform::GetCursorPosition (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::GetCursorPosition (window_t)");
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetCursorVisible");
}

bool Platform::GetCursorVisible (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetCursorVisible");

  return false;
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char*, int, int)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::CreateCursor");
}

void Platform::DestroyCursor (cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::DestroyCursor");
}

void Platform::SetCursor (window_t, cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetCursor");
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  throw xtl::make_not_implemented_exception ("syslib::SetBackgroundColor");
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::SetBackgroundState");
}

Color Platform::GetBackgroundColor (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool Platform::GetBackgroundState (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::GetBackgroundState");
  
  return false;
}
