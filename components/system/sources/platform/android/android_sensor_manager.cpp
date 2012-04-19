#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace syslib
{

/// Дескриптор сенсора
struct sensor_handle
{
  global_ref<jobject>   sensor;
  global_ref<jobject>   event_listener;
  ISensorEventListener* app_listener;
  
  sensor_handle (const global_ref<jobject>& in_sensor) : sensor (in_sensor), app_listener ()
  {
    try
    {
        //получение метода создания слушателя
          
      jmethodID event_listener_class_constructor = find_method (&get_env (), get_context ().sensor_event_listener_class.get (), "<init>", "(J)V");
      
        //создание слушателя
      
      event_listener = get_env ().NewObject (get_context ().sensor_event_listener_class.get (), event_listener_class_constructor, (jlong)this);

      if (!event_listener)
        throw xtl::format_operation_exception ("", "EngineSensorEventListener constructor failed");      
    }    
    catch (xtl::exception& e)
    {
      e.touch ("syslib::android::sensor_handle::sensor_handle");
      throw;
    }
  }
    
  ~sensor_handle ()
  {
    try
    {
        //получение метода сброса ссылки

      jmethodID event_listener_reset_reference = find_method (&get_env (), get_context ().sensor_event_listener_class.get (), "resetSensorRef", "()V");

        //сброс ссылки

      get_env ().CallVoidMethod (event_listener.get (), event_listener_reset_reference);
    }
    catch (...)
    {
    }
  }  
};

}

namespace
{

/*
    Константы
*/

const char* ANDROID_SENSOR_MANAGER_CLASS_NAME    = "android/hardware/SensorManager";
const char* ANDROID_SENSOR_CLASS_NAME            = "android/hardware/Sensor";
const char* ANDROID_LIST_CLASS_NAME              = "java/util/List";
const char* LOG_NAME                             = "system.android.sensors";

/// Типы сенсоров
enum SensorType
{
  SENSOR_TYPE_ACCELEROMETER       = 1,
  SENSOR_TYPE_MAGNETIC_FIELD      = 2,
  SENSOR_TYPE_ORIENTATION         = 3,
  SENSOR_TYPE_GYROSCOPE           = 4,
  SENSOR_TYPE_LIGHT               = 5,
  SENSOR_TYPE_PRESSURE            = 6,
  SENSOR_TYPE_TEMPERATURE         = 7,
  SENSOR_TYPE_PROXIMITY           = 8,
  SENSOR_TYPE_GRAVITY             = 9,
  SENSOR_TYPE_LINEAR_ACCELERATION = 10,
  SENSOR_TYPE_ROTATION_VECTOR     = 11,    
  SENSOR_TYPE_RELATIVE_HUMIDITY   = 12,    
  
