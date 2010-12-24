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

         //получение методов

//        GetMethod ("CreateView", "(Ljava/lang/String;)Landroid/view/View;", create_window_method);
        GetMethod ("CreateView", "()Landroid/view/View;", create_window_method);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowMetaClass::JniWindowMetaClass");
        throw;
      }
    }
    
///Создание окна
    local_ref<jobject> CreateWindow ()
    {
      const ApplicationContext& context = get_context ();     
      
      local_ref<jobject> window = get_env ().CallObjectMethod (get_context ().activity.get (), create_window_method);   
      
      if (!window)
        throw xtl::format_operation_exception ("", "EngineActivity::CreateView failed");
      
      //check exceptions!!!
      
      return window;
    }
    
  private:
///Запрос метода
    void GetMethod (const char* name, const char* signature, jmethodID& method)
    {
      try
      {
        if (!activity_class)
          throw xtl::format_operation_exception ("", "Null activity class");
          
        method = find_method (&get_env (), activity_class.get (), name, signature);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniWindowClass::GetMethod");
        throw;
      }
    }
    
  private:
    global_ref<jclass> activity_class;
    jmethodID          create_window_method;
};

typedef common::Singleton<JniWindowClass> JniWindowClassSingleton;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/


void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for default platform");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char*, void* user_data)
{
  try
  {
    stl::auto_ptr<window_handle> window (new window_handle);
    
    window->android_window  = JniWindowClassSingleton::Instance ()->CreateWindow ();
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
  raise ("syslib::DefaultPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  raise ("syslib::DefaultPlatform::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t)
{
  raise ("syslib::DefaultPlatform::GetNativeWindowHandle");
}

const void* Platform::GetNativeDisplayHandle (window_t)
{
  raise ("syslib::DefaultPlatform::GetNativeDisplayHandle");
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::DefaultPlatform::SetWindowRect");
}

void Platform::SetClientRect (window_t, const Rect&)
{
  raise ("syslib::DefaultPlatform::SetClientRect");
}

void Platform::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetWindowRect");
}

void Platform::GetClientRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::DefaultPlatform::GetWindowFlag");

  return false;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::DefaultPlatform::SetParentWindow");
}

const void* Platform::GetParentWindowHandle (window_t child)
{
  raise ("syslib::DefaultPlatform::GetParentWindow");

  return 0;
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t)
{
  raise ("syslib::DefaultPlatform::InvalidateWindow");
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point&)
{
  raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  raise ("syslib::DefaultPlatform::GetCursorPosition");

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
  raise ("syslib::DefaultPlatform::SetCursorVisible");
}

bool Platform::GetCursorVisible (window_t)
{
  raise ("syslib::DefaultPlatform::GetCursorVisible");

  return false;
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char*, int, int)
{
  raise ("syslib::DefaultPlatform::CreateCursor");
}

void Platform::DestroyCursor (cursor_t)
{
  raise ("syslib::DefaultPlatform::DestroyCursor");
}

void Platform::SetCursor (window_t, cursor_t)
{
  raise ("syslib::DefaultPlatform::SetCursor");
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::SetBackgroundColor");
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::SetBackgroundState");
}

Color Platform::GetBackgroundColor (window_t window)
{
  raise ("syslib::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool Platform::GetBackgroundState (window_t window)
{
  raise ("syslib::GetBackgroundState");
  
  return false;
}
