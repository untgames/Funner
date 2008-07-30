#include <cstdio>
#include <cstring>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>
#include <input/events_source.h>
#include <input/controls_detector.h>

using namespace input;
using namespace input::low_level;
using namespace common;

const char* TEST_DEVICE_NAME = "TestInput";
const char* EVENTS [] = {
  "MouseX axis 0.7",
  "", 
  "MouseX halfaxis 0.35", 
  "'Page Down' down"};

class TestInput;

TestInput* current_input = 0;

//тестовое устройство ввода
class TestInput: virtual public IDevice, public xtl::reference_counter
{
  public:
    TestInput  () {current_input = this;}
    ~TestInput () {current_input = 0;}

///Получение имени устройства
    const char* GetName () { return TEST_DEVICE_NAME; }

///Подписка на события устройства
    void SetEventHandler (const EventHandler& handler)
    {
      event_handler = handler;
    }
    const EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

///Настройки устройства
    const char* GetProperties () {return "";}
    void        SetProperty   (const char* name, float value) 
    {
      throw xtl::make_argument_exception ("TestInput::SetProperty", "name", name);
    }
    float GetProperty   (const char* name)
    {
      throw xtl::make_argument_exception ("TestInput::GetProperty", "name", name);
      return 0;
    }    

///Настройки устройства
    void GenerateTestEvents ()
    {
      for (size_t i = 0; i < sizeof (EVENTS) / sizeof (EVENTS[0]); i++)
        event_handler (EVENTS[i]);
    }
    
///Подсчёт ссылок
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    float        dead_zone;
    float        saturation_zone;
    EventHandler event_handler;
};

//тестовый драйвер
class TestDriver: virtual public IDriver, public xtl::reference_counter
{
  public:
    TestDriver () {}

///Описание драйвера
    const char* GetDescription () { return "TestDriver"; }

///Перечисление доступных устройств ввода
    size_t      GetDevicesCount ()
    {
      return 1;
    }
    const char* GetDeviceName (size_t index)
    {
      return TEST_DEVICE_NAME;
    }

///Создаение устройства ввода
    IDevice* CreateDevice (const char* name, const char* init_string = "")
    {
      return new TestInput;
    }

///Установка функции отладочного протоколирования драйвера
    void SetDebugLog (const LogHandler& in_log)
    {
      log_fn = in_log;
    }
    
    const LogHandler& GetDebugLog ()
    {
      return log_fn;
    }
    
///Подсчёт ссылок
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    LogHandler log_fn;
};

void my_event_handler (const char* action, const char* event, const char* replacement)
{
  printf ("Detected event '%s' for action '%s' with replacement '%s'\n", event, action, replacement);
}

int main ()
{
  printf ("Results of controls_detector1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    EventsSource events_source ("*", "*");
    
    ControlsDetector controls_detector;

    controls_detector.Add ("Action1", "* axis *", "action1 {2}");
    controls_detector.Add ("Action1", "* down", "action1 1");
    controls_detector.Add ("Action1", "", "action1 empty event");
    
    controls_detector.Detect (events_source, "Action1", &my_event_handler);

    if (current_input)
      current_input->GenerateTestEvents ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
