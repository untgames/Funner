#include "shared.h"

using namespace syslib::android;

namespace
{

/// Константы
const char* COMPONENTS_MASK                               = "common.syslib.android.load_handlers.*";                  //маска имён компонентов обработчиков загрузки
const char* ENGINE_ACTIVITY_CLASS_NAME                    = "com/untgames/funner/application/EngineActivity";
const char* ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME = "startApplication";

/// Виртуальная машина
JavaVM* java_vm = 0;

jint JNICALL startApplication (JNIEnv* env, jobject thiz, jstring jprogram_name, jstring jwork_dir, jstring jprogram_args, jstring jenv_vars)
{
  if (!env)
  {  
    log_error ("Bad JNIEnv passed to EngineActivity::startApplication\n");
    return 0;
  }

  if (!jprogram_name || !jprogram_args || !jwork_dir)
  {
    log_error ("Bad arguments passed to EngineActivity::startApplication\n");
    return 0; 
  }

  jni_string program_name (env, jprogram_name), program_args (env, jprogram_args), work_dir (env, jwork_dir), env_vars (env, jenv_vars);

  log_info ("Starting program '%s' (args='%s', workdir='%s', env='%s')\n", program_name.get (), program_args.get (), work_dir.get (), env_vars.get ());

  try
  {   
    if (chdir (work_dir.get ()))
    {
      log_error ("Can't set current working dir to '%s'. %s\n", work_dir.get (), strerror (errno));

      return 0;
    }

    startStdioRedirection (java_vm);

    start_application (java_vm, thiz, program_name.get (), program_args.get (), env_vars.get ());

    return 1;
  }
  catch (std::exception& e)
  {
    log_error ("%s\n  at syslib::android::startApplication\n", e.what ());
    
    return 0;
  }
  catch (...)
  {
    log_error ("unknown exception appears\n  at syslib::android::startApplication\n");
    
    return 0;
  }
}

///Менеджер приложения
class ApplicationManagerImpl
{
  public:
    ///Регистрация обработчиков загрузки приложения
    void RegisterLoadHandler (const char* id, const ApplicationManager::LoadHandler& handler)
    {
      static const char* METHOD_NAME = "syslib::android::ApplicationManager::RegisterLoadHandler";

      if (!id)
        throw xtl::make_null_argument_exception (METHOD_NAME, "id");

      LoadHandlersMap::iterator iter = load_handlers.find (id);

      if (iter != load_handlers.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Load handler with id '%s' already registered", id);

      load_handlers.insert_pair (id, handler);
    }

    void UnregisterLoadHandler (const char* id)
    {
      if (!id)
        throw xtl::make_null_argument_exception ("syslib::android::ApplicationManager::UnregisterLoadHandler", "id");

      load_handlers.erase (id);
    }

    void UnregisterAllLoadHandlers ()
    {
      load_handlers.clear ();
    }

    ///Проверка зарегистрирован ли обработчки загрузки
    bool IsLoadHandlerRegistered (const char* id)
    {
      if (!id)
        throw xtl::make_null_argument_exception ("syslib::android::ApplicationManager::IsLoadHandlerRegistered", "id");

      return load_handlers.find (id) != load_handlers.end ();
    }

    ///Оповещение о загрузке приложения
    void OnLoad (JNIEnv* env)
    {
      for (LoadHandlersMap::iterator iter = load_handlers.begin (), end = load_handlers.end (); iter != end; ++iter)
        iter->second (env);
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ApplicationManager::LoadHandler> LoadHandlersMap;

  private:
    LoadHandlersMap load_handlers;
};

typedef common::Singleton<ApplicationManagerImpl> ApplicationManagerSingleton;

}

/*
   Менеджер приложения
*/

/*
   Регистрация обработчиков загрузки приложения
*/

void ApplicationManager::RegisterLoadHandler (const char* id, const LoadHandler& handler)
{
  ApplicationManagerSingleton::Instance ()->RegisterLoadHandler (id, handler);
}

void ApplicationManager::UnregisterLoadHandler (const char* id)
{
  ApplicationManagerSingleton::Instance ()->UnregisterLoadHandler (id);
}

void ApplicationManager::UnregisterAllLoadHandlers ()
{
  ApplicationManagerSingleton::Instance ()->UnregisterAllLoadHandlers ();
}

/*
   Проверка зарегистрирован ли обработчки загрузки
*/

bool ApplicationManager::IsLoadHandlerRegistered (const char* id)
{
  return ApplicationManagerSingleton::Instance ()->IsLoadHandlerRegistered (id);
}

extern "C"
{

__attribute__ ((visibility("default"))) extern JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void* reserved)
{
  register_crash_handlers ();

  if (!vm)
  {
    log_error ("JavaVM is null\n");

    return -1; 
  }

  java_vm = vm;

  JNIEnv* env;

  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    log_error ("Can't get Java environment\n");
    
    return -1;
  }

  jclass skeletonActivityClass = env->FindClass (ENGINE_ACTIVITY_CLASS_NAME); 
  
  if (!skeletonActivityClass)
  {
    log_error ("Can't find EngineActivity class\n");
    
    return -1;
  }
  
  JNINativeMethod method;
  
  memset (&method, 0, sizeof (JNINativeMethod));
  
  method.name      = ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME;
  method.signature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I";
  method.fnPtr     = (void*)&startApplication;

  jint status = env->RegisterNatives (skeletonActivityClass, &method, 1);

  if (status)
  {
    log_error ("Can't register natives (status=%d)\n", status);
    
    return -1;
  }

  try
  {
    register_activity_callbacks (env, skeletonActivityClass);
    register_window_callbacks (env);
    register_web_view_callbacks (env);
    register_sensor_manager_callbacks (env);
    register_screen_callbacks (env, skeletonActivityClass);

    env->DeleteLocalRef (skeletonActivityClass);

    static common::ComponentLoader loader (COMPONENTS_MASK);

    ApplicationManagerSingleton::Instance ()->OnLoad (env);
  }
  catch (std::exception& e)
  {
    log_error ("Can't register native callbacks: %s\n", e.what ());

    return -1;    
  }
  catch (...)
  {
    log_error ("Can't register native callbacks: unknown exception\n");

    return -1;    
  }

  return JNI_VERSION_1_4;
}

}