  SENSOR_TYPE_ALL                 = -1,    
};

/// Типы частоты обновления сенсоров
enum SensorRateType
{
  SENSOR_DELAY_FASTEST = 0,
  SENSOR_DELAY_GAME    = 1,  
  SENSOR_DELAY_UI      = 2,
  SENSOR_DELAY_NORMAL  = 3,  
};

/*
    JNI шлюз к менеджеру сенсоров
*/

class JniSensorManager
{
  public:
///Конструктор
    JniSensorManager ()
      : log (LOG_NAME)
    {
      try
      {                 
          //сохранение общих методов и классов SensorManager
          
        JNIEnv& env = get_env ();        

        local_ref<jobject> activity = get_activity ();
        
        if (!activity)
          throw xtl::format_operation_exception ("", "Null activity");
          
        local_ref<jclass> activity_class = env.GetObjectClass (activity.get ());
        
        if (!activity_class)
          throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");
        
        sensor_manager_class = env.FindClass (ANDROID_SENSOR_MANAGER_CLASS_NAME);
      
        if (!sensor_manager_class)
          throw xtl::format_operation_exception ("", "Class %s not found in JNI environment", ANDROID_SENSOR_MANAGER_CLASS_NAME);
        
        get_sensor_list_method     = find_method (&env, sensor_manager_class.get (), "getSensorList", "(I)Ljava/util/List;");
        register_listener_method   = find_method (&env, sensor_manager_class.get (), "registerListener", "(Landroid/hardware/SensorEventListener;Landroid/hardware/Sensor;I)Z");
        unregister_listener_method = find_method (&env, sensor_manager_class.get (), "unregisterListener", "(Landroid/hardware/SensorEventListener;Landroid/hardware/Sensor;)V");
        
          //сохранение общих методов и классов List
        
        list_class = env.FindClass (ANDROID_LIST_CLASS_NAME);
        
        if (!list_class)
          throw xtl::format_operation_exception ("", "Class %s not found in JNI environment", ANDROID_LIST_CLASS_NAME);
          
        get_list_size_method = find_method (&env, list_class.get (), "size", "()I");
        get_list_item_method = find_method (&env, list_class.get (), "get", "(I)Ljava/lang/Object;");
        
          //сохранение общих методов и классов Sensor
          
        sensor_class = env.FindClass (ANDROID_SENSOR_CLASS_NAME);
        
        if (!sensor_class)
          throw xtl::format_operation_exception ("", "Class %s not found in JNI environment", ANDROID_SENSOR_CLASS_NAME);
          
        get_sensor_name_method       = find_method (&env, sensor_class.get (), "getName", "()Ljava/lang/String;");
        get_sensor_vendor_method     = find_method (&env, sensor_class.get (), "getVendor", "()Ljava/lang/String;");
        get_sensor_type_method       = find_method (&env, sensor_class.get (), "getType", "()I");
        get_sensor_max_range_method  = find_method (&env, sensor_class.get (), "getMaximumRange", "()F");
        get_sensor_version_method    = find_method (&env, sensor_class.get (), "getVersion", "()I");
        get_sensor_power_method      = find_method (&env, sensor_class.get (), "getPower", "()F");
        get_sensor_resolution_method = find_method (&env, sensor_class.get (), "getResolution", "()F");        
        get_sensor_min_delay_method  = env.GetMethodID (sensor_class.get (), "getMinDelay", "()I");        
        
        if (env.ExceptionOccurred ())
          env.ExceptionClear ();         
                
          //заполнение таблиц сенсоров
        
        AddSensors (SENSOR_TYPE_ALL);        
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniSensorManager::JniSensorManager");
        throw;
      }      
    }
    
///Количество сенсоров
    size_t SensorsCount () { return sensors.size (); }
    
///Получение сенсора
    global_ref<jobject> Sensor (size_t index) { return sensors [index]; }
    
///Получение имени сенсора
    jstring SensorName       (const global_ref<jobject>& sensor) { return (jstring)get_env ().CallObjectMethod (sensor.get (), get_sensor_name_method); }
    jstring SensorVendor     (const global_ref<jobject>& sensor) { return (jstring)get_env ().CallObjectMethod (sensor.get (), get_sensor_vendor_method); }
    int     SensorType       (const global_ref<jobject>& sensor) { return get_env ().CallIntMethod (sensor.get (), get_sensor_type_method); }
    int     SensorMinDelay   (const global_ref<jobject>& sensor) { return get_sensor_min_delay_method ? get_env ().CallIntMethod (sensor.get (), get_sensor_min_delay_method) : 0; }
    float   SensorMaxRange   (const global_ref<jobject>& sensor) { return get_env ().CallFloatMethod (sensor.get (), get_sensor_max_range_method); }
    int     SensorVersion    (const global_ref<jobject>& sensor) { return get_env ().CallIntMethod (sensor.get (), get_sensor_version_method); }
    float   SensorPower      (const global_ref<jobject>& sensor) { return get_env ().CallFloatMethod (sensor.get (), get_sensor_power_method); }
    float   SensorResolution (const global_ref<jobject>& sensor) { return get_env ().CallFloatMethod (sensor.get (), get_sensor_resolution_method); }    
    
///Обработчик события изменения сенсора
    void OnSensorChanged (sensor_handle& handle, jlong timestamp, const local_ref<jfloatArray>& values)
    {
      try
      {        
        JNIEnv& env = get_env ();

        if (!handle.app_listener || !values)
          return;

        jint array_length = env.GetArrayLength (values.get ());

        if (array_length <= 0)
          return;

        SensorEvent event;
        
        memset (&event, 0, sizeof (event));

        static const jint MAX_ARRAY_LENGTH = sizeof (event.data) / sizeof (*event.data);

        if (array_length > MAX_ARRAY_LENGTH)
          array_length = MAX_ARRAY_LENGTH;          

        event.timestamp    = timestamp;
        event.values_count = (unsigned char)array_length;

        float* src_buf = reinterpret_cast<float*> (env.GetPrimitiveArrayCritical (values.get (), 0));
        
        if (!src_buf)
          throw xtl::format_operation_exception ("", "JNIEnv::GetPrimitiveArrayCritical failed");

        memcpy (event.data, src_buf, array_length * sizeof (float));

        env.ReleasePrimitiveArrayCritical (values.get (), src_buf, 0);

        handle.app_listener->OnSensorChanged (event);
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n    at syslib::android::JniSensorManager::OnSensorChanged", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown error\n    at syslib::android::JniSensorManager::OnSensorChanged");
      }
    }
    
///Начало получения событий от сенсора
    void StartSensorPolling (sensor_handle& handle, ISensorEventListener& listener)
    {            
      if (handle.app_listener)
        throw xtl::format_operation_exception ("", "SensorEventListener has already registered");
      
      jboolean status = get_env ().CallBooleanMethod (GetSensorManager ().get (), register_listener_method, handle.event_listener.get (), handle.sensor.get (), SENSOR_DELAY_FASTEST);
      
      if (!status)
        throw xtl::format_operation_exception ("", "SensorManager::registerListener failed");
        
      handle.app_listener = &listener;
    }
    
///Конец получения событий от сенсора
    void StopSensorPolling (sensor_handle& handle)
    {
      if (!handle.app_listener)
        return;
        
      handle.app_listener = 0;        
      
      get_env ().CallVoidMethod (GetSensorManager ().get (), unregister_listener_method, handle.event_listener.get (), handle.sensor.get ());                  
    }    

