#ifndef SOCIAL_FACEBOOK_PLATFORM_HEADER
#define SOCIAL_FACEBOOK_PLATFORM_HEADER

#include <shared/android.h>
#include <shared/default.h>
#include <shared/ios.h>

namespace social
{

namespace facebook
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
