#include "shared.h"

using namespace syslib;

/*
    Описание реализации окна
*/

struct Platform::window_handle
{
  Display*  display; //display for this window
  Window    window;  //window handle
  
  window_handle ()
    : display (0)
    , window (0)
  {
  }
};

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, const char* init_string, void* user_data)
{
  try
  {
    return 0;    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CreateWindow");
    throw;  
  }
}

void Platform::CloseWindow (window_t handle)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CloseWindow");
    throw;  
  }
}

void Platform::DestroyWindow (window_t handle)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::DestroyWindow");
    throw;  
  }
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowTitle");
    throw;  
  }
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowTitle");
    throw;  
  }
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowRect");
    throw;  
  }
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetClientRect");
    throw;  
  }
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowRect");
    throw;  
  }
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetClientRect");
    throw;  
  }
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowFlag");
    throw;  
  }
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    return false;    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowFlag");
    throw;  
  }
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetParentWindow");
    throw;  
  }
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  try
  {
    return 0;    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetParentWindow");
    throw;  
  }
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::InvalidateWindow");
    throw;  
  }
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition ()
{
  try
  {
    return syslib::Point (0, 0);    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition()");
    throw;  
  }
}

void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(window_t,const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition (window_t handle)
{
  try
  {
    return syslib::Point (0, 0);    
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition(window_t)");
    throw;  
  }
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  try
  {
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorVisible");
    throw;  
  }
}

bool Platform::GetCursorVisible (window_t)
{
  try
  {
    return true;    
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorVisible");
    throw;  
  }  
}