  private:
///Получение менеджер сенсоров
    local_ref<jobject> GetSensorManager ()
    {
      JNIEnv& env = get_env ();

      local_ref<jobject> activity = get_activity ();
      
      if (!activity)
        throw xtl::format_operation_exception ("", "Null activity");
        
      local_ref<jclass> activity_class = env.GetObjectClass (activity.get ());
      
      if (!activity_class)
        throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");        

      jmethodID get_system_service_method = find_method (&env, activity_class.get (), "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");        
      
      static const char* ANDROID_SENSOR_MANAGER_SERVICE = "sensor";

      local_ref<jobject> sensor_manager = env.CallObjectMethod (activity.get (), get_system_service_method, tojstring (ANDROID_SENSOR_MANAGER_SERVICE).get ());
      
      if (!sensor_manager)
        throw xtl::format_operation_exception ("", "getSystemService('%s') failed", ANDROID_SENSOR_MANAGER_SERVICE);            
        
      return sensor_manager;
    }
  
///Добавление сенсоров
    void AddSensors (int type)
    {
      try
      {
        JNIEnv& env = get_env ();

        local_ref<jobject> sensor_manager = GetSensorManager ();
        
        local_ref<jobject> sensors_list = env.CallObjectMethod (sensor_manager.get (), get_sensor_list_method, type);
        
        if (!sensors_list)
          throw xtl::format_operation_exception ("", "android.hardware.SensorManager.getSensorList failed");
          
        int items_count = env.CallIntMethod (sensors_list.get (), get_list_size_method);
        
        if (items_count <= 0)
          return;
          
        for (int i=0; i<items_count; i++)
        {
          local_ref<jobject> sensor = env.CallObjectMethod (sensors_list.get (), get_list_item_method, i);
          
          if (!sensor)
            continue;
            
          sensors.push_back (sensor);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::android::JniSensorManager::AddSensors");
        throw;
      }
    }
    
  private:
    typedef global_ref<jobject>    SensorPtr;
    typedef stl::vector<SensorPtr> SensorArray;
      
  private:
    common::Log         log;                           //поток отладочного протоколирования
    SensorArray         sensors;                       //список сенсоров
    global_ref<jclass>  sensor_manager_class;          //класс менеджера сенсоров
    jmethodID           get_sensor_list_method;        //метод получения списка сенсоров
    jmethodID           register_listener_method;      //метод подписки на события сенсоров
    jmethodID           unregister_listener_method;    //метод отмены подписки на события сенсоров    
    global_ref<jclass>  list_class;                    //класс работы со списком
    jmethodID           get_list_size_method;          //метод получения количества элементов в списке
    jmethodID           get_list_item_method;          //метод получения элемента списка
    global_ref<jclass>  sensor_class;                  //класс работы с сенсором
    jmethodID           get_sensor_name_method;        //метод получения имени сенсора
    jmethodID           get_sensor_type_method;        //метод получения типа сенсора
    jmethodID           get_sensor_vendor_method;      //метод получения производителя сенсора
    jmethodID           get_sensor_max_range_method;   //метод получения максимального значения сенсора
    jmethodID           get_sensor_min_delay_method;   //метод получения минимальной задержки между событиями сенсора    
    jmethodID           get_sensor_version_method;     //метод получения версии сенсора
    jmethodID           get_sensor_resolution_method;  //метод получения разрешающей способности сенсора
    jmethodID           get_sensor_power_method;       //метод получения потребления энергии сенсором
    global_ref<jobject> event_listener;                //слушатель событий сенсоров
};

typedef common::Singleton<JniSensorManager> JniSensorManagerSingleton;

void JNICALL on_accuracy_changed (JNIEnv&, jlong sensorRef, jobject sensor, int accuracy)
{
///ignored
}

void JNICALL on_sensor_changed (JNIEnv&, jlong sensorRef, jobject sensor, jint accuracy, jlong timestamp, jfloatArray values)
{
  sensor_t handle = reinterpret_cast<sensor_t> (sensorRef);  
  
  JniSensorManagerSingleton::Instance ()->OnSensorChanged (*handle, timestamp, local_ref<jfloatArray> (values));    
}

}

