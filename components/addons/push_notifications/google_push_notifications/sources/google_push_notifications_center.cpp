#include "shared.h"

#include <syslib/thread.h>

using namespace push_notifications;
using namespace push_notifications::google_push_notifications;
using namespace syslib::android;

namespace
{

const char* LOG_NAME           = "push_notifications.google_push_notifications.GoogleCenter";  //имя протокола
const char* CENTER_DESCRIPTION = "GoogleCenter";                                               //описание центра пуш-сообщений

//forward declarations
void on_registered (JNIEnv& env, jobject, jstring reg_id);
void on_message (JNIEnv& env, jobject, jstring message);
void on_error (JNIEnv& env, jobject, jstring error);

//Центр обработки пуш-сообщений
class GoogleCenter
{
  public:
    ///Конструктор/деструктор
    GoogleCenter ()
      : log (LOG_NAME)
      , is_registering_for_notifications (false)
      , gcm_manager_class (0)
    {}

    ~GoogleCenter ()
    {
      if (gcm_manager_class)
        get_env ().DeleteGlobalRef (gcm_manager_class);
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

        jmethodID register_method = find_static_method (&env, gcm_manager_class, "registerForGCMMessages", "(Landroid/content/Context;)V");

        is_registering_for_notifications    = true;
        register_for_notifications_callback = callback;

        env.CallStaticVoidMethod (gcm_manager_class, register_method, activity.get ());
      }
      catch (xtl::exception& e)
      {
        is_registering_for_notifications    = false;
        register_for_notifications_callback = PushNotificationsCenter::RegisterCallback ();

        e.touch ("push_notifications::GoogleCenter::RegisterForNotifications");
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

        jmethodID unregister_method = find_static_method (&env, gcm_manager_class, "unregisterForGCMMessages", "(Landroid/content/Context;)V");

        env.CallStaticVoidMethod (gcm_manager_class, unregister_method, activity.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("push_notifications::GoogleCenter::UnregisterForNotifications");
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
      static const char* METHOD_NAME = "push_notifications::google_push_notifications::GoogleCenter::InitJavaBindings";

      if (!env)
        throw xtl::make_null_argument_exception (METHOD_NAME, "env");

      jclass gcm_intent_service_class = env->FindClass ("com/untgames/funner/push_notifications/GCMIntentService");

      if (!gcm_intent_service_class)
      {
        if (env->ExceptionOccurred ())
          env->ExceptionClear ();

        log_error ("Google push notifications linked, but GCMIntentService class not found. Push notifications not supported.\n");
        return;
      }

      jclass gcm_manager_class_ref = env->FindClass ("com/untgames/funner/push_notifications/EngineGCMManager");

      if (!gcm_manager_class_ref)
      {
        if (env->ExceptionOccurred ())
          env->ExceptionClear ();

        log_error ("Google push notifications linked, but EngineGCMManager class not found. Push notifications not supported.\n");
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

        jint status = env->RegisterNatives (gcm_intent_service_class, methods, methods_count);

        if (status)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't register natives (status=%d)", status);

        static const JNINativeMethod manager_methods [] = {
          {"onRegisteredCallback", "(Ljava/lang/String;)V", (void*)&on_registered},
          {"onErrorCallback", "(Ljava/lang/String;)V", (void*)&on_error},
        };

        static const size_t manager_methods_count = sizeof (manager_methods) / sizeof (*manager_methods);

        status = env->RegisterNatives (gcm_manager_class_ref, manager_methods, manager_methods_count);

        if (status)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't register natives (status=%d)", status);

        gcm_manager_class = (jclass)env->NewGlobalRef (gcm_manager_class_ref);
      }
      catch (...)
      {
        env->DeleteLocalRef (gcm_intent_service_class);
        env->DeleteLocalRef (gcm_manager_class_ref);

        throw;
      }

      env->DeleteLocalRef (gcm_intent_service_class);
      env->DeleteLocalRef (gcm_manager_class_ref);
    }

    bool IsApiAvailable ()
    {
      return gcm_manager_class;
    }

  private:
    typedef xtl::signal<void (const char*)> NotificationsSignal;

  private:
    common::Log                               log;                                             //протокол
    NotificationsSignal                       notifications_signal;                            //сигнал оповещения о новых сообщениях
    bool                                      is_registering_for_notifications;                //активен ли в данный момент процесс регистрации на пуш-сообщения
    PushNotificationsCenter::RegisterCallback register_for_notifications_callback;             //колбек результата регистрации на пуш-сообщения
    jclass                                    gcm_manager_class;                               //EngineGCMManager class
};

typedef common::Singleton<GoogleCenter> GoogleCenterSingleton;

void on_registered (JNIEnv& env, jobject, jstring reg_id)
{
  common::ActionQueue::PushAction (xtl::bind (&GoogleCenter::OnRegisterForNotificationsSucceeded, &*GoogleCenterSingleton::Instance (), tostring (reg_id)), common::ActionThread_Main);
}

void on_error (JNIEnv& env, jobject, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&GoogleCenter::OnRegisterForNotificationsFailed, &*GoogleCenterSingleton::Instance (), tostring (error)), common::ActionThread_Main);
}

void on_message (JNIEnv& env, jobject, jstring message)
{
  common::ActionQueue::PushAction (xtl::bind (&GoogleCenter::OnNotificationReceived, &*GoogleCenterSingleton::Instance (), tostring (message)), common::ActionThread_Main);
}

}

/*
   Конструктор / деструктор
*/

PushNotificationsCenterImpl::PushNotificationsCenterImpl ()
{
  if (!GoogleCenterSingleton::Instance ()->IsApiAvailable ())
    throw xtl::format_operation_exception ("push_notifications::google_push_notifications::PushNotificationsCenterImpl::PushNotificationsCenterImpl", "GCM API not available");
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
  GoogleCenterSingleton::Instance ()->RegisterForNotifications (callback, properties);
}

void PushNotificationsCenterImpl::UnregisterForNotifications ()
{
  GoogleCenterSingleton::Instance ()->UnregisterForNotifications ();
}

/*
   Подписка на пуш-сообщения
*/

xtl::connection PushNotificationsCenterImpl::RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler& handler)
{
  return GoogleCenterSingleton::Instance ()->RegisterNotificationsHandler (handler);
}

/*
   Инициализация java-биндинга
*/

void PushNotificationsCenterImpl::InitJavaBindings (JNIEnv* env)
{
  GoogleCenterSingleton::Instance ()->InitJavaBindings (env);
}
