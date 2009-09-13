#ifdef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

template <class Fn>
inline void init_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = reinterpret_cast<Fn> (library.GetProcAddress (name, EntrySearch_Library));
}

template <class Fn>
inline void init_extension_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = reinterpret_cast<Fn> (library.GetProcAddress (name, EntrySearch_Context | EntrySearch_NoThrow));
}

}

/*
    Инициализация точек входа OpenGL
*/

void GlEntries::Init (ILibrary& library)
{
    //обнуление адресов точек входа

  memset (this, 0, sizeof GlEntries);
  
    //получение адресов точек входа OpenGL ES 1.1

  init_entry (library, "glAlphaFunc", AlphaFunc);
  init_entry (library, "glClearColor", ClearColor);
  init_entry (library, "glClearDepthf", ClearDepthf);
  init_entry (library, "glClipPlanef", ClipPlanef);
  init_entry (library, "glColor4f", Color4f);
  init_entry (library, "glDepthRangef", DepthRangef);
  init_entry (library, "glFogf", Fogf);
  init_entry (library, "glFogfv", Fogfv);
  init_entry (library, "glFrustumf", Frustumf);
  init_entry (library, "glGetClipPlanef", GetClipPlanef);
  init_entry (library, "glGetFloatv", GetFloatv);
  init_entry (library, "glGetLightfv", GetLightfv);
  init_entry (library, "glGetMaterialfv", GetMaterialfv);
  init_entry (library, "glGetTexEnvfv", GetTexEnvfv);
  init_entry (library, "glGetTexParameterfv", GetTexParameterfv);
  init_entry (library, "glLightModelf", LightModelf);
  init_entry (library, "glLightModelfv", LightModelfv);
  init_entry (library, "glLightf", Lightf);
  init_entry (library, "glLightfv", Lightfv);
  init_entry (library, "glLineWidth", LineWidth);
  init_entry (library, "glLoadMatrixf", LoadMatrixf);
  init_entry (library, "glMaterialf", Materialf);
  init_entry (library, "glMaterialfv", Materialfv);
  init_entry (library, "glMultMatrixf", MultMatrixf);
  init_entry (library, "glMultiTexCoord4f", MultiTexCoord4f);
  init_entry (library, "glNormal3f", Normal3f);
  init_entry (library, "glOrthof", Orthof);
  init_entry (library, "glPointParameterf", PointParameterf);
  init_entry (library, "glPointParameterfv", PointParameterfv);
  init_entry (library, "glPointSize", PointSize);
  init_entry (library, "glPolygonOffset", PolygonOffset);
  init_entry (library, "glRotatef", Rotatef);
  init_entry (library, "glScalef", Scalef);
  init_entry (library, "glTexEnvf", TexEnvf);
  init_entry (library, "glTexEnvfv", TexEnvfv);
  init_entry (library, "glTexParameterf", TexParameterf);
  init_entry (library, "glTexParameterfv", TexParameterfv);
  init_entry (library, "glTranslatef", Translatef);
  init_entry (library, "glActiveTexture", ActiveTexture);
  init_entry (library, "glAlphaFuncx", AlphaFuncx);
  init_entry (library, "glBindBuffer", BindBuffer);
  init_entry (library, "glBindTexture", BindTexture);
  init_entry (library, "glBlendFunc", BlendFunc);
  init_entry (library, "glBufferData", BufferData);
  init_entry (library, "glBufferSubData", BufferSubData);
  init_entry (library, "glClear", Clear);
  init_entry (library, "glClearColorx", ClearColorx);
  init_entry (library, "glClearDepthx", ClearDepthx);
  init_entry (library, "glClearStencil", ClearStencil);
  init_entry (library, "glClientActiveTexture", ClientActiveTexture);
  init_entry (library, "glClipPlanex", ClipPlanex);
  init_entry (library, "glColor4ub", Color4ub);
  init_entry (library, "glColor4x", Color4x);
  init_entry (library, "glColorMask", ColorMask);
  init_entry (library, "glColorPointer", ColorPointer);
  init_entry (library, "glCompressedTexImage2D", CompressedTexImage2D);
  init_entry (library, "glCompressedTexSubImage2D", CompressedTexSubImage2D);
  init_entry (library, "glCopyTexImage2D", CopyTexImage2D);
  init_entry (library, "glCopyTexSubImage2D", CopyTexSubImage2D);
  init_entry (library, "glCullFace", CullFace);
  init_entry (library, "glDeleteBuffers", DeleteBuffers);
  init_entry (library, "glDeleteTextures", DeleteTextures);
  init_entry (library, "glDepthFunc", DepthFunc);
  init_entry (library, "glDepthMask", DepthMask);
  init_entry (library, "glDepthRangex", DepthRangex);
  init_entry (library, "glDisable", Disable);
  init_entry (library, "glDisableClientState", DisableClientState);
  init_entry (library, "glDrawArrays", DrawArrays);
  init_entry (library, "glDrawElements", DrawElements);
  init_entry (library, "glEnable", Enable);
  init_entry (library, "glEnableClientState", EnableClientState);
  init_entry (library, "glFinish", Finish);
  init_entry (library, "glFlush", Flush);
  init_entry (library, "glFogx", Fogx);
  init_entry (library, "glFogxv", Fogxv);
  init_entry (library, "glFrontFace", FrontFace);
  init_entry (library, "glFrustumx", Frustumx);
  init_entry (library, "glGetBooleanv", GetBooleanv);
  init_entry (library, "glGetBufferParameteriv", GetBufferParameteriv);
  init_entry (library, "glGetClipPlanex", GetClipPlanex);
  init_entry (library, "glGenBuffers", GenBuffers);
  init_entry (library, "glGenTextures", GenTextures);
  init_entry (library, "glGetError", GetError);
  init_entry (library, "glGetFixedv", GetFixedv);
  init_entry (library, "glGetIntegerv", GetIntegerv);
  init_entry (library, "glGetLightxv", GetLightxv);
  init_entry (library, "glGetMaterialxv", GetMaterialxv);
  init_entry (library, "glGetPointerv", GetPointerv);
  init_entry (library, "glGetString", GetString);
  init_entry (library, "glGetTexEnviv", GetTexEnviv);
  init_entry (library, "glGetTexEnvxv", GetTexEnvxv);
  init_entry (library, "glGetTexParameteriv", GetTexParameteriv);
  init_entry (library, "glGetTexParameterxv", GetTexParameterxv);
  init_entry (library, "glHint", Hint);
  init_entry (library, "glIsBuffer", IsBuffer);
  init_entry (library, "glIsEnabled", IsEnabled);
  init_entry (library, "glIsTexture", IsTexture);
  init_entry (library, "glLightModelx", LightModelx);
  init_entry (library, "glLightModelxv", LightModelxv);
  init_entry (library, "glLightx", Lightx);
  init_entry (library, "glLightxv", Lightxv);
  init_entry (library, "glLineWidthx", LineWidthx);
  init_entry (library, "glLoadIdentity", LoadIdentity);
  init_entry (library, "glLoadMatrixx", LoadMatrixx);
  init_entry (library, "glLogicOp", LogicOp);
  init_entry (library, "glMaterialx", Materialx);
  init_entry (library, "glMaterialxv", Materialxv);
  init_entry (library, "glMatrixMode", MatrixMode);
  init_entry (library, "glMultMatrixx", MultMatrixx);
  init_entry (library, "glMultiTexCoord4x", MultiTexCoord4x);
  init_entry (library, "glNormal3x", Normal3x);
  init_entry (library, "glNormalPointer", NormalPointer);
  init_entry (library, "glOrthox", Orthox);
  init_entry (library, "glPixelStorei", PixelStorei);
  init_entry (library, "glPointParameterx", PointParameterx);
  init_entry (library, "glPointParameterxv", PointParameterxv);
  init_entry (library, "glPointSizex", PointSizex);
  init_entry (library, "glPolygonOffsetx", PolygonOffsetx);
  init_entry (library, "glPopMatrix", PopMatrix);
  init_entry (library, "glPushMatrix", PushMatrix);
  init_entry (library, "glReadPixels", ReadPixels);
  init_entry (library, "glRotatex", Rotatex);
  init_entry (library, "glSampleCoverage", SampleCoverage);
  init_entry (library, "glSampleCoveragex", SampleCoveragex);
  init_entry (library, "glScalex", Scalex);
  init_entry (library, "glScissor", Scissor);
  init_entry (library, "glShadeModel", ShadeModel);
  init_entry (library, "glStencilFunc", StencilFunc);
  init_entry (library, "glStencilMask", StencilMask);
  init_entry (library, "glStencilOp", StencilOp);
  init_entry (library, "glTexCoordPointer", TexCoordPointer);
  init_entry (library, "glTexEnvi", TexEnvi);
  init_entry (library, "glTexEnvx", TexEnvx);
  init_entry (library, "glTexEnviv", TexEnviv);
  init_entry (library, "glTexEnvxv", TexEnvxv);
  init_entry (library, "glTexImage2D", TexImage2D);
  init_entry (library, "glTexParameteri", TexParameteri);
  init_entry (library, "glTexParameterx", TexParameterx);
  init_entry (library, "glTexParameteriv", TexParameteriv);
  init_entry (library, "glTexParameterxv", TexParameterxv);
  init_entry (library, "glTexSubImage2D", TexSubImage2D);
  init_entry (library, "glTranslatex", Translatex);
  init_entry (library, "glVertexPointer", VertexPointer);
  init_entry (library, "glViewport", Viewport);
  init_entry (library, "glPointSizePointerOES", PointSizePointerOES);
  
    //получение адресов точек входа расширений OpenGL

  init_extension_entry (library, "glBlendEquationSeparateOES", BlendEquationSeparateOES);
  init_extension_entry (library, "glBlendFuncSeparateOES", BlendFuncSeparateOES);
  init_extension_entry (library, "glBlendEquationOES", BlendEquationOES);
  init_extension_entry (library, "glDrawTexsOES", DrawTexsOES);
  init_extension_entry (library, "glDrawTexiOES", DrawTexiOES);
  init_extension_entry (library, "glDrawTexxOES", DrawTexxOES);
  init_extension_entry (library, "glDrawTexsvOES", DrawTexsvOES);
  init_extension_entry (library, "glDrawTexivOES", DrawTexivOES);
  init_extension_entry (library, "glDrawTexxvOES", DrawTexxvOES);
  init_extension_entry (library, "glDrawTexfOES", DrawTexfOES);
  init_extension_entry (library, "glDrawTexfvOES", DrawTexfvOES);
  init_extension_entry (library, "glEGLImageTargetTexture2DOES", EGLImageTargetTexture2DOES);
  init_extension_entry (library, "glEGLImageTargetRenderbufferStorageOES", EGLImageTargetRenderbufferStorageOES);
  init_extension_entry (library, "glAlphaFuncxOES", AlphaFuncxOES);
  init_extension_entry (library, "glClearColorxOES", ClearColorxOES);
  init_extension_entry (library, "glClearDepthxOES", ClearDepthxOES);
  init_extension_entry (library, "glClipPlanexOES", ClipPlanexOES);
  init_extension_entry (library, "glColor4xOES", Color4xOES);
  init_extension_entry (library, "glDepthRangexOES", DepthRangexOES);
  init_extension_entry (library, "glFogxOES", FogxOES);
  init_extension_entry (library, "glFogxvOES", FogxvOES);
  init_extension_entry (library, "glFrustumxOES", FrustumxOES);
  init_extension_entry (library, "glGetClipPlanexOES", GetClipPlanexOES);
  init_extension_entry (library, "glGetFixedvOES", GetFixedvOES);
  init_extension_entry (library, "glGetLightxvOES", GetLightxvOES);
  init_extension_entry (library, "glGetMaterialxvOES", GetMaterialxvOES);
  init_extension_entry (library, "glGetTexEnvxvOES", GetTexEnvxvOES);
  init_extension_entry (library, "glGetTexParameterxvOES", GetTexParameterxvOES);
  init_extension_entry (library, "glLightModelxOES", LightModelxOES);
  init_extension_entry (library, "glLightModelxvOES", LightModelxvOES);
  init_extension_entry (library, "glLightxOES", LightxOES);
  init_extension_entry (library, "glLightxvOES", LightxvOES);
  init_extension_entry (library, "glLineWidthxOES", LineWidthxOES);
  init_extension_entry (library, "glLoadMatrixxOES", LoadMatrixxOES);
  init_extension_entry (library, "glMaterialxOES", MaterialxOES);
  init_extension_entry (library, "glMaterialxvOES", MaterialxvOES);
  init_extension_entry (library, "glMultMatrixxOES", MultMatrixxOES);
  init_extension_entry (library, "glMultiTexCoord4xOES", MultiTexCoord4xOES);
  init_extension_entry (library, "glNormal3xOES", Normal3xOES);
  init_extension_entry (library, "glOrthoxOES", OrthoxOES);
  init_extension_entry (library, "glPointParameterxOES", PointParameterxOES);
  init_extension_entry (library, "glPointParameterxvOES", PointParameterxvOES);
  init_extension_entry (library, "glPointSizexOES", PointSizexOES);
  init_extension_entry (library, "glPolygonOffsetxOES", PolygonOffsetxOES);
  init_extension_entry (library, "glRotatexOES", RotatexOES);
  init_extension_entry (library, "glSampleCoveragexOES", SampleCoveragexOES);
  init_extension_entry (library, "glScalexOES", ScalexOES);
  init_extension_entry (library, "glTexEnvxOES", TexEnvxOES);
  init_extension_entry (library, "glTexEnvxvOES", TexEnvxvOES);
  init_extension_entry (library, "glTexParameterxOES", TexParameterxOES);
  init_extension_entry (library, "glTexParameterxvOES", TexParameterxvOES);
  init_extension_entry (library, "glTranslatexOES", TranslatexOES);
  init_extension_entry (library, "glIsRenderbufferOES", IsRenderbufferOES);
  init_extension_entry (library, "glBindRenderbufferOES", BindRenderbufferOES);
  init_extension_entry (library, "glDeleteRenderbuffersOES", DeleteRenderbuffersOES);
  init_extension_entry (library, "glGenRenderbuffersOES", GenRenderbuffersOES);
  init_extension_entry (library, "glRenderbufferStorageOES", RenderbufferStorageOES);
  init_extension_entry (library, "glGetRenderbufferParameterivOES", GetRenderbufferParameterivOES);
  init_extension_entry (library, "glIsFramebufferOES", IsFramebufferOES);
  init_extension_entry (library, "glBindFramebufferOES", BindFramebufferOES);
  init_extension_entry (library, "glDeleteFramebuffersOES", DeleteFramebuffersOES);
  init_extension_entry (library, "glGenFramebuffersOES", GenFramebuffersOES);
  init_extension_entry (library, "glCheckFramebufferStatusOES", CheckFramebufferStatusOES);
  init_extension_entry (library, "glFramebufferRenderbufferOES", FramebufferRenderbufferOES);
  init_extension_entry (library, "glFramebufferTexture2DOES", FramebufferTexture2DOES);
  init_extension_entry (library, "glGetFramebufferAttachmentParameterivOES", GetFramebufferAttachmentParameterivOES);
  init_extension_entry (library, "glGenerateMipmapOES", GenerateMipmapOES);
  init_extension_entry (library, "glMapBufferOES", MapBufferOES);
  init_extension_entry (library, "glUnmapBufferOES", UnmapBufferOES);
  init_extension_entry (library, "glGetBufferPointervOES", GetBufferPointervOES);
  init_extension_entry (library, "glCurrentPaletteMatrixOES", CurrentPaletteMatrixOES);
  init_extension_entry (library, "glLoadPaletteFromModelViewMatrixOES", LoadPaletteFromModelViewMatrixOES);
  init_extension_entry (library, "glMatrixIndexPointerOES", MatrixIndexPointerOES);
  init_extension_entry (library, "glWeightPointerOES", WeightPointerOES);
  init_extension_entry (library, "glQueryMatrixxOES", QueryMatrixxOES);
  init_extension_entry (library, "glDepthRangefOES", DepthRangefOES);
  init_extension_entry (library, "glFrustumfOES", FrustumfOES);
  init_extension_entry (library, "glOrthofOES", OrthofOES);
  init_extension_entry (library, "glClipPlanefOES", ClipPlanefOES);
  init_extension_entry (library, "glGetClipPlanefOES", GetClipPlanefOES);
  init_extension_entry (library, "glClearDepthfOES", ClearDepthfOES);
  init_extension_entry (library, "glTexGenfOES", TexGenfOES);
  init_extension_entry (library, "glTexGenfvOES", TexGenfvOES);
  init_extension_entry (library, "glTexGeniOES", TexGeniOES);
  init_extension_entry (library, "glTexGenivOES", TexGenivOES);
  init_extension_entry (library, "glTexGenxOES", TexGenxOES);
  init_extension_entry (library, "glTexGenxvOES", TexGenxvOES);
  init_extension_entry (library, "glGetTexGenfvOES", GetTexGenfvOES);
  init_extension_entry (library, "glGetTexGenivOES", GetTexGenivOES);
  init_extension_entry (library, "glGetTexGenxvOES", GetTexGenxvOES);
  init_extension_entry (library, "glClipPlanefIMG", ClipPlanefIMG);
  init_extension_entry (library, "glClipPlanexIMG", ClipPlanexIMG);
  init_extension_entry (library, "glDeleteFencesNV", DeleteFencesNV);
  init_extension_entry (library, "glGenFencesNV", GenFencesNV);
  init_extension_entry (library, "glIsFenceNV", IsFenceNV);
  init_extension_entry (library, "glTestFenceNV", TestFenceNV);
  init_extension_entry (library, "glGetFenceivNV", GetFenceivNV);
  init_extension_entry (library, "glFinishFenceNV", FinishFenceNV);
  init_extension_entry (library, "glSetFenceNV", SetFenceNV);
  init_extension_entry (library, "glGetDriverControlsQCOM", GetDriverControlsQCOM);
  init_extension_entry (library, "glGetDriverControlStringQCOM", GetDriverControlStringQCOM);
  init_extension_entry (library, "glEnableDriverControlQCOM", EnableDriverControlQCOM);
  init_extension_entry (library, "glDisableDriverControlQCOM", DisableDriverControlQCOM);
}

#endif
