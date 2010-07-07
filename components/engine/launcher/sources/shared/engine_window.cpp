#include "shared.h"

using namespace engine;

/*
    Окно
*/

class Window: public IWindow, private xtl::trackable
{
  public:
///Конструктор
    Window (const char* name)
      : system_window (syslib::WindowStyle_Overlapped)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");
          
        window_name = name;
        
        AttachmentRegistry::Register (name, xtl::make_const_ref (xtl::ref (system_window)));
        
        static syslib::WindowEvent events [] = {
          syslib::WindowEvent_OnPaint,
          syslib::WindowEvent_OnSize,
          syslib::WindowEvent_OnMouseMove,
          syslib::WindowEvent_OnMouseLeave,
          syslib::WindowEvent_OnLeftButtonDown,
          syslib::WindowEvent_OnLeftButtonUp,
          syslib::WindowEvent_OnLeftButtonDoubleClick,
          syslib::WindowEvent_OnRightButtonDown,
          syslib::WindowEvent_OnRightButtonUp,
          syslib::WindowEvent_OnRightButtonDoubleClick,
          syslib::WindowEvent_OnMiddleButtonDown,
          syslib::WindowEvent_OnMiddleButtonUp,
          syslib::WindowEvent_OnMiddleButtonDoubleClick,
          syslib::WindowEvent_OnXButton1Down,
          syslib::WindowEvent_OnXButton1Up,
          syslib::WindowEvent_OnXButton1DoubleClick,
          syslib::WindowEvent_OnXButton2Down,
          syslib::WindowEvent_OnXButton2Up,
          syslib::WindowEvent_OnXButton2DoubleClick,
          syslib::WindowEvent_OnKeyDown,
          syslib::WindowEvent_OnKeyUp,
        };
        
        static size_t events_count = sizeof (events) / sizeof (*events);
        
        syslib::Window::EventHandler handler (xtl::bind (&Window::WindowEventHandler, this, _2, _3));

        for (size_t i=0; i<events_count; i++)
          connect_tracker (system_window.RegisterEventHandler (events [i], handler));
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::Window");
        throw;
      }
    }
    
///Деструктор
    ~Window ()
    {
      AttachmentRegistry::Unregister (window_name.c_str (), system_window);
    }
    
///Показ / скрытие окна
    void Show (bool state)
    {
      try
      {
        system_window.SetVisible (state);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::Show");
        throw;
      }
    }

///Установка дескриптора родительского окна
    void SetParentHandle (void* handle)
    {
      try
      {
        system_window.SetParentHandle (handle);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::SetParentHandle");
        throw;
      }
    }

