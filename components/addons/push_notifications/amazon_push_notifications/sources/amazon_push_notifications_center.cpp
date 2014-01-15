#include "shared.h"

#include <syslib/thread.h>

using namespace push_notifications;
using namespace push_notifications::amazon_push_notifications;
using namespace syslib::android;

namespace
{

const char* LOG_NAME           = "push_notifications.amazon_push_notifications.AmazonCenter";  //имя протокола
const char* CENTER_DESCRIPTION = "AmazonCenter";                                               //описание центра пуш-сообщений

//forward declarations
void on_registered (JNIEnv& env, jobject, jstring reg_id);
void on_message (JNIEnv& env, jobject, jstring message);
void on_error (JNIEnv& env, jobject, jstring error);

//Центр обработки пуш-сообщений
class AmazonCenter
{
  public:
    ///Конструктор/деструктор
    AmazonCenter ()
      : log (LOG_NAME)
      , is_registering_for_notifications (false)
      , adm_manager_class (0)
    {}

    ~AmazonCenter ()
    {
      if (adm_manager_class)
        get_env ().DeleteGlobalRef (adm_manager_class);
    }

    ///Регистрация на пуш-сообщения
    void RegisterForNotifications (const PushNotificationsCenter::RegisterCallback& callback, const common::PropertyMap& properties)
    {
      try
      {
        if (is_registering_for_notifications)
          throw xtl::format_operation_exception ("", "Previous registration attempt is not finished yet");

        JNIEnv& env = get_env ();

        local_ref<jobject> activity = get_activity ();

        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");

        jmethodID register_method = find_static_method (&env, adm_manager_class, "registerForADMMessages", "(Landroid/content/Context;)V");

        is_registering_for_notifications    = true;
        register_for_notifications_callback = callback;

        env.CallStaticVoidMethod (adm_manager_class, register_method, activity.get ());
      }
      catch (xtl::exception& e)
      {
        is_registering_for_notifications    = false;
        register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();

        e.touch ("push_notifications::AmazonCenter::RegisterForNotifications");
        throw;
      }
    }

    void UnregisterForNotifications ()
    {
      try
      {
        JNIEnv& env = get_env ();

        local_ref<jobject> activity = get_activity ();

        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");

        jmethodID unregister_method = find_static_method (&env, adm_manager_class, "unregisterForADMMessages", "(Landroid/content/Context;)V");

        env.CallStaticVoidMethod (adm_manager_class, unregister_method, activity.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("push_notifications::AmazonCenter::UnregisterForNotifications");
        throw;
      }
    }

    ///Подписка на пуш-сообщения
    xtl::connection RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler& handler)
    {
      return notifications_signal.connect (handler);
    }

    ///Обработка пуш-сообщений
    void OnNotificationReceived (const stl::string& notification)
    {
      notifications_signal (notification.c_str ());
    }

    ///Обработка результата регистрации на пуш-сообщения
    void OnRegisterForNotificationsSucceeded (const stl::string& token)
    {
      is_registering_for_notifications = false;

      register_for_notifications_callback (true, "", token.c_str ());
      register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();
    }

    void OnRegisterForNotificationsFailed (const stl::string& error)
    {
      is_registering_for_notifications = false;

      register_for_notifications_callback (false, error.c_str (), "");
      register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();
    }

