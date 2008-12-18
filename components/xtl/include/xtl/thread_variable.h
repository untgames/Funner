#ifndef XTL_THREAD_VARIABLE_HEADER
#define XTL_THREAD_VARIABLE_HEADER

#if defined ( _MSC_VER_ ) || defined ( _WIN32 ) || defined ( __CYGWIN__ )
  #define XTL_THREAD_VARIABLE __declspec(thread)
#else
  #error "Unknown platform"
#endif

#endif
