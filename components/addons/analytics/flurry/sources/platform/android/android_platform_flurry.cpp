#include "shared.h"

using namespace analytics::flurry;
using namespace analytics::flurry::android;
using namespace syslib::android;

/*
   ѕлатформа Android
*/

struct FlurryImpl::Impl
{
  bool                is_supported;                 //поддерживаетс€ ли аналитика в данной сборке приложени€
  jclass              flurry_class;                 //класс Flurry
  jclass              flurry_agent_class;           //класс FlurryAgent
  jclass              constants_class;              //класс Constants
  jclass              hash_map_class;               //класс HashMap
  global_ref<jobject> flurry;                       //объект Flurry
  jmethodID           flurry_init_method;           //конструктор Flurry
  jmethodID           start_session_method;         //метод начала сессии
  jmethodID           end_session_method;           //метод окончани€ сессии
  jmethodID           get_release_version_method;   //метод получени€ имени релиза flurry
  jmethodID           set_user_id_method;           //метод установки идентификатора пользовател€
  jmethodID           set_age_method;               //метод установки возраста пользовател€
  jmethodID           set_gender_method;            //метод установки пола пользовател€
  jmethodID           set_use_https_method;         //метод установки необходимости использовать https дл€ отправки аналитики
  jmethodID           log_event_method;             //метод протоколировани€ событи€ приложени€
  jmethodID           end_timed_event_method;       //метод протоколировани€ окончани€ событи€ приложени€
  jmethodID           log_page_view_method;         //метод протоколировани€ просмотра страницы
  jmethodID           set_debug_log_enabled_method; //метод установки необходимости отладочного протоколировани€
  jmethodID           set_log_level_method;         //метод установки уровн€ протоколировани€
  jmethodID           hash_map_init_method;         //конструктор HashMap
  jmethodID           hash_map_put_method;          //метод добавление объекта в карту

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

  ///»нициализаци€ java-биндинга
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

    //flurry methods
    flurry_init_method   = find_method (env, flurry_class, "<init>", "(Lcom/untgames/funner/application/EngineActivity;Ljava/lang/String;)V");
    start_session_method = find_method (env, flurry_class, "startSession", "()V");
    end_session_method   = find_method (env, flurry_class, "endSession", "()V");

    //flurry agent methods
    get_release_version_method   = find_static_method (env, flurry_agent_class, "getReleaseVersion", "()Ljava/lang/String;");
    set_user_id_method           = find_static_method (env, flurry_agent_class, "setUserId", "(Ljava/lang/String;)V");
    set_age_method               = find_static_method (env, flurry_agent_class, "setAge", "(I)V");
    set_gender_method            = find_static_method (env, flurry_agent_class, "setGender", "(B)V");
    set_use_https_method         = find_static_method (env, flurry_agent_class, "setUseHttps", "(Z)V");
    log_event_method             = find_static_method (env, flurry_agent_class, "logEvent", "(Ljava/lang/String;Ljava/util/Map;Z)V");
    end_timed_event_method       = find_static_method (env, flurry_agent_class, "endTimedEvent", "(Ljava/lang/String;Ljava/util/Map;)V");
    log_page_view_method         = find_static_method (env, flurry_agent_class, "onPageView", "()V");
    set_debug_log_enabled_method = find_static_method (env, flurry_agent_class, "setLogEnabled", "(Z)V");
    set_log_level_method         = find_static_method (env, flurry_agent_class, "setLogLevel", "(I)V");

    //hash map methods
    hash_map_init_method = find_method (env, hash_map_class, "<init>", "(I)V");
    hash_map_put_method  = find_method (env, hash_map_class, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    is_supported = true;
  }

  ///ѕолучение имени версии используемой библиотеки
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

