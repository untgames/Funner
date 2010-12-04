#ifndef LUA_ANDROID_LCONV_WRAPPER
#define LUA_ANDROID_LCONV_WRAPPER

#include <locale.h>

#ifdef ANDROID

struct lua_android_lconv_wrapper
{
  const char* decimal_point;
};

inline lua_android_lconv_wrapper* lua_android_lconv ()
{
  static lua_android_lconv_wrapper wrapper = {"."};
  
  return &wrapper;
}

#define lconv      lua_android_lconv_wrapper
#define localeconv lua_android_lconv

#endif

#endif
