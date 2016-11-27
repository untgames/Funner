#ifndef SYSLIB_COOKIE_HEADER
#define SYSLIB_COOKIE_HEADER

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер cookie
///////////////////////////////////////////////////////////////////////////////////////////////////
class CookieManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Политика приема cookie
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetAcceptCookie (bool policy);
    static bool AcceptCookie    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление cookie
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void DeleteCookies    (const char* uri);
    static void DeleteAllCookies ();
};

}

#endif
