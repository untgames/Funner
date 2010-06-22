#include "shared.h"

using namespace syslib;

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback* cleanup)
{
  raise ("syslib::DefaultPlatform::CreateTls");
  return 0;
}

void Platform::DestroyTls (tls_t tls)
{
  raise ("syslib::DefaultPlatform::DestroyTls");
}

void Platform::SetTls (tls_t tls, void* data)
{
  raise ("syslib::DefaultPlatform::SetTls");
}

void* Platform::GetTls (tls_t tls)
{
  raise ("syslib::DefaultPlatform::GetTls");
  return 0;
}
