#include <shared/shared.h>

using namespace xtl;
using namespace common;
using namespace input::low_level::window;
using namespace syslib;

namespace
{

const size_t MESSAGE_BUFFER_SIZE = 32;
const char*  CURSOR_AXIS_NAME    = "Cursor";
const char*  WHEEL_AXIS_NAME     = "Wheel";

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

struct Device::Impl : private xtl::trackable
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string                  name;                         //им€ устройства
  stl::string                  full_name;                    //полное им€ устройства
  stl::string                  properties;                   //настройки
  DeviceSignal                 signals;                      //обработчики событий
  size_t                       x_cursor_pos;                 //последние координаты курсора
  size_t                       y_cursor_pos;                 //последние координаты курсора
  bool                         mouse_in_window;              //курсор мыши в пределах клиентской области окна
  bool                         autocenter_cursor;            //автоматическое центрирование курсора
  float                        cursor_sensitivity;           //множитель delt'ы курсора
  float                        vertical_wheel_sensitivity;   //множитель delt'ы вертикального колеса мыши
  float                        horisontal_wheel_sensitivity; //множитель delt'ы горизонтального колеса мыши
  stl::bitset<syslib::Key_Num> pressed_keys;                 //какие кнопки €вл€ютс€ нажатыми
  stl::wstring                 control_name;                 //им€ контрола

