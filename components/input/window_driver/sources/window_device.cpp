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

const char* AUTOCENTER_CURSOR            = "Cursor.auto_center";
const char* CURSOR_SENSITIVITY           = "Cursor.sensitivity";
const char* VERTICAL_WHEEL_SENSITIVITY   = "WheelY.sensitivity";
const char* HORISONTAL_WHEEL_SENSITIVITY = "WheelX.sensitivity";

const char* PROPERTIES [] = {
  AUTOCENTER_CURSOR,
  CURSOR_SENSITIVITY,
  VERTICAL_WHEEL_SENSITIVITY,
  HORISONTAL_WHEEL_SENSITIVITY
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

}

/*
   Конструктор/деструктор
*/

Device::Device (Window* window, const char* in_name)
  : name (in_name), full_name ("window"), x_cursor_pos (window->CursorPosition ().x), y_cursor_pos (window->CursorPosition ().y),
    autocenter_cursor (0), cursor_sensitivity (1.f), vertical_wheel_sensitivity (1.f), horisontal_wheel_sensitivity (1.f)
{
  try
  {
    Window::EventHandler handler (xtl::bind (&Device::WindowEventHandler, this, _1, _2, _3));

    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMouseMove,               handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMouseVerticalWheel,      handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMouseHorisontalWheel,    handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnLeftButtonDown,          handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnLeftButtonUp,            handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnLeftButtonDoubleClick,   handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnRightButtonDown,         handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnRightButtonUp,           handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnRightButtonDoubleClick,  handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMiddleButtonDown,        handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMiddleButtonUp,          handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnMiddleButtonDoubleClick, handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton1Down,            handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton1Up,              handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton1DoubleClick,     handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton2Down,            handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton2Up,              handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnXButton2DoubleClick,     handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnKeyDown,                 handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnKeyUp,                   handler));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnChar,                    handler));

    for (size_t i = 0; i < PROPERTIES_COUNT; i++)
    {
      properties.append (PROPERTIES[i]);
      properties.append (" ");
    }

    if (!properties.empty ())
      properties.pop_back ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("input::low_level::window::Device::Device");
    throw;
  }

  if (!name.empty ())
  {
    full_name += '.';
    full_name += name;
  }
}

Device::~Device ()
{
}

/*
   Получение имени контрола
*/

const wchar_t* Device::GetControlName (const char* control_id)
{
  control_name = common::towstring (control_id);

  return control_name.c_str ();
}

/*
   Подписка на события устройства
*/

