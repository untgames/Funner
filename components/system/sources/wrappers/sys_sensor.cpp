#include "shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const float DEFAULT_SENSOR_MANAGER_UPDATE_RATE = 10.0f; //частота обновления сенсоров по умолчанию

}

/*
===================================================================================================
    Sensor
===================================================================================================
*/

namespace syslib
{

class SensorImpl: public ISensorEventListener, public xtl::reference_counter, public xtl::trackable
{
  public:
///Конструктор
    SensorImpl (size_t index)
      : handle ()
      , paused (false)
    {
      try
      {
        handle = Platform::CreateSensor (index);
        
        if (!handle)
          throw xtl::format_operation_exception ("", "Null sensor handle returned from Platform::CreateSensor");
          
        name   = Platform::GetSensorName (handle);
        type   = Platform::GetSensorType (handle);
        vendor = Platform::GetSensorVendor (handle);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::SensorImpl");
        throw;
      }
    }
    
///Деструктор
    ~SensorImpl ()
    {
      try
      {
        mutex.Lock ();
        
        disconnect_all_trackers ();        
        
        Platform::DestroySensor (handle);
      }
      catch (...)
      {
      }
    }
    
///Дескриптор сенсора
    sensor_t Handle () { return handle; }
    
///Имя сенсора
    const char* Name () { return name.c_str (); }
    
///Тип сенсора
    const char* Type () { return type.c_str (); }
    
///Производитель сенсора
    const char* Vendor () { return vendor.c_str (); }
    
///Регистрация обработчиков событий
    xtl::connection RegisterEventHandler (const Sensor::EventHandler& handler)
    {
      xtl::connection connection;
      
      try
      {
        Lock lock (mutex);
        
        Sensor::EventHandler wrapped_handler = EventHandlerWrapper (handler);
        
        connection = signal.connect (wrapped_handler);         
        
        connect_tracker (connection);
        
        if (signal.num_slots () == 1 && !paused)
        {
          Platform::StartSensorPolling (handle, *this);
        }
        
        wrapped_handler.target<EventHandlerWrapper> ()->impl = this;
        
        return connection;
      }
      catch (...)
      {
        connection.disconnect ();
        throw;
      }
    }
    
///Опрос сенсора
    void Poll ()
    {
      try
      {
        if (paused)
          return;
          
        Platform::PollSensorEvents (handle);

        while (!events.empty ())
        {                    
          SensorEvent event;
          
          {
            Lock lock (mutex);
          
            event = events.front ();          
          
            events.pop_front ();
          }

          signal (event);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Poll");
        throw;
      }
    }
    
///Реакция на паузу приложения
    void Pause ()
    {
      try
      {        
        Lock lock (mutex);
        
        if (paused)
          return;
          
        if (signal.num_slots () > 0)
          Platform::StopSensorPolling (handle);
          
        paused = true;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Pause");
        throw;
      }
    }
    
///Реакция на восстановление приложения
    void Resume ()
    {
      try
      {
        Lock lock (mutex);

        if (!paused)
          return;                
          
        if (signal.num_slots () > 0)
          Platform::StartSensorPolling (handle, *this);
          
        paused = false;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorImpl::Resume");
        throw;
      }      
    }

///Обработка события сенсора
    void OnSensorChanged (const SensorEvent& event)
    {
      Lock lock (mutex);
      
      events.push_back (event);
    }

  private:
    typedef xtl::signal<Sensor::EventHandler::signature_type> Signal;
    typedef stl::deque<SensorEvent>                           EventQueue;
    
    struct EventHandlerWrapper
    {
      Sensor::EventHandler handler;
      SensorImpl*          impl;
      
      EventHandlerWrapper (const Sensor::EventHandler& in_handler)
        : handler (in_handler)
        , impl ()
      {
      }
      
      ~EventHandlerWrapper ()
      {
        try
        {
          if (impl)
            impl->CheckPolling ();
        }
        catch (...)
        {
        }
      }
      
      void operator () (SensorEvent& event) const
      {
        handler (event);
      }
    };
    
    void CheckPolling ()
    {
      Lock lock (mutex);      
      
      if (signal.num_slots () == 1 && !paused)
        Platform::StopSensorPolling (handle);
    }
  
  private:
    Mutex       mutex;  //мьютекс доступа к сенсору
    sensor_t    handle; //дескриптор сенсора
    Signal      signal; //сигнал событий сенсора
    bool        paused; //флаг паузы принятия событий сенсора
    stl::string name;   //имя сенсора
    stl::string type;   //тип сенсора
    stl::string vendor; //производитель сенсора
    EventQueue  events; //события сенсора
};

}

namespace
{

typedef xtl::intrusive_ptr<SensorImpl> SensorPtr;

/// Менеджер сенсоров
class SensorManagerImpl: public xtl::trackable
{
  public:
///Конструктор
    SensorManagerImpl ()
      : paused (false)      
      , update_rate (0.0f)
    {
      try
      {
        sensors.resize (syslib::Platform::GetSensorsCount ());

        timer.reset (new Timer (xtl::bind (&SensorManagerImpl::PollSensors, this)));

        SetUpdateRate (DEFAULT_SENSOR_MANAGER_UPDATE_RATE);
        
        connect_tracker (Application::RegisterEventHandler (ApplicationEvent_OnPause, xtl::bind (&SensorManagerImpl::Pause, this)));
        connect_tracker (Application::RegisterEventHandler (ApplicationEvent_OnResume, xtl::bind (&SensorManagerImpl::Resume, this)));
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::SensorManagerImpl");
        throw;
      }
    }
    
///Получение сенсора
    SensorImpl& GetSensor (size_t index)
    {
      try
      {      
        if (index >= sensors.size ())
          throw xtl::make_range_exception ("", "index", index, sensors.size ());
          
        SensorPtr sensor = sensors [index];
        
        if (!sensor)
        {
          sensor = SensorPtr (new SensorImpl (index), false);   
          
          if (paused)
            sensor->Pause ();

          sensors [index] = sensor;
        }

        return *sensor;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::GetSensor");
        throw;
      }
    }
    
///Реакция на паузу приложения
    void Pause ()
    {
      try
      {
        if (paused)
          return;
        
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Pause ();
        
        paused = true;          
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::Pause");
        throw;
      }
    }
    
///Реакция на восстановление приложения
    void Resume ()
    {
      try
      {
        if (!paused)
          return;
        
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Resume ();
        
        paused = false; 
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::Resume");
        throw;
      }
    }
    
///Частота опроса очередей сенсоров
    void SetUpdateRate (float rate)
    {
      if (update_rate == rate)
        return;

      timer->SetPeriod (size_t (1000.0f / rate));
        
      update_rate = rate;
    }

