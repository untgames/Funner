#ifdef WINCE
  #include "config-winmobile.h"
#elif _WIN32
  #include "config-win32.h"
#elif defined(__APPLE__) && !defined (__IPHONE__)
  #include "config-macosx.h"
#else
  #include "config-unistd.h"
#endif

#ifdef __GNUC__
  #define HAVE__BOOL 1
#endif
