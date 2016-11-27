#ifndef INPUT_SYSTEM_LOW_LEVEL_DEVICE_HEADER
#define INPUT_SYSTEM_LOW_LEVEL_DEVICE_HEADER

#include <xtl/functional_fwd>

namespace input
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Устройство ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Полное имя устройства (тип.имя.идентификатор)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetFullName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени контрола
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const wchar_t* GetControlName (const char* control_id) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* event)> EventHandler;

    virtual xtl::connection RegisterEventHandler (const EventHandler& handler) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetProperties () = 0; //список имён всех допустимых свойств
    virtual void        SetProperty   (const char* name, float value) = 0;
    virtual float       GetProperty   (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IDevice () {}
};

}

}

#endif
