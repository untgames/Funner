#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No cookie support for this platform");
}

}

/*
   Политика приема cookie
*/

void DefaultCookieManager::SetAcceptCookie (bool policy)
{
  raise ("syslib::DefaultCookieManager::SetAcceptCookie");
}

bool DefaultCookieManager::AcceptCookie ()
{
  raise ("syslib::DefaultCookieManager::AcceptCookie");
  return false;
}

/*
   Удаление cookie
*/

void DefaultCookieManager::DeleteCookies (const char* uri)
{
  raise ("syslib::DefaultCookieManager::DeleteCookies");
}

void DefaultCookieManager::DeleteAllCookies ()
{
  raise ("syslib::DefaultCookieManager::DeleteAllCookies");
}