/*
    Количество сенсоров
*/

size_t AndroidSensorManager::GetSensorsCount ()
{
  try
  {
    return JniSensorManagerSingleton::Instance ()->SensorsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorsCount");
    throw;
  }
}

/*
    Создание / удаление сенсора
*/

sensor_t AndroidSensorManager::CreateSensor (size_t sensor_index)
{
  try
  {
    JniSensorManagerSingleton::Instance manager;
    
    if (sensor_index >= manager->SensorsCount ())
      throw xtl::make_range_exception ("", "sensor_index", sensor_index, manager->SensorsCount ());
      
    return new sensor_handle (manager->Sensor (sensor_index));
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::CreateSensor");
    throw;
  }
}

void AndroidSensorManager::DestroySensor (sensor_t handle)
{
  delete handle;
}

/*
    Имя сенсора
*/

stl::string AndroidSensorManager::GetSensorName (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");
      
    return jni_string (&get_env (), JniSensorManagerSingleton::Instance ()->SensorName (handle->sensor)).get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorName");
    throw;
  }
}

/*
    Производитель сенсора
*/

stl::string AndroidSensorManager::GetSensorVendor (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");
      
    return jni_string (&get_env (), JniSensorManagerSingleton::Instance ()->SensorVendor (handle->sensor)).get (); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorVendor");
    throw;
  }
}

/*
    Тип устройства
*/

