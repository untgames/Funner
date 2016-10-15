#include "shared.h"

using namespace analytics::flurry;
using namespace analytics::flurry::android;
using namespace syslib::android;

/*
   Android platform
*/

struct FlurryImpl::Impl
{
  bool                is_supported;                 //is anayltics supported in current build
  jclass              flurry_class;                 //Flurry class
  jclass              flurry_agent_class;           //FlurryAgent class
  jclass              constants_class;              //Constants class
  jclass              hash_map_class;               //HashMap class
  global_ref<jobject> flurry;                       //Flurry object
  jmethodID           flurry_init_method;           //Flurry constructor
  jmethodID           start_session_method;         //start session method
  jmethodID           end_session_method;           //end session method
  jmethodID           get_release_version_method;   //get flurry release version method
  jmethodID           set_user_id_method;           //set user id method
  jmethodID           set_age_method;               //set user age method
  jmethodID           set_gender_method;            //set user gender method
  jmethodID           log_event_method;             //log application event method
  jmethodID           end_timed_event_method;       //log application event end method
  jmethodID           log_page_view_method;         //log page view method
  jmethodID           set_log_level_method;         //set log level method
  jmethodID           hash_map_init_method;         //HashMap constructor
  jmethodID           hash_map_put_method;          //add object to HashMap method

  Impl ()
    : is_supported (false)
    , flurry_class (0)
    , flurry_agent_class (0)
    , constants_class (0)
    , hash_map_class (0)
    {}

  ~Impl ()
  {
    JNIEnv* env = &get_env ();

    if (flurry)
      env->CallVoidMethod (flurry.get (), end_session_method);

    if (flurry_class)
      env->DeleteGlobalRef (flurry_class);

    if (flurry_agent_class)
      env->DeleteGlobalRef (flurry_agent_class);

    if (constants_class)
      env->DeleteGlobalRef (constants_class);

    if (hash_map_class)
      env->DeleteGlobalRef (hash_map_class);
  }

