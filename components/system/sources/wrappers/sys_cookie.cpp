#include "shared.h"

using namespace syslib;

/*
   Политика приема cookie
*/

void CookieManager::SetAcceptCookie (bool policy)
{
  try
  {
    Platform::SetAcceptCookie (policy);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CookieManager::SetAcceptCookie");
    throw;
  }
}

bool CookieManager::AcceptCookie ()
{
  try
  {
    return Platform::AcceptCookie ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CookieManager::AcceptCookie");
    throw;
  }
}

/*
   Удаление cookie
*/

void CookieManager::DeleteCookies (const char* uri)
{
  try
  {
    if (!uri)
      throw xtl::make_null_argument_exception ("", "uri");

    Platform::DeleteCookies (uri);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CookieManager::DeleteCookies");
    throw;
  }
}

void CookieManager::DeleteAllCookies ()
{
  try
  {
    Platform::DeleteAllCookies ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CookieManager::DeleteAllCookies");
    throw;
  }
}
