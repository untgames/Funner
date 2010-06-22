#include "../shared.h"

using namespace syslib;

Platform::tls_t Platform::CreateTls (IThreadCleanupCallback* cleanup)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::CreateTls");
}

void Platform::DestroyTls (tls_t tls)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::DestroyTls");
}

void Platform::SetTls (tls_t tls, void* data)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetTls");
}

void* Platform::GetTls (tls_t tls)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetTls");
}
