#include "shared.h"

using namespace syslib;

/*
   Политика приема cookie
*/

void CocoaCookieManager::SetAcceptCookie (bool policy)
{
  NSAutoreleasePool   *pool    = [[NSAutoreleasePool alloc] init];
  NSHTTPCookieStorage *storage = [NSHTTPCookieStorage sharedHTTPCookieStorage];

  [storage setCookieAcceptPolicy:policy ? NSHTTPCookieAcceptPolicyAlways : NSHTTPCookieAcceptPolicyNever];

  [pool release];
}

bool CocoaCookieManager::AcceptCookie ()
{
  NSAutoreleasePool   *pool    = [[NSAutoreleasePool alloc] init];
  NSHTTPCookieStorage *storage = [NSHTTPCookieStorage sharedHTTPCookieStorage];

  bool return_value = [storage cookieAcceptPolicy] == NSHTTPCookieAcceptPolicyAlways;

  [pool release];

  return return_value;
}

/*
   Удаление cookie
*/

void CocoaCookieManager::DeleteCookies (const char* uri)
{
  NSAutoreleasePool *pool   = [[NSAutoreleasePool alloc] init];
  NSURL             *ns_url = [NSURL URLWithString:[NSString stringWithUTF8String:uri]];

  if (!ns_url)
  {
    [pool release];

    throw xtl::format_operation_exception ("syslib::CocoaCookieManager::DeleteCookies", "Can't create url from uri '%s'", uri);
  }

  NSHTTPCookieStorage *storage = [NSHTTPCookieStorage sharedHTTPCookieStorage];
  NSArray             *cookies = [storage cookiesForURL:ns_url];

  for (size_t i = 0, count = [cookies count]; i < count; i++)
    [storage deleteCookie:[cookies objectAtIndex:i]];

  [pool release];
}

void CocoaCookieManager::DeleteAllCookies ()
{
  NSAutoreleasePool   *pool    = [[NSAutoreleasePool alloc] init];
  NSHTTPCookieStorage *storage = [NSHTTPCookieStorage sharedHTTPCookieStorage];
  NSArray             *cookies = [storage cookies];

  for (size_t i = 0, count = [cookies count]; i < count; i++)
    [storage deleteCookie:[cookies objectAtIndex:i]];

  [pool release];
}
