#include "shared.h"

using namespace syslib;
using namespace syslib::bada;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Graphics;
using namespace Osp::Ui::Controls;

namespace
{

/*
    Описание реализации окна
*/

class WindowImpl: public Form
{
  public:
///Конструктор
    WindowImpl (const WindowMessageHandler& in_message_handler, void* in_user_data)
      : message_handler (in_message_handler)
      , user_data (in_user_data)
    {
    }
  
  private:
    void Notify (window_t window, WindowEvent event, const WindowEventContext& context)
    {
  /*    try
      {
        message_handler (window, event, context, user_data);
      }
      catch (...)
      {
        //подавление всех исключений
      }*/
    }

  private:
    WindowMessageHandler message_handler;
    void*                user_data;
};

/*
    Утилиты
*/

void convert_rect (int x, int y, int width, int height, Rect& rect)
{
  rect.left   = (size_t)x;
  rect.top    = (size_t)y;
  rect.right  = (size_t)(x + width);
  rect.bottom = (size_t)(y + height);
}

void convert_rect (const Rect& rect, int& x, int& y, int& width, int& height)
{
  x      = (int)rect.left;
  y      = (int)rect.top;
  width  = (int)(rect.right - rect.left);
  height = (int)(rect.bottom - rect.top);
}

void convert_rect (const Rectangle& in_rect, Rect& out_rect)
{
  convert_rect (in_rect.x, in_rect.y, in_rect.width, in_rect.height, out_rect);
}

void convert_rect (const Rect& in_rect, Rectangle& out_rect)
{
  convert_rect (in_rect, out_rect.x, out_rect.y, out_rect.width, out_rect.height);
}

}

struct syslib::window_handle
{
  stl::auto_ptr<WindowImpl> window;
};


/*
    Создание/закрытие/уничтожение окна
*/

window_t BadaWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    switch (style)
    {
      case WindowStyle_Overlapped:
        break;
      case WindowStyle_PopUp:
        throw xtl::format_not_supported_exception ("", "WindowStyle_Popup not supported");
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Child windows are not supported");
      
      //получение объекта приложения
      
    Osp::App::Application& app = get_application ();
      
      //создание и инициализация окна
      
    stl::auto_ptr<window_handle> handle (new window_handle);
    
    handle->window = stl::auto_ptr<WindowImpl> (new WindowImpl (handler, user_data));
    
    result res = handle->window->Construct (FORM_STYLE_NORMAL);
    
    if (IsFailed (res))
      raise ("Osp::Ui::Controls::Form::Construct", res);
      
      //регистрация формы
      
    Frame* app_frame = app.GetAppFrame ()->GetFrame ();
    
    res = app_frame->AddControl (*handle->window);
    
    if (IsFailed (res))
    {
      raise ("Osp::Ui::Controls::Frame::AddControl", res);
    }
    
    return handle.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::CreateWindow");
    throw;
  }
}

void BadaWindowManager::CloseWindow (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaWindowManager::CloseWindow");
}

void BadaWindowManager::DestroyWindow (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaWindowManager::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* BadaWindowManager::GetNativeWindowHandle (window_t handle)
{
  return handle->window.get ();
}

const void* BadaWindowManager::GetNativeDisplayHandle (window_t handle)
{
  return 0;
}

/*
    Заголовок окна
*/

void BadaWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    return; //not supported
    
    result res = handle->window->SetTitleText (title);
    
    if (IsFailed (res))
      raise ("Osp::Ui::Controls::Form::SetTitleText", res);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::SetWindowTitle");
    throw;
  }
}

void BadaWindowManager::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  try
  {
    String title = handle->window->GetTitleText ();

    xtl::xstrncpy (buffer, title.GetPointer (), buffer_size_in_chars);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::GetWindowTitle");
    throw;
  }
}

/*
    Область окна / клиентская область
*/

void BadaWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    Rect current_rect;
    
    GetWindowRect (handle, current_rect);
    
    if (current_rect.left == rect.left && current_rect.right == rect.right && current_rect.top == rect.top &&
      current_rect.bottom == rect.bottom)
        return;
        
    throw xtl::format_not_supported_exception ("", "Can't change window rect for bada Form");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::SetWindowRect");
    throw;
  }
}

void BadaWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaWindowManager::SetClientRect");
}

void BadaWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    convert_rect (handle->window->GetBounds (), rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::GetWindowRect");
    throw;
  }
}

void BadaWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    convert_rect (handle->window->GetClientAreaBounds (), rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void BadaWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        result res = handle->window->SetShowState (state);
        
        if (IsFailed (res))
          raise ("Osp::Ui::Controls::Form::SetShowState", res);
        
        if (state)
        {
          res = handle->window->Draw ();
          
          if (IsFailed (res))
            raise ("Osp::Ui::Controls::Form::Draw", res);
          
          res = handle->window->Show ();
        
          if (IsFailed (res))
            raise ("Osp::Ui::Controls::Form::Show", res);
        }
        
        break;
      }
      case WindowFlag_Active:
      {
        if (state)
        {
          Osp::App::Application& app = get_application ();
          
          app.GetAppFrame ()->GetFrame ()->SetCurrentForm (*handle->window);
          
          result res = handle->window->Draw ();
          
          if (IsFailed (res))
            raise ("Osp::Ui::Controls::Form::Draw", res);
        }
          
        break;
      }
      case WindowFlag_Focus:
      {
        if (state)
        {
          result res = handle->window->SetFocus ();

          if (IsFailed (res))
            raise ("Osp::Ui::Controls::Form::SetFocus", res);  
        }

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::SetWindowFlag");
    throw;
  }
}

bool BadaWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        return handle->window->GetShowState ();
      }
      case WindowFlag_Active:
      {
        return handle->window->IsVisible ();
      }
      case WindowFlag_Focus:
      {
        return handle->window->HasFocus ();
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    Обновление окна
*/

void BadaWindowManager::InvalidateWindow (window_t handle)
{
  try
  {
    handle->window->RequestRedraw ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::BadaWindowManager::InvalidateWindow");
    throw;
  }
}
