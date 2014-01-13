#ifndef PUSH_NOTIFICATIONS_CORE_SHARED_HEADER
#define PUSH_NOTIFICATIONS_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <push_notifications/push_notifications_center.h>

namespace push_notifications
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер центров push-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
class PushNotificationsCentersManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PushNotificationsCentersManagerImpl  ();
    ~PushNotificationsCentersManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация создателей центров пуш-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterCenter       (const char* id, const char* center_name_mask, const PushNotificationsCentersManager::CreateCenterHandler& handler);
    void UnregisterCenter     (const char* id);
    void UnregisterAllCenters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка наличия центра пуш-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCenterRegistered (const char* center_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание центра пуш-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPushNotificationsCenter* CreateCenter (const char* center_name);

  private:
    PushNotificationsCentersManagerImpl (const PushNotificationsCentersManagerImpl&);             //no impl
    PushNotificationsCentersManagerImpl& operator = (const PushNotificationsCentersManagerImpl&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

typedef common::Singleton<PushNotificationsCentersManagerImpl> PushNotificationsCentersManagerSingleton;

}

#endif