    float UpdateRate ()
    {
      return update_rate;
    }

  private:
///Обновление сенсоров
    void PollSensors ()
    {
      try
      {
        if (paused)
          return;
          
        for (SensorArray::iterator iter=sensors.begin (), end=sensors.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->Poll ();        
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::SensorManagerImpl::PollSensors");
        throw;
      }
    }

  private:
    typedef stl::vector<SensorPtr> SensorArray;    

  private:
    SensorArray          sensors;      //список сенсоров
    bool                 paused;       //флаг паузы приложения
    float                update_rate;  //частота обновления сенсоров
    stl::auto_ptr<Timer> timer;        //таймер обновления сенсоров
};

typedef common::Singleton<SensorManagerImpl> SensorManagerSingleton;

}

/*
    Конструкторы / деструктор / присваивание
*/

Sensor::Sensor (size_t sensor_index)  
{
  try
  {
    impl = &SensorManagerSingleton::Instance ()->GetSensor (sensor_index);
    
    addref (impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Sensor(size_t)");
    throw;
  }
}

Sensor::Sensor (const Sensor& sensor)
  : impl (sensor.impl)
{
  addref (impl);
}

Sensor::~Sensor ()
{
  release (impl);
}

Sensor& Sensor::operator = (const Sensor& sensor)
{
  Sensor (sensor).Swap (*this);
  return *this;
}

/*
    Имя
*/

const char* Sensor::Name () const
{
  return impl->Name ();
}

/*
    Тип устройства
*/

const char* Sensor::Type () const
{
  return impl->Type ();
}

/*
    Производитель
*/

const char* Sensor::Vendor () const
{
  return impl->Vendor ();
}

/*
    Максимальное значение
*/

float Sensor::MaxRange () const
{
  try
  {
    return Platform::GetSensorMaxRange (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Name");
    throw;
  }
}

/*
    Частота обновления
*/

void Sensor::SetUpdateRate (float rate)
{
  try
  {
    Platform::SetSensorUpdateRate (impl->Handle (), rate);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::SetSensorUpdateRate");
    throw;
  }
}

float Sensor::UpdateRate () const
{
  try
  {
    return Platform::GetSensorUpdateRate (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::GetSensorUpdateRate");
    throw;
  }
}

/*
    Платформо-зависимый дескриптор сенсора
*/

const void* Sensor::Handle () const
{
  try
  {
    return Platform::GetNativeSensorHandle (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::Handle");
    throw;
  }
}

/*
    Получение платформо-зависимых свойств
*/

void Sensor::GetProperties (common::PropertyMap& properties) const
{
  try
  {
    properties.Clear ();
    
    return Platform::GetSensorProperties (impl->Handle (), properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::GetProperties");
    throw;
  }
}

/*
    Регистрация событий сенсора
*/

xtl::connection Sensor::RegisterEventHandler (const EventHandler& handler) const
{
  try
  {
    return impl->RegisterEventHandler (handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Sensor::RegisterEventHandler");
    throw;
  }
}

/*
    Обмен
*/

void Sensor::Swap (Sensor& sensor)
{
  stl::swap (impl, sensor.impl);
}
      
namespace syslib
{

void swap (Sensor& sensor1, Sensor& sensor2)
{
  sensor1.Swap (sensor2);
}

}

/*
    Сравнение
*/

bool Sensor::operator == (const Sensor& sensor) const
{
  return impl == sensor.impl;
}

bool Sensor::operator != (const Sensor& sensor) const
{
  return !(*this == sensor);
}

/*
===================================================================================================
    SensorManager
===================================================================================================
*/

/*
    Перечисление сенсоров
*/

size_t SensorManager::SensorsCount ()
{
  try
  {
    return Platform::GetSensorsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::SensorManager::SensorsCount");
    throw;
  }
}

syslib::Sensor SensorManager::Sensor (size_t index)
{
  try
  {
    return syslib::Sensor (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::SensorManager::Sensor");
    throw;
  }
}

/*
    Поиск сенсора по типу
*/

int SensorManager::FindSensorByType (const char* type)
{
  try
  {
    if (!type)
      return -1;
    
    for (int i = 0, count = (int)Platform::GetSensorsCount (); i < count; i++)
      if (!strcmp (SensorManager::Sensor (i).Type (), type))
        return i;
        
    return -1;
  }
  catch (...)
  {
    return -1;
  }
}

/*
    Частота опроса очередей сенсоров
*/

void SensorManager::SetUpdateRate (float rate)
{
  SensorManagerSingleton::Instance ()->SetUpdateRate (rate);
}

float SensorManager::UpdateRate ()
{
  return SensorManagerSingleton::Instance ()->UpdateRate ();
}
