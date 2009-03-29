#ifndef INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER
#define INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER

#include <cstdio>

#include <stl/algorithm>
#include <stl/vector>
#include <stl/string>
#include <stl/bitset>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/string.h>
#include <xtl/trackable.h>

#include <common/singleton.h>
#include <common/utf_converter.h>

#include <syslib/window.h>
#include <syslib/keydefs.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>
#include <input/low_level/window_driver.h>

namespace input
{

namespace low_level
{

namespace window
{

class Device: virtual public input::low_level::IDevice, public xtl::reference_counter, private xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// онструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device (syslib::Window* window, const char* name);
    ~Device ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕолучение имени устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕолное им€ устройства (тип.им€.идентификатор)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetFullName () { return full_name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕолучение имени контрола
///////////////////////////////////////////////////////////////////////////////////////////////////
    const wchar_t* GetControlName (const char* control_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕодписка на событи€ устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ќастройки устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties ();
    void        SetProperty   (const char* name, float value);
    float       GetProperty   (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕодсчЄт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///ќбработчик сообщений окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WindowEventHandler (syslib::Window& window, syslib::WindowEvent event, const syslib::WindowEventContext& window_event_context);

  private:
    Device (const Device& source);             //no impl
    Device& operator = (const Device& source); //no impl

  private:
    typedef xtl::signal<void (const char*)> DeviceSignal;

  private:
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
};

}

}

}

#endif
