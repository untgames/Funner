#include "shared.h"

using namespace push_notifications;

/*
   Реализация центра обработки пуш-сообщений
*/

struct PushNotificationsCenter::Impl : public xtl::reference_counter
{
  IPushNotificationsCenter* center; //центр обработки пуш-сообщений

  Impl (const char* center_name)
    : center (0)
  {
    try
    {
      if (!center_name)
        throw xtl::make_null_argument_exception ("", "center_name");

      center = PushNotificationsCentersManagerSingleton::Instance ()->CreateCenter (center_name);
    }
    catch (xtl::exception& e)
    {
      e.touch ("push_notifications::PushNotificationsCenter::PushNotificationsCenter");
      throw;
    }
  }

  ~Impl ()
  {
    delete center;
  }
};

/*
   Конструктор / деструктор / копирование
*/

PushNotificationsCenter::PushNotificationsCenter (const char* center_name)
  : impl (new Impl (center_name))
  {}

PushNotificationsCenter::PushNotificationsCenter (const PushNotificationsCenter& source)
  : impl (source.impl)
{
  addref (impl);
}

PushNotificationsCenter::~PushNotificationsCenter ()
{
  release (impl);
}

PushNotificationsCenter& PushNotificationsCenter::operator = (const PushNotificationsCenter& source)
{
  PushNotificationsCenter (source).Swap (*this);

  return *this;
}

/*
   Описание
*/

const char* PushNotificationsCenter::Description () const
{
  return impl->center->Description ();
}

/*
   Регистрация на пуш-сообщения
*/

void PushNotificationsCenter::RegisterForNotifications (const RegisterCallback& callback, const common::PropertyMap& properties) const
{
  impl->center->RegisterForNotifications (callback, properties);
}

/*
   Подписка на пуш-сообщения
*/

xtl::connection PushNotificationsCenter::RegisterNotificationsHandler (const NotificationsHandler& handler)
{
  return impl->center->RegisterNotificationsHandler (handler);
}

/*
   Обмен
*/

void PushNotificationsCenter::Swap (PushNotificationsCenter& source)
{
  stl::swap (source.impl, impl);
}

namespace push_notifications
{

/*
   Обмен
*/

void swap (PushNotificationsCenter& center1, PushNotificationsCenter& center2)
{
  center1.Swap (center2);
}

}
