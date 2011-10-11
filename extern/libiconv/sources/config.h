#ifdef WINCE
  #include "config-winmobile.h"
#elif _WIN32
  #include "config-win32.h"
#else
  #include "config-unistd.h"
#endif

#ifdef __GNUC__
  #define HAVE__BOOL 1
#endif