    ///Инициализация java-биндинга
    void InitJavaBindings (JNIEnv* env)
    {
      static const char* METHOD_NAME = "push_notifications::amazon_push_notifications::AmazonCenter::InitJavaBindings";

      if (!env)
        throw xtl::make_null_argument_exception (METHOD_NAME, "env");

      jclass adm_service_class = env->FindClass ("com/untgames/funner/push_notifications/ADMService");

      if (!adm_service_class)
      {
        if (env->ExceptionOccurred ())
          env->ExceptionClear ();

        log_error ("Amazon push notifications linked, but ADMService class not found. Push notifications not supported.\n");
        return;
      }

      jclass adm_manager_class_ref = env->FindClass ("com/untgames/funner/push_notifications/EngineADMManager");

      if (!adm_manager_class_ref)
      {
        if (env->ExceptionOccurred ())
          env->ExceptionClear ();

        log_error ("Amazon push notifications linked, but EngineADMManager class not found. Push notifications not supported.\n");
        return;
      }

      try
      {
        static const JNINativeMethod methods [] = {
          {"onRegisteredCallback", "(Ljava/lang/String;)V", (void*)&on_registered},
          {"onMessageCallback", "(Ljava/lang/String;)V", (void*)&on_message},
          {"onErrorCallback", "(Ljava/lang/String;)V", (void*)&on_error},
        };

        static const size_t methods_count = sizeof (methods) / sizeof (*methods);

        jint status = env->RegisterNatives (adm_service_class, methods, methods_count);

        if (status)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't register natives (status=%d)", status);

        static const JNINativeMethod manager_methods [] = {
          {"onRegisteredCallback", "(Ljava/lang/String;)V", (void*)&on_registered},
          {"onErrorCallback", "(Ljava/lang/String;)V", (void*)&on_error},
        };

        static const size_t manager_methods_count = sizeof (manager_methods) / sizeof (*manager_methods);

        status = env->RegisterNatives (adm_manager_class_ref, manager_methods, manager_methods_count);

        if (status)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't register natives (status=%d)", status);

        adm_manager_class = (jclass)env->NewGlobalRef (adm_manager_class_ref);
      }
      catch (...)
      {
        env->DeleteLocalRef (adm_service_class);
        env->DeleteLocalRef (adm_manager_class_ref);

        throw;
      }

      env->DeleteLocalRef (adm_service_class);
      env->DeleteLocalRef (adm_manager_class_ref);
    }

    bool IsApiAvailable ()
    {
      return adm_manager_class;
    }

  private:
    typedef xtl::signal<void (const char*)> NotificationsSignal;

  private:
    common::Log                               log;                                             //протокол
    NotificationsSignal                       notifications_signal;                            //сигнал оповещения о новых сообщениях
    bool                                      is_registering_for_notifications;                //активен ли в данный момент процесс регистрации на пуш-сообщения
    PushNotificationsCenter::RegisterCallback register_for_notifications_callback;             //колбек результата регистрации на пуш-сообщения
    jclass                                    adm_manager_class;                               //EngineADMManager class
};

typedef common::Singleton<AmazonCenter> AmazonCenterSingleton;

void on_registered (JNIEnv& env, jobject, jstring reg_id)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonCenter::OnRegisterForNotificationsSucceeded, &*AmazonCenterSingleton::Instance (), tostring (reg_id)), common::ActionThread_Main);
}

void on_error (JNIEnv& env, jobject, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonCenter::OnRegisterForNotificationsFailed, &*AmazonCenterSingleton::Instance (), tostring (error)), common::ActionThread_Main);
}

void on_message (JNIEnv& env, jobject, jstring message)
{
  common::ActionQueue::PushAction (xtl::bind (&AmazonCenter::OnNotificationReceived, &*AmazonCenterSingleton::Instance (), tostring (message)), common::ActionThread_Main);
}

}

/*
   Конструктор / деструктор
*/

PushNotificationsCenterImpl::PushNotificationsCenterImpl ()
{
  if (!AmazonCenterSingleton::Instance ()->IsApiAvailable ())
    throw xtl::format_operation_exception ("push_notifications::amazon_push_notifications::PushNotificationsCenterImpl::PushNotificationsCenterImpl", "ADM API not available");
}

PushNotificationsCenterImpl::~PushNotificationsCenterImpl ()
{
}

/*
   Описание
*/

const char* PushNotificationsCenterImpl::Description ()
{
  return CENTER_DESCRIPTION;
}

/*
   Регистрация на пуш-сообщения
*/

void PushNotificationsCenterImpl::RegisterForNotifications (const PushNotificationsCenter::RegisterCallback& callback, const common::PropertyMap& properties)
{
  AmazonCenterSingleton::Instance ()->RegisterForNotifications (callback, properties);
}

void PushNotificationsCenterImpl::UnregisterForNotifications ()
{
  AmazonCenterSingleton::Instance ()->UnregisterForNotifications ();
}

/*
   Подписка на пуш-сообщения
*/

xtl::connection PushNotificationsCenterImpl::RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler& handler)
{
  return AmazonCenterSingleton::Instance ()->RegisterNotificationsHandler (handler);
}

/*
   Инициализация java-биндинга
*/

void PushNotificationsCenterImpl::InitJavaBindings (JNIEnv* env)
{
  AmazonCenterSingleton::Instance ()->InitJavaBindings (env);
}
