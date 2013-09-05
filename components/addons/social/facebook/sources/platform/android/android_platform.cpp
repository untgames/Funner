#include "shared.h"

using namespace social;
using namespace social::facebook;
using namespace syslib::android;

namespace
{

//Constants
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.android_facebook_platform"; //application on load handler component name
const char* LOAD_HANDLER_ID             = "AndroidFacebookPlatform";                                       //application on load handler identifier
const char* LOG_NAME                    = "social.facebook.FacebookSession.AndroidPlatform";

//Facebook platform singleton
class AndroidPlatformImpl
{
  public:
    //Constructor/destructor
    AndroidPlatformImpl ()
      : log (LOG_NAME)
      , session_class (0)
      , session (0)
      { }

    ~AndroidPlatformImpl ()
    {
      JNIEnv* env = &get_env ();

      if (session_class)
        env->DeleteGlobalRef (session_class);

      if (session)
        env->DeleteGlobalRef (session);
    }

    //Отправка события установки приложения
    void PublishInstall (const char* app_id)
    {
      try
      {
        if (!app_id)
          throw xtl::make_null_argument_exception ("", "app_id");

        log.Printf ("Publishing install app event");

        JNIEnv* env = &get_env ();

        if (!session)
        {
          session = env->NewGlobalRef (env->NewObject (session_class, session_init_method, get_activity (), tojstring (app_id).get ()));

          if (!session)
            throw xtl::format_operation_exception ("", "Can't create session object");
        }

        env->CallVoidMethod (session, publish_app_install_method);
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::AndroidPlatformImpl::PublishInstall");
        throw;
      }
    }

    ///Инициализация java-биндинга
    void InitJavaBindings (JNIEnv* env)
    {
      static const char* METHOD_NAME = "social::facebook::AndroidPlatformImpl::InitJavaBindings";

      if (!env)
        throw xtl::make_null_argument_exception (METHOD_NAME, "env");

      jclass session_class_ref = env->FindClass ("com/untgames/funner/facebook_session/SessionImpl");

     if (!session_class_ref)
       throw xtl::format_operation_exception ("", "Can't find SessionImpl class\n");

     if (session_class)
       env->DeleteGlobalRef (session_class);

     session_class = (jclass)env->NewGlobalRef (session_class_ref);

     session_init_method        = find_method (env, session_class, "<init>", "(Lcom/untgames/funner/application/EngineActivity;Ljava/lang/String;)V");
     publish_app_install_method = find_method (env, session_class, "publishInstall", "()V");
   }

  private:
    common::Log log;                        //протокол
    jclass      session_class;              //класс Store
    jobject     session;                    //объект Store
    jmethodID   session_init_method;        //конструктор Store
    jmethodID   publish_app_install_method; //метод покупки
};

typedef common::Singleton<AndroidPlatformImpl> AndroidPlatformSingleton;

}

/*
   Android платформа
*/

/*
   Публикация события установки приложения
*/

void AndroidPlatform::PublishAppInstallEvent (const char* app_id)
{
  AndroidPlatformSingleton::Instance ()->PublishInstall (app_id);
}

/*
   Load handler component
*/

namespace components
{

namespace android_facebook
{

class AndroidFacebookLoadHandlerComponent
{
  public:
    AndroidFacebookLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&AndroidFacebookLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      AndroidPlatformSingleton::Instance ()->InitJavaBindings (env);
    }
};

extern "C"
{

common::ComponentRegistrator<AndroidFacebookLoadHandlerComponent> AndroidFacebookLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
