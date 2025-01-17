#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/singleton.h>

#include <syslib/platform/android.h>

#include <shared/android.h>

namespace analytics
{

namespace flurry
{

namespace android
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Flurry
///////////////////////////////////////////////////////////////////////////////////////////////////
class FlurryImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    FlurryImpl  ();
    ~FlurryImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Is analytics supported on current platform
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSupported ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get version name of underlying library
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetReleaseVersion (stl::string& version);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Start analytics
///////////////////////////////////////////////////////////////////////////////////////////////////
    void StartSession (const char* api_key);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Set analytics additional params
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUserId   (const char* user_id);
    void SetAge      (size_t age);
    void SetGender   (Gender gender);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Events tracking
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogEvent      (const char* event, const common::PropertyMap& parameters, bool timed);
    void EndTimedEvent (const char* event, const common::PropertyMap& parameters);
    void LogPageView   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Logging management
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLogLevel (LogLevel level);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Initialization of java-bindings
///////////////////////////////////////////////////////////////////////////////////////////////////
    void InitJavaBindings (JNIEnv* env);

  private:
    FlurryImpl (const FlurryImpl&);               //no impl
    FlurryImpl& operator = (const FlurryImpl&);   //no impl

  private:
    struct Impl;
    Impl* impl;
};

typedef common::Singleton<FlurryImpl> FlurrySingleton;

}

}

}
