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

        GetMethod ("CreateView", "(Ljava/lang/String;)Landroid/view/View;", create_window_method);
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
      
      //local_ref<jobject> window = 
      get_env ().CallObjectMethod (get_context ().activity.get (), create_window_method, tojstring (init_string));
      printf ("!!!1\n"); fflush (stdout);
//      if (!window)
//        throw xtl::format_operation_exception ("", "EngineActivity::CreateView failed");
      
      //check exceptions!!!

      return 0;    
//      return window;
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
printf ("HEYE\n"); fflush (stdout);    
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
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetWindowRect");
}

void Platform::SetClientRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetClientRect");
}

void Platform::GetWindowRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetWindowRect");
}

void Platform::GetClientRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::SetWindowFlag");
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
