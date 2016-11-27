#include "shared.h"

using namespace syslib;
using namespace syslib::android;

/*
   Политика приема cookie
*/

void AndroidCookieManager::SetAcceptCookie (bool policy)
{
  local_ref<jobject> activity (get_activity ());
  local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

  if (!activity_class)
    throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

  jmethodID set_accept_cookie_method = find_method (&get_env (), activity_class.get (), "setAcceptCookie", "(Z)V");

  get_env ().CallVoidMethod (activity.get (), set_accept_cookie_method, policy);
}

bool AndroidCookieManager::AcceptCookie ()
{
  local_ref<jobject> activity (get_activity ());
  local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

  if (!activity_class)
    throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

  jmethodID get_accept_cookie_method = find_method (&get_env (), activity_class.get (), "acceptCookie", "()Z");

  return get_env ().CallBooleanMethod (activity.get (), get_accept_cookie_method);
}

/*
   Удаление cookie
*/

void AndroidCookieManager::DeleteCookies (const char* uri)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidCookieManager::DeleteCookies", "Delete cookies for specific uri not supported on android");
}

void AndroidCookieManager::DeleteAllCookies ()
{
  local_ref<jobject> activity (get_activity ());
  local_ref<jclass>  activity_class (get_env ().GetObjectClass (get_activity ()), false);

  if (!activity_class)
    throw xtl::format_operation_exception ("", "JNIEnv::GetObjectClass failed");

  jmethodID delete_all_cookies_method = find_method (&get_env (), activity_class.get (), "deleteAllCookies", "()V");

  get_env ().CallVoidMethod (activity.get (), delete_all_cookies_method);
}
