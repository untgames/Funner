#ifndef ANALYTICS_FLURRY_PLATFORM_HEADER
#define ANALYTICS_FLURRY_PLATFORM_HEADER

#include <shared/android.h>
#include <shared/default.h>
#include <shared/ios.h>

namespace analytics
{

namespace flurry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///”казание текущей платформы
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef IPHONE
  typedef IOsPlatform Platform;
#elif defined ANDROID
  typedef AndroidPlatform Platform;
#else
  typedef DefaultPlatform Platform;
#endif

}

}

#endif
