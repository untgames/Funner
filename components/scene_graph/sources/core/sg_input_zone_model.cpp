#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации зоны ввода
*/

typedef xtl::signal<void (InputZoneModel& sender, InputZoneEvent event_id)>                                                   EventSignal;
typedef xtl::signal<void (InputZoneModel& sender, const Viewport& viewport, const char* notification_id, const char* params)> NotificationSignal;

struct Notification
{
  stl::hash_key<const char*> name_hash;
  NotificationSignal         signal;
  
  Notification () : name_hash ("") {}
  
  Notification (const Notification&) : name_hash ("") {}
};

typedef stl::list<Notification> NotificationList;

struct InputZoneModel::Impl
{
  NotificationList           notifications;                      //карта событий ввода
  NotificationList::iterator cached_notification;                //сохраненная последняя нотификация
  NotificationSignal         default_notification_signal;        //обработчик нотификаций по умолчанию
  EventSignal                event_signals [InputZoneEvent_Num]; //сигналы зоны
  bool                       is_active;                          //активна ли зона
  
///Конструктор
  Impl ()
    : cached_notification (notifications.end ())
    , is_active (true)
  {
  }
  
///Поиск нотификации
  Notification* FindNotification (const stl::hash_key<const char*>& name_hash)
  {
    if (cached_notification != notifications.end () && cached_notification->name_hash == name_hash)
    {
      return &*cached_notification;
    }
    else
    {
      for (NotificationList::iterator iter=notifications.begin (), end=notifications.end (); iter!=end;)
      {
        if (iter->signal.empty ())
        {          
          NotificationList::iterator next = iter;
          
          ++next;
          
          notifications.erase (iter);
          
          iter = next;
          
          continue;
        }
        
        if (iter->name_hash == name_hash)        
        {
          cached_notification = iter;
          return &*iter;
        }
        
        ++iter;
      }
      
      return 0;
    }        
  }

///Оповещение о событии
  void Notify (InputZoneModel& sender, InputZoneEvent event_id)
  {
    try
    {
      if (!event_signals [event_id])
        return;

      event_signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

InputZoneModel::InputZoneModel ()
  : impl (new Impl)
{
}

InputZoneModel::~InputZoneModel ()
{
}

/*
    Количество зон / получение массива зон
*/

size_t InputZoneModel::ZoneDescsCount () const
{
  return const_cast<InputZoneModel&> (*this).ZoneDescsCountCore ();
}

const InputZoneModel::ZoneDesc* InputZoneModel::ZoneDescs () const
{
  return const_cast<InputZoneModel&> (*this).ZoneDescsCore ();
}

/*
    Активность зоны
*/

bool InputZoneModel::IsActive () const
{
  return impl->is_active;
}

void InputZoneModel::SetActive (bool state)
{
  if (state == impl->is_active)
    return;
    
  impl->is_active = state;
  
  impl->Notify (*this, InputZoneEvent_OnActivityChanged);
  
  UpdateNotify ();
}

/*
    Подписка на события модели
*/

xtl::connection InputZoneModel::RegisterEventHandler (InputZoneEvent event_id, const EventHandler& event_handler) const
{
  switch (event_id)
  {
    case InputZoneEvent_AfterZoneDescsUpdate:
    case InputZoneEvent_OnActivityChanged:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::InputZoneModel::RegisterEventHandler", "event_id", event_id);
  }
  
  return impl->event_signals [event_id].connect (event_handler);
}

xtl::connection InputZoneModel::RegisterNotificationHandler (const char* notification_id, const NotificationHandler& handler) const
{
  if (!notification_id)
    throw xtl::make_null_argument_exception ("scene_graph::InputZoneModel::RegisterNotificationHandler", "notification_id");
    
  stl::hash_key<const char*> name_hash = notification_id;
    
  if (Notification* notification = impl->FindNotification (name_hash))
  {
    return notification->signal.connect (handler);
  }
  else
  {
    impl->notifications.push_back ();
    
    try
    {
      Notification& notification = impl->notifications.back ();
      
      notification.name_hash = name_hash;
      
      return notification.signal.connect (handler);
    }
    catch (...)
    {
      impl->notifications.pop_back ();
      throw;
    }
  }
}

xtl::connection InputZoneModel::RegisterNotificationHandler (const NotificationHandler& handler) const
{
  return impl->default_notification_signal.connect (handler);
}

/*
    Оповещение о возникновении события ввода
*/

void InputZoneModel::Notify (const Viewport& viewport, const char* notification_id, const char* notification_params) const
{
  try
  {
    if (!notification_id)
      throw xtl::make_null_argument_exception ("", "notification_id");

    if (!notification_params)
      throw xtl::make_null_argument_exception ("", "notification_params");            

    if (Notification* notification = impl->FindNotification (notification_id))
      notification->signal (const_cast<InputZoneModel&> (*this), viewport, notification_id, notification_params);

    impl->default_notification_signal (const_cast<InputZoneModel&> (*this), viewport, notification_id, notification_params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::InputZoneModel::Notify");
    throw;
  }
}

bool InputZoneModel::HasNotificationHandler (const char* notification_id) const
{    
  return notification_id && (!impl->default_notification_signal.empty () || impl->FindNotification (notification_id));
}

/*
    Динамическая диспетчеризация
*/

void InputZoneModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Оповещение об изменении данных зоны
*/

void InputZoneModel::UpdateZoneDescsNotify ()
{
  impl->Notify (*this, InputZoneEvent_AfterZoneDescsUpdate);

  UpdateNotify ();
}

/*
    Связывание свойств узла с методами узла
*/

void InputZoneModel::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Active", xtl::bind (&InputZoneModel::IsActive, this), xtl::bind (&InputZoneModel::SetActive, this, _1));
}
