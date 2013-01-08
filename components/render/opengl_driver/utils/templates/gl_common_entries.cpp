#ifndef OPENGL_ES_SUPPORT

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
    ������������� ����� ����� OpenGL
*/

void GlEntries::Init (ILibrary& library)
{
    //��������� ������� ����� �����

  memset (this, 0, sizeof (GlEntries));
  
    //��������� ������� ����� ����� OpenGL 1.1

  init_entry (library, "glAccum", Accum);
  init_entry (library, "glAlphaFunc", AlphaFunc);
  init_entry (library, "glAreTexturesResident", AreTexturesResident);
  init_entry (library, "glArrayElement", ArrayElement);
  init_entry (library, "glBegin", Begin);
  init_entry (library, "glBindTexture", BindTexture);
  init_entry (library, "glBitmap", Bitmap);
  init_entry (library, "glBlendFunc", BlendFunc);
  init_entry (library, "glCallList", CallList);
  init_entry (library, "glCallLists", CallLists);
  init_entry (library, "glClear", Clear);
  init_entry (library, "glClearAccum", ClearAccum);
  init_entry (library, "glClearColor", ClearColor);
  init_entry (library, "glClearDepth", ClearDepth);
  init_entry (library, "glClearIndex", ClearIndex);
  init_entry (library, "glClearStencil", ClearStencil);
  init_entry (library, "glClipPlane", ClipPlane);
  init_entry (library, "glColor3b", Color3b);
  init_entry (library, "glColor3bv", Color3bv);
  init_entry (library, "glColor3d", Color3d);
  init_entry (library, "glColor3dv", Color3dv);
  init_entry (library, "glColor3f", Color3f);
  init_entry (library, "glColor3fv", Color3fv);
  init_entry (library, "glColor3i", Color3i);
  init_entry (library, "glColor3iv", Color3iv);
  init_entry (library, "glColor3s", Color3s);
  init_entry (library, "glColor3sv", Color3sv);
  init_entry (library, "glColor3ub", Color3ub);
  init_entry (library, "glColor3ubv", Color3ubv);
  init_entry (library, "glColor3ui", Color3ui);
  init_entry (library, "glColor3uiv", Color3uiv);
  init_entry (library, "glColor3us", Color3us);
  init_entry (library, "glColor3usv", Color3usv);
  init_entry (library, "glColor4b", Color4b);
  init_entry (library, "glColor4bv", Color4bv);
  init_entry (library, "glColor4d", Color4d);
  init_entry (library, "glColor4dv", Color4dv);
  init_entry (library, "glColor4f", Color4f);
  init_entry (library, "glColor4fv", Color4fv);
  init_entry (library, "glColor4i", Color4i);
  init_entry (library, "glColor4iv", Color4iv);
  init_entry (library, "glColor4s", Color4s);
  init_entry (library, "glColor4sv", Color4sv);
  init_entry (library, "glColor4ub", Color4ub);
  init_entry (library, "glColor4ubv", Color4ubv);
  init_entry (library, "glColor4ui", Color4ui);
  init_entry (library, "glColor4uiv", Color4uiv);
  init_entry (library, "glColor4us", Color4us);
  init_entry (library, "glColor4usv", Color4usv);
  init_entry (library, "glColorMask", ColorMask);
  init_entry (library, "glColorMaterial", ColorMaterial);
  init_entry (library, "glColorPointer", ColorPointer);
  init_entry (library, "glCopyPixels", CopyPixels);
  init_entry (library, "glCopyTexImage1D", CopyTexImage1D);
  init_entry (library, "glCopyTexImage2D", CopyTexImage2D);
  init_entry (library, "glCopyTexSubImage1D", CopyTexSubImage1D);
  init_entry (library, "glCopyTexSubImage2D", CopyTexSubImage2D);
  init_entry (library, "glCullFace", CullFace);
  init_entry (library, "glDeleteLists", DeleteLists);
  init_entry (library, "glDeleteTextures", DeleteTextures);
  init_entry (library, "glDepthFunc", DepthFunc);
  init_entry (library, "glDepthMask", DepthMask);
  init_entry (library, "glDepthRange", DepthRange);
  init_entry (library, "glDisable", Disable);
  init_entry (library, "glDisableClientState", DisableClientState);
  init_entry (library, "glDrawArrays", DrawArrays);
  init_entry (library, "glDrawBuffer", DrawBuffer);
  init_entry (library, "glDrawElements", DrawElements);
  init_entry (library, "glDrawPixels", DrawPixels);
  init_entry (library, "glEdgeFlag", EdgeFlag);
  init_entry (library, "glEdgeFlagPointer", EdgeFlagPointer);
  init_entry (library, "glEdgeFlagv", EdgeFlagv);
  init_entry (library, "glEnable", Enable);
  init_entry (library, "glEnableClientState", EnableClientState);
  init_entry (library, "glEnd", End);
  init_entry (library, "glEndList", EndList);
  init_entry (library, "glEvalCoord1d", EvalCoord1d);
  init_entry (library, "glEvalCoord1dv", EvalCoord1dv);
  init_entry (library, "glEvalCoord1f", EvalCoord1f);
  init_entry (library, "glEvalCoord1fv", EvalCoord1fv);
  init_entry (library, "glEvalCoord2d", EvalCoord2d);
  init_entry (library, "glEvalCoord2dv", EvalCoord2dv);
  init_entry (library, "glEvalCoord2f", EvalCoord2f);
  init_entry (library, "glEvalCoord2fv", EvalCoord2fv);
  init_entry (library, "glEvalMesh1", EvalMesh1);
  init_entry (library, "glEvalMesh2", EvalMesh2);
  init_entry (library, "glEvalPoint1", EvalPoint1);
  init_entry (library, "glEvalPoint2", EvalPoint2);
  init_entry (library, "glFeedbackBuffer", FeedbackBuffer);
  init_entry (library, "glFinish", Finish);
  init_entry (library, "glFlush", Flush);
  init_entry (library, "glFogf", Fogf);
  init_entry (library, "glFogfv", Fogfv);
  init_entry (library, "glFogi", Fogi);
  init_entry (library, "glFogiv", Fogiv);
  init_entry (library, "glFrontFace", FrontFace);
  init_entry (library, "glFrustum", Frustum);
  init_entry (library, "glGenLists", GenLists);
  init_entry (library, "glGenTextures", GenTextures);
  init_entry (library, "glGetBooleanv", GetBooleanv);
  init_entry (library, "glGetClipPlane", GetClipPlane);
  init_entry (library, "glGetDoublev", GetDoublev);
  init_entry (library, "glGetError", GetError);
  init_entry (library, "glGetFloatv", GetFloatv);
  init_entry (library, "glGetIntegerv", GetIntegerv);
  init_entry (library, "glGetLightfv", GetLightfv);
  init_entry (library, "glGetLightiv", GetLightiv);
  init_entry (library, "glGetMapdv", GetMapdv);
  init_entry (library, "glGetMapfv", GetMapfv);
  init_entry (library, "glGetMapiv", GetMapiv);
  init_entry (library, "glGetMaterialfv", GetMaterialfv);
  init_entry (library, "glGetMaterialiv", GetMaterialiv);
  init_entry (library, "glGetPixelMapfv", GetPixelMapfv);
  init_entry (library, "glGetPixelMapuiv", GetPixelMapuiv);
  init_entry (library, "glGetPixelMapusv", GetPixelMapusv);
  init_entry (library, "glGetPointerv", GetPointerv);
  init_entry (library, "glGetPolygonStipple", GetPolygonStipple);
  init_entry (library, "glGetString", GetString);
  init_entry (library, "glGetTexEnvfv", GetTexEnvfv);
  init_entry (library, "glGetTexEnviv", GetTexEnviv);
  init_entry (library, "glGetTexGendv", GetTexGendv);
  init_entry (library, "glGetTexGenfv", GetTexGenfv);
  init_entry (library, "glGetTexGeniv", GetTexGeniv);
  init_entry (library, "glGetTexImage", GetTexImage);
  init_entry (library, "glGetTexLevelParameterfv", GetTexLevelParameterfv);
  init_entry (library, "glGetTexLevelParameteriv", GetTexLevelParameteriv);
  init_entry (library, "glGetTexParameterfv", GetTexParameterfv);
  init_entry (library, "glGetTexParameteriv", GetTexParameteriv);
  init_entry (library, "glHint", Hint);
  init_entry (library, "glIndexMask", IndexMask);
  init_entry (library, "glIndexPointer", IndexPointer);
  init_entry (library, "glIndexd", Indexd);
  init_entry (library, "glIndexdv", Indexdv);
  init_entry (library, "glIndexf", Indexf);
  init_entry (library, "glIndexfv", Indexfv);
  init_entry (library, "glIndexi", Indexi);
  init_entry (library, "glIndexiv", Indexiv);
  init_entry (library, "glIndexs", Indexs);
  init_entry (library, "glIndexsv", Indexsv);
  init_entry (library, "glIndexub", Indexub);
  init_entry (library, "glIndexubv", Indexubv);
  init_entry (library, "glInitNames", InitNames);
  init_entry (library, "glInterleavedArrays", InterleavedArrays);
  init_entry (library, "glIsEnabled", IsEnabled);
  init_entry (library, "glIsList", IsList);
  init_entry (library, "glIsTexture", IsTexture);
  init_entry (library, "glLightModelf", LightModelf);
  init_entry (library, "glLightModelfv", LightModelfv);
  init_entry (library, "glLightModeli", LightModeli);
  init_entry (library, "glLightModeliv", LightModeliv);
  init_entry (library, "glLightf", Lightf);
  init_entry (library, "glLightfv", Lightfv);
  init_entry (library, "glLighti", Lighti);
  init_entry (library, "glLightiv", Lightiv);
  init_entry (library, "glLineStipple", LineStipple);
  init_entry (library, "glLineWidth", LineWidth);
  init_entry (library, "glListBase", ListBase);
  init_entry (library, "glLoadIdentity", LoadIdentity);
  init_entry (library, "glLoadMatrixd", LoadMatrixd);
  init_entry (library, "glLoadMatrixf", LoadMatrixf);
  init_entry (library, "glLoadName", LoadName);
  init_entry (library, "glLogicOp", LogicOp);
  init_entry (library, "glMap1d", Map1d);
  init_entry (library, "glMap1f", Map1f);
  init_entry (library, "glMap2d", Map2d);
  init_entry (library, "glMap2f", Map2f);
  init_entry (library, "glMapGrid1d", MapGrid1d);
  init_entry (library, "glMapGrid1f", MapGrid1f);
  init_entry (library, "glMapGrid2d", MapGrid2d);
  init_entry (library, "glMapGrid2f", MapGrid2f);
  init_entry (library, "glMaterialf", Materialf);
  init_entry (library, "glMaterialfv", Materialfv);
  init_entry (library, "glMateriali", Materiali);
  init_entry (library, "glMaterialiv", Materialiv);
  init_entry (library, "glMatrixMode", MatrixMode);
  init_entry (library, "glMultMatrixd", MultMatrixd);
  init_entry (library, "glMultMatrixf", MultMatrixf);
  init_entry (library, "glNewList", NewList);
  init_entry (library, "glNormal3b", Normal3b);
  init_entry (library, "glNormal3bv", Normal3bv);
  init_entry (library, "glNormal3d", Normal3d);
  init_entry (library, "glNormal3dv", Normal3dv);
  init_entry (library, "glNormal3f", Normal3f);
  init_entry (library, "glNormal3fv", Normal3fv);
  init_entry (library, "glNormal3i", Normal3i);
  init_entry (library, "glNormal3iv", Normal3iv);
  init_entry (library, "glNormal3s", Normal3s);
  init_entry (library, "glNormal3sv", Normal3sv);
  init_entry (library, "glNormalPointer", NormalPointer);
  init_entry (library, "glOrtho", Ortho);
  init_entry (library, "glPassThrough", PassThrough);
  init_entry (library, "glPixelMapfv", PixelMapfv);
  init_entry (library, "glPixelMapuiv", PixelMapuiv);
  init_entry (library, "glPixelMapusv", PixelMapusv);
  init_entry (library, "glPixelStoref", PixelStoref);
  init_entry (library, "glPixelStorei", PixelStorei);
  init_entry (library, "glPixelTransferf", PixelTransferf);
  init_entry (library, "glPixelTransferi", PixelTransferi);
  init_entry (library, "glPixelZoom", PixelZoom);
  init_entry (library, "glPointSize", PointSize);
  init_entry (library, "glPolygonMode", PolygonMode);
  init_entry (library, "glPolygonOffset", PolygonOffset);
  init_entry (library, "glPolygonStipple", PolygonStipple);
  init_entry (library, "glPopAttrib", PopAttrib);
  init_entry (library, "glPopClientAttrib", PopClientAttrib);
  init_entry (library, "glPopMatrix", PopMatrix);
  init_entry (library, "glPopName", PopName);
  init_entry (library, "glPrioritizeTextures", PrioritizeTextures);
  init_entry (library, "glPushAttrib", PushAttrib);
  init_entry (library, "glPushClientAttrib", PushClientAttrib);
  init_entry (library, "glPushMatrix", PushMatrix);
  init_entry (library, "glPushName", PushName);
  init_entry (library, "glRasterPos2d", RasterPos2d);
  init_entry (library, "glRasterPos2dv", RasterPos2dv);
  init_entry (library, "glRasterPos2f", RasterPos2f);
  init_entry (library, "glRasterPos2fv", RasterPos2fv);
  init_entry (library, "glRasterPos2i", RasterPos2i);
  init_entry (library, "glRasterPos2iv", RasterPos2iv);
  init_entry (library, "glRasterPos2s", RasterPos2s);
  init_entry (library, "glRasterPos2sv", RasterPos2sv);
  init_entry (library, "glRasterPos3d", RasterPos3d);
  init_entry (library, "glRasterPos3dv", RasterPos3dv);
  init_entry (library, "glRasterPos3f", RasterPos3f);
  init_entry (library, "glRasterPos3fv", RasterPos3fv);
  init_entry (library, "glRasterPos3i", RasterPos3i);
  init_entry (library, "glRasterPos3iv", RasterPos3iv);
  init_entry (library, "glRasterPos3s", RasterPos3s);
  init_entry (library, "glRasterPos3sv", RasterPos3sv);
  init_entry (library, "glRasterPos4d", RasterPos4d);
  init_entry (library, "glRasterPos4dv", RasterPos4dv);
  init_entry (library, "glRasterPos4f", RasterPos4f);
  init_entry (library, "glRasterPos4fv", RasterPos4fv);
  init_entry (library, "glRasterPos4i", RasterPos4i);
  init_entry (library, "glRasterPos4iv", RasterPos4iv);
  init_entry (library, "glRasterPos4s", RasterPos4s);
  init_entry (library, "glRasterPos4sv", RasterPos4sv);
  init_entry (library, "glReadBuffer", ReadBuffer);
  init_entry (library, "glReadPixels", ReadPixels);
  init_entry (library, "glRectd", Rectd);
  init_entry (library, "glRectdv", Rectdv);
  init_entry (library, "glRectf", Rectf);
  init_entry (library, "glRectfv", Rectfv);
  init_entry (library, "glRecti", Recti);
  init_entry (library, "glRectiv", Rectiv);
  init_entry (library, "glRects", Rects);
  init_entry (library, "glRectsv", Rectsv);
  init_entry (library, "glRenderMode", RenderMode);
  init_entry (library, "glRotated", Rotated);
  init_entry (library, "glRotatef", Rotatef);
  init_entry (library, "glScaled", Scaled);
  init_entry (library, "glScalef", Scalef);
  init_entry (library, "glScissor", Scissor);
  init_entry (library, "glSelectBuffer", SelectBuffer);
  init_entry (library, "glShadeModel", ShadeModel);
  init_entry (library, "glStencilFunc", StencilFunc);
  init_entry (library, "glStencilMask", StencilMask);
  init_entry (library, "glStencilOp", StencilOp);
  init_entry (library, "glTexCoord1d", TexCoord1d);
  init_entry (library, "glTexCoord1dv", TexCoord1dv);
  init_entry (library, "glTexCoord1f", TexCoord1f);
  init_entry (library, "glTexCoord1fv", TexCoord1fv);
  init_entry (library, "glTexCoord1i", TexCoord1i);
  init_entry (library, "glTexCoord1iv", TexCoord1iv);
  init_entry (library, "glTexCoord1s", TexCoord1s);
  init_entry (library, "glTexCoord1sv", TexCoord1sv);
  init_entry (library, "glTexCoord2d", TexCoord2d);
  init_entry (library, "glTexCoord2dv", TexCoord2dv);
  init_entry (library, "glTexCoord2f", TexCoord2f);
  init_entry (library, "glTexCoord2fv", TexCoord2fv);
  init_entry (library, "glTexCoord2i", TexCoord2i);
  init_entry (library, "glTexCoord2iv", TexCoord2iv);
  init_entry (library, "glTexCoord2s", TexCoord2s);
  init_entry (library, "glTexCoord2sv", TexCoord2sv);
  init_entry (library, "glTexCoord3d", TexCoord3d);
  init_entry (library, "glTexCoord3dv", TexCoord3dv);
  init_entry (library, "glTexCoord3f", TexCoord3f);
  init_entry (library, "glTexCoord3fv", TexCoord3fv);
  init_entry (library, "glTexCoord3i", TexCoord3i);
  init_entry (library, "glTexCoord3iv", TexCoord3iv);
  init_entry (library, "glTexCoord3s", TexCoord3s);
  init_entry (library, "glTexCoord3sv", TexCoord3sv);
  init_entry (library, "glTexCoord4d", TexCoord4d);
  init_entry (library, "glTexCoord4dv", TexCoord4dv);
  init_entry (library, "glTexCoord4f", TexCoord4f);
  init_entry (library, "glTexCoord4fv", TexCoord4fv);
  init_entry (library, "glTexCoord4i", TexCoord4i);
  init_entry (library, "glTexCoord4iv", TexCoord4iv);
  init_entry (library, "glTexCoord4s", TexCoord4s);
  init_entry (library, "glTexCoord4sv", TexCoord4sv);
  init_entry (library, "glTexCoordPointer", TexCoordPointer);
  init_entry (library, "glTexEnvf", TexEnvf);
  init_entry (library, "glTexEnvfv", TexEnvfv);
  init_entry (library, "glTexEnvi", TexEnvi);
  init_entry (library, "glTexEnviv", TexEnviv);
  init_entry (library, "glTexGend", TexGend);
  init_entry (library, "glTexGendv", TexGendv);
  init_entry (library, "glTexGenf", TexGenf);
  init_entry (library, "glTexGenfv", TexGenfv);
  init_entry (library, "glTexGeni", TexGeni);
  init_entry (library, "glTexGeniv", TexGeniv);
  init_entry (library, "glTexImage1D", TexImage1D);
  init_entry (library, "glTexImage2D", TexImage2D);
  init_entry (library, "glTexParameterf", TexParameterf);
  init_entry (library, "glTexParameterfv", TexParameterfv);
  init_entry (library, "glTexParameteri", TexParameteri);
  init_entry (library, "glTexParameteriv", TexParameteriv);
  init_entry (library, "glTexSubImage1D", TexSubImage1D);
  init_entry (library, "glTexSubImage2D", TexSubImage2D);
  init_entry (library, "glTranslated", Translated);
  init_entry (library, "glTranslatef", Translatef);
  init_entry (library, "glVertex2d", Vertex2d);
  init_entry (library, "glVertex2dv", Vertex2dv);
  init_entry (library, "glVertex2f", Vertex2f);
  init_entry (library, "glVertex2fv", Vertex2fv);
  init_entry (library, "glVertex2i", Vertex2i);
  init_entry (library, "glVertex2iv", Vertex2iv);
  init_entry (library, "glVertex2s", Vertex2s);
  init_entry (library, "glVertex2sv", Vertex2sv);
  init_entry (library, "glVertex3d", Vertex3d);
  init_entry (library, "glVertex3dv", Vertex3dv);
  init_entry (library, "glVertex3f", Vertex3f);
  init_entry (library, "glVertex3fv", Vertex3fv);
  init_entry (library, "glVertex3i", Vertex3i);
  init_entry (library, "glVertex3iv", Vertex3iv);
  init_entry (library, "glVertex3s", Vertex3s);
  init_entry (library, "glVertex3sv", Vertex3sv);
  init_entry (library, "glVertex4d", Vertex4d);
  init_entry (library, "glVertex4dv", Vertex4dv);
  init_entry (library, "glVertex4f", Vertex4f);
  init_entry (library, "glVertex4fv", Vertex4fv);
  init_entry (library, "glVertex4i", Vertex4i);
  init_entry (library, "glVertex4iv", Vertex4iv);
  init_entry (library, "glVertex4s", Vertex4s);
  init_entry (library, "glVertex4sv", Vertex4sv);
  init_entry (library, "glVertexPointer", VertexPointer);
  init_entry (library, "glViewport", Viewport);
  
    //��������� ������� ����� ����� ���������� OpenGL
<<<GLINIT>>>
}

#endif
