#include <stl/algorithm>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/string.h>

#include <common/exception.h>

#include <syslib/window.h>

#include "shared.h"

using namespace xtl;
using namespace common;
using namespace input::low_level::window;
using namespace syslib;

namespace
{

const size_t CONNECTIONS_COUNT   = 21;
const size_t MESSAGE_BUFFER_SIZE = 32;
const char*  CURSOR_AXIS_NAME    = "Cursor";
const char*  WHEEL_AXIS_NAME     = "Wheel";
const char*  CHAR_INPUT_CHANNEL  = "Window";

const char* AUTOCENTER_CURSOR            = "AUTOCENTER_CURSOR";
const char* CURSOR_SENSITIVITY           = "CURSOR_SENSITIVITY";
const char* VERTICAL_WHEEL_SENSITIVITY   = "VERTICAL_WHEEL_SENSITIVITY";
const char* HORISONTAL_WHEEL_SENSITIVITY = "HORISONTAL_WHEEL_SENSITIVITY";

const char* PROPERTIES [] = {
  AUTOCENTER_CURSOR,
  CURSOR_SENSITIVITY,
  VERTICAL_WHEEL_SENSITIVITY,
  HORISONTAL_WHEEL_SENSITIVITY
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

void default_event_handler (const char*)
{
}

}

/*
   Конструктор/деструктор
*/

Device::Device (Window* window, const char* in_name)
  : event_handler (&default_event_handler), name (in_name), autocenter_cursor (0), cursor_sensitivity (1.f), vertical_wheel_sensitivity (1.f), horisontal_wheel_sensitivity (1.f)
{
  connections.reserve (CONNECTIONS_COUNT);

  try
  {
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMouseMove,               xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMouseVerticalWheel,      xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMouseHorisontalWheel,    xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnLeftButtonDown,          xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnLeftButtonUp,            xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnLeftButtonDoubleClick,   xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnRightButtonDown,         xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnRightButtonUp,           xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnRightButtonDoubleClick,  xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMiddleButtonDown,        xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMiddleButtonUp,          xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnMiddleButtonDoubleClick, xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton1Down,            xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton1Up,              xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton1DoubleClick,     xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton2Down,            xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton2Up,              xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnXButton2DoubleClick,     xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnKeyDown,                 xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnKeyUp,                   xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));
    connections.push_back (window->RegisterEventHandler (WindowEvent_OnChar,                    xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3)));

    for (size_t i = 0; i < PROPERTIES_COUNT; i++)
    {
      properties.append (PROPERTIES[i]);
      properties.append (" ");
    }

    if (!properties.empty ())
      properties.pop_back ();
  }
  catch (Exception& exception)
  {
    connections.clear ();
    exception.Touch ("input::low_level::window::Device::Device");
    throw;
  }
  catch (...)
  {
    connections.clear ();
    throw;
  }
}

Device::~Device ()
{
  Release ();
}

/*
   Настройки устройства
*/

const char* Device::GetProperties ()
{
  return properties.c_str ();
}

void Device::SetProperty (const char* name, float value)
{
  if (!xstrcmp (AUTOCENTER_CURSOR, name))
  {
    autocenter_cursor = value != 0;
    return;
  }
  
  if (!xstrcmp (CURSOR_SENSITIVITY, name))
  {
    cursor_sensitivity = value;
    return;
  }
  
  if (!xstrcmp (VERTICAL_WHEEL_SENSITIVITY, name))
  {
    vertical_wheel_sensitivity = value;
    return;
  }
  
  if (!xstrcmp (HORISONTAL_WHEEL_SENSITIVITY, name))
  {
    horisontal_wheel_sensitivity = value;
    return;
  }

  raise_invalid_argument ("input::low_level::window::Device::SetProperty", "name", name);
}

float Device::GetProperty (const char* name)
{
  if (!xstrcmp (AUTOCENTER_CURSOR, name))
  {
    if (autocenter_cursor) 
      return 1.f;
    return 0.f;
  }
  
  if (!xstrcmp (CURSOR_SENSITIVITY, name))
    return cursor_sensitivity;

  if (!xstrcmp (VERTICAL_WHEEL_SENSITIVITY, name))
    return vertical_wheel_sensitivity;

  if (!xstrcmp (HORISONTAL_WHEEL_SENSITIVITY, name))
    return horisontal_wheel_sensitivity;

  raise_invalid_argument ("input::low_level::window::Device::GetProperty", "name", name);
  return 0.f;
}

/*
   Обработчик сообщений окна
*/

