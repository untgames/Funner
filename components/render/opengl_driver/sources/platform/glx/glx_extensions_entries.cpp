#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

template <class Fn>
inline void init_extension_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = reinterpret_cast<Fn> (library.GetProcAddress (name, EntrySearch_Context | EntrySearch_NoThrow));
}

}

/*
    Инициализация точек входа GLX
*/

void GlxExtensionsEntries::Init (ILibrary& library)
{
    //обнуление адресов точек входа

  memset (this, 0, sizeof (GlxExtensionsEntries));

    //получение адресов точек входа расширений OpenGL

  init_extension_entry (library, "glXGetFBConfigs", GetFBConfigs);
  init_extension_entry (library, "glXChooseFBConfig", ChooseFBConfig);
  init_extension_entry (library, "glXGetFBConfigAttrib", GetFBConfigAttrib);
  init_extension_entry (library, "glXGetVisualFromFBConfig", GetVisualFromFBConfig);
  init_extension_entry (library, "glXCreateWindow", CreateWindow);
  init_extension_entry (library, "glXDestroyWindow", DestroyWindow);
  init_extension_entry (library, "glXCreatePixmap", CreatePixmap);
  init_extension_entry (library, "glXDestroyPixmap", DestroyPixmap);
  init_extension_entry (library, "glXCreatePbuffer", CreatePbuffer);
  init_extension_entry (library, "glXDestroyPbuffer", DestroyPbuffer);
  init_extension_entry (library, "glXQueryDrawable", QueryDrawable);
  init_extension_entry (library, "glXCreateNewContext", CreateNewContext);
  init_extension_entry (library, "glXMakeContextCurrent", MakeContextCurrent);
  init_extension_entry (library, "glXGetCurrentReadDrawable", GetCurrentReadDrawable);
  init_extension_entry (library, "glXGetCurrentDisplay", GetCurrentDisplay);
  init_extension_entry (library, "glXQueryContext", QueryContext);
  init_extension_entry (library, "glXSelectEvent", SelectEvent);
  init_extension_entry (library, "glXGetSelectedEvent", GetSelectedEvent);
  init_extension_entry (library, "glXGetProcAddress", GetProcAddress);
  init_extension_entry (library, "glXGetProcAddressARB", GetProcAddressARB);
  init_extension_entry (library, "glXCreateContextAttribsARB", CreateContextAttribsARB);
  init_extension_entry (library, "glXSwapIntervalSGI", SwapIntervalSGI);
  init_extension_entry (library, "glXGetVideoSyncSGI", GetVideoSyncSGI);
  init_extension_entry (library, "glXWaitVideoSyncSGI", WaitVideoSyncSGI);
  init_extension_entry (library, "glXMakeCurrentReadSGI", MakeCurrentReadSGI);
  init_extension_entry (library, "glXGetCurrentReadDrawableSGI", GetCurrentReadDrawableSGI);
  init_extension_entry (library, "glXCreateGLXVideoSourceSGIX", CreateGLXVideoSourceSGIX);
  init_extension_entry (library, "glXDestroyGLXVideoSourceSGIX", DestroyGLXVideoSourceSGIX);
  init_extension_entry (library, "glXGetCurrentDisplayEXT", GetCurrentDisplayEXT);
  init_extension_entry (library, "glXQueryContextInfoEXT", QueryContextInfoEXT);
  init_extension_entry (library, "glXGetContextIDEXT", GetContextIDEXT);
  init_extension_entry (library, "glXImportContextEXT", ImportContextEXT);
  init_extension_entry (library, "glXFreeContextEXT", FreeContextEXT);
  init_extension_entry (library, "glXGetFBConfigAttribSGIX", GetFBConfigAttribSGIX);
  init_extension_entry (library, "glXChooseFBConfigSGIX", ChooseFBConfigSGIX);
  init_extension_entry (library, "glXCreateGLXPixmapWithConfigSGIX", CreateGLXPixmapWithConfigSGIX);
  init_extension_entry (library, "glXCreateContextWithConfigSGIX", CreateContextWithConfigSGIX);
  init_extension_entry (library, "glXGetVisualFromFBConfigSGIX", GetVisualFromFBConfigSGIX);
  init_extension_entry (library, "glXGetFBConfigFromVisualSGIX", GetFBConfigFromVisualSGIX);
  init_extension_entry (library, "glXCreateGLXPbufferSGIX", CreateGLXPbufferSGIX);
  init_extension_entry (library, "glXDestroyGLXPbufferSGIX", DestroyGLXPbufferSGIX);
  init_extension_entry (library, "glXQueryGLXPbufferSGIX", QueryGLXPbufferSGIX);
  init_extension_entry (library, "glXSelectEventSGIX", SelectEventSGIX);
  init_extension_entry (library, "glXGetSelectedEventSGIX", GetSelectedEventSGIX);
  init_extension_entry (library, "glXCushionSGI", CushionSGI);
  init_extension_entry (library, "glXBindChannelToWindowSGIX", BindChannelToWindowSGIX);
  init_extension_entry (library, "glXChannelRectSGIX", ChannelRectSGIX);
  init_extension_entry (library, "glXQueryChannelRectSGIX", QueryChannelRectSGIX);
  init_extension_entry (library, "glXQueryChannelDeltasSGIX", QueryChannelDeltasSGIX);
  init_extension_entry (library, "glXChannelRectSyncSGIX", ChannelRectSyncSGIX);
  init_extension_entry (library, "glXAssociateDMPbufferSGIX", AssociateDMPbufferSGIX);
  init_extension_entry (library, "glXJoinSwapGroupSGIX", JoinSwapGroupSGIX);
  init_extension_entry (library, "glXBindSwapBarrierSGIX", BindSwapBarrierSGIX);
  init_extension_entry (library, "glXQueryMaxSwapBarriersSGIX", QueryMaxSwapBarriersSGIX);
  init_extension_entry (library, "glXGetTransparentIndexSUN", GetTransparentIndexSUN);
  init_extension_entry (library, "glXCopySubBufferMESA", CopySubBufferMESA);
  init_extension_entry (library, "glXCreateGLXPixmapMESA", CreateGLXPixmapMESA);
  init_extension_entry (library, "glXReleaseBuffersMESA", ReleaseBuffersMESA);
  init_extension_entry (library, "glXSet3DfxModeMESA", Set3DfxModeMESA);
  init_extension_entry (library, "glXGetSyncValuesOML", GetSyncValuesOML);
  init_extension_entry (library, "glXGetMscRateOML", GetMscRateOML);
  init_extension_entry (library, "glXSwapBuffersMscOML", SwapBuffersMscOML);
  init_extension_entry (library, "glXWaitForMscOML", WaitForMscOML);
  init_extension_entry (library, "glXWaitForSbcOML", WaitForSbcOML);
  init_extension_entry (library, "glXQueryHyperpipeNetworkSGIX", QueryHyperpipeNetworkSGIX);
  init_extension_entry (library, "glXHyperpipeConfigSGIX", HyperpipeConfigSGIX);
  init_extension_entry (library, "glXQueryHyperpipeConfigSGIX", QueryHyperpipeConfigSGIX);
  init_extension_entry (library, "glXDestroyHyperpipeConfigSGIX", DestroyHyperpipeConfigSGIX);
  init_extension_entry (library, "glXBindHyperpipeSGIX", BindHyperpipeSGIX);
  init_extension_entry (library, "glXQueryHyperpipeBestAttribSGIX", QueryHyperpipeBestAttribSGIX);
  init_extension_entry (library, "glXHyperpipeAttribSGIX", HyperpipeAttribSGIX);
  init_extension_entry (library, "glXQueryHyperpipeAttribSGIX", QueryHyperpipeAttribSGIX);
  init_extension_entry (library, "glXGetAGPOffsetMESA", GetAGPOffsetMESA);
  init_extension_entry (library, "glXBindTexImageEXT", BindTexImageEXT);
  init_extension_entry (library, "glXReleaseTexImageEXT", ReleaseTexImageEXT);
  init_extension_entry (library, "glXEnumerateVideoDevicesNV", EnumerateVideoDevicesNV);
  init_extension_entry (library, "glXBindVideoDeviceNV", BindVideoDeviceNV);
  init_extension_entry (library, "glXGetVideoDeviceNV", GetVideoDeviceNV);
  init_extension_entry (library, "glXReleaseVideoDeviceNV", ReleaseVideoDeviceNV);
  init_extension_entry (library, "glXBindVideoImageNV", BindVideoImageNV);
  init_extension_entry (library, "glXReleaseVideoImageNV", ReleaseVideoImageNV);
  init_extension_entry (library, "glXSendPbufferToVideoNV", SendPbufferToVideoNV);
  init_extension_entry (library, "glXGetVideoInfoNV", GetVideoInfoNV);
  init_extension_entry (library, "glXJoinSwapGroupNV", JoinSwapGroupNV);
  init_extension_entry (library, "glXBindSwapBarrierNV", BindSwapBarrierNV);
  init_extension_entry (library, "glXQuerySwapGroupNV", QuerySwapGroupNV);
  init_extension_entry (library, "glXQueryMaxSwapGroupsNV", QueryMaxSwapGroupsNV);
  init_extension_entry (library, "glXQueryFrameCountNV", QueryFrameCountNV);
  init_extension_entry (library, "glXResetFrameCountNV", ResetFrameCountNV);
  init_extension_entry (library, "glXBindVideoCaptureDeviceNV", BindVideoCaptureDeviceNV);
  init_extension_entry (library, "glXEnumerateVideoCaptureDevicesNV", EnumerateVideoCaptureDevicesNV);
  init_extension_entry (library, "glXLockVideoCaptureDeviceNV", LockVideoCaptureDeviceNV);
  init_extension_entry (library, "glXQueryVideoCaptureDeviceNV", QueryVideoCaptureDeviceNV);
  init_extension_entry (library, "glXReleaseVideoCaptureDeviceNV", ReleaseVideoCaptureDeviceNV);
  init_extension_entry (library, "glXSwapIntervalEXT", SwapIntervalEXT);
  init_extension_entry (library, "glXCopyImageSubDataNV", CopyImageSubDataNV);
}
