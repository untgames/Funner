#include "shared.h"

using namespace syslib;

struct Platform::tls_handle
{
  DWORD tls;
};

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback*)
{
  try
  {
    stl::auto_ptr<tls_handle> handle (new tls_handle);

    handle->tls = TlsAlloc ();

    if (handle->tls == TLS_OUT_OF_INDEXES)
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

  TlsFree (tls->tls);

  delete tls;
}

void Platform::SetTls (tls_t tls, void* data)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");

    if (!TlsSetValue (tls->tls, data))
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

    void *value = TlsGetValue (tls->tls);

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
