#include "shared.h"

//!!!!!добавить очистки ресурсов процесса pthread_win32_process_detach_np ()

using namespace syslib;

/*
    Утилиты
*/

namespace
{

//генерация исключения с кодом ошибки
void raise (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", common::strerror (status), status);
}

#ifdef _WIN32

//функция инициализации библиотеки
inline void thread_init ()
{
  static bool is_process_initialized = false;
  
  if (!is_process_initialized)
  {
    pthread_win32_process_attach_np ();
    
    is_process_initialized = true;
  }

  static __declspec(thread) bool is_thread_initialized = false;

  if (is_thread_initialized)
    return;

  pthread_win32_thread_attach_np ();

  is_thread_initialized = true;
}

//функция деинициализации библиотеки
inline void thread_done (void*)
{
  pthread_win32_thread_detach_np ();
}

#else

inline void thread_init ()
{
}

inline void thread_done (void*)
{
}

#endif

//функция нити
void* thread_run (void* data)
{
  if (!data)
    return 0;

  xtl::com_ptr<IThreadCallback> callback (reinterpret_cast<IThreadCallback*> (data));

  thread_init ();  

  pthread_cleanup_push  (&thread_done, 0);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 0);

  try
  {
    callback->Run ();
  }
  catch (...)
  {
    thread_done (0);
    throw;
  }

  pthread_cleanup_pop (1);

  return 0;
}

//получение дескриптора нити
pthread_t get_handle (Platform::thread_t thread)
{
  return thread ? *(pthread_t*)thread : pthread_self ();
}

}

/*
    Создание / удаление нити
*/

Platform::thread_t Platform::CreateThread (IThreadCallback* in_callback)
{
  try
  {
      //проверка корректности аргументов, захват объекта обслуживания нити

    if (!in_callback)
      throw xtl::make_null_argument_exception ("", "callback");

    xtl::com_ptr<IThreadCallback> callback (in_callback);

      //инициализации библиотеки

    thread_init ();

      //создание нити

    stl::auto_ptr<pthread_t> handle (new pthread_t);

    int status = pthread_create (handle.get (), 0, &thread_run, callback.get ());

    if (status)
      raise ("::pthread_create", status);
      
    return (thread_t)handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::CreateThread");
    throw;
  }
}

void Platform::DeleteThread (thread_t thread)
{
  if (!thread)
    return;    

  pthread_t* handle = (pthread_t*)thread;    

  delete handle;
}

/*
    Отмена нити
*/

void Platform::CancelThread (thread_t thread)
{
  try
  {
    thread_init ();    

    int status = pthread_cancel (get_handle (thread));

    if (status)
      raise ("::pthread_cancel", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::CancelThread");
    throw;
  }    
}

/*
    Ожидание завершения нити
*/

void Platform::JoinThread (thread_t thread)
{
  try
  {
    thread_init ();
    
    void* exit_code = 0;

    int status = pthread_join (get_handle (thread), &exit_code);

    if (status)
      raise ("::pthread_join", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::JoinThread");
    throw;
  }
}

/*
    Работа с локальными данными нити
*/

namespace
{

//хранилище переменных нити
struct TlsValueImpl
{
  void*                   data;
  IThreadCleanupCallback* cleanup;

  TlsValueImpl (IThreadCleanupCallback* in_cleanup) : data (0), cleanup (in_cleanup) {}  
  
  static void Cleanup (void* data)
  {
    TlsValueImpl* value = (TlsValueImpl*)data;
    
    if (!value || !value->cleanup)
      return;

    value->cleanup->Cleanup (value->data);
  }
};

//ключ переменных нити
struct TlsKeyImpl
{
  pthread_key_t           key;
  IThreadCleanupCallback* cleanup;
  
  TlsKeyImpl (IThreadCleanupCallback* in_cleanup) : cleanup (in_cleanup)
  {
    int status = pthread_key_create (&key, &TlsValueImpl::Cleanup);
    
    if (status)
      raise ("::pthread_create", status);
  }
  
  ~TlsKeyImpl ()
  {
    pthread_key_delete (key);
  }
  
  TlsValueImpl& GetValue ()
  {
    TlsValueImpl* value = (TlsValueImpl*)pthread_getspecific (key);

    if (value)
      return *value;

    stl::auto_ptr<TlsValueImpl> new_value (new TlsValueImpl (cleanup));

    int status = pthread_setspecific (key, new_value.get ());

    if (status)
      raise ("::pthread_setspecific", status);

    return *new_value.release ();
  }
};

}

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback* cleanup)
{
  try
  {
    thread_init ();

    return (tls_t)new TlsKeyImpl (cleanup);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::CreateTls");
    throw;
  }
}

void Platform::DeleteTls (tls_t tls)
{
  try
  {
    if (!tls)
      return;
      
    thread_init ();

    TlsKeyImpl* handle = (TlsKeyImpl*)tls;
      
    delete handle;
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

void Platform::SetTls (tls_t tls, void* data)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");
    
    thread_init ();
    
    TlsKeyImpl* key = (TlsKeyImpl*)tls;

    key->GetValue ().data = data;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::SetTls");
    throw;
  }
}

void* Platform::GetTls (tls_t tls)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");
    
    thread_init ();
    
    TlsKeyImpl* key = (TlsKeyImpl*)tls;

    return key->GetValue ().data;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Platform::GetTls");
    throw;
  }
}
