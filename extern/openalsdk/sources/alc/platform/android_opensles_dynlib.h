#include <dlfcn.h>

#include <SLES/OpenSLES_Android.h>

static SLInterfaceID SL_IID_BUFFERQUEUE_DYNLIB              = NULL;
static SLInterfaceID SL_IID_PLAY_DYNLIB                     = NULL;
static SLInterfaceID SL_IID_ANDROIDSIMPLEBUFFERQUEUE_DYNLIB = NULL;
static SLInterfaceID SL_IID_ENGINE_DYNLIB                   = NULL;

#define SL_IID_BUFFERQUEUE              SL_IID_BUFFERQUEUE_DYNLIB
#define SL_IID_PLAY                     SL_IID_PLAY_DYNLIB
#define SL_IID_ANDROIDSIMPLEBUFFERQUEUE SL_IID_ANDROIDSIMPLEBUFFERQUEUE_DYNLIB
#define SL_IID_ENGINE                   SL_IID_ENGINE_DYNLIB

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

  *(SLInterfaceID*)&SL_IID_BUFFERQUEUE              = NULL;
  *(SLInterfaceID*)&SL_IID_PLAY                     = NULL;
  *(SLInterfaceID*)&SL_IID_ANDROIDSIMPLEBUFFERQUEUE = NULL;
  *(SLInterfaceID*)&SL_IID_ENGINE                   = NULL;
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

  *(void**)&SL_IID_BUFFERQUEUE              = dlsym (openSlesLibraryHandle, "SL_IID_BUFFERQUEUE");
  *(void**)&SL_IID_PLAY                     = dlsym (openSlesLibraryHandle, "SL_IID_PLAY");
  *(void**)&SL_IID_ANDROIDSIMPLEBUFFERQUEUE = dlsym (openSlesLibraryHandle, "SL_IID_ANDROIDSIMPLEBUFFERQUEUE");
  *(void**)&SL_IID_ENGINE                   = dlsym (openSlesLibraryHandle, "SL_IID_ENGINE");

  if (!slQuerySupportedEngineInterfacesPtr || !slQueryNumSupportedEngineInterfacesPtr || !slCreateEnginePtr || !SL_IID_BUFFERQUEUE || !SL_IID_PLAY || !SL_IID_ANDROIDSIMPLEBUFFERQUEUE || !SL_IID_ENGINE)
  {
    dynlibOpenSlesShutdown ();    
    return ALC_FALSE;
  }
  
  SLuint32 numSupportedInterfaces = 0;
  
  if (slQueryNumSupportedEngineInterfaces (&numSupportedInterfaces) != SL_RESULT_SUCCESS)
  {
    dynlibOpenSlesShutdown ();
    return ALC_FALSE;
  }
  
  const SLInterfaceID ids [] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ENGINE};
  
  size_t i, j;
  
  for (i=0; i<sizeof (ids) / sizeof (*ids); i++)
  {
    const SLInterfaceID id = ids [i];
    
    int found = 0;
    
    for (j=0; j<numSupportedInterfaces; j++)
    {
      SLInterfaceID check_id = 0;
      
      if (slQuerySupportedEngineInterfaces (j, &check_id) != SL_RESULT_SUCCESS)
      {
        dynlibOpenSlesShutdown ();
        return ALC_FALSE;
      }
      
      if (check_id == id)
      {
        found = 1;
        break;
      }            
    }
    
    if (!found)
    {
      dynlibOpenSlesShutdown ();
      return ALC_FALSE;
    }
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