void Device::WindowEventHandler (Window& window, WindowEvent event, const WindowEventContext& window_event_context)
{
  static char message[MESSAGE_BUFFER_SIZE];
  float       axis_pos;

  switch (event)
  {
    case WindowEvent_OnLeftButtonDoubleClick:
    case WindowEvent_OnLeftButtonDown:
    case WindowEvent_OnLeftButtonUp:
    case WindowEvent_OnRightButtonDoubleClick:
    case WindowEvent_OnRightButtonDown:
    case WindowEvent_OnRightButtonUp:
    case WindowEvent_OnMiddleButtonDoubleClick:
    case WindowEvent_OnMiddleButtonDown:
    case WindowEvent_OnMiddleButtonUp:
    case WindowEvent_OnXButton1DoubleClick:
    case WindowEvent_OnXButton1Down:
    case WindowEvent_OnXButton1Up:
    case WindowEvent_OnXButton2DoubleClick:
    case WindowEvent_OnXButton2Down:
    case WindowEvent_OnXButton2Up:
      window.SetCursorPosition (window_event_context.cursor_position.x, window_event_context.cursor_position.y);
      WindowEventHandler (window, WindowEvent_OnMouseMove, window_event_context);
  }

  switch (event)
  {
    case WindowEvent_OnMouseMove:
      if (x_cursor_pos == window_event_context.cursor_position.x && y_cursor_pos == window_event_context.cursor_position.y)
        break;

      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s at %u %u", CURSOR_AXIS_NAME, window_event_context.cursor_position.x, window_event_context.cursor_position.y);
      event_handler (message);
      
      if (x_cursor_pos != window_event_context.cursor_position.x)
      {
        if (window.Width ())
        {
          axis_pos = (float)window_event_context.cursor_position.x * 2.f / window.Width () - 1.f;

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX axis %f", CURSOR_AXIS_NAME, axis_pos);
          event_handler (message);

          int dx = window_event_context.cursor_position.x - x_cursor_pos;

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX delta %f", CURSOR_AXIS_NAME, (float)dx / stl::max (window.Width (), window.Height ()) * cursor_sensitivity);
          event_handler (message);
        }

        x_cursor_pos = window_event_context.cursor_position.x;
      }

      if (y_cursor_pos != window_event_context.cursor_position.y)
      {
        if (window.Height ())
        {
          axis_pos = -(float)window_event_context.cursor_position.y * 2.f / window.Height () + 1.f;

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY axis %f", CURSOR_AXIS_NAME, axis_pos);
          event_handler (message);

          int dx = y_cursor_pos - window_event_context.cursor_position.y;

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY delta %f", CURSOR_AXIS_NAME, (float)dx / stl::max (window.Width (), window.Height ()) * cursor_sensitivity);
          event_handler (message);
        }

        y_cursor_pos = window_event_context.cursor_position.y;
      }

      if (autocenter_cursor)
      {
        x_cursor_pos = (window_event_context.window_rect.right - window_event_context.window_rect.left) / 2;
        y_cursor_pos = (window_event_context.window_rect.bottom - window_event_context.window_rect.top) / 2;
//        x_cursor_pos = window.Width () / 2;
//        y_cursor_pos = window.Height () / 2;
//        printf ("\nSetting cursor from %u.%u to %u.%u\n\n", window_event_context.cursor_position.x, window_event_context.cursor_position.y, x_cursor_pos, y_cursor_pos);
        window.SetCursorPosition (window.Width () / 2, window.Height () / 2);
      }

      break;
    case WindowEvent_OnMouseVerticalWheel:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY delta %f", WHEEL_AXIS_NAME, window_event_context.mouse_vertical_wheel_delta * vertical_wheel_sensitivity);
      event_handler (message);
       
      if (window_event_context.mouse_vertical_wheel_delta > 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYUp down", WHEEL_AXIS_NAME);
        event_handler (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYUp up", WHEEL_AXIS_NAME);
        event_handler (message);
      }

      if (window_event_context.mouse_vertical_wheel_delta < 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYDown down", WHEEL_AXIS_NAME);
        event_handler (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYDown up", WHEEL_AXIS_NAME);
        event_handler (message);
      }

      break;
    case WindowEvent_OnMouseHorisontalWheel:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX delta %f", WHEEL_AXIS_NAME, window_event_context.mouse_horisontal_wheel_delta * horisontal_wheel_sensitivity);
      event_handler (message);
       
      if (window_event_context.mouse_horisontal_wheel_delta > 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXRight down", WHEEL_AXIS_NAME);
        event_handler (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXRight up", WHEEL_AXIS_NAME);
        event_handler (message);
      }

      if (window_event_context.mouse_horisontal_wheel_delta < 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXLeft down", WHEEL_AXIS_NAME);
        event_handler (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXLeft up", WHEEL_AXIS_NAME);
        event_handler (message);
      }

      break;
    case WindowEvent_OnLeftButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 dblclk");
      event_handler (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 down");
      event_handler (message);
      break;
    case WindowEvent_OnLeftButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 down");
      event_handler (message);
      break;
    case WindowEvent_OnLeftButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 up");
      event_handler (message);
      break;
    case WindowEvent_OnRightButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 dblclk");
      event_handler (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 down");
      event_handler (message);
      break;
    case WindowEvent_OnRightButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 down");
      event_handler (message);
      break;
    case WindowEvent_OnRightButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 up");
      event_handler (message);
      break;
    case WindowEvent_OnMiddleButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 dblclk");
      event_handler (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 down");
      event_handler (message);
      break;
    case WindowEvent_OnMiddleButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 down");
      event_handler (message);
      break;
    case WindowEvent_OnMiddleButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 up");
      event_handler (message);
      break;
    case WindowEvent_OnXButton1DoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 dblclk");
      event_handler (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 down");
      event_handler (message);
      break;
    case WindowEvent_OnXButton1Down:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 down");
      event_handler (message);
      break;
    case WindowEvent_OnXButton1Up:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 up");
      event_handler (message);
      break;
    case WindowEvent_OnXButton2DoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 dblclk");
      event_handler (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 down");
      event_handler (message);
      break;
    case WindowEvent_OnXButton2Down:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 down");
      event_handler (message);
      break;
    case WindowEvent_OnXButton2Up:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 up");
      event_handler (message);
      break;
    case WindowEvent_OnKeyDown:
      if (pressed_keys[window_event_context.key_scan_code])
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' repeat", get_key_scan_name (window_event_context.key_scan_code));
      else
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' down", get_key_scan_name (window_event_context.key_scan_code));
        pressed_keys.set (window_event_context.key_scan_code);
      }
      event_handler (message);
      break;
    case WindowEvent_OnKeyUp:
      pressed_keys.reset (window_event_context.key_scan_code);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' up", get_key_scan_name (window_event_context.key_scan_code));
      event_handler (message);
      break;
    case WindowEvent_OnChar:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s char %C", CHAR_INPUT_CHANNEL, window_event_context.char_code);
      event_handler (message);
      break;
  }
}
