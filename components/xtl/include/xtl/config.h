#ifndef XTL_CONFIG_HEADER
#define XTL_CONFIG_HEADER

#ifdef _MSC_VER  
  #if _MSC_VER < 1300
    #error Microsoft Visual C/C++ 7.0 or higher reqiured
  #endif

  //MSVC 7 typedefs wchar_t
  #if _MSC_VER < 1400
    #define XTL_NO_WCHAR
  #endif
#endif

#endif
