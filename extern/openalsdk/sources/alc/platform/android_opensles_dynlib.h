#include <dlfcn.h>

#include <SLES/OpenSLES_Android.h>

static const struct SLInterfaceID_ SL_IID_ANDROIDSIMPLEBUFFERQUEUE_ = { 0x198e4940, 0xc5d7, 0x11df, 0xa2a6, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
SLAPIENTRY const SLInterfaceID SL_IID_ANDROIDSIMPLEBUFFERQUEUE = &SL_IID_ANDROIDSIMPLEBUFFERQUEUE_; 

static const struct SLInterfaceID_ SL_IID_ENGINE_ = { 0x8d97c260, 0xddd4, 0x11db, 0x958f, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
SLAPIENTRY const SLInterfaceID SL_IID_ENGINE = &SL_IID_ENGINE_; 

static const struct SLInterfaceID_ SL_IID_BUFFERQUEUE_ = { 0x2bc99cc0, 0xddd4, 0x11db, 0x8d99, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
SLAPIENTRY const SLInterfaceID SL_IID_BUFFERQUEUE = &SL_IID_BUFFERQUEUE_; 

static const struct SLInterfaceID_ SL_IID_PLAY_ = {  0xef0bd9c0, 0xddd7, 0x11db, 0xbf49, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
SLAPIENTRY const SLInterfaceID SL_IID_PLAY = &SL_IID_PLAY_; 

typedef SLresult SLAPIENTRY (*slCreateEngineFn)(
  SLObjectItf             *pEngine,
  SLuint32                numOptions,
  const SLEngineOption    *pEngineOptions,
  SLuint32                numInterfaces,
  const SLInterfaceID     *pInterfaceIds,
  const SLboolean         * pInterfaceRequired
);

typedef SLresult SLAPIENTRY (*slQueryNumSupportedEngineInterfacesFn)(
  SLuint32 * pNumSupportedInterfaces
);

typedef  SLresult SLAPIENTRY (*slQuerySupportedEngineInterfacesFn)(
  SLuint32 index,
  SLInterfaceID * pInterfaceId
);

static void*                                 openSlesLibraryHandle                  = 0;
static slQueryNumSupportedEngineInterfacesFn slQueryNumSupportedEngineInterfacesPtr = 0;
static slQuerySupportedEngineInterfacesFn    slQuerySupportedEngineInterfacesPtr    = 0;
static slCreateEngineFn                      slCreateEnginePtr                      = 0;

static void dynlibOpenSlesShutdown ()
{
  if (!openSlesLibraryHandle)
    return;
    
  dlclose (openSlesLibraryHandle);
  
  openSlesLibraryHandle                  = 0;
  slCreateEnginePtr                      = 0;
  slQueryNumSupportedEngineInterfacesPtr = 0;  
  slQuerySupportedEngineInterfacesPtr    = 0;
}

static ALCboolean dynlibOpenSlesInitialize ()
{
  if (openSlesLibraryHandle)
    return ALC_TRUE;

  openSlesLibraryHandle = dlopen ("libOpenSLES.so", RTLD_NOW | RTLD_GLOBAL);

  if (!openSlesLibraryHandle)
    return ALC_FALSE;

  slCreateEnginePtr                      = (slCreateEngineFn)dlsym (openSlesLibraryHandle, "slCreateEngine");
  slQueryNumSupportedEngineInterfacesPtr = (slQueryNumSupportedEngineInterfacesFn)dlsym (openSlesLibraryHandle, "slQueryNumSupportedEngineInterfaces");
  slQuerySupportedEngineInterfacesPtr    = (slQuerySupportedEngineInterfacesFn)dlsym (openSlesLibraryHandle, "slQuerySupportedEngineInterfaces");

  if (!slQuerySupportedEngineInterfacesPtr || !slQueryNumSupportedEngineInterfacesPtr || !slCreateEnginePtr)
  {
    dynlibOpenSlesShutdown ();    
    return ALC_FALSE;
  }

  return ALC_TRUE;
}

SLresult SLAPIENTRY slCreateEngine (
  SLObjectItf             *pEngine,
  SLuint32                numOptions,
  const SLEngineOption    *pEngineOptions,
  SLuint32                numInterfaces,
  const SLInterfaceID     *pInterfaceIds,
  const SLboolean         * pInterfaceRequired
)
{
  if (!slCreateEnginePtr)
    return SL_RESULT_FEATURE_UNSUPPORTED;

  return slCreateEnginePtr (pEngine, numOptions, pEngineOptions, numInterfaces, pInterfaceIds, pInterfaceRequired);
}

SLresult SLAPIENTRY slQueryNumSupportedEngineInterfaces(
  SLuint32 * pNumSupportedInterfaces
)
{
  if (!slQueryNumSupportedEngineInterfacesPtr)
    return SL_RESULT_FEATURE_UNSUPPORTED;

  return slQueryNumSupportedEngineInterfacesPtr (pNumSupportedInterfaces);
}

SLresult SLAPIENTRY slQuerySupportedEngineInterfaces(
  SLuint32 index,
  SLInterfaceID * pInterfaceId
)
{
  if (!slQuerySupportedEngineInterfacesPtr)
    return SL_RESULT_FEATURE_UNSUPPORTED;

  return slQuerySupportedEngineInterfacesPtr (index, pInterfaceId);
}