xtl::connection Device::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return signals.connect (handler);
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

  throw xtl::make_argument_exception ("input::low_level::window::Device::SetProperty", "name", name);
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

  throw xtl::make_argument_exception ("input::low_level::window::Device::GetProperty", "name", name);
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
    case WindowEvent_OnMouseMove:
    {
      if (x_cursor_pos == window_event_context.cursor_position.x && y_cursor_pos == window_event_context.cursor_position.y)
        break;

      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s at %u %u", CURSOR_AXIS_NAME, window_event_context.cursor_position.x, window_event_context.cursor_position.y);
      signals (message);

      Rect client_rect = window.ClientRect ();

      size_t client_rect_width  = client_rect.right - client_rect.left;
      size_t client_rect_height = client_rect.bottom - client_rect.top;

      if (x_cursor_pos != window_event_context.cursor_position.x)
      {
        if (window.Width ())
        {
          if (!autocenter_cursor)
          {
            axis_pos = (float)window_event_context.cursor_position.x * 2.f / client_rect_width - 1.f;

            xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX axis %f", CURSOR_AXIS_NAME, axis_pos);
            signals (message);
          }

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX delta %f", CURSOR_AXIS_NAME, ((float)((float)window_event_context.cursor_position.x - (float)x_cursor_pos)) * cursor_sensitivity);
          signals (message);
        }

        x_cursor_pos = window_event_context.cursor_position.x;
      }

      if (y_cursor_pos != window_event_context.cursor_position.y)
      {
        if (window.Height ())
        {
          if (!autocenter_cursor)
          {
            axis_pos = -(float)window_event_context.cursor_position.y * 2.f / client_rect_height + 1.f;

            xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY axis %f", CURSOR_AXIS_NAME, axis_pos);
            signals (message);
          }

          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY delta %f", CURSOR_AXIS_NAME, ((float)((float)window_event_context.cursor_position.y - (float)y_cursor_pos)) * cursor_sensitivity);
          signals (message);
        }

        y_cursor_pos = window_event_context.cursor_position.y;
      }

      if (autocenter_cursor)
      {
        x_cursor_pos = window.Width () / 2;
        y_cursor_pos = window.Height () / 2;
        window.SetCursorPosition (window.Width () / 2, window.Height () / 2);
      }

      break;
    }
    case WindowEvent_OnMouseVerticalWheel:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY delta %f", WHEEL_AXIS_NAME, window_event_context.mouse_vertical_wheel_delta * vertical_wheel_sensitivity);
      signals (message);

      if (window_event_context.mouse_vertical_wheel_delta > 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYUp down", WHEEL_AXIS_NAME);
        signals (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYUp up", WHEEL_AXIS_NAME);
        signals (message);
      }

      if (window_event_context.mouse_vertical_wheel_delta < 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYDown down", WHEEL_AXIS_NAME);
        signals (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sYDown up", WHEEL_AXIS_NAME);
        signals (message);
      }

      break;
    case WindowEvent_OnMouseHorisontalWheel:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX delta %f", WHEEL_AXIS_NAME, window_event_context.mouse_horisontal_wheel_delta * horisontal_wheel_sensitivity);
      signals (message);

      if (window_event_context.mouse_horisontal_wheel_delta > 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXRight down", WHEEL_AXIS_NAME);
        signals (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXRight up", WHEEL_AXIS_NAME);
        signals (message);
      }

      if (window_event_context.mouse_horisontal_wheel_delta < 0)
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXLeft down", WHEEL_AXIS_NAME);
        signals (message);
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sXLeft up", WHEEL_AXIS_NAME);
        signals (message);
      }

      break;
    case WindowEvent_OnLeftButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 dblclk");
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 down");
      signals (message);
      break;
    case WindowEvent_OnLeftButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 down");
      signals (message);
      break;
    case WindowEvent_OnLeftButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse0 up");
      signals (message);
      break;
    case WindowEvent_OnRightButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 dblclk");
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 down");
      signals (message);
      break;
    case WindowEvent_OnRightButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 down");
      signals (message);
      break;
    case WindowEvent_OnRightButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse1 up");
      signals (message);
      break;
    case WindowEvent_OnMiddleButtonDoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 dblclk");
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 down");
      signals (message);
      break;
    case WindowEvent_OnMiddleButtonDown:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 down");
      signals (message);
      break;
    case WindowEvent_OnMiddleButtonUp:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Mouse2 up");
      signals (message);
      break;
    case WindowEvent_OnXButton1DoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 dblclk");
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 down");
      signals (message);
      break;
    case WindowEvent_OnXButton1Down:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 down");
      signals (message);
      break;
    case WindowEvent_OnXButton1Up:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX1 up");
      signals (message);
      break;
    case WindowEvent_OnXButton2DoubleClick:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 dblclk");
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 down");
      signals (message);
      break;
    case WindowEvent_OnXButton2Down:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 down");
      signals (message);
      break;
    case WindowEvent_OnXButton2Up:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "MouseX2 up");
      signals (message);
      break;
    case WindowEvent_OnKeyDown:
      if (pressed_keys[window_event_context.key])
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' repeat", get_key_name (window_event_context.key));
      else
      {
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' down", get_key_name (window_event_context.key));
        pressed_keys.set (window_event_context.key);
      }
      signals (message);
      break;
    case WindowEvent_OnKeyUp:
      pressed_keys.reset (window_event_context.key);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' up", get_key_name (window_event_context.key));
      signals (message);
      break;
    case WindowEvent_OnChar:
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "Char '%C'", window_event_context.char_code);
      signals (message);
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "CharCode %u", window_event_context.char_code);
      signals (message);
      break;
    default:
      break;
  }
}
