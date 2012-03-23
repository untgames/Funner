#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace syslib
{

namespace android
{

int log_vprintf (int level, const char* format, va_list args)
{
  return __android_log_vprint (level, "funner", format, args);
}

int log_printf (int level, const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return log_vprintf (level, format, args);
}

int log_info (const char* format, ...)
{
  va_list args;

  va_start (args, format);

  return log_vprintf (ANDROID_LOG_INFO, format, args);
}

int log_error (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return log_vprintf (ANDROID_LOG_ERROR, format, args);
}

}

}
