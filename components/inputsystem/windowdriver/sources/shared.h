#ifndef INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER
#define INPUT_LOW_LEVEL_WINDOW_DRIVER_SHARED_HEADER

#include <stl/string>
#include <stl/vector>
#include <stl/bitset>

#include <xtl/reference_counter.h>
#include <xtl/function.h>

#include <input/low_level/device.h>

#include <syslib/window.h>
#include <syslib/keydefs.h>

namespace input
{

namespace low_level
{

namespace window
{

class Device: virtual public input::low_level::IDevice, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device (syslib::Window* window, const char* name);
    ~Device ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetEventHandler (const input::low_level::IDevice::EventHandler& handler)
    {
      event_handler = handler;
    }
    const input::low_level::IDevice::EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties ();
    void        SetProperty   (const char* name, float value);
    float       GetProperty   (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик сообщений окна
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void WindowEventHandler (syslib::Window& window, syslib::WindowEvent event, const syslib::WindowEventContext& window_event_context);

  private:
    Device (const Device& source);             //no impl
    Device& operator = (const Device& source); //no impl
    Device ();                                 //no impl

  private:
    stl::vector<xtl::connection>            connections;                  //подключения на события окна
    stl::string                             name;                         //имя устройства
    stl::string                             properties;                   //настройки
    input::low_level::IDevice::EventHandler event_handler;                //обработчик событий
    size_t                                  x_cursor_pos;                 //последние координаты курсора
    size_t                                  y_cursor_pos;                 //последние координаты курсора
    bool                                    autocenter_cursor;            //автоматическое центрирование курсора
    float                                   cursor_sensitivity;           //множитель delt'ы курсора
    float                                   vertical_wheel_sensitivity;   //множитель delt'ы вертикального колеса мыши
    float                                   horisontal_wheel_sensitivity; //множитель delt'ы горизонтального колеса мыши
    stl::bitset<syslib::ScanCode_Num>       pressed_keys;                 //какие кнопки являются нажатыми
};

}

}

}

#endif
