#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

const char* REGISTER_FOR_PUSH_NOTIFICATIONS_PREFIX = "RegisterForPushNotification Register"; //Префикс сообщения инициализации подписки на пуш-сообщения

//Менеджер google cloud messaging
class GcmManagerImpl : public MessageQueue::Handler, xtl::noncopyable
{
  public:
    //Конструктор/деструктор
    GcmManagerImpl ()
      : gcm_manager_class (0)
    {
      MessageQueueSingleton::Instance ()->RegisterHandler (*this);
    }

    ~GcmManagerImpl ()
    {
      MessageQueueSingleton::Instance ()->UnregisterHandler (*this);

      if (gcm_manager_class)
        get_env ().DeleteGlobalRef (gcm_manager_class);
    }

    //Подписка на события регистрации на push-сообщения
    void RegisterForNotifications (JNIEnv* env, jclass in_gcm_manager_class)
    {
      if (!in_gcm_manager_class)
        throw xtl::make_null_argument_exception ("syslib::GcmManager::RegisterForNotifications", "gcm_manager_class");

      if (gcm_manager_class)
        env->DeleteGlobalRef (gcm_manager_class);

      gcm_manager_class = (jclass)env->NewGlobalRef (in_gcm_manager_class);

      register_for_push_notifications_connection = Application::RegisterNotificationHandler (common::format ("%s*", REGISTER_FOR_PUSH_NOTIFICATIONS_PREFIX).c_str (),
                                                                                             xtl::bind (&GcmManagerImpl::RegisterForPushNotifications, this, _1));
    }

    //Обработка события успешной регистрации
    void OnRegistered (const stl::string& registration_id)
    {
      stl::string notification = common::format ("RegisterForPushNotification Succeeded %s", registration_id.c_str ());

      syslib::Application::PostNotification (notification.c_str ());
    }

    //Обработка события ошибки регистрации
    void OnError (const stl::string& error)
    {
      stl::string notification = common::format ("RegisterForPushNotification Failed %s", error.c_str ());

      syslib::Application::PostNotification (notification.c_str ());
    }

    //Обработка события получения сообщения
    void OnMessage (const stl::string& message)
    {
      stl::string notification = common::format ("PushNotificationReceived %s", message.c_str ());

      syslib::Application::PostNotification (notification.c_str ());
    }

  private:
    ///Регистрация обработки push-сообщений
    void RegisterForPushNotifications (const char* notification)
    {
      try
      {
        JNIEnv& env = get_env ();

        local_ref<jobject> activity = get_activity ();

        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");

        jmethodID register_method = find_static_method (&env, gcm_manager_class, "registerForGCMMessages", "(Landroid/content/Context;)V");

        env.CallStaticVoidMethod (gcm_manager_class, register_method, activity.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::GcmManager::RegisterForPushNotifications");
        throw;
      }
    }

  private:
    jclass               gcm_manager_class;                          //EngineGCMManager class
    xtl::auto_connection register_for_push_notifications_connection; //соединение события инициации подписки на push-сообщения
};

typedef common::Singleton <GcmManagerImpl> GcmManagerSingleton;

template <class Fn> class GcmMessage: public MessageQueue::Message
{
  public:
    GcmMessage (const Fn& in_fn)
      : fn (in_fn) {}

    void Dispatch ()
    {
      fn ();
    }

  private:
    Fn fn;
};

template <class Fn> void push_message (const Fn& fn)
{
  try
  {
    MessageQueueSingleton::Instance ()->PushMessage (*GcmManagerSingleton::Instance (), MessageQueue::MessagePtr (new GcmMessage<Fn> (fn), false));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void on_registered (JNIEnv& env, jobject, jstring reg_id)
{
  push_message (xtl::bind (&GcmManagerImpl::OnRegistered, &*GcmManagerSingleton::Instance (), tostring (reg_id)));
}

void on_message (JNIEnv& env, jobject, jstring message)
{
  push_message (xtl::bind (&GcmManagerImpl::OnMessage, &*GcmManagerSingleton::Instance (), tostring (message)));
}

void on_error (JNIEnv& env, jobject, jstring error)
{
  push_message (xtl::bind (&GcmManagerImpl::OnError, &*GcmManagerSingleton::Instance (), tostring (error)));
}

}

namespace syslib
{

namespace android
{

/// регистрация методов обратного вызова google cloud messaging
void register_gcm_callbacks (JNIEnv* env)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    jclass gcm_intent_service_class = env->FindClass ("com/untgames/funner/application/GCMIntentService");

    if (!gcm_intent_service_class)
      throw xtl::format_operation_exception ("", "Can't find GCMIntentService class\n");

    static const JNINativeMethod methods [] = {
      {"onRegisteredCallback", "(Ljava/lang/String;)V", (void*)&on_registered},
      {"onMessageCallback", "(Ljava/lang/String;)V", (void*)&on_message},
      {"onErrorCallback", "(Ljava/lang/String;)V", (void*)&on_error},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (gcm_intent_service_class, methods, methods_count);

    env->DeleteLocalRef (gcm_intent_service_class);

    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);

    jclass gcm_manager_class = env->FindClass ("com/untgames/funner/application/EngineGCMManager");

    if (!gcm_manager_class)
      throw xtl::format_operation_exception ("", "Can't find EngineGCMManager class\n");

    static const JNINativeMethod manager_methods [] = {
      {"onRegisteredCallback", "(Ljava/lang/String;)V", (void*)&on_registered},
      {"onErrorCallback", "(Ljava/lang/String;)V", (void*)&on_error},
    };

    static const size_t manager_methods_count = sizeof (manager_methods) / sizeof (*manager_methods);

    status = env->RegisterNatives (gcm_manager_class, manager_methods, manager_methods_count);

    if (status)
    {
      env->DeleteLocalRef (gcm_manager_class);
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
    }

    //Инициализация синглтона
    GcmManagerSingleton::Instance ()->RegisterForNotifications (env, gcm_manager_class);

    env->DeleteLocalRef (gcm_manager_class);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_gcm_callbacks");
    throw;
  }
}

}

}
