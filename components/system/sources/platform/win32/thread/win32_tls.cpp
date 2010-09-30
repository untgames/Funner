#include "shared.h"

using namespace syslib;

struct Platform::tls_handle
{
  IThreadCleanupCallback* cleanup;
  DWORD                   key;
  
  tls_handle (IThreadCleanupCallback* in_cleanup) : cleanup (in_cleanup), key (0) {}
};

namespace
{

/*
    Менеджер TLS
*/

class TlsManager
{
  public:
///Регистрация TLS
    void Register (Platform::tls_t tls)
    {
      ThreadPair tls_thread_pair (GetCurrentThreadId (), tls->key);
      
      if (registered_tls.find (tls_thread_pair) != registered_tls.end ())
        return;

      CleanupMap::iterator iter = cleanup_map.insert_pair (GetCurrentThreadId (), tls);

      try
      {
        registered_tls.insert (tls_thread_pair);
      }
      catch (...)
      {
        cleanup_map.erase (iter);
        throw;
      }
    }
    
///Проверка наличия tls для данной нити
    bool IsPresent (Platform::tls_t tls)
    {
      ThreadPair tls_thread_pair (GetCurrentThreadId (), tls->key);

      return registered_tls.find (tls_thread_pair) != registered_tls.end ();
    }
    
///Очистка для текущей нити
    void Cleanup ()
    {
      DWORD current_thread_id = GetCurrentThreadId ();
      
      stl::pair<CleanupMap::iterator, CleanupMap::iterator> range = cleanup_map.equal_range (current_thread_id);
      
      for (CleanupMap::iterator iter=range.first; iter != range.second; ++iter)
      {
        Platform::tls_t tls = iter->second;

        try
        {
          if (tls->cleanup)
            tls->cleanup->Cleanup (Platform::GetTls (tls));
        }
        catch (...)
        {
          ///подавление всех исключений
        }

        registered_tls.erase (ThreadPair (current_thread_id, tls->key));
      }

      cleanup_map.erase (range.first, range.second);
    }
  
  private:
    typedef stl::pair<DWORD, DWORD>                    ThreadPair;
    typedef stl::hash_set<ThreadPair>                  RegistrationSet;
    typedef stl::hash_multimap<DWORD, Platform::tls_t> CleanupMap;

  private:
    RegistrationSet registered_tls;
    CleanupMap      cleanup_map;
};

typedef common::Singleton<TlsManager, common::SingletonStatic> TlsManagerSingleton;

}

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback* cleanup)
{
  try
  {
    stl::auto_ptr<tls_handle> handle (new tls_handle (cleanup));

    handle->key = TlsAlloc ();

    if (handle->key == TLS_OUT_OF_INDEXES)
      raise_error ("::TlsAlloc");

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateTls");
    throw;
  }
}

void Platform::DestroyTls (tls_t tls)
{
  if (!tls)
    return;

  TlsFree (tls->key);

  delete tls;
}

void Platform::SetTls (tls_t tls, void* data)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");

    TlsManagerSingleton::Instance ()->Register (tls);

    if (!TlsSetValue (tls->key, data))
      raise_error ("::TlsSetValue");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetTls");
    throw;
  }
}

void* Platform::GetTls (tls_t tls)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");

    if (!TlsManagerSingleton::Instance ()->IsPresent (tls))
      return 0;    

    void *value = TlsGetValue (tls->key);

    if (!value && (GetLastError () != ERROR_SUCCESS))
      raise_error ("::GetLastError");

    return value;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetTls");
    throw;
  }
}

namespace syslib
{

void cleanup_tls ()
{
  try
  {
    TlsManagerSingleton::Instance ()->Cleanup ();
  }
  catch (...)
  {
  }
}

}