///Получение дескриптора родительского окна
    void* GetParentHandle ()
    {
      try
      {
        return const_cast<void*> (system_window.ParentHandle ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::GetParentHandle");
        throw;
      }
    }
    
///Установка размеров окна
    void SetSize (size_t width, size_t height)
    {
      try
      {
        system_window.SetSize (width, height);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::SetSize");
        throw;
      }
    }
    
///Получение ширины окна
    size_t GetWidth ()
    {
      try
      {
        return system_window.Width ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::GetWidth");
        throw;
      }
    }
    
///Получение высоты окна
    size_t GetHeight ()
    {
      try
      {
        return system_window.Height ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::GetHeight");
        throw;
      }
    }
    
///Установка положения окна
    void SetPosition (int x, int y)
    {
      try
      {
        return system_window.SetPosition (x, y);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::SetPosition");
        throw;
      }
    }

///Получение абсциссы положения окна
    int GetX ()
    {
      try
      {
        return system_window.Position ().x;
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::GetX");
        throw;
      }
    }

///Получение ординаты положения окна
    int GetY ()
    {
      try
      {
        return system_window.Position ().y;
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Window::GetY");
        throw;
      }
    }

///Добавление слушателя событий
    void AttachListener (IWindowListener* listener)
    {
      if (!listener)
        throw xtl::make_null_argument_exception ("engine::Window::AttachListener", "listener");
      
      listeners.push_back (listener);
    }

///Удаление слушателя событий
    void DetachListener (IWindowListener* listener)
    {
      if (!listener)
        return;
        
      listeners.erase (stl::remove (listeners.begin (), listeners.end (), listener), listeners.end ());
    }

///Удаление всех слушателей событий
    void DetachAllListeners ()
    {
      listeners.clear ();
    }

  private:
///Обработчик событий окна
    void WindowEventHandler (syslib::WindowEvent event, const syslib::WindowEventContext& context)
    {
      switch (event)
      {
        case syslib::WindowEvent_OnPaint:
        printf ("ONPAINT!!!!!!!!!!!!!!!! my\n");
        fflush (stdout);        
          Notify (&IWindowListener::OnPaint);
          break;
        case syslib::WindowEvent_OnSize:
        printf ("ONSIZE!!!!!!!!!!!!!!!! my\n");
        fflush (stdout);
          Notify (&IWindowListener::OnSize, system_window.ClientWidth (), system_window.ClientHeight ());
          break;
        case syslib::WindowEvent_OnMouseMove:
          Notify (&IWindowListener::OnMouseMove, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnMouseLeave:
          Notify (&IWindowListener::OnMouseLeave, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnLeftButtonDown:
          Notify (&IWindowListener::OnMouseDown, MouseButton_Left, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnLeftButtonUp:
          Notify (&IWindowListener::OnMouseUp, MouseButton_Left, context.cursor_position.x, context.cursor_position.y);        
          break;
        case syslib::WindowEvent_OnLeftButtonDoubleClick:
          Notify (&IWindowListener::OnMouseDoubleClick, MouseButton_Left, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnRightButtonDown:
          Notify (&IWindowListener::OnMouseDown, MouseButton_Right, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnRightButtonUp:
          Notify (&IWindowListener::OnMouseUp, MouseButton_Right, context.cursor_position.x, context.cursor_position.y);        
          break;
        case syslib::WindowEvent_OnRightButtonDoubleClick:
          Notify (&IWindowListener::OnMouseDoubleClick, MouseButton_Right, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnMiddleButtonDown:
          Notify (&IWindowListener::OnMouseDown, MouseButton_Middle, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnMiddleButtonUp:
          Notify (&IWindowListener::OnMouseUp, MouseButton_Middle, context.cursor_position.x, context.cursor_position.y);        
          break;
        case syslib::WindowEvent_OnMiddleButtonDoubleClick:
          Notify (&IWindowListener::OnMouseDoubleClick, MouseButton_Middle, context.cursor_position.x, context.cursor_position.y);
          break;          
        case syslib::WindowEvent_OnXButton1Down:
          Notify (&IWindowListener::OnMouseDown, MouseButton_X1, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnXButton1Up:
          Notify (&IWindowListener::OnMouseUp, MouseButton_X1, context.cursor_position.x, context.cursor_position.y);        
          break;
        case syslib::WindowEvent_OnXButton1DoubleClick:
          Notify (&IWindowListener::OnMouseDoubleClick, MouseButton_X1, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnXButton2Down:
          Notify (&IWindowListener::OnMouseDown, MouseButton_X2, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnXButton2Up:
          Notify (&IWindowListener::OnMouseUp, MouseButton_X2, context.cursor_position.x, context.cursor_position.y);        
          break;
        case syslib::WindowEvent_OnXButton2DoubleClick:
          Notify (&IWindowListener::OnMouseDoubleClick, MouseButton_X2, context.cursor_position.x, context.cursor_position.y);
          break;
        case syslib::WindowEvent_OnKeyDown:
          Notify (&IWindowListener::OnKeyDown, get_key_name (context.key));
          break;
        case syslib::WindowEvent_OnKeyUp:
          Notify (&IWindowListener::OnKeyUp, get_key_name (context.key));
          break;
        default:
          break;
      }
    }

///Оповещение слушателей
    template <class Fn> void Notify (Fn fn)
    {
      for (typename ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      {
        try
        {
          ((**iter).*fn)();
        }
        catch (...)
        {
        }
      }
    }
    
    template <class Fn, class T> void Notify (Fn fn, T arg)
    {
      for (typename ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      {
        try
        {
          ((**iter).*fn)(arg);
        }
        catch (...)
        {
        }
      }
    }
    
    template <class Fn, class T1, class T2> void Notify (Fn fn, T1 arg1, T2 arg2)
    {
      for (typename ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      {
        try
        {
          ((**iter).*fn)(arg1, arg2);
        }
        catch (...)
        {
        }
      }
    }
    
    template <class Fn, class T1, class T2, class T3> void Notify (Fn fn, T1 arg1, T2 arg2, T3 arg3)
    {
      for (typename ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      {
        try
        {
          ((**iter).*fn)(arg1, arg2, arg3);
        }
        catch (...)
        {
        }
      }
    }    

  private:
    typedef stl::vector<IWindowListener*> ListenerArray;

  private:
    syslib::Window system_window; //системное окно
    stl::string    window_name;   //имя окна
    ListenerArray  listeners;     //слушатели событий окна
};

namespace engine
{

IWindow* create_window (const char* name)
{
  try
  {
    return new Window (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("engine::create_window");
    throw;
  }
}

}