  /// онструктор
  Impl (Window* window, const char* in_name, const char* in_full_name)
    : name (in_name)
    , full_name (in_full_name)
    , x_cursor_pos (window->CursorPosition ().x)
    , y_cursor_pos (window->CursorPosition ().y)
    , mouse_in_window (false)
    , autocenter_cursor (false)
    , cursor_sensitivity (1.f)
    , vertical_wheel_sensitivity (1.f)
    , horisontal_wheel_sensitivity (1.f)
  {
    try
    {
      Window::EventHandler handler (xtl::bind (&Device::Impl::WindowEventHandler, this, _1, _2, _3));

      static WindowEvent events [] = {
                                      WindowEvent_OnMouseMove,
                                      WindowEvent_OnMouseLeave,
                                      WindowEvent_OnMouseVerticalWheel,
                                      WindowEvent_OnMouseHorisontalWheel,
                                      WindowEvent_OnLeftButtonDown,
                                      WindowEvent_OnLeftButtonUp,
                                      WindowEvent_OnLeftButtonDoubleClick,
                                      WindowEvent_OnRightButtonDown,
                                      WindowEvent_OnRightButtonUp,
                                      WindowEvent_OnRightButtonDoubleClick,
                                      WindowEvent_OnMiddleButtonDown,
                                      WindowEvent_OnMiddleButtonUp,
                                      WindowEvent_OnMiddleButtonDoubleClick,
                                      WindowEvent_OnXButton1Down,
                                      WindowEvent_OnXButton1Up,
                                      WindowEvent_OnXButton1DoubleClick,
                                      WindowEvent_OnXButton2Down,
                                      WindowEvent_OnXButton2Up,
                                      WindowEvent_OnXButton2DoubleClick,
                                      WindowEvent_OnKeyDown,
                                      WindowEvent_OnKeyUp,
                                      WindowEvent_OnChar,
                                      WindowEvent_OnClose,
                                      WindowEvent_OnActivate,
                                      WindowEvent_OnDeactivate,
                                      WindowEvent_OnShow,
                                      WindowEvent_OnHide,
                                      WindowEvent_OnSetFocus,
                                      WindowEvent_OnLostFocus
      };

      static size_t events_num = sizeof (events) / sizeof (*events);

      for (size_t i=0; i<events_num; i++)
        connect_tracker (window->RegisterEventHandler (events [i], handler));

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
  }

  ///ќбработчик сообщений окна
  void WindowEventHandler (syslib::Window& window, syslib::WindowEvent event, const syslib::WindowEventContext& window_event_context)
  {
    static char message [MESSAGE_BUFFER_SIZE];
    float       axis_pos;

    switch (event)
    {
      case WindowEvent_OnMouseMove:
      {
        Rect viewport_rect = window.Viewport ();

        size_t viewport_width  = viewport_rect.right - viewport_rect.left;
        size_t viewport_height = viewport_rect.bottom - viewport_rect.top;

        Point cursor_position_in_viewport = window_event_context.cursor_position;

        cursor_position_in_viewport.x = cursor_position_in_viewport.x > viewport_rect.left ? cursor_position_in_viewport.x - viewport_rect.left : 0;
        cursor_position_in_viewport.y = cursor_position_in_viewport.y > viewport_rect.top ? cursor_position_in_viewport.y - viewport_rect.top : 0;

        if (window_event_context.cursor_position.x < viewport_rect.left || window_event_context.cursor_position.y < viewport_rect.top ||
            cursor_position_in_viewport.x > viewport_width || cursor_position_in_viewport.y > viewport_height)
        {
          OnLeave ();
          break;
        }

        if (!mouse_in_window)
        {
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s enter", CURSOR_AXIS_NAME);
          signals (message);

          mouse_in_window = true;
        }

        if (x_cursor_pos == cursor_position_in_viewport.x && y_cursor_pos == cursor_position_in_viewport.y)
          break;

        xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s at %u %u", CURSOR_AXIS_NAME, cursor_position_in_viewport.x, cursor_position_in_viewport.y);
        signals (message);

        if (x_cursor_pos != cursor_position_in_viewport.x)
        {
          if (window.Width ())
          {
            if (!autocenter_cursor)
            {
              axis_pos = (float)cursor_position_in_viewport.x * 2.f / viewport_width - 1.f;

              xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX axis %f", CURSOR_AXIS_NAME, axis_pos);
              signals (message);
            }

            xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sX delta %f", CURSOR_AXIS_NAME, ((float)((float)window_event_context.cursor_position.x - (float)x_cursor_pos)) * cursor_sensitivity);
            signals (message);
          }

          x_cursor_pos = cursor_position_in_viewport.x;
        }

        if (y_cursor_pos != cursor_position_in_viewport.y)
        {
          if (window.Height ())
          {
            if (!autocenter_cursor)
            {
              axis_pos = -(float)cursor_position_in_viewport.y * 2.f / viewport_height + 1.f;

              xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY axis %f", CURSOR_AXIS_NAME, axis_pos);
              signals (message);
            }

            xsnprintf (message, MESSAGE_BUFFER_SIZE, "%sY delta %f", CURSOR_AXIS_NAME, ((float)((float)window_event_context.cursor_position.y - (float)y_cursor_pos)) * cursor_sensitivity);
            signals (message);
          }

          y_cursor_pos = cursor_position_in_viewport.y;
        }

        if (autocenter_cursor)
        {
          x_cursor_pos = window.Width () / 2;
          y_cursor_pos = window.Height () / 2;
          window.SetCursorPosition (window.Width () / 2, window.Height () / 2);
        }

        break;
      }
      case WindowEvent_OnMouseLeave:
        OnLeave ();
        break;
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
        signals ("Mouse0 dblclk");
        signals ("Mouse0 down");
        break;
      case WindowEvent_OnLeftButtonDown:
        signals ("Mouse0 down");
        break;
      case WindowEvent_OnLeftButtonUp:
        signals ("Mouse0 up");
        break;
      case WindowEvent_OnRightButtonDoubleClick:
        signals ("Mouse1 dblclk");
        signals ("Mouse1 down");
        break;
      case WindowEvent_OnRightButtonDown:
        signals ("Mouse1 down");
        break;
      case WindowEvent_OnRightButtonUp:
        signals ("Mouse1 up");
        break;
      case WindowEvent_OnMiddleButtonDoubleClick:
        signals ("Mouse2 dblclk");
        signals ("Mouse2 down");
        break;
      case WindowEvent_OnMiddleButtonDown:
        signals ("Mouse2 down");
        break;
      case WindowEvent_OnMiddleButtonUp:
        signals ("Mouse2 up");
        break;
      case WindowEvent_OnXButton1DoubleClick:
        signals ("MouseX1 dblclk");
        signals ("MouseX1 down");
        break;
      case WindowEvent_OnXButton1Down:
        signals ("MouseX1 down");
        break;
      case WindowEvent_OnXButton1Up:
        signals ("MouseX1 up");
        break;
      case WindowEvent_OnXButton2DoubleClick:
        signals ("MouseX2 dblclk");
        signals ("MouseX2 down");
        break;
      case WindowEvent_OnXButton2Down:
        signals ("MouseX2 down");
        break;
      case WindowEvent_OnXButton2Up:
        signals ("MouseX2 up");
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
      case WindowEvent_OnClose:
        signals ("Window closed");
        break;
      case WindowEvent_OnActivate:
        signals ("Window activated");
        break;
      case WindowEvent_OnDeactivate:
        signals ("Window deactivated");
        break;
      case WindowEvent_OnShow:
        signals ("Window shown");
        break;
      case WindowEvent_OnHide:
        signals ("Window hidden");
        break;
      case WindowEvent_OnSetFocus:
        signals ("Window set_focus");
        break;
      case WindowEvent_OnLostFocus:
        signals ("Window lost_focus");
        break;
      default:
        break;
    }
  }

  void OnLeave ()
  {
    if (!mouse_in_window)
      return;

    static char message [MESSAGE_BUFFER_SIZE];

    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s leave", CURSOR_AXIS_NAME);
    signals (message);

    mouse_in_window = false;
  }
};

/*
    онструктор/деструктор
*/

Device::Device (Window* window, const char* name, const char* full_name)
  : impl (new Impl (window, name, full_name))
{
}

Device::~Device ()
{
  delete impl;
}

/*
   ѕолучение имени контрола
*/

const wchar_t* Device::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   ѕодписка на событи€ устройства
*/

xtl::connection Device::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   Ќастройки устройства
*/

const char* Device::GetProperties ()
{
  return impl->properties.c_str ();
}

void Device::SetProperty (const char* name, float value)
{
  if (!xstrcmp (AUTOCENTER_CURSOR, name))
  {
    impl->autocenter_cursor = value != 0;
    return;
  }

  if (!xstrcmp (CURSOR_SENSITIVITY, name))
  {
    impl->cursor_sensitivity = value;
    return;
  }

  if (!xstrcmp (VERTICAL_WHEEL_SENSITIVITY, name))
  {
    impl->vertical_wheel_sensitivity = value;
    return;
  }

  if (!xstrcmp (HORISONTAL_WHEEL_SENSITIVITY, name))
  {
    impl->horisontal_wheel_sensitivity = value;
    return;
  }

  throw xtl::make_argument_exception ("input::low_level::window::Device::SetProperty", "name", name);
}

float Device::GetProperty (const char* name)
{
  if (!xstrcmp (AUTOCENTER_CURSOR, name))
  {
    if (impl->autocenter_cursor)
      return 1.f;
    return 0.f;
  }

  if (!xstrcmp (CURSOR_SENSITIVITY, name))
    return impl->cursor_sensitivity;

  if (!xstrcmp (VERTICAL_WHEEL_SENSITIVITY, name))
    return impl->vertical_wheel_sensitivity;

  if (!xstrcmp (HORISONTAL_WHEEL_SENSITIVITY, name))
    return impl->horisontal_wheel_sensitivity;

  throw xtl::make_argument_exception ("input::low_level::window::Device::GetProperty", "name", name);
}

/*
   ѕолучение имени устройства
*/

const char* Device::GetName ()
{
  return impl->name.c_str ();
}

/*
   ѕолное им€ устройства (тип.им€.идентификатор)
*/

const char* Device::GetFullName ()
{
  return impl->full_name.c_str ();
}