  ///Initialize java-bindings
  void InitJavaBindings (JNIEnv* env)
  {
    static const char* METHOD_NAME = "analytics::flurry::android::FlurryImpl::InitJavaBindings";

    if (!env)
      throw xtl::make_null_argument_exception (METHOD_NAME, "env");

    jclass flurry_class_ref = env->FindClass ("com/untgames/funner/analytics/Flurry");

    if (!flurry_class_ref)
    {
      if (env->ExceptionOccurred ())
        env->ExceptionClear ();

      log_error ("Flurry analytics linked, but flurry class not found. Analytics not supported.\n");
      return;
    }

    jclass flurry_agent_class_ref = env->FindClass ("com/flurry/android/FlurryAgent");

    if (!flurry_agent_class_ref)
    {
      if (env->ExceptionOccurred ())
        env->ExceptionClear ();

      log_error ("Flurry analytics linked, but flurry agent class not found. Analytics not supported.\n");
      return;
    }

    jclass constants_class_ref = env->FindClass ("com/flurry/android/Constants");

    if (!constants_class_ref)
    {
      if (env->ExceptionOccurred ())
        env->ExceptionClear ();

      log_error ("Flurry analytics linked, but flurry constants class not found. Analytics not supported.\n");
      return;
    }

    jclass hash_map_class_ref = env->FindClass ("java/util/HashMap");

    if (!hash_map_class_ref)
    {
      if (env->ExceptionOccurred ())
        env->ExceptionClear ();

      log_error ("HashMap class not found. Analytics not supported.\n");
      return;
    }

    flurry_class       = (jclass)env->NewGlobalRef (flurry_class_ref);
    flurry_agent_class = (jclass)env->NewGlobalRef (flurry_agent_class_ref);
    constants_class    = (jclass)env->NewGlobalRef (constants_class_ref);
    hash_map_class     = (jclass)env->NewGlobalRef (hash_map_class_ref);

    env->DeleteLocalRef (flurry_class_ref);
    env->DeleteLocalRef (flurry_agent_class_ref);
    env->DeleteLocalRef (constants_class_ref);
    env->DeleteLocalRef (hash_map_class_ref);

    //flurry methods
    flurry_init_method   = find_method (env, flurry_class, "<init>", "(Lcom/untgames/funner/application/EngineActivity;Ljava/lang/String;)V");
    start_session_method = find_method (env, flurry_class, "startSession", "()V");
    end_session_method   = find_method (env, flurry_class, "endSession", "()V");
    log_event_method     = find_static_method (env, flurry_class, "logEvent", "(Ljava/lang/String;Ljava/util/Map;Z)V");

    //flurry agent methods
    get_release_version_method = find_static_method (env, flurry_agent_class, "getReleaseVersion", "()Ljava/lang/String;");
    set_user_id_method         = find_static_method (env, flurry_agent_class, "setUserId", "(Ljava/lang/String;)V");
    set_age_method             = find_static_method (env, flurry_agent_class, "setAge", "(I)V");
    set_gender_method          = find_static_method (env, flurry_agent_class, "setGender", "(B)V");
    end_timed_event_method     = find_static_method (env, flurry_agent_class, "endTimedEvent", "(Ljava/lang/String;Ljava/util/Map;)V");
    log_page_view_method       = find_static_method (env, flurry_agent_class, "onPageView", "()V");
    set_log_level_method       = find_static_method (env, flurry_agent_class, "setLogLevel", "(I)V");

    //hash map methods
    hash_map_init_method = find_method (env, hash_map_class, "<init>", "(I)V");
    hash_map_put_method  = find_method (env, hash_map_class, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    is_supported = true;
  }

  ///Get version name of underlying library
  void GetReleaseVersion (stl::string& version)
  {
    try
    {
      CheckIsSupported ();

      version = tostring ((jstring)get_env ().CallStaticObjectMethod (flurry_agent_class, get_release_version_method));
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::GetReleaseVersion");
      throw;
    }
  }

  ///Start analytics
  void StartSession (const char* api_key)
  {
    try
    {
      CheckIsSupported ();

      if (flurry)
        throw xtl::format_operation_exception ("", "Session already started");

      JNIEnv& env = get_env ();

      flurry = local_ref<jobject> (env.NewObject (flurry_class, flurry_init_method, get_activity (), tojstring (api_key).get ()), false);

      if (!flurry)
        throw xtl::format_operation_exception ("", "Can't create flurry object");

      env.CallVoidMethod (flurry.get (), start_session_method);
    }
    catch (xtl::exception& e)
    {
      flurry = 0;

      e.touch ("analytics::flurry::android::FlurryImpl::StartSession");
      throw;
    }
    catch (...)
    {
      flurry = 0;
      throw;
    }
  }

  ///Set analytics additional params
  void SetUserId (const char* user_id)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, set_user_id_method, tojstring (user_id).get ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetUserId");
      throw;
    }
  }

  void SetAge (size_t age)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, set_age_method, age);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetAge");
      throw;
    }
  }

  void SetGender (Gender gender)
  {
    try
    {
      CheckIsSupported ();

      JNIEnv& env = get_env ();

      jfieldID constant_field;

      switch (gender)
      {
        case Gender_Male:
          constant_field = env.GetStaticFieldID (constants_class, "MALE", "B");
          break;
        case Gender_Female:
          constant_field = env.GetStaticFieldID (constants_class, "FEMALE", "B");
          break;
        default:
          throw xtl::make_argument_exception ("", "gender", gender);
      }

      if (!constant_field)
        throw xtl::format_operation_exception ("", "Can't find gender field for gender %d", gender);

      jbyte gender_byte = env.GetStaticByteField (constants_class, constant_field);

      env.CallStaticVoidMethod (flurry_agent_class, set_gender_method, gender_byte);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetGender");
      throw;
    }
  }

  ///Build Java HashMap
  local_ref<jobject> ToHashMap (const common::PropertyMap& properties)
  {
    static const char* METHOD_NAME = "analytics::flurry::android::FlurryImpl::ToHashMap";

    JNIEnv& env = get_env ();

    size_t properties_count = properties.Size ();

    local_ref<jobject> return_value (env.NewObject (hash_map_class, hash_map_init_method, properties_count), false);

    if (!return_value)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't create hash map object");

    for (size_t i = 0; i < properties_count; i++)
      env.CallObjectMethod (return_value.get (), hash_map_put_method, tojstring (properties.PropertyName (i)).get (), tojstring (properties.GetString (i)).get ());

    return return_value;
  }

  ///Track events
  void LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, log_event_method, tojstring (event).get (), ToHashMap (parameters).get (), timed);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::LogEvent");
      throw;
    }
  }

  void EndTimedEvent (const char* event, const common::PropertyMap& parameters)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, end_timed_event_method, tojstring (event).get (), ToHashMap (parameters).get ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::EndTimedEvent");
      throw;
    }
  }

  void LogPageView ()
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, log_page_view_method);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::LogPageView");
      throw;
    }
  }

  ///Logging management
  void SetLogLevel (LogLevel level)
  {
    try
    {
      CheckIsSupported ();

      JNIEnv& env = get_env ();

      local_ref<jclass> log_class (env.FindClass ("android/util/Log"), false);

      if (!log_class)
        throw xtl::format_operation_exception ("", "Can't find android log class");

      jfieldID constant_field;

      switch (level)
      {
        case LogLevel_None:
          constant_field = env.GetStaticFieldID (log_class.get (), "ASSERT", "I");
          break;
        case LogLevel_CriticalOnly:
          constant_field = env.GetStaticFieldID (log_class.get (), "ERROR", "I");
          break;
        case LogLevel_Debug:
          constant_field = env.GetStaticFieldID (log_class.get (), "DEBUG", "I");
          break;
        case LogLevel_All:
          constant_field = env.GetStaticFieldID (log_class.get (), "VERBOSE", "I");
          break;
        default:
          throw xtl::make_argument_exception ("", "level", level);
      }

      if (!constant_field)
        throw xtl::format_operation_exception ("", "Can't find log level field for level %d", level);

      int android_level = env.GetStaticIntField (log_class.get (), constant_field);

      env.CallStaticVoidMethod (flurry_agent_class, set_log_level_method, android_level);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetLogLevel");
      throw;
    }
  }

  //Check if flurry supported
  void CheckIsSupported ()
  {
    if (!is_supported)
      throw xtl::format_operation_exception ("analytics::flurry::android::FlurryImpl::CheckIsSupported", "Analytics not supported in this build.");
  }
};

