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

void on_login_succeeded (JNIEnv& env, jobject, jstring token);
void on_login_failed (JNIEnv& env, jobject, jstring error);
void on_login_canceled (JNIEnv& env, jobject);

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
      CancelLogin ();

      JNIEnv* env = &get_env ();

      if (session_class)
        env->DeleteGlobalRef (session_class);

      if (session)
        env->DeleteGlobalRef (session);
    }

    //Login/cancel login
    void Login (const char* app_id, const social::facebook::DefaultPlatform::PlatformLoginCallback& callback, const common::PropertyMap& properties)
    {
      try
      {
        InitSession (app_id);

        JNIEnv* env = &get_env ();

        if (!env->CallBooleanMethod (session, can_login_method))
        {
          callback (false, OperationStatus_Failure, "", "");
          return;
        }

        stl::string permissions;

        if (properties.IsPresent ("Permissions"))
          permissions = properties.GetString ("Permissions");

        env->CallVoidMethod (session, login_method, tojstring (permissions.c_str ()).get ());

        check_errors ();

        login_handler = callback;
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::AndroidPlatformImpl::Login");
        throw;
      }
    }

    void CancelLogin ()
    {
      login_handler = social::facebook::DefaultPlatform::PlatformLoginCallback ();
    }

    void LogOut ()
    {
      try
      {
        JNIEnv* env = &get_env ();

        env->CallStaticVoidMethod (session_class, logout_method);
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::AndroidPlatformImpl::LogOut");
        throw;
      }
    }

    //Track app install
    void PublishInstall (const char* app_id)
    {
      try
      {
        log.Printf ("Publishing install app event");

        InitSession (app_id);

        JNIEnv* env = &get_env ();

        env->CallVoidMethod (session, publish_app_install_method);
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::AndroidPlatformImpl::PublishInstall");
        throw;
      }
    }

    ///Initialize java bindings
    void InitJavaBindings (JNIEnv* env)
    {
      static const char* METHOD_NAME = "social::facebook::AndroidPlatformImpl::InitJavaBindings";

      if (!env)
        throw xtl::make_null_argument_exception (METHOD_NAME, "env");

      jclass session_class_ref = env->FindClass ("com/untgames/funner/facebook_session/SessionImpl");

     if (!session_class_ref)
       throw xtl::format_operation_exception (METHOD_NAME, "Can't find SessionImpl class\n");

     if (session_class)
       env->DeleteGlobalRef (session_class);

     session_class = (jclass)env->NewGlobalRef (session_class_ref);

     session_init_method        = find_method (env, session_class, "<init>", "(Lcom/untgames/funner/application/EngineActivity;Ljava/lang/String;)V");
     can_login_method           = find_method (env, session_class, "canLogin", "()Z");
     login_method               = find_method (env, session_class, "login", "(Ljava/lang/String;)V");
     logout_method              = find_static_method (env, session_class, "logout", "()V");
     publish_app_install_method = find_method (env, session_class, "publishInstall", "()V");

     try
     {
       static const JNINativeMethod methods [] = {
         {"onLoginSucceeded", "(Ljava/lang/String;)V", (void*)&on_login_succeeded},
         {"onLoginFailed", "(Ljava/lang/String;)V", (void*)&on_login_failed},
         {"onLoginCanceled", "()V", (void*)&on_login_canceled},
       };

       static const size_t methods_count = sizeof (methods) / sizeof (*methods);

       jint status = env->RegisterNatives (session_class, methods, methods_count);

       if (status)
         throw xtl::format_operation_exception (METHOD_NAME, "Can't register natives (status=%d)", status);
     }
     catch (...)
     {
       if (session_class)
         env->DeleteGlobalRef (session_class);

       session_class = 0;

       throw;
     }
   }

    void OnLoginSucceeded (const stl::string& token)
    {
      login_handler (true, OperationStatus_Success, "", token.c_str ());
    }

    void OnLoginFailed (const stl::string& error)
    {
      login_handler (true, OperationStatus_Failure, error.c_str (), "");
    }

    void OnLoginCanceled ()
    {
      login_handler (true, OperationStatus_Canceled, "", "");
    }

  private:
    void InitSession (const char* app_id)
    {
      if (session) //already initiated
        return;

      JNIEnv* env = &get_env ();

      session = env->NewGlobalRef (env->NewObject (session_class, session_init_method, get_activity (), tojstring (app_id).get ()));

      if (!session)
        throw xtl::format_operation_exception ("social::facebook::AndroidPlatformImpl::InitSession", "Can't create session object");
    }

  private:
    common::Log log;                        //logger
    jclass      session_class;              //session class
    jobject     session;                    //session isntance
    jmethodID   session_init_method;        //session constructor
    jmethodID   can_login_method;           //check if login available method
    jmethodID   login_method;               //login method
    jmethodID   logout_method;              //logout method
    jmethodID   publish_app_install_method; //session publish install method
    social::facebook::DefaultPlatform::PlatformLoginCallback login_handler; //login result handler
};

typedef common::Singleton<AndroidPlatformImpl> AndroidPlatformSingleton;

void on_login_failed (JNIEnv& env, jobject, jstring error)
{
  common::ActionQueue::PushAction (xtl::bind (&AndroidPlatformImpl::OnLoginFailed, &*AndroidPlatformSingleton::Instance (), tostring (error)), common::ActionThread_Main);
}

void on_login_succeeded (JNIEnv& env, jobject, jstring token)
{
  common::ActionQueue::PushAction (xtl::bind (&AndroidPlatformImpl::OnLoginSucceeded, &*AndroidPlatformSingleton::Instance (), tostring (token)), common::ActionThread_Main);
}

void on_login_canceled (JNIEnv& env, jobject)
{
  common::ActionQueue::PushAction (xtl::bind (&AndroidPlatformImpl::OnLoginCanceled, &*AndroidPlatformSingleton::Instance ()), common::ActionThread_Main);
}

}

/*
   Android platform
*/

/*
   Login (returns true if platform-specific login initiated)
*/

void AndroidPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  AndroidPlatformSingleton::Instance ()->Login (app_id, callback, properties);
}

void AndroidPlatform::CancelLogin ()
{
  AndroidPlatformSingleton::Instance ()->CancelLogin ();
}

void AndroidPlatform::LogOut ()
{
  AndroidPlatformSingleton::Instance ()->LogOut ();
}

/*
   Track app installed event
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