stl::string AndroidSensorManager::GetSensorType (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");    
      
    int type = JniSensorManagerSingleton::Instance ()->SensorType (handle->sensor);
    
    switch (type)
    {
      case SENSOR_TYPE_ACCELEROMETER:       return "accelerometer";
      case SENSOR_TYPE_MAGNETIC_FIELD:      return "magnetic_field";
      case SENSOR_TYPE_ORIENTATION:         return "orientation";
      case SENSOR_TYPE_GYROSCOPE:           return "gyroscope";
      case SENSOR_TYPE_LIGHT:               return "light";
      case SENSOR_TYPE_PRESSURE:            return "pressure";
      case SENSOR_TYPE_TEMPERATURE:         return "temperature";
      case SENSOR_TYPE_PROXIMITY:           return "proximity";
      case SENSOR_TYPE_GRAVITY:             return "gravity";
      case SENSOR_TYPE_LINEAR_ACCELERATION: return "linear_acceleration";
      case SENSOR_TYPE_ROTATION_VECTOR:     return "rotation_vector";
      case SENSOR_TYPE_RELATIVE_HUMIDITY:   return "relative_humidity";
      default:                              return common::format ("type%02d", type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorType");
    throw;
  }
}

/*
    Максимальное значение
*/

float AndroidSensorManager::GetSensorMaxRange (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");    
      
    return JniSensorManagerSingleton::Instance ()->SensorMaxRange (handle->sensor);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorMaxRange");
    throw;
  }
}

/*
    Частота обновления
*/

void AndroidSensorManager::SetSensorUpdateRate (sensor_t handle, float rate)
{
  if (!handle)   
    throw xtl::make_null_argument_exception ("syslib::AndroidSensorManager::SetSensorUpdateRate", "handle");

///ignored
}

float AndroidSensorManager::GetSensorUpdateRate (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");
      
    int min_delay = JniSensorManagerSingleton::Instance ()->SensorMinDelay (handle->sensor);
    
    if (min_delay <= 0)
      min_delay = 1;
      
    return 1000.0f / min_delay;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorUpdateRate");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* AndroidSensorManager::GetNativeSensorHandle (sensor_t handle)
{
  if (!handle)   
    throw xtl::make_null_argument_exception ("syslib::AndroidSensorManager::GetNativeSensorHandle", "handle");
    
  return (const void*)handle->sensor.get ();
}

/*
    Получение платформо-зависимых свойств экрана
*/

void AndroidSensorManager::GetSensorProperties (sensor_t handle, common::PropertyMap& properties)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");
      
    JniSensorManagerSingleton::Instance manager;
    
    properties.SetProperty ("Version", manager->SensorVersion (handle->sensor));
    properties.SetProperty ("Resolution", manager->SensorResolution (handle->sensor));
    properties.SetProperty ("Power", manager->SensorPower (handle->sensor));
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::GetSensorProperties");
    throw;
  }
}

/*
    Чтение событий сенсора
*/

void AndroidSensorManager::StartSensorPolling (sensor_t handle, ISensorEventListener& listener)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");    
    
    JniSensorManagerSingleton::Instance ()->StartSensorPolling (*handle, listener);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::StartSensorPolling");
    throw;
  }
}

void AndroidSensorManager::StopSensorPolling (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");
        
    JniSensorManagerSingleton::Instance ()->StopSensorPolling (*handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::AndroidSensorManager::StopSensorPolling");
    throw;
  }
}

void AndroidSensorManager::PollSensorEvents (sensor_t)
{
}

namespace syslib
{

namespace android
{

/// регистрация методов обратного вызова менеджера сенсоров
void register_sensor_manager_callbacks (JNIEnv* env)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    jclass event_listener_class = env->FindClass ("com/untgames/funner/application/EngineSensorEventListener");

    if (!event_listener_class)
      throw xtl::format_operation_exception ("", "Can't find EngineSensorEventListener class\n");

    static const JNINativeMethod methods [] = {
      {"onAccuracyChangedCallback", "(JLandroid/hardware/Sensor;I)V", (void*)&on_accuracy_changed},
      {"onSensorChangedCallback", "(JLandroid/hardware/Sensor;IJ[F)V", (void*)&on_sensor_changed},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (event_listener_class, methods, methods_count);

    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_sensor_manager_callbacks");
    throw;
  }
}

}

}
