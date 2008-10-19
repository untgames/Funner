#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

template <class Fn>
inline void init_extension_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = (Fn)library.GetProcAddress (name, EntrySearch_Context | EntrySearch_NoThrow);
}

}

/*
    Инициализация точек входа WGL
*/

void WglExtensionEntries::Init (ILibrary& library)
{
    //обнуление адресов точек входа

  memset (this, 0, sizeof (WglExtensionEntries));

    //получение адресов точек входа расширений OpenGL

  init_extension_entry (library, "wglCreateBufferRegionARB", CreateBufferRegionARB);
  init_extension_entry (library, "wglDeleteBufferRegionARB", DeleteBufferRegionARB);
  init_extension_entry (library, "wglSaveBufferRegionARB", SaveBufferRegionARB);
  init_extension_entry (library, "wglRestoreBufferRegionARB", RestoreBufferRegionARB);
  init_extension_entry (library, "wglGetExtensionsStringARB", GetExtensionsStringARB);
  init_extension_entry (library, "wglGetPixelFormatAttribivARB", GetPixelFormatAttribivARB);
  init_extension_entry (library, "wglGetPixelFormatAttribfvARB", GetPixelFormatAttribfvARB);
  init_extension_entry (library, "wglChoosePixelFormatARB", ChoosePixelFormatARB);
  init_extension_entry (library, "wglMakeContextCurrentARB", MakeContextCurrentARB);
  init_extension_entry (library, "wglGetCurrentReadDCARB", GetCurrentReadDCARB);
  init_extension_entry (library, "wglCreatePbufferARB", CreatePbufferARB);
  init_extension_entry (library, "wglGetPbufferDCARB", GetPbufferDCARB);
  init_extension_entry (library, "wglReleasePbufferDCARB", ReleasePbufferDCARB);
  init_extension_entry (library, "wglDestroyPbufferARB", DestroyPbufferARB);
  init_extension_entry (library, "wglQueryPbufferARB", QueryPbufferARB);
  init_extension_entry (library, "wglBindTexImageARB", BindTexImageARB);
  init_extension_entry (library, "wglReleaseTexImageARB", ReleaseTexImageARB);
  init_extension_entry (library, "wglSetPbufferAttribARB", SetPbufferAttribARB);
  init_extension_entry (library, "wglCreateContextAttribsARB", CreateContextAttribsARB);
  init_extension_entry (library, "wglCreateDisplayColorTableEXT", CreateDisplayColorTableEXT);
  init_extension_entry (library, "wglLoadDisplayColorTableEXT", LoadDisplayColorTableEXT);
  init_extension_entry (library, "wglBindDisplayColorTableEXT", BindDisplayColorTableEXT);
  init_extension_entry (library, "wglDestroyDisplayColorTableEXT", DestroyDisplayColorTableEXT);
  init_extension_entry (library, "wglGetExtensionsStringEXT", GetExtensionsStringEXT);
  init_extension_entry (library, "wglMakeContextCurrentEXT", MakeContextCurrentEXT);
  init_extension_entry (library, "wglGetCurrentReadDCEXT", GetCurrentReadDCEXT);
  init_extension_entry (library, "wglCreatePbufferEXT", CreatePbufferEXT);
  init_extension_entry (library, "wglGetPbufferDCEXT", GetPbufferDCEXT);
  init_extension_entry (library, "wglReleasePbufferDCEXT", ReleasePbufferDCEXT);
  init_extension_entry (library, "wglDestroyPbufferEXT", DestroyPbufferEXT);
  init_extension_entry (library, "wglQueryPbufferEXT", QueryPbufferEXT);
  init_extension_entry (library, "wglGetPixelFormatAttribivEXT", GetPixelFormatAttribivEXT);
  init_extension_entry (library, "wglGetPixelFormatAttribfvEXT", GetPixelFormatAttribfvEXT);
  init_extension_entry (library, "wglChoosePixelFormatEXT", ChoosePixelFormatEXT);
  init_extension_entry (library, "wglSwapIntervalEXT", SwapIntervalEXT);
  init_extension_entry (library, "wglGetSwapIntervalEXT", GetSwapIntervalEXT);
  init_extension_entry (library, "wglAllocateMemoryNV", AllocateMemoryNV);
  init_extension_entry (library, "wglFreeMemoryNV", FreeMemoryNV);
  init_extension_entry (library, "wglGetSyncValuesOML", GetSyncValuesOML);
  init_extension_entry (library, "wglGetMscRateOML", GetMscRateOML);
  init_extension_entry (library, "wglSwapBuffersMscOML", SwapBuffersMscOML);
  init_extension_entry (library, "wglSwapLayerBuffersMscOML", SwapLayerBuffersMscOML);
  init_extension_entry (library, "wglWaitForMscOML", WaitForMscOML);
  init_extension_entry (library, "wglWaitForSbcOML", WaitForSbcOML);
  init_extension_entry (library, "wglGetDigitalVideoParametersI3D", GetDigitalVideoParametersI3D);
  init_extension_entry (library, "wglSetDigitalVideoParametersI3D", SetDigitalVideoParametersI3D);
  init_extension_entry (library, "wglGetGammaTableParametersI3D", GetGammaTableParametersI3D);
  init_extension_entry (library, "wglSetGammaTableParametersI3D", SetGammaTableParametersI3D);
  init_extension_entry (library, "wglGetGammaTableI3D", GetGammaTableI3D);
  init_extension_entry (library, "wglSetGammaTableI3D", SetGammaTableI3D);
  init_extension_entry (library, "wglEnableGenlockI3D", EnableGenlockI3D);
  init_extension_entry (library, "wglDisableGenlockI3D", DisableGenlockI3D);
  init_extension_entry (library, "wglIsEnabledGenlockI3D", IsEnabledGenlockI3D);
  init_extension_entry (library, "wglGenlockSourceI3D", GenlockSourceI3D);
  init_extension_entry (library, "wglGetGenlockSourceI3D", GetGenlockSourceI3D);
  init_extension_entry (library, "wglGenlockSourceEdgeI3D", GenlockSourceEdgeI3D);
  init_extension_entry (library, "wglGetGenlockSourceEdgeI3D", GetGenlockSourceEdgeI3D);
  init_extension_entry (library, "wglGenlockSampleRateI3D", GenlockSampleRateI3D);
  init_extension_entry (library, "wglGetGenlockSampleRateI3D", GetGenlockSampleRateI3D);
  init_extension_entry (library, "wglGenlockSourceDelayI3D", GenlockSourceDelayI3D);
  init_extension_entry (library, "wglGetGenlockSourceDelayI3D", GetGenlockSourceDelayI3D);
  init_extension_entry (library, "wglQueryGenlockMaxSourceDelayI3D", QueryGenlockMaxSourceDelayI3D);
  init_extension_entry (library, "wglCreateImageBufferI3D", CreateImageBufferI3D);
  init_extension_entry (library, "wglDestroyImageBufferI3D", DestroyImageBufferI3D);
  init_extension_entry (library, "wglAssociateImageBufferEventsI3D", AssociateImageBufferEventsI3D);
  init_extension_entry (library, "wglReleaseImageBufferEventsI3D", ReleaseImageBufferEventsI3D);
  init_extension_entry (library, "wglEnableFrameLockI3D", EnableFrameLockI3D);
  init_extension_entry (library, "wglDisableFrameLockI3D", DisableFrameLockI3D);
  init_extension_entry (library, "wglIsEnabledFrameLockI3D", IsEnabledFrameLockI3D);
  init_extension_entry (library, "wglQueryFrameLockMasterI3D", QueryFrameLockMasterI3D);
  init_extension_entry (library, "wglGetFrameUsageI3D", GetFrameUsageI3D);
  init_extension_entry (library, "wglBeginFrameTrackingI3D", BeginFrameTrackingI3D);
  init_extension_entry (library, "wglEndFrameTrackingI3D", EndFrameTrackingI3D);
  init_extension_entry (library, "wglQueryFrameTrackingI3D", QueryFrameTrackingI3D);
}
