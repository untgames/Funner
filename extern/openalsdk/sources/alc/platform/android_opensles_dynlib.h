#include <dlfcn.h>

static struct SLInterfaceID_ SL_IID_BUFFERQUEUE_DATA              = { 0x2bc99cc0, 0xddd4, 0x11db, 0x8d99, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
static struct SLInterfaceID_ SL_IID_PLAY_DATA                     = { 0xef0bd9c0, 0xddd7, 0x11db, 0xbf49, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
static struct SLInterfaceID_ SL_IID_ANDROIDSIMPLEBUFFERQUEUE_DATA = { 0x198e4940, 0xc5d7, 0x11df, 0xa2a6, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
static struct SLInterfaceID_ SL_IID_ENGINE_DATA                   = { 0x8d97c260, 0xddd4, 0x11db, 0x958f, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };

SLAPIENTRY const SLInterfaceID SL_IID_BUFFERQUEUE              = &SL_IID_BUFFERQUEUE_DATA;
SLAPIENTRY const SLInterfaceID SL_IID_PLAY                     = &SL_IID_PLAY_DATA;
SLAPIENTRY const SLInterfaceID SL_IID_ANDROIDSIMPLEBUFFERQUEUE = &SL_IID_ANDROIDSIMPLEBUFFERQUEUE_DATA;
SLAPIENTRY const SLInterfaceID SL_IID_ENGINE                   = &SL_IID_ENGINE_DATA;

typedef SLresult SLAPIENTRY (*slCreateEngineFn)(
  SLObjectItf             *pEngine,
  SLuint32                numOptions,
  const SLEngineOption    *pEngineOptions,
  SLuint32                numInterfaces,
  const SLInterfaceID     *pInterfaceIds,
  const SLboolean         * pInterfaceRequired
);

static void*            openSlesLibraryHandle = 0;
static slCreateEngineFn slCreateEnginePtr   = 0;

static void dynlibOpenSlesShutdown ()
{
  if (!openSlesLibraryHandle)
    return;
    
  dlclose (openSlesLibraryHandle);
  
  openSlesLibraryHandle = 0;
  slCreateEnginePtr   = 0;
}

static ALCboolean dynlibOpenSlesInitialize ()
{
  if (openSlesLibraryHandle)
    return ALC_TRUE;
    
  openSlesLibraryHandle = dlopen ("libOpenSLES.so", RTLD_NOW | RTLD_GLOBAL);
  
  if (!openSlesLibraryHandle)
    return ALC_FALSE;
    
  slCreateEnginePtr = (slCreateEngineFn)dlsym (openSlesLibraryHandle, "slCreateEngine");
  
  if (!slCreateEnginePtr)
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