  ///«апуск аналитики
  void StartSession (const char* api_key)
  {
    try
    {
      CheckIsSupported ();

      if (flurry)
        throw xtl::format_operation_exception ("", "Session already started");

      JNIEnv& env = get_env ();

      flurry = env.NewObject (flurry_class, flurry_init_method, get_activity (), tojstring (api_key).get ());

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

  ///”становка дополнительных параметров аналитики
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

  void SetUseHttps (bool use_https)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, set_use_https_method, use_https);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetUseHttps");
      throw;
    }
  }

  ///‘ормирование Java HashMap
  local_ref<jobject> ToHashMap (const common::PropertyMap& properties)
  {
    static const char* METHOD_NAME = "analytics::flurry::android::FlurryImpl::ToHashMap";

    JNIEnv& env = get_env ();

    size_t properties_count = properties.Size ();

    local_ref<jobject> return_value = env.NewObject (hash_map_class, hash_map_init_method, properties_count);

    if (!return_value)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't create hash map object");

    for (size_t i = 0; i < properties_count; i++)
      env.CallObjectMethod (return_value.get (), hash_map_put_method, tojstring (properties.PropertyName (i)).get (), tojstring (properties.GetString (i)).get ());

    return return_value;
  }

  ///“рекинг событий
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

  ///”правление протоколированием
  void SetDebugLogEnabled (bool state)
  {
    try
    {
      CheckIsSupported ();

      get_env ().CallStaticVoidMethod (flurry_agent_class, set_debug_log_enabled_method, state);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetDebugLogEnabled");
      throw;
    }
  }

  void SetLogLevel (LogLevel level)
  {
    try
    {
      CheckIsSupported ();

      JNIEnv& env = get_env ();

      jclass log_class = env.FindClass ("android/util/Log");

      if (!log_class)
        throw xtl::format_operation_exception ("", "Can't find android log class");

      jfieldID constant_field;

      switch (level)
      {
        case LogLevel_None:
          constant_field = env.GetStaticFieldID (log_class, "ASSERT", "I");
          break;
        case LogLevel_CriticalOnly:
          constant_field = env.GetStaticFieldID (log_class, "ERROR", "I");
          break;
        case LogLevel_Debug:
          constant_field = env.GetStaticFieldID (log_class, "DEBUG", "I");
          break;
        case LogLevel_All:
          constant_field = env.GetStaticFieldID (log_class, "VERBOSE", "I");
          break;
        default:
          throw xtl::make_argument_exception ("", "level", level);
      }

      if (!constant_field)
        throw xtl::format_operation_exception ("", "Can't find log level field for level %d", level);

      int android_level = env.GetStaticIntField (log_class, constant_field);

      env.CallStaticVoidMethod (flurry_agent_class, set_log_level_method, android_level);
    }
    catch (xtl::exception& e)
    {
      e.touch ("analytics::flurry::android::FlurryImpl::SetLogLevel");
      throw;
    }
  }

  //ѕроверка поддерживаетс€ ли flurry
  void CheckIsSupported ()
  {
    if (!is_supported)
      throw xtl::format_operation_exception ("analytics::flurry::android::FlurryImpl::CheckIsSupported", "Analytics not supported in this build.");
  }
};

/*
    онструктор / деструктор
*/

FlurryImpl::FlurryImpl ()
  : impl (new Impl)
  {}

FlurryImpl::~FlurryImpl ()
{
  delete impl;
}

/*
   ѕоддерживаетс€ ли аналитика на данной платформе
*/

bool FlurryImpl::IsSupported ()
{
  return impl->is_supported;
}

/*
   ѕолучение имени версии используемой библиотеки
*/

void FlurryImpl::GetReleaseVersion (stl::string& version)
{
  impl->GetReleaseVersion (version);
}

/*
   «апуск аналитики
*/

void FlurryImpl::StartSession (const char* api_key)
{
  impl->StartSession (api_key);
}

/*
   ”становка дополнительных параметров аналитики
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

void FlurryImpl::SetUseHttps (bool use_https)
{
  impl->SetUseHttps (use_https);
}

/*
   “рекинг событий
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
   ”правление протоколированием
*/

void FlurryImpl::SetDebugLogEnabled (bool state)
{
  impl->SetDebugLogEnabled (state);
}

void FlurryImpl::SetLogLevel (LogLevel level)
{
  impl->SetLogLevel (level);
}

/*
   »нициализаци€ java-биндинга
*/

void FlurryImpl::InitJavaBindings (JNIEnv* env)
{
  impl->InitJavaBindings (env);
}