/*
   Constructor / destructor
*/

FlurryImpl::FlurryImpl ()
  : impl (new Impl)
  {}

FlurryImpl::~FlurryImpl ()
{
  delete impl;
}

/*
   Is analytics supported on current platform
*/

bool FlurryImpl::IsSupported ()
{
  return impl->is_supported;
}

/*
   Get version name of underlying library
*/

void FlurryImpl::GetReleaseVersion (stl::string& version)
{
  impl->GetReleaseVersion (version);
}

/*
   Start analytics
*/

void FlurryImpl::StartSession (const char* api_key)
{
  impl->StartSession (api_key);
}

/*
   Set analytics additional params
*/

void FlurryImpl::SetUserId (const char* user_id)
{
  impl->SetUserId (user_id);
}

void FlurryImpl::SetAge (size_t age)
{
  impl->SetAge (age);
}

void FlurryImpl::SetGender (Gender gender)
{
  impl->SetGender (gender);
}

/*
   Events tracking
*/

void FlurryImpl::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  impl->LogEvent (event, parameters, timed);
}

void FlurryImpl::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  impl->EndTimedEvent (event, parameters);
}

void FlurryImpl::LogPageView ()
{
  impl->LogPageView ();
}

/*
   Logging management
*/

void FlurryImpl::SetLogLevel (LogLevel level)
{
  impl->SetLogLevel (level);
}

/*
   Initialization of java-bindings
*/

void FlurryImpl::InitJavaBindings (JNIEnv* env)
{
  impl->InitJavaBindings (env);
}
