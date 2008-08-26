#ifndef RENDER_GL_DRIVER_ENTRIES_HEADER
#define RENDER_GL_DRIVER_ENTRIES_HEADER

#ifndef APIENTRY
#  define APIENTRY_DEFINED
#  if defined(__MINGW32__)
#    define APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif

#include <shared/gl/gl_types.h>
#include <shared/gl/glext.h>
#include <shared/library.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Точки входа OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct GlEntries
{
  /*
      Точки входа OpenGL 1.1  
  */

  void (APIENTRY* Accum)(GLenum op, GLfloat value);
  void (APIENTRY* AlphaFunc)(GLenum func, GLclampf ref);
  GLboolean (APIENTRY* AreTexturesResident)(GLsizei n, const GLuint *textures, GLboolean *residences);
  void (APIENTRY* ArrayElement)(GLint i);
  void (APIENTRY* Begin)(GLenum mode);
  void (APIENTRY* BindTexture)(GLenum target, GLuint texture);
  void (APIENTRY* Bitmap)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
  void (APIENTRY* BlendFunc)(GLenum sfactor, GLenum dfactor);
  void (APIENTRY* CallList)(GLuint list);
  void (APIENTRY* CallLists)(GLsizei n, GLenum type, const GLvoid *lists);
  void (APIENTRY* Clear)(GLbitfield mask);
  void (APIENTRY* ClearAccum)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  void (APIENTRY* ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void (APIENTRY* ClearDepth)(GLclampd depth);
  void (APIENTRY* ClearIndex)(GLfloat c);
  void (APIENTRY* ClearStencil)(GLint s);
  void (APIENTRY* ClipPlane)(GLenum plane, const GLdouble *equation);
  void (APIENTRY* Color3b)(GLbyte red, GLbyte green, GLbyte blue);
  void (APIENTRY* Color3bv)(const GLbyte *v);
  void (APIENTRY* Color3d)(GLdouble red, GLdouble green, GLdouble blue);
  void (APIENTRY* Color3dv)(const GLdouble *v);
  void (APIENTRY* Color3f)(GLfloat red, GLfloat green, GLfloat blue);
  void (APIENTRY* Color3fv)(const GLfloat *v);
  void (APIENTRY* Color3i)(GLint red, GLint green, GLint blue);
  void (APIENTRY* Color3iv)(const GLint *v);
  void (APIENTRY* Color3s)(GLshort red, GLshort green, GLshort blue);
  void (APIENTRY* Color3sv)(const GLshort *v);
  void (APIENTRY* Color3ub)(GLubyte red, GLubyte green, GLubyte blue);
  void (APIENTRY* Color3ubv)(const GLubyte *v);
  void (APIENTRY* Color3ui)(GLuint red, GLuint green, GLuint blue);
  void (APIENTRY* Color3uiv)(const GLuint *v);
  void (APIENTRY* Color3us)(GLushort red, GLushort green, GLushort blue);
  void (APIENTRY* Color3usv)(const GLushort *v);
  void (APIENTRY* Color4b)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
  void (APIENTRY* Color4bv)(const GLbyte *v);
  void (APIENTRY* Color4d)(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
  void (APIENTRY* Color4dv)(const GLdouble *v);
  void (APIENTRY* Color4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  void (APIENTRY* Color4fv)(const GLfloat *v);
  void (APIENTRY* Color4i)(GLint red, GLint green, GLint blue, GLint alpha);
  void (APIENTRY* Color4iv)(const GLint *v);
  void (APIENTRY* Color4s)(GLshort red, GLshort green, GLshort blue, GLshort alpha);
  void (APIENTRY* Color4sv)(const GLshort *v);
  void (APIENTRY* Color4ub)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  void (APIENTRY* Color4ubv)(const GLubyte *v);
  void (APIENTRY* Color4ui)(GLuint red, GLuint green, GLuint blue, GLuint alpha);
  void (APIENTRY* Color4uiv)(const GLuint *v);
  void (APIENTRY* Color4us)(GLushort red, GLushort green, GLushort blue, GLushort alpha);
  void (APIENTRY* Color4usv)(const GLushort *v);
  void (APIENTRY* ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
  void (APIENTRY* ColorMaterial)(GLenum face, GLenum mode);
  void (APIENTRY* ColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* CopyPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
  void (APIENTRY* CopyTexImage1D)(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
  void (APIENTRY* CopyTexImage2D)(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
  void (APIENTRY* CopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
  void (APIENTRY* CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  void (APIENTRY* CullFace)(GLenum mode);
  void (APIENTRY* DeleteLists)(GLuint list, GLsizei range);
  void (APIENTRY* DeleteTextures)(GLsizei n, const GLuint *textures);
  void (APIENTRY* DepthFunc)(GLenum func);
  void (APIENTRY* DepthMask)(GLboolean flag);
  void (APIENTRY* DepthRange)(GLclampd zNear, GLclampd zFar);
  void (APIENTRY* Disable)(GLenum cap);
  void (APIENTRY* DisableClientState)(GLenum array);
  void (APIENTRY* DrawArrays)(GLenum mode, GLint first, GLsizei count);
  void (APIENTRY* DrawBuffer)(GLenum mode);
  void (APIENTRY* DrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
  void (APIENTRY* DrawPixels)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  void (APIENTRY* EdgeFlag)(GLboolean flag);
  void (APIENTRY* EdgeFlagPointer)(GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* EdgeFlagv)(const GLboolean *flag);
  void (APIENTRY* Enable)(GLenum cap);
  void (APIENTRY* EnableClientState)(GLenum array);
  void (APIENTRY* End)(void);
  void (APIENTRY* EndList)(void);
  void (APIENTRY* EvalCoord1d)(GLdouble u);
  void (APIENTRY* EvalCoord1dv)(const GLdouble *u);
  void (APIENTRY* EvalCoord1f)(GLfloat u);
  void (APIENTRY* EvalCoord1fv)(const GLfloat *u);
  void (APIENTRY* EvalCoord2d)(GLdouble u, GLdouble v);
  void (APIENTRY* EvalCoord2dv)(const GLdouble *u);
  void (APIENTRY* EvalCoord2f)(GLfloat u, GLfloat v);
  void (APIENTRY* EvalCoord2fv)(const GLfloat *u);
  void (APIENTRY* EvalMesh1)(GLenum mode, GLint i1, GLint i2);
  void (APIENTRY* EvalMesh2)(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
  void (APIENTRY* EvalPoint1)(GLint i);
  void (APIENTRY* EvalPoint2)(GLint i, GLint j);
  void (APIENTRY* FeedbackBuffer)(GLsizei size, GLenum type, GLfloat *buffer);
  void (APIENTRY* Finish)(void);
  void (APIENTRY* Flush)(void);
  void (APIENTRY* Fogf)(GLenum pname, GLfloat param);
  void (APIENTRY* Fogfv)(GLenum pname, const GLfloat *params);
  void (APIENTRY* Fogi)(GLenum pname, GLint param);
  void (APIENTRY* Fogiv)(GLenum pname, const GLint *params);
  void (APIENTRY* FrontFace)(GLenum mode);
  void (APIENTRY* Frustum)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
  GLuint (APIENTRY* GenLists)(GLsizei range);
  void (APIENTRY* GenTextures)(GLsizei n, GLuint *textures);
  void (APIENTRY* GetBooleanv)(GLenum pname, GLboolean *params);
  void (APIENTRY* GetClipPlane)(GLenum plane, GLdouble *equation);
  void (APIENTRY* GetDoublev)(GLenum pname, GLdouble *params);
  GLenum (APIENTRY* GetError)(void);
  void (APIENTRY* GetFloatv)(GLenum pname, GLfloat *params);
  void (APIENTRY* GetIntegerv)(GLenum pname, GLint *params);
  void (APIENTRY* GetLightfv)(GLenum light, GLenum pname, GLfloat *params);
  void (APIENTRY* GetLightiv)(GLenum light, GLenum pname, GLint *params);
  void (APIENTRY* GetMapdv)(GLenum target, GLenum query, GLdouble *v);
  void (APIENTRY* GetMapfv)(GLenum target, GLenum query, GLfloat *v);
  void (APIENTRY* GetMapiv)(GLenum target, GLenum query, GLint *v);
  void (APIENTRY* GetMaterialfv)(GLenum face, GLenum pname, GLfloat *params);
  void (APIENTRY* GetMaterialiv)(GLenum face, GLenum pname, GLint *params);
  void (APIENTRY* GetPixelMapfv)(GLenum map, GLfloat *values);
  void (APIENTRY* GetPixelMapuiv)(GLenum map, GLuint *values);
  void (APIENTRY* GetPixelMapusv)(GLenum map, GLushort *values);
  void (APIENTRY* GetPointerv)(GLenum pname, GLvoid* *params);
  void (APIENTRY* GetPolygonStipple)(GLubyte *mask);
  const GLubyte * (APIENTRY* GetString)(GLenum name);
  void (APIENTRY* GetTexEnvfv)(GLenum target, GLenum pname, GLfloat *params);
  void (APIENTRY* GetTexEnviv)(GLenum target, GLenum pname, GLint *params);
  void (APIENTRY* GetTexGendv)(GLenum coord, GLenum pname, GLdouble *params);
  void (APIENTRY* GetTexGenfv)(GLenum coord, GLenum pname, GLfloat *params);
  void (APIENTRY* GetTexGeniv)(GLenum coord, GLenum pname, GLint *params);
  void (APIENTRY* GetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
  void (APIENTRY* GetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat *params);
  void (APIENTRY* GetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint *params);
  void (APIENTRY* GetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
  void (APIENTRY* GetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
  void (APIENTRY* Hint)(GLenum target, GLenum mode);
  void (APIENTRY* IndexMask)(GLuint mask);
  void (APIENTRY* IndexPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* Indexd)(GLdouble c);
  void (APIENTRY* Indexdv)(const GLdouble *c);
  void (APIENTRY* Indexf)(GLfloat c);
  void (APIENTRY* Indexfv)(const GLfloat *c);
  void (APIENTRY* Indexi)(GLint c);
  void (APIENTRY* Indexiv)(const GLint *c);
  void (APIENTRY* Indexs)(GLshort c);
  void (APIENTRY* Indexsv)(const GLshort *c);
  void (APIENTRY* Indexub)(GLubyte c);
  void (APIENTRY* Indexubv)(const GLubyte *c);
  void (APIENTRY* InitNames)(void);
  void (APIENTRY* InterleavedArrays)(GLenum format, GLsizei stride, const GLvoid *pointer);
  GLboolean (APIENTRY* IsEnabled)(GLenum cap);
  GLboolean (APIENTRY* IsList)(GLuint list);
  GLboolean (APIENTRY* IsTexture)(GLuint texture);
  void (APIENTRY* LightModelf)(GLenum pname, GLfloat param);
  void (APIENTRY* LightModelfv)(GLenum pname, const GLfloat *params);
  void (APIENTRY* LightModeli)(GLenum pname, GLint param);
  void (APIENTRY* LightModeliv)(GLenum pname, const GLint *params);
  void (APIENTRY* Lightf)(GLenum light, GLenum pname, GLfloat param);
  void (APIENTRY* Lightfv)(GLenum light, GLenum pname, const GLfloat *params);
  void (APIENTRY* Lighti)(GLenum light, GLenum pname, GLint param);
  void (APIENTRY* Lightiv)(GLenum light, GLenum pname, const GLint *params);
  void (APIENTRY* LineStipple)(GLint factor, GLushort pattern);
  void (APIENTRY* LineWidth)(GLfloat width);
  void (APIENTRY* ListBase)(GLuint base);
  void (APIENTRY* LoadIdentity)(void);
  void (APIENTRY* LoadMatrixd)(const GLdouble *m);
  void (APIENTRY* LoadMatrixf)(const GLfloat *m);
  void (APIENTRY* LoadName)(GLuint name);
  void (APIENTRY* LogicOp)(GLenum opcode);
  void (APIENTRY* Map1d)(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
  void (APIENTRY* Map1f)(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
  void (APIENTRY* Map2d)(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
  void (APIENTRY* Map2f)(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
  void (APIENTRY* MapGrid1d)(GLint un, GLdouble u1, GLdouble u2);
  void (APIENTRY* MapGrid1f)(GLint un, GLfloat u1, GLfloat u2);
  void (APIENTRY* MapGrid2d)(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
  void (APIENTRY* MapGrid2f)(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
  void (APIENTRY* Materialf)(GLenum face, GLenum pname, GLfloat param);
  void (APIENTRY* Materialfv)(GLenum face, GLenum pname, const GLfloat *params);
  void (APIENTRY* Materiali)(GLenum face, GLenum pname, GLint param);
  void (APIENTRY* Materialiv)(GLenum face, GLenum pname, const GLint *params);
  void (APIENTRY* MatrixMode)(GLenum mode);
  void (APIENTRY* MultMatrixd)(const GLdouble *m);
  void (APIENTRY* MultMatrixf)(const GLfloat *m);
  void (APIENTRY* NewList)(GLuint list, GLenum mode);
  void (APIENTRY* Normal3b)(GLbyte nx, GLbyte ny, GLbyte nz);
  void (APIENTRY* Normal3bv)(const GLbyte *v);
  void (APIENTRY* Normal3d)(GLdouble nx, GLdouble ny, GLdouble nz);
  void (APIENTRY* Normal3dv)(const GLdouble *v);
  void (APIENTRY* Normal3f)(GLfloat nx, GLfloat ny, GLfloat nz);
  void (APIENTRY* Normal3fv)(const GLfloat *v);
  void (APIENTRY* Normal3i)(GLint nx, GLint ny, GLint nz);
  void (APIENTRY* Normal3iv)(const GLint *v);
  void (APIENTRY* Normal3s)(GLshort nx, GLshort ny, GLshort nz);
  void (APIENTRY* Normal3sv)(const GLshort *v);
  void (APIENTRY* NormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* Ortho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
  void (APIENTRY* PassThrough)(GLfloat token);
  void (APIENTRY* PixelMapfv)(GLenum map, GLsizei mapsize, const GLfloat *values);
  void (APIENTRY* PixelMapuiv)(GLenum map, GLsizei mapsize, const GLuint *values);
  void (APIENTRY* PixelMapusv)(GLenum map, GLsizei mapsize, const GLushort *values);
  void (APIENTRY* PixelStoref)(GLenum pname, GLfloat param);
  void (APIENTRY* PixelStorei)(GLenum pname, GLint param);
  void (APIENTRY* PixelTransferf)(GLenum pname, GLfloat param);
  void (APIENTRY* PixelTransferi)(GLenum pname, GLint param);
  void (APIENTRY* PixelZoom)(GLfloat xfactor, GLfloat yfactor);
  void (APIENTRY* PointSize)(GLfloat size);
  void (APIENTRY* PolygonMode)(GLenum face, GLenum mode);
  void (APIENTRY* PolygonOffset)(GLfloat factor, GLfloat units);
  void (APIENTRY* PolygonStipple)(const GLubyte *mask);
  void (APIENTRY* PopAttrib)(void);
  void (APIENTRY* PopClientAttrib)(void);
  void (APIENTRY* PopMatrix)(void);
  void (APIENTRY* PopName)(void);
  void (APIENTRY* PrioritizeTextures)(GLsizei n, const GLuint *textures, const GLclampf *priorities);
  void (APIENTRY* PushAttrib)(GLbitfield mask);
  void (APIENTRY* PushClientAttrib)(GLbitfield mask);
  void (APIENTRY* PushMatrix)(void);
  void (APIENTRY* PushName)(GLuint name);
  void (APIENTRY* RasterPos2d)(GLdouble x, GLdouble y);
  void (APIENTRY* RasterPos2dv)(const GLdouble *v);
  void (APIENTRY* RasterPos2f)(GLfloat x, GLfloat y);
  void (APIENTRY* RasterPos2fv)(const GLfloat *v);
  void (APIENTRY* RasterPos2i)(GLint x, GLint y);
  void (APIENTRY* RasterPos2iv)(const GLint *v);
  void (APIENTRY* RasterPos2s)(GLshort x, GLshort y);
  void (APIENTRY* RasterPos2sv)(const GLshort *v);
  void (APIENTRY* RasterPos3d)(GLdouble x, GLdouble y, GLdouble z);
  void (APIENTRY* RasterPos3dv)(const GLdouble *v);
  void (APIENTRY* RasterPos3f)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY* RasterPos3fv)(const GLfloat *v);
  void (APIENTRY* RasterPos3i)(GLint x, GLint y, GLint z);
  void (APIENTRY* RasterPos3iv)(const GLint *v);
  void (APIENTRY* RasterPos3s)(GLshort x, GLshort y, GLshort z);
  void (APIENTRY* RasterPos3sv)(const GLshort *v);
  void (APIENTRY* RasterPos4d)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  void (APIENTRY* RasterPos4dv)(const GLdouble *v);
  void (APIENTRY* RasterPos4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void (APIENTRY* RasterPos4fv)(const GLfloat *v);
  void (APIENTRY* RasterPos4i)(GLint x, GLint y, GLint z, GLint w);
  void (APIENTRY* RasterPos4iv)(const GLint *v);
  void (APIENTRY* RasterPos4s)(GLshort x, GLshort y, GLshort z, GLshort w);
  void (APIENTRY* RasterPos4sv)(const GLshort *v);
  void (APIENTRY* ReadBuffer)(GLenum mode);
  void (APIENTRY* ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
  void (APIENTRY* Rectd)(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  void (APIENTRY* Rectdv)(const GLdouble *v1, const GLdouble *v2);
  void (APIENTRY* Rectf)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
  void (APIENTRY* Rectfv)(const GLfloat *v1, const GLfloat *v2);
  void (APIENTRY* Recti)(GLint x1, GLint y1, GLint x2, GLint y2);
  void (APIENTRY* Rectiv)(const GLint *v1, const GLint *v2);
  void (APIENTRY* Rects)(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
  void (APIENTRY* Rectsv)(const GLshort *v1, const GLshort *v2);
  GLint (APIENTRY* RenderMode)(GLenum mode);
  void (APIENTRY* Rotated)(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
  void (APIENTRY* Rotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY* Scaled)(GLdouble x, GLdouble y, GLdouble z);
  void (APIENTRY* Scalef)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY* Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
  void (APIENTRY* SelectBuffer)(GLsizei size, GLuint *buffer);
  void (APIENTRY* ShadeModel)(GLenum mode);
  void (APIENTRY* StencilFunc)(GLenum func, GLint ref, GLuint mask);
  void (APIENTRY* StencilMask)(GLuint mask);
  void (APIENTRY* StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
  void (APIENTRY* TexCoord1d)(GLdouble s);
  void (APIENTRY* TexCoord1dv)(const GLdouble *v);
  void (APIENTRY* TexCoord1f)(GLfloat s);
  void (APIENTRY* TexCoord1fv)(const GLfloat *v);
  void (APIENTRY* TexCoord1i)(GLint s);
  void (APIENTRY* TexCoord1iv)(const GLint *v);
  void (APIENTRY* TexCoord1s)(GLshort s);
  void (APIENTRY* TexCoord1sv)(const GLshort *v);
  void (APIENTRY* TexCoord2d)(GLdouble s, GLdouble t);
  void (APIENTRY* TexCoord2dv)(const GLdouble *v);
  void (APIENTRY* TexCoord2f)(GLfloat s, GLfloat t);
  void (APIENTRY* TexCoord2fv)(const GLfloat *v);
  void (APIENTRY* TexCoord2i)(GLint s, GLint t);
  void (APIENTRY* TexCoord2iv)(const GLint *v);
  void (APIENTRY* TexCoord2s)(GLshort s, GLshort t);
  void (APIENTRY* TexCoord2sv)(const GLshort *v);
  void (APIENTRY* TexCoord3d)(GLdouble s, GLdouble t, GLdouble r);
  void (APIENTRY* TexCoord3dv)(const GLdouble *v);
  void (APIENTRY* TexCoord3f)(GLfloat s, GLfloat t, GLfloat r);
  void (APIENTRY* TexCoord3fv)(const GLfloat *v);
  void (APIENTRY* TexCoord3i)(GLint s, GLint t, GLint r);
  void (APIENTRY* TexCoord3iv)(const GLint *v);
  void (APIENTRY* TexCoord3s)(GLshort s, GLshort t, GLshort r);
  void (APIENTRY* TexCoord3sv)(const GLshort *v);
  void (APIENTRY* TexCoord4d)(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
  void (APIENTRY* TexCoord4dv)(const GLdouble *v);
  void (APIENTRY* TexCoord4f)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  void (APIENTRY* TexCoord4fv)(const GLfloat *v);
  void (APIENTRY* TexCoord4i)(GLint s, GLint t, GLint r, GLint q);
  void (APIENTRY* TexCoord4iv)(const GLint *v);
  void (APIENTRY* TexCoord4s)(GLshort s, GLshort t, GLshort r, GLshort q);
  void (APIENTRY* TexCoord4sv)(const GLshort *v);
  void (APIENTRY* TexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* TexEnvf)(GLenum target, GLenum pname, GLfloat param);
  void (APIENTRY* TexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
  void (APIENTRY* TexEnvi)(GLenum target, GLenum pname, GLint param);
  void (APIENTRY* TexEnviv)(GLenum target, GLenum pname, const GLint *params);
  void (APIENTRY* TexGend)(GLenum coord, GLenum pname, GLdouble param);
  void (APIENTRY* TexGendv)(GLenum coord, GLenum pname, const GLdouble *params);
  void (APIENTRY* TexGenf)(GLenum coord, GLenum pname, GLfloat param);
  void (APIENTRY* TexGenfv)(GLenum coord, GLenum pname, const GLfloat *params);
  void (APIENTRY* TexGeni)(GLenum coord, GLenum pname, GLint param);
  void (APIENTRY* TexGeniv)(GLenum coord, GLenum pname, const GLint *params);
  void (APIENTRY* TexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  void (APIENTRY* TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  void (APIENTRY* TexParameterf)(GLenum target, GLenum pname, GLfloat param);
  void (APIENTRY* TexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
  void (APIENTRY* TexParameteri)(GLenum target, GLenum pname, GLint param);
  void (APIENTRY* TexParameteriv)(GLenum target, GLenum pname, const GLint *params);
  void (APIENTRY* TexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
  void (APIENTRY* TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  void (APIENTRY* Translated)(GLdouble x, GLdouble y, GLdouble z);
  void (APIENTRY* Translatef)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY* Vertex2d)(GLdouble x, GLdouble y);
  void (APIENTRY* Vertex2dv)(const GLdouble *v);
  void (APIENTRY* Vertex2f)(GLfloat x, GLfloat y);
  void (APIENTRY* Vertex2fv)(const GLfloat *v);
  void (APIENTRY* Vertex2i)(GLint x, GLint y);
  void (APIENTRY* Vertex2iv)(const GLint *v);
  void (APIENTRY* Vertex2s)(GLshort x, GLshort y);
  void (APIENTRY* Vertex2sv)(const GLshort *v);
  void (APIENTRY* Vertex3d)(GLdouble x, GLdouble y, GLdouble z);
  void (APIENTRY* Vertex3dv)(const GLdouble *v);
  void (APIENTRY* Vertex3f)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY* Vertex3fv)(const GLfloat *v);
  void (APIENTRY* Vertex3i)(GLint x, GLint y, GLint z);
  void (APIENTRY* Vertex3iv)(const GLint *v);
  void (APIENTRY* Vertex3s)(GLshort x, GLshort y, GLshort z);
  void (APIENTRY* Vertex3sv)(const GLshort *v);
  void (APIENTRY* Vertex4d)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  void (APIENTRY* Vertex4dv)(const GLdouble *v);
  void (APIENTRY* Vertex4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void (APIENTRY* Vertex4fv)(const GLfloat *v);
  void (APIENTRY* Vertex4i)(GLint x, GLint y, GLint z, GLint w);
  void (APIENTRY* Vertex4iv)(const GLint *v);
  void (APIENTRY* Vertex4s)(GLshort x, GLshort y, GLshort z, GLshort w);
  void (APIENTRY* Vertex4sv)(const GLshort *v);
  void (APIENTRY* VertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (APIENTRY* Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);
  
  /*
      Точки входа расширений OpenGL
  */

  PFNGLBLENDCOLORPROC BlendColor;
  PFNGLBLENDEQUATIONPROC BlendEquation;
  PFNGLDRAWRANGEELEMENTSPROC DrawRangeElements;
  PFNGLCOLORTABLEPROC ColorTable;
  PFNGLCOLORTABLEPARAMETERFVPROC ColorTableParameterfv;
  PFNGLCOLORTABLEPARAMETERIVPROC ColorTableParameteriv;
  PFNGLCOPYCOLORTABLEPROC CopyColorTable;
  PFNGLGETCOLORTABLEPROC GetColorTable;
  PFNGLGETCOLORTABLEPARAMETERFVPROC GetColorTableParameterfv;
  PFNGLGETCOLORTABLEPARAMETERIVPROC GetColorTableParameteriv;
  PFNGLCOLORSUBTABLEPROC ColorSubTable;
  PFNGLCOPYCOLORSUBTABLEPROC CopyColorSubTable;
  PFNGLCONVOLUTIONFILTER1DPROC ConvolutionFilter1D;
  PFNGLCONVOLUTIONFILTER2DPROC ConvolutionFilter2D;
  PFNGLCONVOLUTIONPARAMETERFPROC ConvolutionParameterf;
  PFNGLCONVOLUTIONPARAMETERFVPROC ConvolutionParameterfv;
  PFNGLCONVOLUTIONPARAMETERIPROC ConvolutionParameteri;
  PFNGLCONVOLUTIONPARAMETERIVPROC ConvolutionParameteriv;
  PFNGLCOPYCONVOLUTIONFILTER1DPROC CopyConvolutionFilter1D;
  PFNGLCOPYCONVOLUTIONFILTER2DPROC CopyConvolutionFilter2D;
  PFNGLGETCONVOLUTIONFILTERPROC GetConvolutionFilter;
  PFNGLGETCONVOLUTIONPARAMETERFVPROC GetConvolutionParameterfv;
  PFNGLGETCONVOLUTIONPARAMETERIVPROC GetConvolutionParameteriv;
  PFNGLGETSEPARABLEFILTERPROC GetSeparableFilter;
  PFNGLSEPARABLEFILTER2DPROC SeparableFilter2D;
  PFNGLGETHISTOGRAMPROC GetHistogram;
  PFNGLGETHISTOGRAMPARAMETERFVPROC GetHistogramParameterfv;
  PFNGLGETHISTOGRAMPARAMETERIVPROC GetHistogramParameteriv;
  PFNGLGETMINMAXPROC GetMinmax;
  PFNGLGETMINMAXPARAMETERFVPROC GetMinmaxParameterfv;
  PFNGLGETMINMAXPARAMETERIVPROC GetMinmaxParameteriv;
  PFNGLHISTOGRAMPROC Histogram;
  PFNGLMINMAXPROC Minmax;
  PFNGLRESETHISTOGRAMPROC ResetHistogram;
  PFNGLRESETMINMAXPROC ResetMinmax;
  PFNGLTEXIMAGE3DPROC TexImage3D;
  PFNGLTEXSUBIMAGE3DPROC TexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC CopyTexSubImage3D;
  PFNGLACTIVETEXTUREPROC ActiveTexture;
  PFNGLCLIENTACTIVETEXTUREPROC ClientActiveTexture;
  PFNGLMULTITEXCOORD1DPROC MultiTexCoord1d;
  PFNGLMULTITEXCOORD1DVPROC MultiTexCoord1dv;
  PFNGLMULTITEXCOORD1FPROC MultiTexCoord1f;
  PFNGLMULTITEXCOORD1FVPROC MultiTexCoord1fv;
  PFNGLMULTITEXCOORD1IPROC MultiTexCoord1i;
  PFNGLMULTITEXCOORD1IVPROC MultiTexCoord1iv;
  PFNGLMULTITEXCOORD1SPROC MultiTexCoord1s;
  PFNGLMULTITEXCOORD1SVPROC MultiTexCoord1sv;
  PFNGLMULTITEXCOORD2DPROC MultiTexCoord2d;
  PFNGLMULTITEXCOORD2DVPROC MultiTexCoord2dv;
  PFNGLMULTITEXCOORD2FPROC MultiTexCoord2f;
  PFNGLMULTITEXCOORD2FVPROC MultiTexCoord2fv;
  PFNGLMULTITEXCOORD2IPROC MultiTexCoord2i;
  PFNGLMULTITEXCOORD2IVPROC MultiTexCoord2iv;
  PFNGLMULTITEXCOORD2SPROC MultiTexCoord2s;
  PFNGLMULTITEXCOORD2SVPROC MultiTexCoord2sv;
  PFNGLMULTITEXCOORD3DPROC MultiTexCoord3d;
  PFNGLMULTITEXCOORD3DVPROC MultiTexCoord3dv;
  PFNGLMULTITEXCOORD3FPROC MultiTexCoord3f;
  PFNGLMULTITEXCOORD3FVPROC MultiTexCoord3fv;
  PFNGLMULTITEXCOORD3IPROC MultiTexCoord3i;
  PFNGLMULTITEXCOORD3IVPROC MultiTexCoord3iv;
  PFNGLMULTITEXCOORD3SPROC MultiTexCoord3s;
  PFNGLMULTITEXCOORD3SVPROC MultiTexCoord3sv;
  PFNGLMULTITEXCOORD4DPROC MultiTexCoord4d;
  PFNGLMULTITEXCOORD4DVPROC MultiTexCoord4dv;
  PFNGLMULTITEXCOORD4FPROC MultiTexCoord4f;
  PFNGLMULTITEXCOORD4FVPROC MultiTexCoord4fv;
  PFNGLMULTITEXCOORD4IPROC MultiTexCoord4i;
  PFNGLMULTITEXCOORD4IVPROC MultiTexCoord4iv;
  PFNGLMULTITEXCOORD4SPROC MultiTexCoord4s;
  PFNGLMULTITEXCOORD4SVPROC MultiTexCoord4sv;
  PFNGLLOADTRANSPOSEMATRIXFPROC LoadTransposeMatrixf;
  PFNGLLOADTRANSPOSEMATRIXDPROC LoadTransposeMatrixd;
  PFNGLMULTTRANSPOSEMATRIXFPROC MultTransposeMatrixf;
  PFNGLMULTTRANSPOSEMATRIXDPROC MultTransposeMatrixd;
  PFNGLSAMPLECOVERAGEPROC SampleCoverage;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC CompressedTexImage3D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC CompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC CompressedTexImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC CompressedTexSubImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC CompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC CompressedTexSubImage1D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC GetCompressedTexImage;
  PFNGLBLENDFUNCSEPARATEPROC BlendFuncSeparate;
  PFNGLFOGCOORDFPROC FogCoordf;
  PFNGLFOGCOORDFVPROC FogCoordfv;
  PFNGLFOGCOORDDPROC FogCoordd;
  PFNGLFOGCOORDDVPROC FogCoorddv;
  PFNGLFOGCOORDPOINTERPROC FogCoordPointer;
  PFNGLMULTIDRAWARRAYSPROC MultiDrawArrays;
  PFNGLMULTIDRAWELEMENTSPROC MultiDrawElements;
  PFNGLPOINTPARAMETERFPROC PointParameterf;
  PFNGLPOINTPARAMETERFVPROC PointParameterfv;
  PFNGLPOINTPARAMETERIPROC PointParameteri;
  PFNGLPOINTPARAMETERIVPROC PointParameteriv;
  PFNGLSECONDARYCOLOR3BPROC SecondaryColor3b;
  PFNGLSECONDARYCOLOR3BVPROC SecondaryColor3bv;
  PFNGLSECONDARYCOLOR3DPROC SecondaryColor3d;
  PFNGLSECONDARYCOLOR3DVPROC SecondaryColor3dv;
  PFNGLSECONDARYCOLOR3FPROC SecondaryColor3f;
  PFNGLSECONDARYCOLOR3FVPROC SecondaryColor3fv;
  PFNGLSECONDARYCOLOR3IPROC SecondaryColor3i;
  PFNGLSECONDARYCOLOR3IVPROC SecondaryColor3iv;
  PFNGLSECONDARYCOLOR3SPROC SecondaryColor3s;
  PFNGLSECONDARYCOLOR3SVPROC SecondaryColor3sv;
  PFNGLSECONDARYCOLOR3UBPROC SecondaryColor3ub;
  PFNGLSECONDARYCOLOR3UBVPROC SecondaryColor3ubv;
  PFNGLSECONDARYCOLOR3UIPROC SecondaryColor3ui;
  PFNGLSECONDARYCOLOR3UIVPROC SecondaryColor3uiv;
  PFNGLSECONDARYCOLOR3USPROC SecondaryColor3us;
  PFNGLSECONDARYCOLOR3USVPROC SecondaryColor3usv;
  PFNGLSECONDARYCOLORPOINTERPROC SecondaryColorPointer;
  PFNGLWINDOWPOS2DPROC WindowPos2d;
  PFNGLWINDOWPOS2DVPROC WindowPos2dv;
  PFNGLWINDOWPOS2FPROC WindowPos2f;
  PFNGLWINDOWPOS2FVPROC WindowPos2fv;
  PFNGLWINDOWPOS2IPROC WindowPos2i;
  PFNGLWINDOWPOS2IVPROC WindowPos2iv;
  PFNGLWINDOWPOS2SPROC WindowPos2s;
  PFNGLWINDOWPOS2SVPROC WindowPos2sv;
  PFNGLWINDOWPOS3DPROC WindowPos3d;
  PFNGLWINDOWPOS3DVPROC WindowPos3dv;
  PFNGLWINDOWPOS3FPROC WindowPos3f;
  PFNGLWINDOWPOS3FVPROC WindowPos3fv;
  PFNGLWINDOWPOS3IPROC WindowPos3i;
  PFNGLWINDOWPOS3IVPROC WindowPos3iv;
  PFNGLWINDOWPOS3SPROC WindowPos3s;
  PFNGLWINDOWPOS3SVPROC WindowPos3sv;
  PFNGLGENQUERIESPROC GenQueries;
  PFNGLDELETEQUERIESPROC DeleteQueries;
  PFNGLISQUERYPROC IsQuery;
  PFNGLBEGINQUERYPROC BeginQuery;
  PFNGLENDQUERYPROC EndQuery;
  PFNGLGETQUERYIVPROC GetQueryiv;
  PFNGLGETQUERYOBJECTIVPROC GetQueryObjectiv;
  PFNGLGETQUERYOBJECTUIVPROC GetQueryObjectuiv;
  PFNGLBINDBUFFERPROC BindBuffer;
  PFNGLDELETEBUFFERSPROC DeleteBuffers;
  PFNGLGENBUFFERSPROC GenBuffers;
  PFNGLISBUFFERPROC IsBuffer;
  PFNGLBUFFERDATAPROC BufferData;
  PFNGLBUFFERSUBDATAPROC BufferSubData;
  PFNGLGETBUFFERSUBDATAPROC GetBufferSubData;
  PFNGLMAPBUFFERPROC MapBuffer;
  PFNGLUNMAPBUFFERPROC UnmapBuffer;
  PFNGLGETBUFFERPARAMETERIVPROC GetBufferParameteriv;
  PFNGLGETBUFFERPOINTERVPROC GetBufferPointerv;
  PFNGLBLENDEQUATIONSEPARATEPROC BlendEquationSeparate;
  PFNGLDRAWBUFFERSPROC DrawBuffers;
  PFNGLSTENCILOPSEPARATEPROC StencilOpSeparate;
  PFNGLSTENCILFUNCSEPARATEPROC StencilFuncSeparate;
  PFNGLSTENCILMASKSEPARATEPROC StencilMaskSeparate;
  PFNGLATTACHSHADERPROC AttachShader;
  PFNGLBINDATTRIBLOCATIONPROC BindAttribLocation;
  PFNGLCOMPILESHADERPROC CompileShader;
  PFNGLCREATEPROGRAMPROC CreateProgram;
  PFNGLCREATESHADERPROC CreateShader;
  PFNGLDELETEPROGRAMPROC DeleteProgram;
  PFNGLDELETESHADERPROC DeleteShader;
  PFNGLDETACHSHADERPROC DetachShader;
  PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray;
  PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
  PFNGLGETACTIVEATTRIBPROC GetActiveAttrib;
  PFNGLGETACTIVEUNIFORMPROC GetActiveUniform;
  PFNGLGETATTACHEDSHADERSPROC GetAttachedShaders;
  PFNGLGETATTRIBLOCATIONPROC GetAttribLocation;
  PFNGLGETPROGRAMIVPROC GetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
  PFNGLGETSHADERIVPROC GetShaderiv;
  PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
  PFNGLGETSHADERSOURCEPROC GetShaderSource;
  PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
  PFNGLGETUNIFORMFVPROC GetUniformfv;
  PFNGLGETUNIFORMIVPROC GetUniformiv;
  PFNGLGETVERTEXATTRIBDVPROC GetVertexAttribdv;
  PFNGLGETVERTEXATTRIBFVPROC GetVertexAttribfv;
  PFNGLGETVERTEXATTRIBIVPROC GetVertexAttribiv;
  PFNGLGETVERTEXATTRIBPOINTERVPROC GetVertexAttribPointerv;
  PFNGLISPROGRAMPROC IsProgram;
  PFNGLISSHADERPROC IsShader;
  PFNGLLINKPROGRAMPROC LinkProgram;
  PFNGLSHADERSOURCEPROC ShaderSource;
  PFNGLUSEPROGRAMPROC UseProgram;
  PFNGLUNIFORM1FPROC Uniform1f;
  PFNGLUNIFORM2FPROC Uniform2f;
  PFNGLUNIFORM3FPROC Uniform3f;
  PFNGLUNIFORM4FPROC Uniform4f;
  PFNGLUNIFORM1IPROC Uniform1i;
  PFNGLUNIFORM2IPROC Uniform2i;
  PFNGLUNIFORM3IPROC Uniform3i;
  PFNGLUNIFORM4IPROC Uniform4i;
  PFNGLUNIFORM1FVPROC Uniform1fv;
  PFNGLUNIFORM2FVPROC Uniform2fv;
  PFNGLUNIFORM3FVPROC Uniform3fv;
  PFNGLUNIFORM4FVPROC Uniform4fv;
  PFNGLUNIFORM1IVPROC Uniform1iv;
  PFNGLUNIFORM2IVPROC Uniform2iv;
  PFNGLUNIFORM3IVPROC Uniform3iv;
  PFNGLUNIFORM4IVPROC Uniform4iv;
  PFNGLUNIFORMMATRIX2FVPROC UniformMatrix2fv;
  PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv;
  PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;
  PFNGLVALIDATEPROGRAMPROC ValidateProgram;
  PFNGLVERTEXATTRIB1DPROC VertexAttrib1d;
  PFNGLVERTEXATTRIB1DVPROC VertexAttrib1dv;
  PFNGLVERTEXATTRIB1FPROC VertexAttrib1f;
  PFNGLVERTEXATTRIB1FVPROC VertexAttrib1fv;
  PFNGLVERTEXATTRIB1SPROC VertexAttrib1s;
  PFNGLVERTEXATTRIB1SVPROC VertexAttrib1sv;
  PFNGLVERTEXATTRIB2DPROC VertexAttrib2d;
  PFNGLVERTEXATTRIB2DVPROC VertexAttrib2dv;
  PFNGLVERTEXATTRIB2FPROC VertexAttrib2f;
  PFNGLVERTEXATTRIB2FVPROC VertexAttrib2fv;
  PFNGLVERTEXATTRIB2SPROC VertexAttrib2s;
  PFNGLVERTEXATTRIB2SVPROC VertexAttrib2sv;
  PFNGLVERTEXATTRIB3DPROC VertexAttrib3d;
  PFNGLVERTEXATTRIB3DVPROC VertexAttrib3dv;
  PFNGLVERTEXATTRIB3FPROC VertexAttrib3f;
  PFNGLVERTEXATTRIB3FVPROC VertexAttrib3fv;
  PFNGLVERTEXATTRIB3SPROC VertexAttrib3s;
  PFNGLVERTEXATTRIB3SVPROC VertexAttrib3sv;
  PFNGLVERTEXATTRIB4NBVPROC VertexAttrib4Nbv;
  PFNGLVERTEXATTRIB4NIVPROC VertexAttrib4Niv;
  PFNGLVERTEXATTRIB4NSVPROC VertexAttrib4Nsv;
  PFNGLVERTEXATTRIB4NUBPROC VertexAttrib4Nub;
  PFNGLVERTEXATTRIB4NUBVPROC VertexAttrib4Nubv;
  PFNGLVERTEXATTRIB4NUIVPROC VertexAttrib4Nuiv;
  PFNGLVERTEXATTRIB4NUSVPROC VertexAttrib4Nusv;
  PFNGLVERTEXATTRIB4BVPROC VertexAttrib4bv;
  PFNGLVERTEXATTRIB4DPROC VertexAttrib4d;
  PFNGLVERTEXATTRIB4DVPROC VertexAttrib4dv;
  PFNGLVERTEXATTRIB4FPROC VertexAttrib4f;
  PFNGLVERTEXATTRIB4FVPROC VertexAttrib4fv;
  PFNGLVERTEXATTRIB4IVPROC VertexAttrib4iv;
  PFNGLVERTEXATTRIB4SPROC VertexAttrib4s;
  PFNGLVERTEXATTRIB4SVPROC VertexAttrib4sv;
  PFNGLVERTEXATTRIB4UBVPROC VertexAttrib4ubv;
  PFNGLVERTEXATTRIB4UIVPROC VertexAttrib4uiv;
  PFNGLVERTEXATTRIB4USVPROC VertexAttrib4usv;
  PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
  PFNGLUNIFORMMATRIX2X3FVPROC UniformMatrix2x3fv;
  PFNGLUNIFORMMATRIX3X2FVPROC UniformMatrix3x2fv;
  PFNGLUNIFORMMATRIX2X4FVPROC UniformMatrix2x4fv;
  PFNGLUNIFORMMATRIX4X2FVPROC UniformMatrix4x2fv;
  PFNGLUNIFORMMATRIX3X4FVPROC UniformMatrix3x4fv;
  PFNGLUNIFORMMATRIX4X3FVPROC UniformMatrix4x3fv;
  PFNGLACTIVETEXTUREARBPROC ActiveTextureARB;
  PFNGLCLIENTACTIVETEXTUREARBPROC ClientActiveTextureARB;
  PFNGLMULTITEXCOORD1DARBPROC MultiTexCoord1dARB;
  PFNGLMULTITEXCOORD1DVARBPROC MultiTexCoord1dvARB;
  PFNGLMULTITEXCOORD1FARBPROC MultiTexCoord1fARB;
  PFNGLMULTITEXCOORD1FVARBPROC MultiTexCoord1fvARB;
  PFNGLMULTITEXCOORD1IARBPROC MultiTexCoord1iARB;
  PFNGLMULTITEXCOORD1IVARBPROC MultiTexCoord1ivARB;
  PFNGLMULTITEXCOORD1SARBPROC MultiTexCoord1sARB;
  PFNGLMULTITEXCOORD1SVARBPROC MultiTexCoord1svARB;
  PFNGLMULTITEXCOORD2DARBPROC MultiTexCoord2dARB;
  PFNGLMULTITEXCOORD2DVARBPROC MultiTexCoord2dvARB;
  PFNGLMULTITEXCOORD2FARBPROC MultiTexCoord2fARB;
  PFNGLMULTITEXCOORD2FVARBPROC MultiTexCoord2fvARB;
  PFNGLMULTITEXCOORD2IARBPROC MultiTexCoord2iARB;
  PFNGLMULTITEXCOORD2IVARBPROC MultiTexCoord2ivARB;
  PFNGLMULTITEXCOORD2SARBPROC MultiTexCoord2sARB;
  PFNGLMULTITEXCOORD2SVARBPROC MultiTexCoord2svARB;
  PFNGLMULTITEXCOORD3DARBPROC MultiTexCoord3dARB;
  PFNGLMULTITEXCOORD3DVARBPROC MultiTexCoord3dvARB;
  PFNGLMULTITEXCOORD3FARBPROC MultiTexCoord3fARB;
  PFNGLMULTITEXCOORD3FVARBPROC MultiTexCoord3fvARB;
  PFNGLMULTITEXCOORD3IARBPROC MultiTexCoord3iARB;
  PFNGLMULTITEXCOORD3IVARBPROC MultiTexCoord3ivARB;
  PFNGLMULTITEXCOORD3SARBPROC MultiTexCoord3sARB;
  PFNGLMULTITEXCOORD3SVARBPROC MultiTexCoord3svARB;
  PFNGLMULTITEXCOORD4DARBPROC MultiTexCoord4dARB;
  PFNGLMULTITEXCOORD4DVARBPROC MultiTexCoord4dvARB;
  PFNGLMULTITEXCOORD4FARBPROC MultiTexCoord4fARB;
  PFNGLMULTITEXCOORD4FVARBPROC MultiTexCoord4fvARB;
  PFNGLMULTITEXCOORD4IARBPROC MultiTexCoord4iARB;
  PFNGLMULTITEXCOORD4IVARBPROC MultiTexCoord4ivARB;
  PFNGLMULTITEXCOORD4SARBPROC MultiTexCoord4sARB;
  PFNGLMULTITEXCOORD4SVARBPROC MultiTexCoord4svARB;
  PFNGLLOADTRANSPOSEMATRIXFARBPROC LoadTransposeMatrixfARB;
  PFNGLLOADTRANSPOSEMATRIXDARBPROC LoadTransposeMatrixdARB;
  PFNGLMULTTRANSPOSEMATRIXFARBPROC MultTransposeMatrixfARB;
  PFNGLMULTTRANSPOSEMATRIXDARBPROC MultTransposeMatrixdARB;
  PFNGLSAMPLECOVERAGEARBPROC SampleCoverageARB;
  PFNGLCOMPRESSEDTEXIMAGE3DARBPROC CompressedTexImage3DARB;
  PFNGLCOMPRESSEDTEXIMAGE2DARBPROC CompressedTexImage2DARB;
  PFNGLCOMPRESSEDTEXIMAGE1DARBPROC CompressedTexImage1DARB;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC CompressedTexSubImage3DARB;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC CompressedTexSubImage2DARB;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC CompressedTexSubImage1DARB;
  PFNGLGETCOMPRESSEDTEXIMAGEARBPROC GetCompressedTexImageARB;
  PFNGLPOINTPARAMETERFARBPROC PointParameterfARB;
  PFNGLPOINTPARAMETERFVARBPROC PointParameterfvARB;
  PFNGLWEIGHTBVARBPROC WeightbvARB;
  PFNGLWEIGHTSVARBPROC WeightsvARB;
  PFNGLWEIGHTIVARBPROC WeightivARB;
  PFNGLWEIGHTFVARBPROC WeightfvARB;
  PFNGLWEIGHTDVARBPROC WeightdvARB;
  PFNGLWEIGHTUBVARBPROC WeightubvARB;
  PFNGLWEIGHTUSVARBPROC WeightusvARB;
  PFNGLWEIGHTUIVARBPROC WeightuivARB;
  PFNGLWEIGHTPOINTERARBPROC WeightPointerARB;
  PFNGLVERTEXBLENDARBPROC VertexBlendARB;
  PFNGLCURRENTPALETTEMATRIXARBPROC CurrentPaletteMatrixARB;
  PFNGLMATRIXINDEXUBVARBPROC MatrixIndexubvARB;
  PFNGLMATRIXINDEXUSVARBPROC MatrixIndexusvARB;
  PFNGLMATRIXINDEXUIVARBPROC MatrixIndexuivARB;
  PFNGLMATRIXINDEXPOINTERARBPROC MatrixIndexPointerARB;
  PFNGLWINDOWPOS2DARBPROC WindowPos2dARB;
  PFNGLWINDOWPOS2DVARBPROC WindowPos2dvARB;
  PFNGLWINDOWPOS2FARBPROC WindowPos2fARB;
  PFNGLWINDOWPOS2FVARBPROC WindowPos2fvARB;
  PFNGLWINDOWPOS2IARBPROC WindowPos2iARB;
  PFNGLWINDOWPOS2IVARBPROC WindowPos2ivARB;
  PFNGLWINDOWPOS2SARBPROC WindowPos2sARB;
  PFNGLWINDOWPOS2SVARBPROC WindowPos2svARB;
  PFNGLWINDOWPOS3DARBPROC WindowPos3dARB;
  PFNGLWINDOWPOS3DVARBPROC WindowPos3dvARB;
  PFNGLWINDOWPOS3FARBPROC WindowPos3fARB;
  PFNGLWINDOWPOS3FVARBPROC WindowPos3fvARB;
  PFNGLWINDOWPOS3IARBPROC WindowPos3iARB;
  PFNGLWINDOWPOS3IVARBPROC WindowPos3ivARB;
  PFNGLWINDOWPOS3SARBPROC WindowPos3sARB;
  PFNGLWINDOWPOS3SVARBPROC WindowPos3svARB;
  PFNGLVERTEXATTRIB1DARBPROC VertexAttrib1dARB;
  PFNGLVERTEXATTRIB1DVARBPROC VertexAttrib1dvARB;
  PFNGLVERTEXATTRIB1FARBPROC VertexAttrib1fARB;
  PFNGLVERTEXATTRIB1FVARBPROC VertexAttrib1fvARB;
  PFNGLVERTEXATTRIB1SARBPROC VertexAttrib1sARB;
  PFNGLVERTEXATTRIB1SVARBPROC VertexAttrib1svARB;
  PFNGLVERTEXATTRIB2DARBPROC VertexAttrib2dARB;
  PFNGLVERTEXATTRIB2DVARBPROC VertexAttrib2dvARB;
  PFNGLVERTEXATTRIB2FARBPROC VertexAttrib2fARB;
  PFNGLVERTEXATTRIB2FVARBPROC VertexAttrib2fvARB;
  PFNGLVERTEXATTRIB2SARBPROC VertexAttrib2sARB;
  PFNGLVERTEXATTRIB2SVARBPROC VertexAttrib2svARB;
  PFNGLVERTEXATTRIB3DARBPROC VertexAttrib3dARB;
  PFNGLVERTEXATTRIB3DVARBPROC VertexAttrib3dvARB;
  PFNGLVERTEXATTRIB3FARBPROC VertexAttrib3fARB;
  PFNGLVERTEXATTRIB3FVARBPROC VertexAttrib3fvARB;
  PFNGLVERTEXATTRIB3SARBPROC VertexAttrib3sARB;
  PFNGLVERTEXATTRIB3SVARBPROC VertexAttrib3svARB;
  PFNGLVERTEXATTRIB4NBVARBPROC VertexAttrib4NbvARB;
  PFNGLVERTEXATTRIB4NIVARBPROC VertexAttrib4NivARB;
  PFNGLVERTEXATTRIB4NSVARBPROC VertexAttrib4NsvARB;
  PFNGLVERTEXATTRIB4NUBARBPROC VertexAttrib4NubARB;
  PFNGLVERTEXATTRIB4NUBVARBPROC VertexAttrib4NubvARB;
  PFNGLVERTEXATTRIB4NUIVARBPROC VertexAttrib4NuivARB;
  PFNGLVERTEXATTRIB4NUSVARBPROC VertexAttrib4NusvARB;
  PFNGLVERTEXATTRIB4BVARBPROC VertexAttrib4bvARB;
  PFNGLVERTEXATTRIB4DARBPROC VertexAttrib4dARB;
  PFNGLVERTEXATTRIB4DVARBPROC VertexAttrib4dvARB;
  PFNGLVERTEXATTRIB4FARBPROC VertexAttrib4fARB;
  PFNGLVERTEXATTRIB4FVARBPROC VertexAttrib4fvARB;
  PFNGLVERTEXATTRIB4IVARBPROC VertexAttrib4ivARB;
  PFNGLVERTEXATTRIB4SARBPROC VertexAttrib4sARB;
  PFNGLVERTEXATTRIB4SVARBPROC VertexAttrib4svARB;
  PFNGLVERTEXATTRIB4UBVARBPROC VertexAttrib4ubvARB;
  PFNGLVERTEXATTRIB4UIVARBPROC VertexAttrib4uivARB;
  PFNGLVERTEXATTRIB4USVARBPROC VertexAttrib4usvARB;
  PFNGLVERTEXATTRIBPOINTERARBPROC VertexAttribPointerARB;
  PFNGLENABLEVERTEXATTRIBARRAYARBPROC EnableVertexAttribArrayARB;
  PFNGLDISABLEVERTEXATTRIBARRAYARBPROC DisableVertexAttribArrayARB;
  PFNGLPROGRAMSTRINGARBPROC ProgramStringARB;
  PFNGLBINDPROGRAMARBPROC BindProgramARB;
  PFNGLDELETEPROGRAMSARBPROC DeleteProgramsARB;
  PFNGLGENPROGRAMSARBPROC GenProgramsARB;
  PFNGLPROGRAMENVPARAMETER4DARBPROC ProgramEnvParameter4dARB;
  PFNGLPROGRAMENVPARAMETER4DVARBPROC ProgramEnvParameter4dvARB;
  PFNGLPROGRAMENVPARAMETER4FARBPROC ProgramEnvParameter4fARB;
  PFNGLPROGRAMENVPARAMETER4FVARBPROC ProgramEnvParameter4fvARB;
  PFNGLPROGRAMLOCALPARAMETER4DARBPROC ProgramLocalParameter4dARB;
  PFNGLPROGRAMLOCALPARAMETER4DVARBPROC ProgramLocalParameter4dvARB;
  PFNGLPROGRAMLOCALPARAMETER4FARBPROC ProgramLocalParameter4fARB;
  PFNGLPROGRAMLOCALPARAMETER4FVARBPROC ProgramLocalParameter4fvARB;
  PFNGLGETPROGRAMENVPARAMETERDVARBPROC GetProgramEnvParameterdvARB;
  PFNGLGETPROGRAMENVPARAMETERFVARBPROC GetProgramEnvParameterfvARB;
  PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC GetProgramLocalParameterdvARB;
  PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC GetProgramLocalParameterfvARB;
  PFNGLGETPROGRAMIVARBPROC GetProgramivARB;
  PFNGLGETPROGRAMSTRINGARBPROC GetProgramStringARB;
  PFNGLGETVERTEXATTRIBDVARBPROC GetVertexAttribdvARB;
  PFNGLGETVERTEXATTRIBFVARBPROC GetVertexAttribfvARB;
  PFNGLGETVERTEXATTRIBIVARBPROC GetVertexAttribivARB;
  PFNGLGETVERTEXATTRIBPOINTERVARBPROC GetVertexAttribPointervARB;
  PFNGLISPROGRAMARBPROC IsProgramARB;
  PFNGLBINDBUFFERARBPROC BindBufferARB;
  PFNGLDELETEBUFFERSARBPROC DeleteBuffersARB;
  PFNGLGENBUFFERSARBPROC GenBuffersARB;
  PFNGLISBUFFERARBPROC IsBufferARB;
  PFNGLBUFFERDATAARBPROC BufferDataARB;
  PFNGLBUFFERSUBDATAARBPROC BufferSubDataARB;
  PFNGLGETBUFFERSUBDATAARBPROC GetBufferSubDataARB;
  PFNGLMAPBUFFERARBPROC MapBufferARB;
  PFNGLUNMAPBUFFERARBPROC UnmapBufferARB;
  PFNGLGETBUFFERPARAMETERIVARBPROC GetBufferParameterivARB;
  PFNGLGETBUFFERPOINTERVARBPROC GetBufferPointervARB;
  PFNGLGENQUERIESARBPROC GenQueriesARB;
  PFNGLDELETEQUERIESARBPROC DeleteQueriesARB;
  PFNGLISQUERYARBPROC IsQueryARB;
  PFNGLBEGINQUERYARBPROC BeginQueryARB;
  PFNGLENDQUERYARBPROC EndQueryARB;
  PFNGLGETQUERYIVARBPROC GetQueryivARB;
  PFNGLGETQUERYOBJECTIVARBPROC GetQueryObjectivARB;
  PFNGLGETQUERYOBJECTUIVARBPROC GetQueryObjectuivARB;
  PFNGLDELETEOBJECTARBPROC DeleteObjectARB;
  PFNGLGETHANDLEARBPROC GetHandleARB;
  PFNGLDETACHOBJECTARBPROC DetachObjectARB;
  PFNGLCREATESHADEROBJECTARBPROC CreateShaderObjectARB;
  PFNGLSHADERSOURCEARBPROC ShaderSourceARB;
  PFNGLCOMPILESHADERARBPROC CompileShaderARB;
  PFNGLCREATEPROGRAMOBJECTARBPROC CreateProgramObjectARB;
  PFNGLATTACHOBJECTARBPROC AttachObjectARB;
  PFNGLLINKPROGRAMARBPROC LinkProgramARB;
  PFNGLUSEPROGRAMOBJECTARBPROC UseProgramObjectARB;
  PFNGLVALIDATEPROGRAMARBPROC ValidateProgramARB;
  PFNGLUNIFORM1FARBPROC Uniform1fARB;
  PFNGLUNIFORM2FARBPROC Uniform2fARB;
  PFNGLUNIFORM3FARBPROC Uniform3fARB;
  PFNGLUNIFORM4FARBPROC Uniform4fARB;
  PFNGLUNIFORM1IARBPROC Uniform1iARB;
  PFNGLUNIFORM2IARBPROC Uniform2iARB;
  PFNGLUNIFORM3IARBPROC Uniform3iARB;
  PFNGLUNIFORM4IARBPROC Uniform4iARB;
  PFNGLUNIFORM1FVARBPROC Uniform1fvARB;
  PFNGLUNIFORM2FVARBPROC Uniform2fvARB;
  PFNGLUNIFORM3FVARBPROC Uniform3fvARB;
  PFNGLUNIFORM4FVARBPROC Uniform4fvARB;
  PFNGLUNIFORM1IVARBPROC Uniform1ivARB;
  PFNGLUNIFORM2IVARBPROC Uniform2ivARB;
  PFNGLUNIFORM3IVARBPROC Uniform3ivARB;
  PFNGLUNIFORM4IVARBPROC Uniform4ivARB;
  PFNGLUNIFORMMATRIX2FVARBPROC UniformMatrix2fvARB;
  PFNGLUNIFORMMATRIX3FVARBPROC UniformMatrix3fvARB;
  PFNGLUNIFORMMATRIX4FVARBPROC UniformMatrix4fvARB;
  PFNGLGETOBJECTPARAMETERFVARBPROC GetObjectParameterfvARB;
  PFNGLGETOBJECTPARAMETERIVARBPROC GetObjectParameterivARB;
  PFNGLGETINFOLOGARBPROC GetInfoLogARB;
  PFNGLGETATTACHEDOBJECTSARBPROC GetAttachedObjectsARB;
  PFNGLGETUNIFORMLOCATIONARBPROC GetUniformLocationARB;
  PFNGLGETACTIVEUNIFORMARBPROC GetActiveUniformARB;
  PFNGLGETUNIFORMFVARBPROC GetUniformfvARB;
  PFNGLGETUNIFORMIVARBPROC GetUniformivARB;
  PFNGLGETSHADERSOURCEARBPROC GetShaderSourceARB;
  PFNGLBINDATTRIBLOCATIONARBPROC BindAttribLocationARB;
  PFNGLGETACTIVEATTRIBARBPROC GetActiveAttribARB;
  PFNGLGETATTRIBLOCATIONARBPROC GetAttribLocationARB;
  PFNGLDRAWBUFFERSARBPROC DrawBuffersARB;
  PFNGLCLAMPCOLORARBPROC ClampColorARB;
  PFNGLBLENDCOLOREXTPROC BlendColorEXT;
  PFNGLPOLYGONOFFSETEXTPROC PolygonOffsetEXT;
  PFNGLTEXIMAGE3DEXTPROC TexImage3DEXT;
  PFNGLTEXSUBIMAGE3DEXTPROC TexSubImage3DEXT;
  PFNGLGETTEXFILTERFUNCSGISPROC GetTexFilterFuncSGIS;
  PFNGLTEXFILTERFUNCSGISPROC TexFilterFuncSGIS;
  PFNGLTEXSUBIMAGE1DEXTPROC TexSubImage1DEXT;
  PFNGLTEXSUBIMAGE2DEXTPROC TexSubImage2DEXT;
  PFNGLCOPYTEXIMAGE1DEXTPROC CopyTexImage1DEXT;
  PFNGLCOPYTEXIMAGE2DEXTPROC CopyTexImage2DEXT;
  PFNGLCOPYTEXSUBIMAGE1DEXTPROC CopyTexSubImage1DEXT;
  PFNGLCOPYTEXSUBIMAGE2DEXTPROC CopyTexSubImage2DEXT;
  PFNGLCOPYTEXSUBIMAGE3DEXTPROC CopyTexSubImage3DEXT;
  PFNGLGETHISTOGRAMEXTPROC GetHistogramEXT;
  PFNGLGETHISTOGRAMPARAMETERFVEXTPROC GetHistogramParameterfvEXT;
  PFNGLGETHISTOGRAMPARAMETERIVEXTPROC GetHistogramParameterivEXT;
  PFNGLGETMINMAXEXTPROC GetMinmaxEXT;
  PFNGLGETMINMAXPARAMETERFVEXTPROC GetMinmaxParameterfvEXT;
  PFNGLGETMINMAXPARAMETERIVEXTPROC GetMinmaxParameterivEXT;
  PFNGLHISTOGRAMEXTPROC HistogramEXT;
  PFNGLMINMAXEXTPROC MinmaxEXT;
  PFNGLRESETHISTOGRAMEXTPROC ResetHistogramEXT;
  PFNGLRESETMINMAXEXTPROC ResetMinmaxEXT;
  PFNGLCONVOLUTIONFILTER1DEXTPROC ConvolutionFilter1DEXT;
  PFNGLCONVOLUTIONFILTER2DEXTPROC ConvolutionFilter2DEXT;
  PFNGLCONVOLUTIONPARAMETERFEXTPROC ConvolutionParameterfEXT;
  PFNGLCONVOLUTIONPARAMETERFVEXTPROC ConvolutionParameterfvEXT;
  PFNGLCONVOLUTIONPARAMETERIEXTPROC ConvolutionParameteriEXT;
  PFNGLCONVOLUTIONPARAMETERIVEXTPROC ConvolutionParameterivEXT;
  PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC CopyConvolutionFilter1DEXT;
  PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC CopyConvolutionFilter2DEXT;
  PFNGLGETCONVOLUTIONFILTEREXTPROC GetConvolutionFilterEXT;
  PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC GetConvolutionParameterfvEXT;
  PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC GetConvolutionParameterivEXT;
  PFNGLGETSEPARABLEFILTEREXTPROC GetSeparableFilterEXT;
  PFNGLSEPARABLEFILTER2DEXTPROC SeparableFilter2DEXT;
  PFNGLCOLORTABLESGIPROC ColorTableSGI;
  PFNGLCOLORTABLEPARAMETERFVSGIPROC ColorTableParameterfvSGI;
  PFNGLCOLORTABLEPARAMETERIVSGIPROC ColorTableParameterivSGI;
  PFNGLCOPYCOLORTABLESGIPROC CopyColorTableSGI;
  PFNGLGETCOLORTABLESGIPROC GetColorTableSGI;
  PFNGLGETCOLORTABLEPARAMETERFVSGIPROC GetColorTableParameterfvSGI;
  PFNGLGETCOLORTABLEPARAMETERIVSGIPROC GetColorTableParameterivSGI;
  PFNGLPIXELTEXGENSGIXPROC PixelTexGenSGIX;
  PFNGLPIXELTEXGENPARAMETERISGISPROC PixelTexGenParameteriSGIS;
  PFNGLPIXELTEXGENPARAMETERIVSGISPROC PixelTexGenParameterivSGIS;
  PFNGLPIXELTEXGENPARAMETERFSGISPROC PixelTexGenParameterfSGIS;
  PFNGLPIXELTEXGENPARAMETERFVSGISPROC PixelTexGenParameterfvSGIS;
  PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC GetPixelTexGenParameterivSGIS;
  PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC GetPixelTexGenParameterfvSGIS;
  PFNGLTEXIMAGE4DSGISPROC TexImage4DSGIS;
  PFNGLTEXSUBIMAGE4DSGISPROC TexSubImage4DSGIS;
  PFNGLARETEXTURESRESIDENTEXTPROC AreTexturesResidentEXT;
  PFNGLBINDTEXTUREEXTPROC BindTextureEXT;
  PFNGLDELETETEXTURESEXTPROC DeleteTexturesEXT;
  PFNGLGENTEXTURESEXTPROC GenTexturesEXT;
  PFNGLISTEXTUREEXTPROC IsTextureEXT;
  PFNGLPRIORITIZETEXTURESEXTPROC PrioritizeTexturesEXT;
  PFNGLDETAILTEXFUNCSGISPROC DetailTexFuncSGIS;
  PFNGLGETDETAILTEXFUNCSGISPROC GetDetailTexFuncSGIS;
  PFNGLSHARPENTEXFUNCSGISPROC SharpenTexFuncSGIS;
  PFNGLGETSHARPENTEXFUNCSGISPROC GetSharpenTexFuncSGIS;
  PFNGLSAMPLEMASKSGISPROC SampleMaskSGIS;
  PFNGLSAMPLEPATTERNSGISPROC SamplePatternSGIS;
  PFNGLARRAYELEMENTEXTPROC ArrayElementEXT;
  PFNGLCOLORPOINTEREXTPROC ColorPointerEXT;
  PFNGLDRAWARRAYSEXTPROC DrawArraysEXT;
  PFNGLEDGEFLAGPOINTEREXTPROC EdgeFlagPointerEXT;
  PFNGLGETPOINTERVEXTPROC GetPointervEXT;
  PFNGLINDEXPOINTEREXTPROC IndexPointerEXT;
  PFNGLNORMALPOINTEREXTPROC NormalPointerEXT;
  PFNGLTEXCOORDPOINTEREXTPROC TexCoordPointerEXT;
  PFNGLVERTEXPOINTEREXTPROC VertexPointerEXT;
  PFNGLBLENDEQUATIONEXTPROC BlendEquationEXT;
  PFNGLSPRITEPARAMETERFSGIXPROC SpriteParameterfSGIX;
  PFNGLSPRITEPARAMETERFVSGIXPROC SpriteParameterfvSGIX;
  PFNGLSPRITEPARAMETERISGIXPROC SpriteParameteriSGIX;
  PFNGLSPRITEPARAMETERIVSGIXPROC SpriteParameterivSGIX;
  PFNGLPOINTPARAMETERFEXTPROC PointParameterfEXT;
  PFNGLPOINTPARAMETERFVEXTPROC PointParameterfvEXT;
  PFNGLPOINTPARAMETERFSGISPROC PointParameterfSGIS;
  PFNGLPOINTPARAMETERFVSGISPROC PointParameterfvSGIS;
  PFNGLGETINSTRUMENTSSGIXPROC GetInstrumentsSGIX;
  PFNGLINSTRUMENTSBUFFERSGIXPROC InstrumentsBufferSGIX;
  PFNGLPOLLINSTRUMENTSSGIXPROC PollInstrumentsSGIX;
  PFNGLREADINSTRUMENTSSGIXPROC ReadInstrumentsSGIX;
  PFNGLSTARTINSTRUMENTSSGIXPROC StartInstrumentsSGIX;
  PFNGLSTOPINSTRUMENTSSGIXPROC StopInstrumentsSGIX;
  PFNGLFRAMEZOOMSGIXPROC FrameZoomSGIX;
  PFNGLTAGSAMPLEBUFFERSGIXPROC TagSampleBufferSGIX;
  PFNGLDEFORMATIONMAP3DSGIXPROC DeformationMap3dSGIX;
  PFNGLDEFORMATIONMAP3FSGIXPROC DeformationMap3fSGIX;
  PFNGLDEFORMSGIXPROC DeformSGIX;
  PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC LoadIdentityDeformationMapSGIX;
  PFNGLREFERENCEPLANESGIXPROC ReferencePlaneSGIX;
  PFNGLFLUSHRASTERSGIXPROC FlushRasterSGIX;
  PFNGLFOGFUNCSGISPROC FogFuncSGIS;
  PFNGLGETFOGFUNCSGISPROC GetFogFuncSGIS;
  PFNGLIMAGETRANSFORMPARAMETERIHPPROC ImageTransformParameteriHP;
  PFNGLIMAGETRANSFORMPARAMETERFHPPROC ImageTransformParameterfHP;
  PFNGLIMAGETRANSFORMPARAMETERIVHPPROC ImageTransformParameterivHP;
  PFNGLIMAGETRANSFORMPARAMETERFVHPPROC ImageTransformParameterfvHP;
  PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC GetImageTransformParameterivHP;
  PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC GetImageTransformParameterfvHP;
  PFNGLCOLORSUBTABLEEXTPROC ColorSubTableEXT;
  PFNGLCOPYCOLORSUBTABLEEXTPROC CopyColorSubTableEXT;
  PFNGLHINTPGIPROC HintPGI;
  PFNGLCOLORTABLEEXTPROC ColorTableEXT;
  PFNGLGETCOLORTABLEEXTPROC GetColorTableEXT;
  PFNGLGETCOLORTABLEPARAMETERIVEXTPROC GetColorTableParameterivEXT;
  PFNGLGETCOLORTABLEPARAMETERFVEXTPROC GetColorTableParameterfvEXT;
  PFNGLGETLISTPARAMETERFVSGIXPROC GetListParameterfvSGIX;
  PFNGLGETLISTPARAMETERIVSGIXPROC GetListParameterivSGIX;
  PFNGLLISTPARAMETERFSGIXPROC ListParameterfSGIX;
  PFNGLLISTPARAMETERFVSGIXPROC ListParameterfvSGIX;
  PFNGLLISTPARAMETERISGIXPROC ListParameteriSGIX;
  PFNGLLISTPARAMETERIVSGIXPROC ListParameterivSGIX;
  PFNGLINDEXMATERIALEXTPROC IndexMaterialEXT;
  PFNGLINDEXFUNCEXTPROC IndexFuncEXT;
  PFNGLLOCKARRAYSEXTPROC LockArraysEXT;
  PFNGLUNLOCKARRAYSEXTPROC UnlockArraysEXT;
  PFNGLCULLPARAMETERDVEXTPROC CullParameterdvEXT;
  PFNGLCULLPARAMETERFVEXTPROC CullParameterfvEXT;
  PFNGLFRAGMENTCOLORMATERIALSGIXPROC FragmentColorMaterialSGIX;
  PFNGLFRAGMENTLIGHTFSGIXPROC FragmentLightfSGIX;
  PFNGLFRAGMENTLIGHTFVSGIXPROC FragmentLightfvSGIX;
  PFNGLFRAGMENTLIGHTISGIXPROC FragmentLightiSGIX;
  PFNGLFRAGMENTLIGHTIVSGIXPROC FragmentLightivSGIX;
  PFNGLFRAGMENTLIGHTMODELFSGIXPROC FragmentLightModelfSGIX;
  PFNGLFRAGMENTLIGHTMODELFVSGIXPROC FragmentLightModelfvSGIX;
  PFNGLFRAGMENTLIGHTMODELISGIXPROC FragmentLightModeliSGIX;
  PFNGLFRAGMENTLIGHTMODELIVSGIXPROC FragmentLightModelivSGIX;
  PFNGLFRAGMENTMATERIALFSGIXPROC FragmentMaterialfSGIX;
  PFNGLFRAGMENTMATERIALFVSGIXPROC FragmentMaterialfvSGIX;
  PFNGLFRAGMENTMATERIALISGIXPROC FragmentMaterialiSGIX;
  PFNGLFRAGMENTMATERIALIVSGIXPROC FragmentMaterialivSGIX;
  PFNGLGETFRAGMENTLIGHTFVSGIXPROC GetFragmentLightfvSGIX;
  PFNGLGETFRAGMENTLIGHTIVSGIXPROC GetFragmentLightivSGIX;
  PFNGLGETFRAGMENTMATERIALFVSGIXPROC GetFragmentMaterialfvSGIX;
  PFNGLGETFRAGMENTMATERIALIVSGIXPROC GetFragmentMaterialivSGIX;
  PFNGLLIGHTENVISGIXPROC LightEnviSGIX;
  PFNGLDRAWRANGEELEMENTSEXTPROC DrawRangeElementsEXT;
  PFNGLAPPLYTEXTUREEXTPROC ApplyTextureEXT;
  PFNGLTEXTURELIGHTEXTPROC TextureLightEXT;
  PFNGLTEXTUREMATERIALEXTPROC TextureMaterialEXT;
  PFNGLASYNCMARKERSGIXPROC AsyncMarkerSGIX;
  PFNGLFINISHASYNCSGIXPROC FinishAsyncSGIX;
  PFNGLPOLLASYNCSGIXPROC PollAsyncSGIX;
  PFNGLGENASYNCMARKERSSGIXPROC GenAsyncMarkersSGIX;
  PFNGLDELETEASYNCMARKERSSGIXPROC DeleteAsyncMarkersSGIX;
  PFNGLISASYNCMARKERSGIXPROC IsAsyncMarkerSGIX;
  PFNGLVERTEXPOINTERVINTELPROC VertexPointervINTEL;
  PFNGLNORMALPOINTERVINTELPROC NormalPointervINTEL;
  PFNGLCOLORPOINTERVINTELPROC ColorPointervINTEL;
  PFNGLTEXCOORDPOINTERVINTELPROC TexCoordPointervINTEL;
  PFNGLPIXELTRANSFORMPARAMETERIEXTPROC PixelTransformParameteriEXT;
  PFNGLPIXELTRANSFORMPARAMETERFEXTPROC PixelTransformParameterfEXT;
  PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC PixelTransformParameterivEXT;
  PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC PixelTransformParameterfvEXT;
  PFNGLSECONDARYCOLOR3BEXTPROC SecondaryColor3bEXT;
  PFNGLSECONDARYCOLOR3BVEXTPROC SecondaryColor3bvEXT;
  PFNGLSECONDARYCOLOR3DEXTPROC SecondaryColor3dEXT;
  PFNGLSECONDARYCOLOR3DVEXTPROC SecondaryColor3dvEXT;
  PFNGLSECONDARYCOLOR3FEXTPROC SecondaryColor3fEXT;
  PFNGLSECONDARYCOLOR3FVEXTPROC SecondaryColor3fvEXT;
  PFNGLSECONDARYCOLOR3IEXTPROC SecondaryColor3iEXT;
  PFNGLSECONDARYCOLOR3IVEXTPROC SecondaryColor3ivEXT;
  PFNGLSECONDARYCOLOR3SEXTPROC SecondaryColor3sEXT;
  PFNGLSECONDARYCOLOR3SVEXTPROC SecondaryColor3svEXT;
  PFNGLSECONDARYCOLOR3UBEXTPROC SecondaryColor3ubEXT;
  PFNGLSECONDARYCOLOR3UBVEXTPROC SecondaryColor3ubvEXT;
  PFNGLSECONDARYCOLOR3UIEXTPROC SecondaryColor3uiEXT;
  PFNGLSECONDARYCOLOR3UIVEXTPROC SecondaryColor3uivEXT;
  PFNGLSECONDARYCOLOR3USEXTPROC SecondaryColor3usEXT;
  PFNGLSECONDARYCOLOR3USVEXTPROC SecondaryColor3usvEXT;
  PFNGLSECONDARYCOLORPOINTEREXTPROC SecondaryColorPointerEXT;
  PFNGLTEXTURENORMALEXTPROC TextureNormalEXT;
  PFNGLMULTIDRAWARRAYSEXTPROC MultiDrawArraysEXT;
  PFNGLMULTIDRAWELEMENTSEXTPROC MultiDrawElementsEXT;
  PFNGLFOGCOORDFEXTPROC FogCoordfEXT;
  PFNGLFOGCOORDFVEXTPROC FogCoordfvEXT;
  PFNGLFOGCOORDDEXTPROC FogCoorddEXT;
  PFNGLFOGCOORDDVEXTPROC FogCoorddvEXT;
  PFNGLFOGCOORDPOINTEREXTPROC FogCoordPointerEXT;
  PFNGLTANGENT3BEXTPROC Tangent3bEXT;
  PFNGLTANGENT3BVEXTPROC Tangent3bvEXT;
  PFNGLTANGENT3DEXTPROC Tangent3dEXT;
  PFNGLTANGENT3DVEXTPROC Tangent3dvEXT;
  PFNGLTANGENT3FEXTPROC Tangent3fEXT;
  PFNGLTANGENT3FVEXTPROC Tangent3fvEXT;
  PFNGLTANGENT3IEXTPROC Tangent3iEXT;
  PFNGLTANGENT3IVEXTPROC Tangent3ivEXT;
  PFNGLTANGENT3SEXTPROC Tangent3sEXT;
  PFNGLTANGENT3SVEXTPROC Tangent3svEXT;
  PFNGLBINORMAL3BEXTPROC Binormal3bEXT;
  PFNGLBINORMAL3BVEXTPROC Binormal3bvEXT;
  PFNGLBINORMAL3DEXTPROC Binormal3dEXT;
  PFNGLBINORMAL3DVEXTPROC Binormal3dvEXT;
  PFNGLBINORMAL3FEXTPROC Binormal3fEXT;
  PFNGLBINORMAL3FVEXTPROC Binormal3fvEXT;
  PFNGLBINORMAL3IEXTPROC Binormal3iEXT;
  PFNGLBINORMAL3IVEXTPROC Binormal3ivEXT;
  PFNGLBINORMAL3SEXTPROC Binormal3sEXT;
  PFNGLBINORMAL3SVEXTPROC Binormal3svEXT;
  PFNGLTANGENTPOINTEREXTPROC TangentPointerEXT;
  PFNGLBINORMALPOINTEREXTPROC BinormalPointerEXT;
  PFNGLFINISHTEXTURESUNXPROC FinishTextureSUNX;
  PFNGLGLOBALALPHAFACTORBSUNPROC GlobalAlphaFactorbSUN;
  PFNGLGLOBALALPHAFACTORSSUNPROC GlobalAlphaFactorsSUN;
  PFNGLGLOBALALPHAFACTORISUNPROC GlobalAlphaFactoriSUN;
  PFNGLGLOBALALPHAFACTORFSUNPROC GlobalAlphaFactorfSUN;
  PFNGLGLOBALALPHAFACTORDSUNPROC GlobalAlphaFactordSUN;
  PFNGLGLOBALALPHAFACTORUBSUNPROC GlobalAlphaFactorubSUN;
  PFNGLGLOBALALPHAFACTORUSSUNPROC GlobalAlphaFactorusSUN;
  PFNGLGLOBALALPHAFACTORUISUNPROC GlobalAlphaFactoruiSUN;
  PFNGLREPLACEMENTCODEUISUNPROC ReplacementCodeuiSUN;
  PFNGLREPLACEMENTCODEUSSUNPROC ReplacementCodeusSUN;
  PFNGLREPLACEMENTCODEUBSUNPROC ReplacementCodeubSUN;
  PFNGLREPLACEMENTCODEUIVSUNPROC ReplacementCodeuivSUN;
  PFNGLREPLACEMENTCODEUSVSUNPROC ReplacementCodeusvSUN;
  PFNGLREPLACEMENTCODEUBVSUNPROC ReplacementCodeubvSUN;
  PFNGLREPLACEMENTCODEPOINTERSUNPROC ReplacementCodePointerSUN;
  PFNGLCOLOR4UBVERTEX2FSUNPROC Color4ubVertex2fSUN;
  PFNGLCOLOR4UBVERTEX2FVSUNPROC Color4ubVertex2fvSUN;
  PFNGLCOLOR4UBVERTEX3FSUNPROC Color4ubVertex3fSUN;
  PFNGLCOLOR4UBVERTEX3FVSUNPROC Color4ubVertex3fvSUN;
  PFNGLCOLOR3FVERTEX3FSUNPROC Color3fVertex3fSUN;
  PFNGLCOLOR3FVERTEX3FVSUNPROC Color3fVertex3fvSUN;
  PFNGLNORMAL3FVERTEX3FSUNPROC Normal3fVertex3fSUN;
  PFNGLNORMAL3FVERTEX3FVSUNPROC Normal3fVertex3fvSUN;
  PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC Color4fNormal3fVertex3fSUN;
  PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC Color4fNormal3fVertex3fvSUN;
  PFNGLTEXCOORD2FVERTEX3FSUNPROC TexCoord2fVertex3fSUN;
  PFNGLTEXCOORD2FVERTEX3FVSUNPROC TexCoord2fVertex3fvSUN;
  PFNGLTEXCOORD4FVERTEX4FSUNPROC TexCoord4fVertex4fSUN;
  PFNGLTEXCOORD4FVERTEX4FVSUNPROC TexCoord4fVertex4fvSUN;
  PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC TexCoord2fColor4ubVertex3fSUN;
  PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC TexCoord2fColor4ubVertex3fvSUN;
  PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC TexCoord2fColor3fVertex3fSUN;
  PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC TexCoord2fColor3fVertex3fvSUN;
  PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC TexCoord2fNormal3fVertex3fSUN;
  PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC TexCoord2fNormal3fVertex3fvSUN;
  PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC TexCoord2fColor4fNormal3fVertex3fSUN;
  PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC TexCoord2fColor4fNormal3fVertex3fvSUN;
  PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC TexCoord4fColor4fNormal3fVertex4fSUN;
  PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC TexCoord4fColor4fNormal3fVertex4fvSUN;
  PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC ReplacementCodeuiVertex3fSUN;
  PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC ReplacementCodeuiVertex3fvSUN;
  PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC ReplacementCodeuiColor4ubVertex3fSUN;
  PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC ReplacementCodeuiColor4ubVertex3fvSUN;
  PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC ReplacementCodeuiColor3fVertex3fSUN;
  PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC ReplacementCodeuiColor3fVertex3fvSUN;
  PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC ReplacementCodeuiNormal3fVertex3fSUN;
  PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC ReplacementCodeuiNormal3fVertex3fvSUN;
  PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC ReplacementCodeuiColor4fNormal3fVertex3fSUN;
  PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC ReplacementCodeuiColor4fNormal3fVertex3fvSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC ReplacementCodeuiTexCoord2fVertex3fSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC ReplacementCodeuiTexCoord2fVertex3fvSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN;
  PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN;
  PFNGLBLENDFUNCSEPARATEEXTPROC BlendFuncSeparateEXT;
  PFNGLBLENDFUNCSEPARATEINGRPROC BlendFuncSeparateINGR;
  PFNGLVERTEXWEIGHTFEXTPROC VertexWeightfEXT;
  PFNGLVERTEXWEIGHTFVEXTPROC VertexWeightfvEXT;
  PFNGLVERTEXWEIGHTPOINTEREXTPROC VertexWeightPointerEXT;
  PFNGLFLUSHVERTEXARRAYRANGENVPROC FlushVertexArrayRangeNV;
  PFNGLVERTEXARRAYRANGENVPROC VertexArrayRangeNV;
  PFNGLCOMBINERPARAMETERFVNVPROC CombinerParameterfvNV;
  PFNGLCOMBINERPARAMETERFNVPROC CombinerParameterfNV;
  PFNGLCOMBINERPARAMETERIVNVPROC CombinerParameterivNV;
  PFNGLCOMBINERPARAMETERINVPROC CombinerParameteriNV;
  PFNGLCOMBINERINPUTNVPROC CombinerInputNV;
  PFNGLCOMBINEROUTPUTNVPROC CombinerOutputNV;
  PFNGLFINALCOMBINERINPUTNVPROC FinalCombinerInputNV;
  PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC GetCombinerInputParameterfvNV;
  PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC GetCombinerInputParameterivNV;
  PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC GetCombinerOutputParameterfvNV;
  PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC GetCombinerOutputParameterivNV;
  PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC GetFinalCombinerInputParameterfvNV;
  PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC GetFinalCombinerInputParameterivNV;
  PFNGLRESIZEBUFFERSMESAPROC ResizeBuffersMESA;
  PFNGLWINDOWPOS2DMESAPROC WindowPos2dMESA;
  PFNGLWINDOWPOS2DVMESAPROC WindowPos2dvMESA;
  PFNGLWINDOWPOS2FMESAPROC WindowPos2fMESA;
  PFNGLWINDOWPOS2FVMESAPROC WindowPos2fvMESA;
  PFNGLWINDOWPOS2IMESAPROC WindowPos2iMESA;
  PFNGLWINDOWPOS2IVMESAPROC WindowPos2ivMESA;
  PFNGLWINDOWPOS2SMESAPROC WindowPos2sMESA;
  PFNGLWINDOWPOS2SVMESAPROC WindowPos2svMESA;
  PFNGLWINDOWPOS3DMESAPROC WindowPos3dMESA;
  PFNGLWINDOWPOS3DVMESAPROC WindowPos3dvMESA;
  PFNGLWINDOWPOS3FMESAPROC WindowPos3fMESA;
  PFNGLWINDOWPOS3FVMESAPROC WindowPos3fvMESA;
  PFNGLWINDOWPOS3IMESAPROC WindowPos3iMESA;
  PFNGLWINDOWPOS3IVMESAPROC WindowPos3ivMESA;
  PFNGLWINDOWPOS3SMESAPROC WindowPos3sMESA;
  PFNGLWINDOWPOS3SVMESAPROC WindowPos3svMESA;
  PFNGLWINDOWPOS4DMESAPROC WindowPos4dMESA;
  PFNGLWINDOWPOS4DVMESAPROC WindowPos4dvMESA;
  PFNGLWINDOWPOS4FMESAPROC WindowPos4fMESA;
  PFNGLWINDOWPOS4FVMESAPROC WindowPos4fvMESA;
  PFNGLWINDOWPOS4IMESAPROC WindowPos4iMESA;
  PFNGLWINDOWPOS4IVMESAPROC WindowPos4ivMESA;
  PFNGLWINDOWPOS4SMESAPROC WindowPos4sMESA;
  PFNGLWINDOWPOS4SVMESAPROC WindowPos4svMESA;
  PFNGLMULTIMODEDRAWARRAYSIBMPROC MultiModeDrawArraysIBM;
  PFNGLMULTIMODEDRAWELEMENTSIBMPROC MultiModeDrawElementsIBM;
  PFNGLCOLORPOINTERLISTIBMPROC ColorPointerListIBM;
  PFNGLSECONDARYCOLORPOINTERLISTIBMPROC SecondaryColorPointerListIBM;
  PFNGLEDGEFLAGPOINTERLISTIBMPROC EdgeFlagPointerListIBM;
  PFNGLFOGCOORDPOINTERLISTIBMPROC FogCoordPointerListIBM;
  PFNGLINDEXPOINTERLISTIBMPROC IndexPointerListIBM;
  PFNGLNORMALPOINTERLISTIBMPROC NormalPointerListIBM;
  PFNGLTEXCOORDPOINTERLISTIBMPROC TexCoordPointerListIBM;
  PFNGLVERTEXPOINTERLISTIBMPROC VertexPointerListIBM;
  PFNGLTBUFFERMASK3DFXPROC TbufferMask3DFX;
  PFNGLSAMPLEMASKEXTPROC SampleMaskEXT;
  PFNGLSAMPLEPATTERNEXTPROC SamplePatternEXT;
  PFNGLTEXTURECOLORMASKSGISPROC TextureColorMaskSGIS;
  PFNGLIGLOOINTERFACESGIXPROC IglooInterfaceSGIX;
  PFNGLDELETEFENCESNVPROC DeleteFencesNV;
  PFNGLGENFENCESNVPROC GenFencesNV;
  PFNGLISFENCENVPROC IsFenceNV;
  PFNGLTESTFENCENVPROC TestFenceNV;
  PFNGLGETFENCEIVNVPROC GetFenceivNV;
  PFNGLFINISHFENCENVPROC FinishFenceNV;
  PFNGLSETFENCENVPROC SetFenceNV;
  PFNGLMAPCONTROLPOINTSNVPROC MapControlPointsNV;
  PFNGLMAPPARAMETERIVNVPROC MapParameterivNV;
  PFNGLMAPPARAMETERFVNVPROC MapParameterfvNV;
  PFNGLGETMAPCONTROLPOINTSNVPROC GetMapControlPointsNV;
  PFNGLGETMAPPARAMETERIVNVPROC GetMapParameterivNV;
  PFNGLGETMAPPARAMETERFVNVPROC GetMapParameterfvNV;
  PFNGLGETMAPATTRIBPARAMETERIVNVPROC GetMapAttribParameterivNV;
  PFNGLGETMAPATTRIBPARAMETERFVNVPROC GetMapAttribParameterfvNV;
  PFNGLEVALMAPSNVPROC EvalMapsNV;
  PFNGLCOMBINERSTAGEPARAMETERFVNVPROC CombinerStageParameterfvNV;
  PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC GetCombinerStageParameterfvNV;
  PFNGLAREPROGRAMSRESIDENTNVPROC AreProgramsResidentNV;
  PFNGLBINDPROGRAMNVPROC BindProgramNV;
  PFNGLDELETEPROGRAMSNVPROC DeleteProgramsNV;
  PFNGLEXECUTEPROGRAMNVPROC ExecuteProgramNV;
  PFNGLGENPROGRAMSNVPROC GenProgramsNV;
  PFNGLGETPROGRAMPARAMETERDVNVPROC GetProgramParameterdvNV;
  PFNGLGETPROGRAMPARAMETERFVNVPROC GetProgramParameterfvNV;
  PFNGLGETPROGRAMIVNVPROC GetProgramivNV;
  PFNGLGETPROGRAMSTRINGNVPROC GetProgramStringNV;
  PFNGLGETTRACKMATRIXIVNVPROC GetTrackMatrixivNV;
  PFNGLGETVERTEXATTRIBDVNVPROC GetVertexAttribdvNV;
  PFNGLGETVERTEXATTRIBFVNVPROC GetVertexAttribfvNV;
  PFNGLGETVERTEXATTRIBIVNVPROC GetVertexAttribivNV;
  PFNGLGETVERTEXATTRIBPOINTERVNVPROC GetVertexAttribPointervNV;
  PFNGLISPROGRAMNVPROC IsProgramNV;
  PFNGLLOADPROGRAMNVPROC LoadProgramNV;
  PFNGLPROGRAMPARAMETER4DNVPROC ProgramParameter4dNV;
  PFNGLPROGRAMPARAMETER4DVNVPROC ProgramParameter4dvNV;
  PFNGLPROGRAMPARAMETER4FNVPROC ProgramParameter4fNV;
  PFNGLPROGRAMPARAMETER4FVNVPROC ProgramParameter4fvNV;
  PFNGLPROGRAMPARAMETERS4DVNVPROC ProgramParameters4dvNV;
  PFNGLPROGRAMPARAMETERS4FVNVPROC ProgramParameters4fvNV;
  PFNGLREQUESTRESIDENTPROGRAMSNVPROC RequestResidentProgramsNV;
  PFNGLTRACKMATRIXNVPROC TrackMatrixNV;
  PFNGLVERTEXATTRIBPOINTERNVPROC VertexAttribPointerNV;
  PFNGLVERTEXATTRIB1DNVPROC VertexAttrib1dNV;
  PFNGLVERTEXATTRIB1DVNVPROC VertexAttrib1dvNV;
  PFNGLVERTEXATTRIB1FNVPROC VertexAttrib1fNV;
  PFNGLVERTEXATTRIB1FVNVPROC VertexAttrib1fvNV;
  PFNGLVERTEXATTRIB1SNVPROC VertexAttrib1sNV;
  PFNGLVERTEXATTRIB1SVNVPROC VertexAttrib1svNV;
  PFNGLVERTEXATTRIB2DNVPROC VertexAttrib2dNV;
  PFNGLVERTEXATTRIB2DVNVPROC VertexAttrib2dvNV;
  PFNGLVERTEXATTRIB2FNVPROC VertexAttrib2fNV;
  PFNGLVERTEXATTRIB2FVNVPROC VertexAttrib2fvNV;
  PFNGLVERTEXATTRIB2SNVPROC VertexAttrib2sNV;
  PFNGLVERTEXATTRIB2SVNVPROC VertexAttrib2svNV;
  PFNGLVERTEXATTRIB3DNVPROC VertexAttrib3dNV;
  PFNGLVERTEXATTRIB3DVNVPROC VertexAttrib3dvNV;
  PFNGLVERTEXATTRIB3FNVPROC VertexAttrib3fNV;
  PFNGLVERTEXATTRIB3FVNVPROC VertexAttrib3fvNV;
  PFNGLVERTEXATTRIB3SNVPROC VertexAttrib3sNV;
  PFNGLVERTEXATTRIB3SVNVPROC VertexAttrib3svNV;
  PFNGLVERTEXATTRIB4DNVPROC VertexAttrib4dNV;
  PFNGLVERTEXATTRIB4DVNVPROC VertexAttrib4dvNV;
  PFNGLVERTEXATTRIB4FNVPROC VertexAttrib4fNV;
  PFNGLVERTEXATTRIB4FVNVPROC VertexAttrib4fvNV;
  PFNGLVERTEXATTRIB4SNVPROC VertexAttrib4sNV;
  PFNGLVERTEXATTRIB4SVNVPROC VertexAttrib4svNV;
  PFNGLVERTEXATTRIB4UBNVPROC VertexAttrib4ubNV;
  PFNGLVERTEXATTRIB4UBVNVPROC VertexAttrib4ubvNV;
  PFNGLVERTEXATTRIBS1DVNVPROC VertexAttribs1dvNV;
  PFNGLVERTEXATTRIBS1FVNVPROC VertexAttribs1fvNV;
  PFNGLVERTEXATTRIBS1SVNVPROC VertexAttribs1svNV;
  PFNGLVERTEXATTRIBS2DVNVPROC VertexAttribs2dvNV;
  PFNGLVERTEXATTRIBS2FVNVPROC VertexAttribs2fvNV;
  PFNGLVERTEXATTRIBS2SVNVPROC VertexAttribs2svNV;
  PFNGLVERTEXATTRIBS3DVNVPROC VertexAttribs3dvNV;
  PFNGLVERTEXATTRIBS3FVNVPROC VertexAttribs3fvNV;
  PFNGLVERTEXATTRIBS3SVNVPROC VertexAttribs3svNV;
  PFNGLVERTEXATTRIBS4DVNVPROC VertexAttribs4dvNV;
  PFNGLVERTEXATTRIBS4FVNVPROC VertexAttribs4fvNV;
  PFNGLVERTEXATTRIBS4SVNVPROC VertexAttribs4svNV;
  PFNGLVERTEXATTRIBS4UBVNVPROC VertexAttribs4ubvNV;
  PFNGLTEXBUMPPARAMETERIVATIPROC TexBumpParameterivATI;
  PFNGLTEXBUMPPARAMETERFVATIPROC TexBumpParameterfvATI;
  PFNGLGETTEXBUMPPARAMETERIVATIPROC GetTexBumpParameterivATI;
  PFNGLGETTEXBUMPPARAMETERFVATIPROC GetTexBumpParameterfvATI;
  PFNGLGENFRAGMENTSHADERSATIPROC GenFragmentShadersATI;
  PFNGLBINDFRAGMENTSHADERATIPROC BindFragmentShaderATI;
  PFNGLDELETEFRAGMENTSHADERATIPROC DeleteFragmentShaderATI;
  PFNGLBEGINFRAGMENTSHADERATIPROC BeginFragmentShaderATI;
  PFNGLENDFRAGMENTSHADERATIPROC EndFragmentShaderATI;
  PFNGLPASSTEXCOORDATIPROC PassTexCoordATI;
  PFNGLSAMPLEMAPATIPROC SampleMapATI;
  PFNGLCOLORFRAGMENTOP1ATIPROC ColorFragmentOp1ATI;
  PFNGLCOLORFRAGMENTOP2ATIPROC ColorFragmentOp2ATI;
  PFNGLCOLORFRAGMENTOP3ATIPROC ColorFragmentOp3ATI;
  PFNGLALPHAFRAGMENTOP1ATIPROC AlphaFragmentOp1ATI;
  PFNGLALPHAFRAGMENTOP2ATIPROC AlphaFragmentOp2ATI;
  PFNGLALPHAFRAGMENTOP3ATIPROC AlphaFragmentOp3ATI;
  PFNGLSETFRAGMENTSHADERCONSTANTATIPROC SetFragmentShaderConstantATI;
  PFNGLPNTRIANGLESIATIPROC PNTrianglesiATI;
  PFNGLPNTRIANGLESFATIPROC PNTrianglesfATI;
  PFNGLNEWOBJECTBUFFERATIPROC NewObjectBufferATI;
  PFNGLISOBJECTBUFFERATIPROC IsObjectBufferATI;
  PFNGLUPDATEOBJECTBUFFERATIPROC UpdateObjectBufferATI;
  PFNGLGETOBJECTBUFFERFVATIPROC GetObjectBufferfvATI;
  PFNGLGETOBJECTBUFFERIVATIPROC GetObjectBufferivATI;
  PFNGLFREEOBJECTBUFFERATIPROC FreeObjectBufferATI;
  PFNGLARRAYOBJECTATIPROC ArrayObjectATI;
  PFNGLGETARRAYOBJECTFVATIPROC GetArrayObjectfvATI;
  PFNGLGETARRAYOBJECTIVATIPROC GetArrayObjectivATI;
  PFNGLVARIANTARRAYOBJECTATIPROC VariantArrayObjectATI;
  PFNGLGETVARIANTARRAYOBJECTFVATIPROC GetVariantArrayObjectfvATI;
  PFNGLGETVARIANTARRAYOBJECTIVATIPROC GetVariantArrayObjectivATI;
  PFNGLBEGINVERTEXSHADEREXTPROC BeginVertexShaderEXT;
  PFNGLENDVERTEXSHADEREXTPROC EndVertexShaderEXT;
  PFNGLBINDVERTEXSHADEREXTPROC BindVertexShaderEXT;
  PFNGLGENVERTEXSHADERSEXTPROC GenVertexShadersEXT;
  PFNGLDELETEVERTEXSHADEREXTPROC DeleteVertexShaderEXT;
  PFNGLSHADEROP1EXTPROC ShaderOp1EXT;
  PFNGLSHADEROP2EXTPROC ShaderOp2EXT;
  PFNGLSHADEROP3EXTPROC ShaderOp3EXT;
  PFNGLSWIZZLEEXTPROC SwizzleEXT;
  PFNGLWRITEMASKEXTPROC WriteMaskEXT;
  PFNGLINSERTCOMPONENTEXTPROC InsertComponentEXT;
  PFNGLEXTRACTCOMPONENTEXTPROC ExtractComponentEXT;
  PFNGLGENSYMBOLSEXTPROC GenSymbolsEXT;
  PFNGLSETINVARIANTEXTPROC SetInvariantEXT;
  PFNGLSETLOCALCONSTANTEXTPROC SetLocalConstantEXT;
  PFNGLVARIANTBVEXTPROC VariantbvEXT;
  PFNGLVARIANTSVEXTPROC VariantsvEXT;
  PFNGLVARIANTIVEXTPROC VariantivEXT;
  PFNGLVARIANTFVEXTPROC VariantfvEXT;
  PFNGLVARIANTDVEXTPROC VariantdvEXT;
  PFNGLVARIANTUBVEXTPROC VariantubvEXT;
  PFNGLVARIANTUSVEXTPROC VariantusvEXT;
  PFNGLVARIANTUIVEXTPROC VariantuivEXT;
  PFNGLVARIANTPOINTEREXTPROC VariantPointerEXT;
  PFNGLENABLEVARIANTCLIENTSTATEEXTPROC EnableVariantClientStateEXT;
  PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC DisableVariantClientStateEXT;
  PFNGLBINDLIGHTPARAMETEREXTPROC BindLightParameterEXT;
  PFNGLBINDMATERIALPARAMETEREXTPROC BindMaterialParameterEXT;
  PFNGLBINDTEXGENPARAMETEREXTPROC BindTexGenParameterEXT;
  PFNGLBINDTEXTUREUNITPARAMETEREXTPROC BindTextureUnitParameterEXT;
  PFNGLBINDPARAMETEREXTPROC BindParameterEXT;
  PFNGLISVARIANTENABLEDEXTPROC IsVariantEnabledEXT;
  PFNGLGETVARIANTBOOLEANVEXTPROC GetVariantBooleanvEXT;
  PFNGLGETVARIANTINTEGERVEXTPROC GetVariantIntegervEXT;
  PFNGLGETVARIANTFLOATVEXTPROC GetVariantFloatvEXT;
  PFNGLGETVARIANTPOINTERVEXTPROC GetVariantPointervEXT;
  PFNGLGETINVARIANTBOOLEANVEXTPROC GetInvariantBooleanvEXT;
  PFNGLGETINVARIANTINTEGERVEXTPROC GetInvariantIntegervEXT;
  PFNGLGETINVARIANTFLOATVEXTPROC GetInvariantFloatvEXT;
  PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC GetLocalConstantBooleanvEXT;
  PFNGLGETLOCALCONSTANTINTEGERVEXTPROC GetLocalConstantIntegervEXT;
  PFNGLGETLOCALCONSTANTFLOATVEXTPROC GetLocalConstantFloatvEXT;
  PFNGLVERTEXSTREAM1SATIPROC VertexStream1sATI;
  PFNGLVERTEXSTREAM1SVATIPROC VertexStream1svATI;
  PFNGLVERTEXSTREAM1IATIPROC VertexStream1iATI;
  PFNGLVERTEXSTREAM1IVATIPROC VertexStream1ivATI;
  PFNGLVERTEXSTREAM1FATIPROC VertexStream1fATI;
  PFNGLVERTEXSTREAM1FVATIPROC VertexStream1fvATI;
  PFNGLVERTEXSTREAM1DATIPROC VertexStream1dATI;
  PFNGLVERTEXSTREAM1DVATIPROC VertexStream1dvATI;
  PFNGLVERTEXSTREAM2SATIPROC VertexStream2sATI;
  PFNGLVERTEXSTREAM2SVATIPROC VertexStream2svATI;
  PFNGLVERTEXSTREAM2IATIPROC VertexStream2iATI;
  PFNGLVERTEXSTREAM2IVATIPROC VertexStream2ivATI;
  PFNGLVERTEXSTREAM2FATIPROC VertexStream2fATI;
  PFNGLVERTEXSTREAM2FVATIPROC VertexStream2fvATI;
  PFNGLVERTEXSTREAM2DATIPROC VertexStream2dATI;
  PFNGLVERTEXSTREAM2DVATIPROC VertexStream2dvATI;
  PFNGLVERTEXSTREAM3SATIPROC VertexStream3sATI;
  PFNGLVERTEXSTREAM3SVATIPROC VertexStream3svATI;
  PFNGLVERTEXSTREAM3IATIPROC VertexStream3iATI;
  PFNGLVERTEXSTREAM3IVATIPROC VertexStream3ivATI;
  PFNGLVERTEXSTREAM3FATIPROC VertexStream3fATI;
  PFNGLVERTEXSTREAM3FVATIPROC VertexStream3fvATI;
  PFNGLVERTEXSTREAM3DATIPROC VertexStream3dATI;
  PFNGLVERTEXSTREAM3DVATIPROC VertexStream3dvATI;
  PFNGLVERTEXSTREAM4SATIPROC VertexStream4sATI;
  PFNGLVERTEXSTREAM4SVATIPROC VertexStream4svATI;
  PFNGLVERTEXSTREAM4IATIPROC VertexStream4iATI;
  PFNGLVERTEXSTREAM4IVATIPROC VertexStream4ivATI;
  PFNGLVERTEXSTREAM4FATIPROC VertexStream4fATI;
  PFNGLVERTEXSTREAM4FVATIPROC VertexStream4fvATI;
  PFNGLVERTEXSTREAM4DATIPROC VertexStream4dATI;
  PFNGLVERTEXSTREAM4DVATIPROC VertexStream4dvATI;
  PFNGLNORMALSTREAM3BATIPROC NormalStream3bATI;
  PFNGLNORMALSTREAM3BVATIPROC NormalStream3bvATI;
  PFNGLNORMALSTREAM3SATIPROC NormalStream3sATI;
  PFNGLNORMALSTREAM3SVATIPROC NormalStream3svATI;
  PFNGLNORMALSTREAM3IATIPROC NormalStream3iATI;
  PFNGLNORMALSTREAM3IVATIPROC NormalStream3ivATI;
  PFNGLNORMALSTREAM3FATIPROC NormalStream3fATI;
  PFNGLNORMALSTREAM3FVATIPROC NormalStream3fvATI;
  PFNGLNORMALSTREAM3DATIPROC NormalStream3dATI;
  PFNGLNORMALSTREAM3DVATIPROC NormalStream3dvATI;
  PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC ClientActiveVertexStreamATI;
  PFNGLVERTEXBLENDENVIATIPROC VertexBlendEnviATI;
  PFNGLVERTEXBLENDENVFATIPROC VertexBlendEnvfATI;
  PFNGLELEMENTPOINTERATIPROC ElementPointerATI;
  PFNGLDRAWELEMENTARRAYATIPROC DrawElementArrayATI;
  PFNGLDRAWRANGEELEMENTARRAYATIPROC DrawRangeElementArrayATI;
  PFNGLDRAWMESHARRAYSSUNPROC DrawMeshArraysSUN;
  PFNGLGENOCCLUSIONQUERIESNVPROC GenOcclusionQueriesNV;
  PFNGLDELETEOCCLUSIONQUERIESNVPROC DeleteOcclusionQueriesNV;
  PFNGLISOCCLUSIONQUERYNVPROC IsOcclusionQueryNV;
  PFNGLBEGINOCCLUSIONQUERYNVPROC BeginOcclusionQueryNV;
  PFNGLENDOCCLUSIONQUERYNVPROC EndOcclusionQueryNV;
  PFNGLGETOCCLUSIONQUERYIVNVPROC GetOcclusionQueryivNV;
  PFNGLGETOCCLUSIONQUERYUIVNVPROC GetOcclusionQueryuivNV;
  PFNGLPOINTPARAMETERINVPROC PointParameteriNV;
  PFNGLPOINTPARAMETERIVNVPROC PointParameterivNV;
  PFNGLACTIVESTENCILFACEEXTPROC ActiveStencilFaceEXT;
  PFNGLELEMENTPOINTERAPPLEPROC ElementPointerAPPLE;
  PFNGLDRAWELEMENTARRAYAPPLEPROC DrawElementArrayAPPLE;
  PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC DrawRangeElementArrayAPPLE;
  PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC MultiDrawElementArrayAPPLE;
  PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC MultiDrawRangeElementArrayAPPLE;
  PFNGLGENFENCESAPPLEPROC GenFencesAPPLE;
  PFNGLDELETEFENCESAPPLEPROC DeleteFencesAPPLE;
  PFNGLSETFENCEAPPLEPROC SetFenceAPPLE;
  PFNGLISFENCEAPPLEPROC IsFenceAPPLE;
  PFNGLTESTFENCEAPPLEPROC TestFenceAPPLE;
  PFNGLFINISHFENCEAPPLEPROC FinishFenceAPPLE;
  PFNGLTESTOBJECTAPPLEPROC TestObjectAPPLE;
  PFNGLFINISHOBJECTAPPLEPROC FinishObjectAPPLE;
  PFNGLBINDVERTEXARRAYAPPLEPROC BindVertexArrayAPPLE;
  PFNGLDELETEVERTEXARRAYSAPPLEPROC DeleteVertexArraysAPPLE;
  PFNGLGENVERTEXARRAYSAPPLEPROC GenVertexArraysAPPLE;
  PFNGLISVERTEXARRAYAPPLEPROC IsVertexArrayAPPLE;
  PFNGLVERTEXARRAYRANGEAPPLEPROC VertexArrayRangeAPPLE;
  PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC FlushVertexArrayRangeAPPLE;
  PFNGLVERTEXARRAYPARAMETERIAPPLEPROC VertexArrayParameteriAPPLE;
  PFNGLDRAWBUFFERSATIPROC DrawBuffersATI;
  PFNGLPROGRAMNAMEDPARAMETER4FNVPROC ProgramNamedParameter4fNV;
  PFNGLPROGRAMNAMEDPARAMETER4DNVPROC ProgramNamedParameter4dNV;
  PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC ProgramNamedParameter4fvNV;
  PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC ProgramNamedParameter4dvNV;
  PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC GetProgramNamedParameterfvNV;
  PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC GetProgramNamedParameterdvNV;
  PFNGLVERTEX2HNVPROC Vertex2hNV;
  PFNGLVERTEX2HVNVPROC Vertex2hvNV;
  PFNGLVERTEX3HNVPROC Vertex3hNV;
  PFNGLVERTEX3HVNVPROC Vertex3hvNV;
  PFNGLVERTEX4HNVPROC Vertex4hNV;
  PFNGLVERTEX4HVNVPROC Vertex4hvNV;
  PFNGLNORMAL3HNVPROC Normal3hNV;
  PFNGLNORMAL3HVNVPROC Normal3hvNV;
  PFNGLCOLOR3HNVPROC Color3hNV;
  PFNGLCOLOR3HVNVPROC Color3hvNV;
  PFNGLCOLOR4HNVPROC Color4hNV;
  PFNGLCOLOR4HVNVPROC Color4hvNV;
  PFNGLTEXCOORD1HNVPROC TexCoord1hNV;
  PFNGLTEXCOORD1HVNVPROC TexCoord1hvNV;
  PFNGLTEXCOORD2HNVPROC TexCoord2hNV;
  PFNGLTEXCOORD2HVNVPROC TexCoord2hvNV;
  PFNGLTEXCOORD3HNVPROC TexCoord3hNV;
  PFNGLTEXCOORD3HVNVPROC TexCoord3hvNV;
  PFNGLTEXCOORD4HNVPROC TexCoord4hNV;
  PFNGLTEXCOORD4HVNVPROC TexCoord4hvNV;
  PFNGLMULTITEXCOORD1HNVPROC MultiTexCoord1hNV;
  PFNGLMULTITEXCOORD1HVNVPROC MultiTexCoord1hvNV;
  PFNGLMULTITEXCOORD2HNVPROC MultiTexCoord2hNV;
  PFNGLMULTITEXCOORD2HVNVPROC MultiTexCoord2hvNV;
  PFNGLMULTITEXCOORD3HNVPROC MultiTexCoord3hNV;
  PFNGLMULTITEXCOORD3HVNVPROC MultiTexCoord3hvNV;
  PFNGLMULTITEXCOORD4HNVPROC MultiTexCoord4hNV;
  PFNGLMULTITEXCOORD4HVNVPROC MultiTexCoord4hvNV;
  PFNGLFOGCOORDHNVPROC FogCoordhNV;
  PFNGLFOGCOORDHVNVPROC FogCoordhvNV;
  PFNGLSECONDARYCOLOR3HNVPROC SecondaryColor3hNV;
  PFNGLSECONDARYCOLOR3HVNVPROC SecondaryColor3hvNV;
  PFNGLVERTEXWEIGHTHNVPROC VertexWeighthNV;
  PFNGLVERTEXWEIGHTHVNVPROC VertexWeighthvNV;
  PFNGLVERTEXATTRIB1HNVPROC VertexAttrib1hNV;
  PFNGLVERTEXATTRIB1HVNVPROC VertexAttrib1hvNV;
  PFNGLVERTEXATTRIB2HNVPROC VertexAttrib2hNV;
  PFNGLVERTEXATTRIB2HVNVPROC VertexAttrib2hvNV;
  PFNGLVERTEXATTRIB3HNVPROC VertexAttrib3hNV;
  PFNGLVERTEXATTRIB3HVNVPROC VertexAttrib3hvNV;
  PFNGLVERTEXATTRIB4HNVPROC VertexAttrib4hNV;
  PFNGLVERTEXATTRIB4HVNVPROC VertexAttrib4hvNV;
  PFNGLVERTEXATTRIBS1HVNVPROC VertexAttribs1hvNV;
  PFNGLVERTEXATTRIBS2HVNVPROC VertexAttribs2hvNV;
  PFNGLVERTEXATTRIBS3HVNVPROC VertexAttribs3hvNV;
  PFNGLVERTEXATTRIBS4HVNVPROC VertexAttribs4hvNV;
  PFNGLPIXELDATARANGENVPROC PixelDataRangeNV;
  PFNGLFLUSHPIXELDATARANGENVPROC FlushPixelDataRangeNV;
  PFNGLPRIMITIVERESTARTNVPROC PrimitiveRestartNV;
  PFNGLPRIMITIVERESTARTINDEXNVPROC PrimitiveRestartIndexNV;
  PFNGLMAPOBJECTBUFFERATIPROC MapObjectBufferATI;
  PFNGLUNMAPOBJECTBUFFERATIPROC UnmapObjectBufferATI;
  PFNGLSTENCILOPSEPARATEATIPROC StencilOpSeparateATI;
  PFNGLSTENCILFUNCSEPARATEATIPROC StencilFuncSeparateATI;
  PFNGLVERTEXATTRIBARRAYOBJECTATIPROC VertexAttribArrayObjectATI;
  PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC GetVertexAttribArrayObjectfvATI;
  PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC GetVertexAttribArrayObjectivATI;
  PFNGLDEPTHBOUNDSEXTPROC DepthBoundsEXT;
  PFNGLBLENDEQUATIONSEPARATEEXTPROC BlendEquationSeparateEXT;
  PFNGLISRENDERBUFFEREXTPROC IsRenderbufferEXT;
  PFNGLBINDRENDERBUFFEREXTPROC BindRenderbufferEXT;
  PFNGLDELETERENDERBUFFERSEXTPROC DeleteRenderbuffersEXT;
  PFNGLGENRENDERBUFFERSEXTPROC GenRenderbuffersEXT;
  PFNGLRENDERBUFFERSTORAGEEXTPROC RenderbufferStorageEXT;
  PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC GetRenderbufferParameterivEXT;
  PFNGLISFRAMEBUFFEREXTPROC IsFramebufferEXT;
  PFNGLBINDFRAMEBUFFEREXTPROC BindFramebufferEXT;
  PFNGLDELETEFRAMEBUFFERSEXTPROC DeleteFramebuffersEXT;
  PFNGLGENFRAMEBUFFERSEXTPROC GenFramebuffersEXT;
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC CheckFramebufferStatusEXT;
  PFNGLFRAMEBUFFERTEXTURE1DEXTPROC FramebufferTexture1DEXT;
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC FramebufferTexture2DEXT;
  PFNGLFRAMEBUFFERTEXTURE3DEXTPROC FramebufferTexture3DEXT;
  PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC FramebufferRenderbufferEXT;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC GetFramebufferAttachmentParameterivEXT;
  PFNGLGENERATEMIPMAPEXTPROC GenerateMipmapEXT;
  PFNGLSTRINGMARKERGREMEDYPROC StringMarkerGREMEDY;
  PFNGLSTENCILCLEARTAGEXTPROC StencilClearTagEXT;
  PFNGLBLITFRAMEBUFFEREXTPROC BlitFramebufferEXT;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC RenderbufferStorageMultisampleEXT;
  PFNGLGETQUERYOBJECTI64VEXTPROC GetQueryObjecti64vEXT;
  PFNGLGETQUERYOBJECTUI64VEXTPROC GetQueryObjectui64vEXT;
  PFNGLPROGRAMENVPARAMETERS4FVEXTPROC ProgramEnvParameters4fvEXT;
  PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC ProgramLocalParameters4fvEXT;
  PFNGLBUFFERPARAMETERIAPPLEPROC BufferParameteriAPPLE;
  PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC FlushMappedBufferRangeAPPLE;
  PFNGLPROGRAMLOCALPARAMETERI4INVPROC ProgramLocalParameterI4iNV;
  PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC ProgramLocalParameterI4ivNV;
  PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC ProgramLocalParametersI4ivNV;
  PFNGLPROGRAMLOCALPARAMETERI4UINVPROC ProgramLocalParameterI4uiNV;
  PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC ProgramLocalParameterI4uivNV;
  PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC ProgramLocalParametersI4uivNV;
  PFNGLPROGRAMENVPARAMETERI4INVPROC ProgramEnvParameterI4iNV;
  PFNGLPROGRAMENVPARAMETERI4IVNVPROC ProgramEnvParameterI4ivNV;
  PFNGLPROGRAMENVPARAMETERSI4IVNVPROC ProgramEnvParametersI4ivNV;
  PFNGLPROGRAMENVPARAMETERI4UINVPROC ProgramEnvParameterI4uiNV;
  PFNGLPROGRAMENVPARAMETERI4UIVNVPROC ProgramEnvParameterI4uivNV;
  PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC ProgramEnvParametersI4uivNV;
  PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC GetProgramLocalParameterIivNV;
  PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC GetProgramLocalParameterIuivNV;
  PFNGLGETPROGRAMENVPARAMETERIIVNVPROC GetProgramEnvParameterIivNV;
  PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC GetProgramEnvParameterIuivNV;
  PFNGLPROGRAMVERTEXLIMITNVPROC ProgramVertexLimitNV;
  PFNGLFRAMEBUFFERTEXTUREEXTPROC FramebufferTextureEXT;
  PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC FramebufferTextureLayerEXT;
  PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC FramebufferTextureFaceEXT;
  PFNGLPROGRAMPARAMETERIEXTPROC ProgramParameteriEXT;
  PFNGLVERTEXATTRIBI1IEXTPROC VertexAttribI1iEXT;
  PFNGLVERTEXATTRIBI2IEXTPROC VertexAttribI2iEXT;
  PFNGLVERTEXATTRIBI3IEXTPROC VertexAttribI3iEXT;
  PFNGLVERTEXATTRIBI4IEXTPROC VertexAttribI4iEXT;
  PFNGLVERTEXATTRIBI1UIEXTPROC VertexAttribI1uiEXT;
  PFNGLVERTEXATTRIBI2UIEXTPROC VertexAttribI2uiEXT;
  PFNGLVERTEXATTRIBI3UIEXTPROC VertexAttribI3uiEXT;
  PFNGLVERTEXATTRIBI4UIEXTPROC VertexAttribI4uiEXT;
  PFNGLVERTEXATTRIBI1IVEXTPROC VertexAttribI1ivEXT;
  PFNGLVERTEXATTRIBI2IVEXTPROC VertexAttribI2ivEXT;
  PFNGLVERTEXATTRIBI3IVEXTPROC VertexAttribI3ivEXT;
  PFNGLVERTEXATTRIBI4IVEXTPROC VertexAttribI4ivEXT;
  PFNGLVERTEXATTRIBI1UIVEXTPROC VertexAttribI1uivEXT;
  PFNGLVERTEXATTRIBI2UIVEXTPROC VertexAttribI2uivEXT;
  PFNGLVERTEXATTRIBI3UIVEXTPROC VertexAttribI3uivEXT;
  PFNGLVERTEXATTRIBI4UIVEXTPROC VertexAttribI4uivEXT;
  PFNGLVERTEXATTRIBI4BVEXTPROC VertexAttribI4bvEXT;
  PFNGLVERTEXATTRIBI4SVEXTPROC VertexAttribI4svEXT;
  PFNGLVERTEXATTRIBI4UBVEXTPROC VertexAttribI4ubvEXT;
  PFNGLVERTEXATTRIBI4USVEXTPROC VertexAttribI4usvEXT;
  PFNGLVERTEXATTRIBIPOINTEREXTPROC VertexAttribIPointerEXT;
  PFNGLGETVERTEXATTRIBIIVEXTPROC GetVertexAttribIivEXT;
  PFNGLGETVERTEXATTRIBIUIVEXTPROC GetVertexAttribIuivEXT;
  PFNGLGETUNIFORMUIVEXTPROC GetUniformuivEXT;
  PFNGLBINDFRAGDATALOCATIONEXTPROC BindFragDataLocationEXT;
  PFNGLGETFRAGDATALOCATIONEXTPROC GetFragDataLocationEXT;
  PFNGLUNIFORM1UIEXTPROC Uniform1uiEXT;
  PFNGLUNIFORM2UIEXTPROC Uniform2uiEXT;
  PFNGLUNIFORM3UIEXTPROC Uniform3uiEXT;
  PFNGLUNIFORM4UIEXTPROC Uniform4uiEXT;
  PFNGLUNIFORM1UIVEXTPROC Uniform1uivEXT;
  PFNGLUNIFORM2UIVEXTPROC Uniform2uivEXT;
  PFNGLUNIFORM3UIVEXTPROC Uniform3uivEXT;
  PFNGLUNIFORM4UIVEXTPROC Uniform4uivEXT;
  PFNGLDRAWARRAYSINSTANCEDEXTPROC DrawArraysInstancedEXT;
  PFNGLDRAWELEMENTSINSTANCEDEXTPROC DrawElementsInstancedEXT;
  PFNGLTEXBUFFEREXTPROC TexBufferEXT;
  PFNGLDEPTHRANGEDNVPROC DepthRangedNV;
  PFNGLCLEARDEPTHDNVPROC ClearDepthdNV;
  PFNGLDEPTHBOUNDSDNVPROC DepthBoundsdNV;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC RenderbufferStorageMultisampleCoverageNV;
  PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC ProgramBufferParametersfvNV;
  PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC ProgramBufferParametersIivNV;
  PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC ProgramBufferParametersIuivNV;
  PFNGLCOLORMASKINDEXEDEXTPROC ColorMaskIndexedEXT;
  PFNGLGETBOOLEANINDEXEDVEXTPROC GetBooleanIndexedvEXT;
  PFNGLGETINTEGERINDEXEDVEXTPROC GetIntegerIndexedvEXT;
  PFNGLENABLEINDEXEDEXTPROC EnableIndexedEXT;
  PFNGLDISABLEINDEXEDEXTPROC DisableIndexedEXT;
  PFNGLISENABLEDINDEXEDEXTPROC IsEnabledIndexedEXT;
  PFNGLBEGINTRANSFORMFEEDBACKNVPROC BeginTransformFeedbackNV;
  PFNGLENDTRANSFORMFEEDBACKNVPROC EndTransformFeedbackNV;
  PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC TransformFeedbackAttribsNV;
  PFNGLBINDBUFFERRANGENVPROC BindBufferRangeNV;
  PFNGLBINDBUFFEROFFSETNVPROC BindBufferOffsetNV;
  PFNGLBINDBUFFERBASENVPROC BindBufferBaseNV;
  PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC TransformFeedbackVaryingsNV;
  PFNGLACTIVEVARYINGNVPROC ActiveVaryingNV;
  PFNGLGETVARYINGLOCATIONNVPROC GetVaryingLocationNV;
  PFNGLGETACTIVEVARYINGNVPROC GetActiveVaryingNV;
  PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC GetTransformFeedbackVaryingNV;
  PFNGLUNIFORMBUFFEREXTPROC UniformBufferEXT;
  PFNGLGETUNIFORMBUFFERSIZEEXTPROC GetUniformBufferSizeEXT;
  PFNGLGETUNIFORMOFFSETEXTPROC GetUniformOffsetEXT;
  PFNGLTEXPARAMETERIIVEXTPROC TexParameterIivEXT;
  PFNGLTEXPARAMETERIUIVEXTPROC TexParameterIuivEXT;
  PFNGLGETTEXPARAMETERIIVEXTPROC GetTexParameterIivEXT;
  PFNGLGETTEXPARAMETERIUIVEXTPROC GetTexParameterIuivEXT;
  PFNGLCLEARCOLORIIEXTPROC ClearColorIiEXT;
  PFNGLCLEARCOLORIUIEXTPROC ClearColorIuiEXT;
  PFNGLFRAMETERMINATORGREMEDYPROC FrameTerminatorGREMEDY;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация точек входа
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Init (ILibrary&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение таблицы точек входа для текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
const GlEntries* get_gl_entries ();

/*
    Переопределения OpenGL 1.1
*/

#define glAccum get_gl_entries ()->Accum
#define glAlphaFunc get_gl_entries ()->AlphaFunc
#define glAreTexturesResident get_gl_entries ()->AreTexturesResident
#define glArrayElement get_gl_entries ()->ArrayElement
#define glBegin get_gl_entries ()->Begin
#define glBindTexture get_gl_entries ()->BindTexture
#define glBitmap get_gl_entries ()->Bitmap
#define glBlendFunc get_gl_entries ()->BlendFunc
#define glCallList get_gl_entries ()->CallList
#define glCallLists get_gl_entries ()->CallLists
#define glClear get_gl_entries ()->Clear
#define glClearAccum get_gl_entries ()->ClearAccum
#define glClearColor get_gl_entries ()->ClearColor
#define glClearDepth get_gl_entries ()->ClearDepth
#define glClearIndex get_gl_entries ()->ClearIndex
#define glClearStencil get_gl_entries ()->ClearStencil
#define glClipPlane get_gl_entries ()->ClipPlane
#define glColor3b get_gl_entries ()->Color3b
#define glColor3bv get_gl_entries ()->Color3bv
#define glColor3d get_gl_entries ()->Color3d
#define glColor3dv get_gl_entries ()->Color3dv
#define glColor3f get_gl_entries ()->Color3f
#define glColor3fv get_gl_entries ()->Color3fv
#define glColor3i get_gl_entries ()->Color3i
#define glColor3iv get_gl_entries ()->Color3iv
#define glColor3s get_gl_entries ()->Color3s
#define glColor3sv get_gl_entries ()->Color3sv
#define glColor3ub get_gl_entries ()->Color3ub
#define glColor3ubv get_gl_entries ()->Color3ubv
#define glColor3ui get_gl_entries ()->Color3ui
#define glColor3uiv get_gl_entries ()->Color3uiv
#define glColor3us get_gl_entries ()->Color3us
#define glColor3usv get_gl_entries ()->Color3usv
#define glColor4b get_gl_entries ()->Color4b
#define glColor4bv get_gl_entries ()->Color4bv
#define glColor4d get_gl_entries ()->Color4d
#define glColor4dv get_gl_entries ()->Color4dv
#define glColor4f get_gl_entries ()->Color4f
#define glColor4fv get_gl_entries ()->Color4fv
#define glColor4i get_gl_entries ()->Color4i
#define glColor4iv get_gl_entries ()->Color4iv
#define glColor4s get_gl_entries ()->Color4s
#define glColor4sv get_gl_entries ()->Color4sv
#define glColor4ub get_gl_entries ()->Color4ub
#define glColor4ubv get_gl_entries ()->Color4ubv
#define glColor4ui get_gl_entries ()->Color4ui
#define glColor4uiv get_gl_entries ()->Color4uiv
#define glColor4us get_gl_entries ()->Color4us
#define glColor4usv get_gl_entries ()->Color4usv
#define glColorMask get_gl_entries ()->ColorMask
#define glColorMaterial get_gl_entries ()->ColorMaterial
#define glColorPointer get_gl_entries ()->ColorPointer
#define glCopyPixels get_gl_entries ()->CopyPixels
#define glCopyTexImage1D get_gl_entries ()->CopyTexImage1D
#define glCopyTexImage2D get_gl_entries ()->CopyTexImage2D
#define glCopyTexSubImage1D get_gl_entries ()->CopyTexSubImage1D
#define glCopyTexSubImage2D get_gl_entries ()->CopyTexSubImage2D
#define glCullFace get_gl_entries ()->CullFace
#define glDeleteLists get_gl_entries ()->DeleteLists
#define glDeleteTextures get_gl_entries ()->DeleteTextures
#define glDepthFunc get_gl_entries ()->DepthFunc
#define glDepthMask get_gl_entries ()->DepthMask
#define glDepthRange get_gl_entries ()->DepthRange
#define glDisable get_gl_entries ()->Disable
#define glDisableClientState get_gl_entries ()->DisableClientState
#define glDrawArrays get_gl_entries ()->DrawArrays
#define glDrawBuffer get_gl_entries ()->DrawBuffer
#define glDrawElements get_gl_entries ()->DrawElements
#define glDrawPixels get_gl_entries ()->DrawPixels
#define glEdgeFlag get_gl_entries ()->EdgeFlag
#define glEdgeFlagPointer get_gl_entries ()->EdgeFlagPointer
#define glEdgeFlagv get_gl_entries ()->EdgeFlagv
#define glEnable get_gl_entries ()->Enable
#define glEnableClientState get_gl_entries ()->EnableClientState
#define glEnd get_gl_entries ()->End
#define glEndList get_gl_entries ()->EndList
#define glEvalCoord1d get_gl_entries ()->EvalCoord1d
#define glEvalCoord1dv get_gl_entries ()->EvalCoord1dv
#define glEvalCoord1f get_gl_entries ()->EvalCoord1f
#define glEvalCoord1fv get_gl_entries ()->EvalCoord1fv
#define glEvalCoord2d get_gl_entries ()->EvalCoord2d
#define glEvalCoord2dv get_gl_entries ()->EvalCoord2dv
#define glEvalCoord2f get_gl_entries ()->EvalCoord2f
#define glEvalCoord2fv get_gl_entries ()->EvalCoord2fv
#define glEvalMesh1 get_gl_entries ()->EvalMesh1
#define glEvalMesh2 get_gl_entries ()->EvalMesh2
#define glEvalPoint1 get_gl_entries ()->EvalPoint1
#define glEvalPoint2 get_gl_entries ()->EvalPoint2
#define glFeedbackBuffer get_gl_entries ()->FeedbackBuffer
#define glFinish get_gl_entries ()->Finish
#define glFlush get_gl_entries ()->Flush
#define glFogf get_gl_entries ()->Fogf
#define glFogfv get_gl_entries ()->Fogfv
#define glFogi get_gl_entries ()->Fogi
#define glFogiv get_gl_entries ()->Fogiv
#define glFrontFace get_gl_entries ()->FrontFace
#define glFrustum get_gl_entries ()->Frustum
#define glGenLists get_gl_entries ()->GenLists
#define glGenTextures get_gl_entries ()->GenTextures
#define glGetBooleanv get_gl_entries ()->GetBooleanv
#define glGetClipPlane get_gl_entries ()->GetClipPlane
#define glGetDoublev get_gl_entries ()->GetDoublev
#define glGetError get_gl_entries ()->GetError
#define glGetFloatv get_gl_entries ()->GetFloatv
#define glGetIntegerv get_gl_entries ()->GetIntegerv
#define glGetLightfv get_gl_entries ()->GetLightfv
#define glGetLightiv get_gl_entries ()->GetLightiv
#define glGetMapdv get_gl_entries ()->GetMapdv
#define glGetMapfv get_gl_entries ()->GetMapfv
#define glGetMapiv get_gl_entries ()->GetMapiv
#define glGetMaterialfv get_gl_entries ()->GetMaterialfv
#define glGetMaterialiv get_gl_entries ()->GetMaterialiv
#define glGetPixelMapfv get_gl_entries ()->GetPixelMapfv
#define glGetPixelMapuiv get_gl_entries ()->GetPixelMapuiv
#define glGetPixelMapusv get_gl_entries ()->GetPixelMapusv
#define glGetPointerv get_gl_entries ()->GetPointerv
#define glGetPolygonStipple get_gl_entries ()->GetPolygonStipple
#define glGetString get_gl_entries ()->GetString
#define glGetTexEnvfv get_gl_entries ()->GetTexEnvfv
#define glGetTexEnviv get_gl_entries ()->GetTexEnviv
#define glGetTexGendv get_gl_entries ()->GetTexGendv
#define glGetTexGenfv get_gl_entries ()->GetTexGenfv
#define glGetTexGeniv get_gl_entries ()->GetTexGeniv
#define glGetTexImage get_gl_entries ()->GetTexImage
#define glGetTexLevelParameterfv get_gl_entries ()->GetTexLevelParameterfv
#define glGetTexLevelParameteriv get_gl_entries ()->GetTexLevelParameteriv
#define glGetTexParameterfv get_gl_entries ()->GetTexParameterfv
#define glGetTexParameteriv get_gl_entries ()->GetTexParameteriv
#define glHint get_gl_entries ()->Hint
#define glIndexMask get_gl_entries ()->IndexMask
#define glIndexPointer get_gl_entries ()->IndexPointer
#define glIndexd get_gl_entries ()->Indexd
#define glIndexdv get_gl_entries ()->Indexdv
#define glIndexf get_gl_entries ()->Indexf
#define glIndexfv get_gl_entries ()->Indexfv
#define glIndexi get_gl_entries ()->Indexi
#define glIndexiv get_gl_entries ()->Indexiv
#define glIndexs get_gl_entries ()->Indexs
#define glIndexsv get_gl_entries ()->Indexsv
#define glIndexub get_gl_entries ()->Indexub
#define glIndexubv get_gl_entries ()->Indexubv
#define glInitNames get_gl_entries ()->InitNames
#define glInterleavedArrays get_gl_entries ()->InterleavedArrays
#define glIsEnabled get_gl_entries ()->IsEnabled
#define glIsList get_gl_entries ()->IsList
#define glIsTexture get_gl_entries ()->IsTexture
#define glLightModelf get_gl_entries ()->LightModelf
#define glLightModelfv get_gl_entries ()->LightModelfv
#define glLightModeli get_gl_entries ()->LightModeli
#define glLightModeliv get_gl_entries ()->LightModeliv
#define glLightf get_gl_entries ()->Lightf
#define glLightfv get_gl_entries ()->Lightfv
#define glLighti get_gl_entries ()->Lighti
#define glLightiv get_gl_entries ()->Lightiv
#define glLineStipple get_gl_entries ()->LineStipple
#define glLineWidth get_gl_entries ()->LineWidth
#define glListBase get_gl_entries ()->ListBase
#define glLoadIdentity get_gl_entries ()->LoadIdentity
#define glLoadMatrixd get_gl_entries ()->LoadMatrixd
#define glLoadMatrixf get_gl_entries ()->LoadMatrixf
#define glLoadName get_gl_entries ()->LoadName
#define glLogicOp get_gl_entries ()->LogicOp
#define glMap1d get_gl_entries ()->Map1d
#define glMap1f get_gl_entries ()->Map1f
#define glMap2d get_gl_entries ()->Map2d
#define glMap2f get_gl_entries ()->Map2f
#define glMapGrid1d get_gl_entries ()->MapGrid1d
#define glMapGrid1f get_gl_entries ()->MapGrid1f
#define glMapGrid2d get_gl_entries ()->MapGrid2d
#define glMapGrid2f get_gl_entries ()->MapGrid2f
#define glMaterialf get_gl_entries ()->Materialf
#define glMaterialfv get_gl_entries ()->Materialfv
#define glMateriali get_gl_entries ()->Materiali
#define glMaterialiv get_gl_entries ()->Materialiv
#define glMatrixMode get_gl_entries ()->MatrixMode
#define glMultMatrixd get_gl_entries ()->MultMatrixd
#define glMultMatrixf get_gl_entries ()->MultMatrixf
#define glNewList get_gl_entries ()->NewList
#define glNormal3b get_gl_entries ()->Normal3b
#define glNormal3bv get_gl_entries ()->Normal3bv
#define glNormal3d get_gl_entries ()->Normal3d
#define glNormal3dv get_gl_entries ()->Normal3dv
#define glNormal3f get_gl_entries ()->Normal3f
#define glNormal3fv get_gl_entries ()->Normal3fv
#define glNormal3i get_gl_entries ()->Normal3i
#define glNormal3iv get_gl_entries ()->Normal3iv
#define glNormal3s get_gl_entries ()->Normal3s
#define glNormal3sv get_gl_entries ()->Normal3sv
#define glNormalPointer get_gl_entries ()->NormalPointer
#define glOrtho get_gl_entries ()->Ortho
#define glPassThrough get_gl_entries ()->PassThrough
#define glPixelMapfv get_gl_entries ()->PixelMapfv
#define glPixelMapuiv get_gl_entries ()->PixelMapuiv
#define glPixelMapusv get_gl_entries ()->PixelMapusv
#define glPixelStoref get_gl_entries ()->PixelStoref
#define glPixelStorei get_gl_entries ()->PixelStorei
#define glPixelTransferf get_gl_entries ()->PixelTransferf
#define glPixelTransferi get_gl_entries ()->PixelTransferi
#define glPixelZoom get_gl_entries ()->PixelZoom
#define glPointSize get_gl_entries ()->PointSize
#define glPolygonMode get_gl_entries ()->PolygonMode
#define glPolygonOffset get_gl_entries ()->PolygonOffset
#define glPolygonStipple get_gl_entries ()->PolygonStipple
#define glPopAttrib get_gl_entries ()->PopAttrib
#define glPopClientAttrib get_gl_entries ()->PopClientAttrib
#define glPopMatrix get_gl_entries ()->PopMatrix
#define glPopName get_gl_entries ()->PopName
#define glPrioritizeTextures get_gl_entries ()->PrioritizeTextures
#define glPushAttrib get_gl_entries ()->PushAttrib
#define glPushClientAttrib get_gl_entries ()->PushClientAttrib
#define glPushMatrix get_gl_entries ()->PushMatrix
#define glPushName get_gl_entries ()->PushName
#define glRasterPos2d get_gl_entries ()->RasterPos2d
#define glRasterPos2dv get_gl_entries ()->RasterPos2dv
#define glRasterPos2f get_gl_entries ()->RasterPos2f
#define glRasterPos2fv get_gl_entries ()->RasterPos2fv
#define glRasterPos2i get_gl_entries ()->RasterPos2i
#define glRasterPos2iv get_gl_entries ()->RasterPos2iv
#define glRasterPos2s get_gl_entries ()->RasterPos2s
#define glRasterPos2sv get_gl_entries ()->RasterPos2sv
#define glRasterPos3d get_gl_entries ()->RasterPos3d
#define glRasterPos3dv get_gl_entries ()->RasterPos3dv
#define glRasterPos3f get_gl_entries ()->RasterPos3f
#define glRasterPos3fv get_gl_entries ()->RasterPos3fv
#define glRasterPos3i get_gl_entries ()->RasterPos3i
#define glRasterPos3iv get_gl_entries ()->RasterPos3iv
#define glRasterPos3s get_gl_entries ()->RasterPos3s
#define glRasterPos3sv get_gl_entries ()->RasterPos3sv
#define glRasterPos4d get_gl_entries ()->RasterPos4d
#define glRasterPos4dv get_gl_entries ()->RasterPos4dv
#define glRasterPos4f get_gl_entries ()->RasterPos4f
#define glRasterPos4fv get_gl_entries ()->RasterPos4fv
#define glRasterPos4i get_gl_entries ()->RasterPos4i
#define glRasterPos4iv get_gl_entries ()->RasterPos4iv
#define glRasterPos4s get_gl_entries ()->RasterPos4s
#define glRasterPos4sv get_gl_entries ()->RasterPos4sv
#define glReadBuffer get_gl_entries ()->ReadBuffer
#define glReadPixels get_gl_entries ()->ReadPixels
#define glRectd get_gl_entries ()->Rectd
#define glRectdv get_gl_entries ()->Rectdv
#define glRectf get_gl_entries ()->Rectf
#define glRectfv get_gl_entries ()->Rectfv
#define glRecti get_gl_entries ()->Recti
#define glRectiv get_gl_entries ()->Rectiv
#define glRects get_gl_entries ()->Rects
#define glRectsv get_gl_entries ()->Rectsv
#define glRenderMode get_gl_entries ()->RenderMode
#define glRotated get_gl_entries ()->Rotated
#define glRotatef get_gl_entries ()->Rotatef
#define glScaled get_gl_entries ()->Scaled
#define glScalef get_gl_entries ()->Scalef
#define glScissor get_gl_entries ()->Scissor
#define glSelectBuffer get_gl_entries ()->SelectBuffer
#define glShadeModel get_gl_entries ()->ShadeModel
#define glStencilFunc get_gl_entries ()->StencilFunc
#define glStencilMask get_gl_entries ()->StencilMask
#define glStencilOp get_gl_entries ()->StencilOp
#define glTexCoord1d get_gl_entries ()->TexCoord1d
#define glTexCoord1dv get_gl_entries ()->TexCoord1dv
#define glTexCoord1f get_gl_entries ()->TexCoord1f
#define glTexCoord1fv get_gl_entries ()->TexCoord1fv
#define glTexCoord1i get_gl_entries ()->TexCoord1i
#define glTexCoord1iv get_gl_entries ()->TexCoord1iv
#define glTexCoord1s get_gl_entries ()->TexCoord1s
#define glTexCoord1sv get_gl_entries ()->TexCoord1sv
#define glTexCoord2d get_gl_entries ()->TexCoord2d
#define glTexCoord2dv get_gl_entries ()->TexCoord2dv
#define glTexCoord2f get_gl_entries ()->TexCoord2f
#define glTexCoord2fv get_gl_entries ()->TexCoord2fv
#define glTexCoord2i get_gl_entries ()->TexCoord2i
#define glTexCoord2iv get_gl_entries ()->TexCoord2iv
#define glTexCoord2s get_gl_entries ()->TexCoord2s
#define glTexCoord2sv get_gl_entries ()->TexCoord2sv
#define glTexCoord3d get_gl_entries ()->TexCoord3d
#define glTexCoord3dv get_gl_entries ()->TexCoord3dv
#define glTexCoord3f get_gl_entries ()->TexCoord3f
#define glTexCoord3fv get_gl_entries ()->TexCoord3fv
#define glTexCoord3i get_gl_entries ()->TexCoord3i
#define glTexCoord3iv get_gl_entries ()->TexCoord3iv
#define glTexCoord3s get_gl_entries ()->TexCoord3s
#define glTexCoord3sv get_gl_entries ()->TexCoord3sv
#define glTexCoord4d get_gl_entries ()->TexCoord4d
#define glTexCoord4dv get_gl_entries ()->TexCoord4dv
#define glTexCoord4f get_gl_entries ()->TexCoord4f
#define glTexCoord4fv get_gl_entries ()->TexCoord4fv
#define glTexCoord4i get_gl_entries ()->TexCoord4i
#define glTexCoord4iv get_gl_entries ()->TexCoord4iv
#define glTexCoord4s get_gl_entries ()->TexCoord4s
#define glTexCoord4sv get_gl_entries ()->TexCoord4sv
#define glTexCoordPointer get_gl_entries ()->TexCoordPointer
#define glTexEnvf get_gl_entries ()->TexEnvf
#define glTexEnvfv get_gl_entries ()->TexEnvfv
#define glTexEnvi get_gl_entries ()->TexEnvi
#define glTexEnviv get_gl_entries ()->TexEnviv
#define glTexGend get_gl_entries ()->TexGend
#define glTexGendv get_gl_entries ()->TexGendv
#define glTexGenf get_gl_entries ()->TexGenf
#define glTexGenfv get_gl_entries ()->TexGenfv
#define glTexGeni get_gl_entries ()->TexGeni
#define glTexGeniv get_gl_entries ()->TexGeniv
#define glTexImage1D get_gl_entries ()->TexImage1D
#define glTexImage2D get_gl_entries ()->TexImage2D
#define glTexParameterf get_gl_entries ()->TexParameterf
#define glTexParameterfv get_gl_entries ()->TexParameterfv
#define glTexParameteri get_gl_entries ()->TexParameteri
#define glTexParameteriv get_gl_entries ()->TexParameteriv
#define glTexSubImage1D get_gl_entries ()->TexSubImage1D
#define glTexSubImage2D get_gl_entries ()->TexSubImage2D
#define glTranslated get_gl_entries ()->Translated
#define glTranslatef get_gl_entries ()->Translatef
#define glVertex2d get_gl_entries ()->Vertex2d
#define glVertex2dv get_gl_entries ()->Vertex2dv
#define glVertex2f get_gl_entries ()->Vertex2f
#define glVertex2fv get_gl_entries ()->Vertex2fv
#define glVertex2i get_gl_entries ()->Vertex2i
#define glVertex2iv get_gl_entries ()->Vertex2iv
#define glVertex2s get_gl_entries ()->Vertex2s
#define glVertex2sv get_gl_entries ()->Vertex2sv
#define glVertex3d get_gl_entries ()->Vertex3d
#define glVertex3dv get_gl_entries ()->Vertex3dv
#define glVertex3f get_gl_entries ()->Vertex3f
#define glVertex3fv get_gl_entries ()->Vertex3fv
#define glVertex3i get_gl_entries ()->Vertex3i
#define glVertex3iv get_gl_entries ()->Vertex3iv
#define glVertex3s get_gl_entries ()->Vertex3s
#define glVertex3sv get_gl_entries ()->Vertex3sv
#define glVertex4d get_gl_entries ()->Vertex4d
#define glVertex4dv get_gl_entries ()->Vertex4dv
#define glVertex4f get_gl_entries ()->Vertex4f
#define glVertex4fv get_gl_entries ()->Vertex4fv
#define glVertex4i get_gl_entries ()->Vertex4i
#define glVertex4iv get_gl_entries ()->Vertex4iv
#define glVertex4s get_gl_entries ()->Vertex4s
#define glVertex4sv get_gl_entries ()->Vertex4sv
#define glVertexPointer get_gl_entries ()->VertexPointer
#define glViewport get_gl_entries ()->Viewport

/*
    Переопределения расширений OpenGL
*/

#define glBlendColor get_gl_entries ()->BlendColor
#define glBlendEquation get_gl_entries ()->BlendEquation
#define glDrawRangeElements get_gl_entries ()->DrawRangeElements
#define glColorTable get_gl_entries ()->ColorTable
#define glColorTableParameterfv get_gl_entries ()->ColorTableParameterfv
#define glColorTableParameteriv get_gl_entries ()->ColorTableParameteriv
#define glCopyColorTable get_gl_entries ()->CopyColorTable
#define glGetColorTable get_gl_entries ()->GetColorTable
#define glGetColorTableParameterfv get_gl_entries ()->GetColorTableParameterfv
#define glGetColorTableParameteriv get_gl_entries ()->GetColorTableParameteriv
#define glColorSubTable get_gl_entries ()->ColorSubTable
#define glCopyColorSubTable get_gl_entries ()->CopyColorSubTable
#define glConvolutionFilter1D get_gl_entries ()->ConvolutionFilter1D
#define glConvolutionFilter2D get_gl_entries ()->ConvolutionFilter2D
#define glConvolutionParameterf get_gl_entries ()->ConvolutionParameterf
#define glConvolutionParameterfv get_gl_entries ()->ConvolutionParameterfv
#define glConvolutionParameteri get_gl_entries ()->ConvolutionParameteri
#define glConvolutionParameteriv get_gl_entries ()->ConvolutionParameteriv
#define glCopyConvolutionFilter1D get_gl_entries ()->CopyConvolutionFilter1D
#define glCopyConvolutionFilter2D get_gl_entries ()->CopyConvolutionFilter2D
#define glGetConvolutionFilter get_gl_entries ()->GetConvolutionFilter
#define glGetConvolutionParameterfv get_gl_entries ()->GetConvolutionParameterfv
#define glGetConvolutionParameteriv get_gl_entries ()->GetConvolutionParameteriv
#define glGetSeparableFilter get_gl_entries ()->GetSeparableFilter
#define glSeparableFilter2D get_gl_entries ()->SeparableFilter2D
#define glGetHistogram get_gl_entries ()->GetHistogram
#define glGetHistogramParameterfv get_gl_entries ()->GetHistogramParameterfv
#define glGetHistogramParameteriv get_gl_entries ()->GetHistogramParameteriv
#define glGetMinmax get_gl_entries ()->GetMinmax
#define glGetMinmaxParameterfv get_gl_entries ()->GetMinmaxParameterfv
#define glGetMinmaxParameteriv get_gl_entries ()->GetMinmaxParameteriv
#define glHistogram get_gl_entries ()->Histogram
#define glMinmax get_gl_entries ()->Minmax
#define glResetHistogram get_gl_entries ()->ResetHistogram
#define glResetMinmax get_gl_entries ()->ResetMinmax
#define glTexImage3D get_gl_entries ()->TexImage3D
#define glTexSubImage3D get_gl_entries ()->TexSubImage3D
#define glCopyTexSubImage3D get_gl_entries ()->CopyTexSubImage3D
#define glActiveTexture get_gl_entries ()->ActiveTexture
#define glClientActiveTexture get_gl_entries ()->ClientActiveTexture
#define glMultiTexCoord1d get_gl_entries ()->MultiTexCoord1d
#define glMultiTexCoord1dv get_gl_entries ()->MultiTexCoord1dv
#define glMultiTexCoord1f get_gl_entries ()->MultiTexCoord1f
#define glMultiTexCoord1fv get_gl_entries ()->MultiTexCoord1fv
#define glMultiTexCoord1i get_gl_entries ()->MultiTexCoord1i
#define glMultiTexCoord1iv get_gl_entries ()->MultiTexCoord1iv
#define glMultiTexCoord1s get_gl_entries ()->MultiTexCoord1s
#define glMultiTexCoord1sv get_gl_entries ()->MultiTexCoord1sv
#define glMultiTexCoord2d get_gl_entries ()->MultiTexCoord2d
#define glMultiTexCoord2dv get_gl_entries ()->MultiTexCoord2dv
#define glMultiTexCoord2f get_gl_entries ()->MultiTexCoord2f
#define glMultiTexCoord2fv get_gl_entries ()->MultiTexCoord2fv
#define glMultiTexCoord2i get_gl_entries ()->MultiTexCoord2i
#define glMultiTexCoord2iv get_gl_entries ()->MultiTexCoord2iv
#define glMultiTexCoord2s get_gl_entries ()->MultiTexCoord2s
#define glMultiTexCoord2sv get_gl_entries ()->MultiTexCoord2sv
#define glMultiTexCoord3d get_gl_entries ()->MultiTexCoord3d
#define glMultiTexCoord3dv get_gl_entries ()->MultiTexCoord3dv
#define glMultiTexCoord3f get_gl_entries ()->MultiTexCoord3f
#define glMultiTexCoord3fv get_gl_entries ()->MultiTexCoord3fv
#define glMultiTexCoord3i get_gl_entries ()->MultiTexCoord3i
#define glMultiTexCoord3iv get_gl_entries ()->MultiTexCoord3iv
#define glMultiTexCoord3s get_gl_entries ()->MultiTexCoord3s
#define glMultiTexCoord3sv get_gl_entries ()->MultiTexCoord3sv
#define glMultiTexCoord4d get_gl_entries ()->MultiTexCoord4d
#define glMultiTexCoord4dv get_gl_entries ()->MultiTexCoord4dv
#define glMultiTexCoord4f get_gl_entries ()->MultiTexCoord4f
#define glMultiTexCoord4fv get_gl_entries ()->MultiTexCoord4fv
#define glMultiTexCoord4i get_gl_entries ()->MultiTexCoord4i
#define glMultiTexCoord4iv get_gl_entries ()->MultiTexCoord4iv
#define glMultiTexCoord4s get_gl_entries ()->MultiTexCoord4s
#define glMultiTexCoord4sv get_gl_entries ()->MultiTexCoord4sv
#define glLoadTransposeMatrixf get_gl_entries ()->LoadTransposeMatrixf
#define glLoadTransposeMatrixd get_gl_entries ()->LoadTransposeMatrixd
#define glMultTransposeMatrixf get_gl_entries ()->MultTransposeMatrixf
#define glMultTransposeMatrixd get_gl_entries ()->MultTransposeMatrixd
#define glSampleCoverage get_gl_entries ()->SampleCoverage
#define glCompressedTexImage3D get_gl_entries ()->CompressedTexImage3D
#define glCompressedTexImage2D get_gl_entries ()->CompressedTexImage2D
#define glCompressedTexImage1D get_gl_entries ()->CompressedTexImage1D
#define glCompressedTexSubImage3D get_gl_entries ()->CompressedTexSubImage3D
#define glCompressedTexSubImage2D get_gl_entries ()->CompressedTexSubImage2D
#define glCompressedTexSubImage1D get_gl_entries ()->CompressedTexSubImage1D
#define glGetCompressedTexImage get_gl_entries ()->GetCompressedTexImage
#define glBlendFuncSeparate get_gl_entries ()->BlendFuncSeparate
#define glFogCoordf get_gl_entries ()->FogCoordf
#define glFogCoordfv get_gl_entries ()->FogCoordfv
#define glFogCoordd get_gl_entries ()->FogCoordd
#define glFogCoorddv get_gl_entries ()->FogCoorddv
#define glFogCoordPointer get_gl_entries ()->FogCoordPointer
#define glMultiDrawArrays get_gl_entries ()->MultiDrawArrays
#define glMultiDrawElements get_gl_entries ()->MultiDrawElements
#define glPointParameterf get_gl_entries ()->PointParameterf
#define glPointParameterfv get_gl_entries ()->PointParameterfv
#define glPointParameteri get_gl_entries ()->PointParameteri
#define glPointParameteriv get_gl_entries ()->PointParameteriv
#define glSecondaryColor3b get_gl_entries ()->SecondaryColor3b
#define glSecondaryColor3bv get_gl_entries ()->SecondaryColor3bv
#define glSecondaryColor3d get_gl_entries ()->SecondaryColor3d
#define glSecondaryColor3dv get_gl_entries ()->SecondaryColor3dv
#define glSecondaryColor3f get_gl_entries ()->SecondaryColor3f
#define glSecondaryColor3fv get_gl_entries ()->SecondaryColor3fv
#define glSecondaryColor3i get_gl_entries ()->SecondaryColor3i
#define glSecondaryColor3iv get_gl_entries ()->SecondaryColor3iv
#define glSecondaryColor3s get_gl_entries ()->SecondaryColor3s
#define glSecondaryColor3sv get_gl_entries ()->SecondaryColor3sv
#define glSecondaryColor3ub get_gl_entries ()->SecondaryColor3ub
#define glSecondaryColor3ubv get_gl_entries ()->SecondaryColor3ubv
#define glSecondaryColor3ui get_gl_entries ()->SecondaryColor3ui
#define glSecondaryColor3uiv get_gl_entries ()->SecondaryColor3uiv
#define glSecondaryColor3us get_gl_entries ()->SecondaryColor3us
#define glSecondaryColor3usv get_gl_entries ()->SecondaryColor3usv
#define glSecondaryColorPointer get_gl_entries ()->SecondaryColorPointer
#define glWindowPos2d get_gl_entries ()->WindowPos2d
#define glWindowPos2dv get_gl_entries ()->WindowPos2dv
#define glWindowPos2f get_gl_entries ()->WindowPos2f
#define glWindowPos2fv get_gl_entries ()->WindowPos2fv
#define glWindowPos2i get_gl_entries ()->WindowPos2i
#define glWindowPos2iv get_gl_entries ()->WindowPos2iv
#define glWindowPos2s get_gl_entries ()->WindowPos2s
#define glWindowPos2sv get_gl_entries ()->WindowPos2sv
#define glWindowPos3d get_gl_entries ()->WindowPos3d
#define glWindowPos3dv get_gl_entries ()->WindowPos3dv
#define glWindowPos3f get_gl_entries ()->WindowPos3f
#define glWindowPos3fv get_gl_entries ()->WindowPos3fv
#define glWindowPos3i get_gl_entries ()->WindowPos3i
#define glWindowPos3iv get_gl_entries ()->WindowPos3iv
#define glWindowPos3s get_gl_entries ()->WindowPos3s
#define glWindowPos3sv get_gl_entries ()->WindowPos3sv
#define glGenQueries get_gl_entries ()->GenQueries
#define glDeleteQueries get_gl_entries ()->DeleteQueries
#define glIsQuery get_gl_entries ()->IsQuery
#define glBeginQuery get_gl_entries ()->BeginQuery
#define glEndQuery get_gl_entries ()->EndQuery
#define glGetQueryiv get_gl_entries ()->GetQueryiv
#define glGetQueryObjectiv get_gl_entries ()->GetQueryObjectiv
#define glGetQueryObjectuiv get_gl_entries ()->GetQueryObjectuiv
#define glBindBuffer get_gl_entries ()->BindBuffer
#define glDeleteBuffers get_gl_entries ()->DeleteBuffers
#define glGenBuffers get_gl_entries ()->GenBuffers
#define glIsBuffer get_gl_entries ()->IsBuffer
#define glBufferData get_gl_entries ()->BufferData
#define glBufferSubData get_gl_entries ()->BufferSubData
#define glGetBufferSubData get_gl_entries ()->GetBufferSubData
#define glMapBuffer get_gl_entries ()->MapBuffer
#define glUnmapBuffer get_gl_entries ()->UnmapBuffer
#define glGetBufferParameteriv get_gl_entries ()->GetBufferParameteriv
#define glGetBufferPointerv get_gl_entries ()->GetBufferPointerv
#define glBlendEquationSeparate get_gl_entries ()->BlendEquationSeparate
#define glDrawBuffers get_gl_entries ()->DrawBuffers
#define glStencilOpSeparate get_gl_entries ()->StencilOpSeparate
#define glStencilFuncSeparate get_gl_entries ()->StencilFuncSeparate
#define glStencilMaskSeparate get_gl_entries ()->StencilMaskSeparate
#define glAttachShader get_gl_entries ()->AttachShader
#define glBindAttribLocation get_gl_entries ()->BindAttribLocation
#define glCompileShader get_gl_entries ()->CompileShader
#define glCreateProgram get_gl_entries ()->CreateProgram
#define glCreateShader get_gl_entries ()->CreateShader
#define glDeleteProgram get_gl_entries ()->DeleteProgram
#define glDeleteShader get_gl_entries ()->DeleteShader
#define glDetachShader get_gl_entries ()->DetachShader
#define glDisableVertexAttribArray get_gl_entries ()->DisableVertexAttribArray
#define glEnableVertexAttribArray get_gl_entries ()->EnableVertexAttribArray
#define glGetActiveAttrib get_gl_entries ()->GetActiveAttrib
#define glGetActiveUniform get_gl_entries ()->GetActiveUniform
#define glGetAttachedShaders get_gl_entries ()->GetAttachedShaders
#define glGetAttribLocation get_gl_entries ()->GetAttribLocation
#define glGetProgramiv get_gl_entries ()->GetProgramiv
#define glGetProgramInfoLog get_gl_entries ()->GetProgramInfoLog
#define glGetShaderiv get_gl_entries ()->GetShaderiv
#define glGetShaderInfoLog get_gl_entries ()->GetShaderInfoLog
#define glGetShaderSource get_gl_entries ()->GetShaderSource
#define glGetUniformLocation get_gl_entries ()->GetUniformLocation
#define glGetUniformfv get_gl_entries ()->GetUniformfv
#define glGetUniformiv get_gl_entries ()->GetUniformiv
#define glGetVertexAttribdv get_gl_entries ()->GetVertexAttribdv
#define glGetVertexAttribfv get_gl_entries ()->GetVertexAttribfv
#define glGetVertexAttribiv get_gl_entries ()->GetVertexAttribiv
#define glGetVertexAttribPointerv get_gl_entries ()->GetVertexAttribPointerv
#define glIsProgram get_gl_entries ()->IsProgram
#define glIsShader get_gl_entries ()->IsShader
#define glLinkProgram get_gl_entries ()->LinkProgram
#define glShaderSource get_gl_entries ()->ShaderSource
#define glUseProgram get_gl_entries ()->UseProgram
#define glUniform1f get_gl_entries ()->Uniform1f
#define glUniform2f get_gl_entries ()->Uniform2f
#define glUniform3f get_gl_entries ()->Uniform3f
#define glUniform4f get_gl_entries ()->Uniform4f
#define glUniform1i get_gl_entries ()->Uniform1i
#define glUniform2i get_gl_entries ()->Uniform2i
#define glUniform3i get_gl_entries ()->Uniform3i
#define glUniform4i get_gl_entries ()->Uniform4i
#define glUniform1fv get_gl_entries ()->Uniform1fv
#define glUniform2fv get_gl_entries ()->Uniform2fv
#define glUniform3fv get_gl_entries ()->Uniform3fv
#define glUniform4fv get_gl_entries ()->Uniform4fv
#define glUniform1iv get_gl_entries ()->Uniform1iv
#define glUniform2iv get_gl_entries ()->Uniform2iv
#define glUniform3iv get_gl_entries ()->Uniform3iv
#define glUniform4iv get_gl_entries ()->Uniform4iv
#define glUniformMatrix2fv get_gl_entries ()->UniformMatrix2fv
#define glUniformMatrix3fv get_gl_entries ()->UniformMatrix3fv
#define glUniformMatrix4fv get_gl_entries ()->UniformMatrix4fv
#define glValidateProgram get_gl_entries ()->ValidateProgram
#define glVertexAttrib1d get_gl_entries ()->VertexAttrib1d
#define glVertexAttrib1dv get_gl_entries ()->VertexAttrib1dv
#define glVertexAttrib1f get_gl_entries ()->VertexAttrib1f
#define glVertexAttrib1fv get_gl_entries ()->VertexAttrib1fv
#define glVertexAttrib1s get_gl_entries ()->VertexAttrib1s
#define glVertexAttrib1sv get_gl_entries ()->VertexAttrib1sv
#define glVertexAttrib2d get_gl_entries ()->VertexAttrib2d
#define glVertexAttrib2dv get_gl_entries ()->VertexAttrib2dv
#define glVertexAttrib2f get_gl_entries ()->VertexAttrib2f
#define glVertexAttrib2fv get_gl_entries ()->VertexAttrib2fv
#define glVertexAttrib2s get_gl_entries ()->VertexAttrib2s
#define glVertexAttrib2sv get_gl_entries ()->VertexAttrib2sv
#define glVertexAttrib3d get_gl_entries ()->VertexAttrib3d
#define glVertexAttrib3dv get_gl_entries ()->VertexAttrib3dv
#define glVertexAttrib3f get_gl_entries ()->VertexAttrib3f
#define glVertexAttrib3fv get_gl_entries ()->VertexAttrib3fv
#define glVertexAttrib3s get_gl_entries ()->VertexAttrib3s
#define glVertexAttrib3sv get_gl_entries ()->VertexAttrib3sv
#define glVertexAttrib4Nbv get_gl_entries ()->VertexAttrib4Nbv
#define glVertexAttrib4Niv get_gl_entries ()->VertexAttrib4Niv
#define glVertexAttrib4Nsv get_gl_entries ()->VertexAttrib4Nsv
#define glVertexAttrib4Nub get_gl_entries ()->VertexAttrib4Nub
#define glVertexAttrib4Nubv get_gl_entries ()->VertexAttrib4Nubv
#define glVertexAttrib4Nuiv get_gl_entries ()->VertexAttrib4Nuiv
#define glVertexAttrib4Nusv get_gl_entries ()->VertexAttrib4Nusv
#define glVertexAttrib4bv get_gl_entries ()->VertexAttrib4bv
#define glVertexAttrib4d get_gl_entries ()->VertexAttrib4d
#define glVertexAttrib4dv get_gl_entries ()->VertexAttrib4dv
#define glVertexAttrib4f get_gl_entries ()->VertexAttrib4f
#define glVertexAttrib4fv get_gl_entries ()->VertexAttrib4fv
#define glVertexAttrib4iv get_gl_entries ()->VertexAttrib4iv
#define glVertexAttrib4s get_gl_entries ()->VertexAttrib4s
#define glVertexAttrib4sv get_gl_entries ()->VertexAttrib4sv
#define glVertexAttrib4ubv get_gl_entries ()->VertexAttrib4ubv
#define glVertexAttrib4uiv get_gl_entries ()->VertexAttrib4uiv
#define glVertexAttrib4usv get_gl_entries ()->VertexAttrib4usv
#define glVertexAttribPointer get_gl_entries ()->VertexAttribPointer
#define glUniformMatrix2x3fv get_gl_entries ()->UniformMatrix2x3fv
#define glUniformMatrix3x2fv get_gl_entries ()->UniformMatrix3x2fv
#define glUniformMatrix2x4fv get_gl_entries ()->UniformMatrix2x4fv
#define glUniformMatrix4x2fv get_gl_entries ()->UniformMatrix4x2fv
#define glUniformMatrix3x4fv get_gl_entries ()->UniformMatrix3x4fv
#define glUniformMatrix4x3fv get_gl_entries ()->UniformMatrix4x3fv
#define glActiveTextureARB get_gl_entries ()->ActiveTextureARB
#define glClientActiveTextureARB get_gl_entries ()->ClientActiveTextureARB
#define glMultiTexCoord1dARB get_gl_entries ()->MultiTexCoord1dARB
#define glMultiTexCoord1dvARB get_gl_entries ()->MultiTexCoord1dvARB
#define glMultiTexCoord1fARB get_gl_entries ()->MultiTexCoord1fARB
#define glMultiTexCoord1fvARB get_gl_entries ()->MultiTexCoord1fvARB
#define glMultiTexCoord1iARB get_gl_entries ()->MultiTexCoord1iARB
#define glMultiTexCoord1ivARB get_gl_entries ()->MultiTexCoord1ivARB
#define glMultiTexCoord1sARB get_gl_entries ()->MultiTexCoord1sARB
#define glMultiTexCoord1svARB get_gl_entries ()->MultiTexCoord1svARB
#define glMultiTexCoord2dARB get_gl_entries ()->MultiTexCoord2dARB
#define glMultiTexCoord2dvARB get_gl_entries ()->MultiTexCoord2dvARB
#define glMultiTexCoord2fARB get_gl_entries ()->MultiTexCoord2fARB
#define glMultiTexCoord2fvARB get_gl_entries ()->MultiTexCoord2fvARB
#define glMultiTexCoord2iARB get_gl_entries ()->MultiTexCoord2iARB
#define glMultiTexCoord2ivARB get_gl_entries ()->MultiTexCoord2ivARB
#define glMultiTexCoord2sARB get_gl_entries ()->MultiTexCoord2sARB
#define glMultiTexCoord2svARB get_gl_entries ()->MultiTexCoord2svARB
#define glMultiTexCoord3dARB get_gl_entries ()->MultiTexCoord3dARB
#define glMultiTexCoord3dvARB get_gl_entries ()->MultiTexCoord3dvARB
#define glMultiTexCoord3fARB get_gl_entries ()->MultiTexCoord3fARB
#define glMultiTexCoord3fvARB get_gl_entries ()->MultiTexCoord3fvARB
#define glMultiTexCoord3iARB get_gl_entries ()->MultiTexCoord3iARB
#define glMultiTexCoord3ivARB get_gl_entries ()->MultiTexCoord3ivARB
#define glMultiTexCoord3sARB get_gl_entries ()->MultiTexCoord3sARB
#define glMultiTexCoord3svARB get_gl_entries ()->MultiTexCoord3svARB
#define glMultiTexCoord4dARB get_gl_entries ()->MultiTexCoord4dARB
#define glMultiTexCoord4dvARB get_gl_entries ()->MultiTexCoord4dvARB
#define glMultiTexCoord4fARB get_gl_entries ()->MultiTexCoord4fARB
#define glMultiTexCoord4fvARB get_gl_entries ()->MultiTexCoord4fvARB
#define glMultiTexCoord4iARB get_gl_entries ()->MultiTexCoord4iARB
#define glMultiTexCoord4ivARB get_gl_entries ()->MultiTexCoord4ivARB
#define glMultiTexCoord4sARB get_gl_entries ()->MultiTexCoord4sARB
#define glMultiTexCoord4svARB get_gl_entries ()->MultiTexCoord4svARB
#define glLoadTransposeMatrixfARB get_gl_entries ()->LoadTransposeMatrixfARB
#define glLoadTransposeMatrixdARB get_gl_entries ()->LoadTransposeMatrixdARB
#define glMultTransposeMatrixfARB get_gl_entries ()->MultTransposeMatrixfARB
#define glMultTransposeMatrixdARB get_gl_entries ()->MultTransposeMatrixdARB
#define glSampleCoverageARB get_gl_entries ()->SampleCoverageARB
#define glCompressedTexImage3DARB get_gl_entries ()->CompressedTexImage3DARB
#define glCompressedTexImage2DARB get_gl_entries ()->CompressedTexImage2DARB
#define glCompressedTexImage1DARB get_gl_entries ()->CompressedTexImage1DARB
#define glCompressedTexSubImage3DARB get_gl_entries ()->CompressedTexSubImage3DARB
#define glCompressedTexSubImage2DARB get_gl_entries ()->CompressedTexSubImage2DARB
#define glCompressedTexSubImage1DARB get_gl_entries ()->CompressedTexSubImage1DARB
#define glGetCompressedTexImageARB get_gl_entries ()->GetCompressedTexImageARB
#define glPointParameterfARB get_gl_entries ()->PointParameterfARB
#define glPointParameterfvARB get_gl_entries ()->PointParameterfvARB
#define glWeightbvARB get_gl_entries ()->WeightbvARB
#define glWeightsvARB get_gl_entries ()->WeightsvARB
#define glWeightivARB get_gl_entries ()->WeightivARB
#define glWeightfvARB get_gl_entries ()->WeightfvARB
#define glWeightdvARB get_gl_entries ()->WeightdvARB
#define glWeightubvARB get_gl_entries ()->WeightubvARB
#define glWeightusvARB get_gl_entries ()->WeightusvARB
#define glWeightuivARB get_gl_entries ()->WeightuivARB
#define glWeightPointerARB get_gl_entries ()->WeightPointerARB
#define glVertexBlendARB get_gl_entries ()->VertexBlendARB
#define glCurrentPaletteMatrixARB get_gl_entries ()->CurrentPaletteMatrixARB
#define glMatrixIndexubvARB get_gl_entries ()->MatrixIndexubvARB
#define glMatrixIndexusvARB get_gl_entries ()->MatrixIndexusvARB
#define glMatrixIndexuivARB get_gl_entries ()->MatrixIndexuivARB
#define glMatrixIndexPointerARB get_gl_entries ()->MatrixIndexPointerARB
#define glWindowPos2dARB get_gl_entries ()->WindowPos2dARB
#define glWindowPos2dvARB get_gl_entries ()->WindowPos2dvARB
#define glWindowPos2fARB get_gl_entries ()->WindowPos2fARB
#define glWindowPos2fvARB get_gl_entries ()->WindowPos2fvARB
#define glWindowPos2iARB get_gl_entries ()->WindowPos2iARB
#define glWindowPos2ivARB get_gl_entries ()->WindowPos2ivARB
#define glWindowPos2sARB get_gl_entries ()->WindowPos2sARB
#define glWindowPos2svARB get_gl_entries ()->WindowPos2svARB
#define glWindowPos3dARB get_gl_entries ()->WindowPos3dARB
#define glWindowPos3dvARB get_gl_entries ()->WindowPos3dvARB
#define glWindowPos3fARB get_gl_entries ()->WindowPos3fARB
#define glWindowPos3fvARB get_gl_entries ()->WindowPos3fvARB
#define glWindowPos3iARB get_gl_entries ()->WindowPos3iARB
#define glWindowPos3ivARB get_gl_entries ()->WindowPos3ivARB
#define glWindowPos3sARB get_gl_entries ()->WindowPos3sARB
#define glWindowPos3svARB get_gl_entries ()->WindowPos3svARB
#define glVertexAttrib1dARB get_gl_entries ()->VertexAttrib1dARB
#define glVertexAttrib1dvARB get_gl_entries ()->VertexAttrib1dvARB
#define glVertexAttrib1fARB get_gl_entries ()->VertexAttrib1fARB
#define glVertexAttrib1fvARB get_gl_entries ()->VertexAttrib1fvARB
#define glVertexAttrib1sARB get_gl_entries ()->VertexAttrib1sARB
#define glVertexAttrib1svARB get_gl_entries ()->VertexAttrib1svARB
#define glVertexAttrib2dARB get_gl_entries ()->VertexAttrib2dARB
#define glVertexAttrib2dvARB get_gl_entries ()->VertexAttrib2dvARB
#define glVertexAttrib2fARB get_gl_entries ()->VertexAttrib2fARB
#define glVertexAttrib2fvARB get_gl_entries ()->VertexAttrib2fvARB
#define glVertexAttrib2sARB get_gl_entries ()->VertexAttrib2sARB
#define glVertexAttrib2svARB get_gl_entries ()->VertexAttrib2svARB
#define glVertexAttrib3dARB get_gl_entries ()->VertexAttrib3dARB
#define glVertexAttrib3dvARB get_gl_entries ()->VertexAttrib3dvARB
#define glVertexAttrib3fARB get_gl_entries ()->VertexAttrib3fARB
#define glVertexAttrib3fvARB get_gl_entries ()->VertexAttrib3fvARB
#define glVertexAttrib3sARB get_gl_entries ()->VertexAttrib3sARB
#define glVertexAttrib3svARB get_gl_entries ()->VertexAttrib3svARB
#define glVertexAttrib4NbvARB get_gl_entries ()->VertexAttrib4NbvARB
#define glVertexAttrib4NivARB get_gl_entries ()->VertexAttrib4NivARB
#define glVertexAttrib4NsvARB get_gl_entries ()->VertexAttrib4NsvARB
#define glVertexAttrib4NubARB get_gl_entries ()->VertexAttrib4NubARB
#define glVertexAttrib4NubvARB get_gl_entries ()->VertexAttrib4NubvARB
#define glVertexAttrib4NuivARB get_gl_entries ()->VertexAttrib4NuivARB
#define glVertexAttrib4NusvARB get_gl_entries ()->VertexAttrib4NusvARB
#define glVertexAttrib4bvARB get_gl_entries ()->VertexAttrib4bvARB
#define glVertexAttrib4dARB get_gl_entries ()->VertexAttrib4dARB
#define glVertexAttrib4dvARB get_gl_entries ()->VertexAttrib4dvARB
#define glVertexAttrib4fARB get_gl_entries ()->VertexAttrib4fARB
#define glVertexAttrib4fvARB get_gl_entries ()->VertexAttrib4fvARB
#define glVertexAttrib4ivARB get_gl_entries ()->VertexAttrib4ivARB
#define glVertexAttrib4sARB get_gl_entries ()->VertexAttrib4sARB
#define glVertexAttrib4svARB get_gl_entries ()->VertexAttrib4svARB
#define glVertexAttrib4ubvARB get_gl_entries ()->VertexAttrib4ubvARB
#define glVertexAttrib4uivARB get_gl_entries ()->VertexAttrib4uivARB
#define glVertexAttrib4usvARB get_gl_entries ()->VertexAttrib4usvARB
#define glVertexAttribPointerARB get_gl_entries ()->VertexAttribPointerARB
#define glEnableVertexAttribArrayARB get_gl_entries ()->EnableVertexAttribArrayARB
#define glDisableVertexAttribArrayARB get_gl_entries ()->DisableVertexAttribArrayARB
#define glProgramStringARB get_gl_entries ()->ProgramStringARB
#define glBindProgramARB get_gl_entries ()->BindProgramARB
#define glDeleteProgramsARB get_gl_entries ()->DeleteProgramsARB
#define glGenProgramsARB get_gl_entries ()->GenProgramsARB
#define glProgramEnvParameter4dARB get_gl_entries ()->ProgramEnvParameter4dARB
#define glProgramEnvParameter4dvARB get_gl_entries ()->ProgramEnvParameter4dvARB
#define glProgramEnvParameter4fARB get_gl_entries ()->ProgramEnvParameter4fARB
#define glProgramEnvParameter4fvARB get_gl_entries ()->ProgramEnvParameter4fvARB
#define glProgramLocalParameter4dARB get_gl_entries ()->ProgramLocalParameter4dARB
#define glProgramLocalParameter4dvARB get_gl_entries ()->ProgramLocalParameter4dvARB
#define glProgramLocalParameter4fARB get_gl_entries ()->ProgramLocalParameter4fARB
#define glProgramLocalParameter4fvARB get_gl_entries ()->ProgramLocalParameter4fvARB
#define glGetProgramEnvParameterdvARB get_gl_entries ()->GetProgramEnvParameterdvARB
#define glGetProgramEnvParameterfvARB get_gl_entries ()->GetProgramEnvParameterfvARB
#define glGetProgramLocalParameterdvARB get_gl_entries ()->GetProgramLocalParameterdvARB
#define glGetProgramLocalParameterfvARB get_gl_entries ()->GetProgramLocalParameterfvARB
#define glGetProgramivARB get_gl_entries ()->GetProgramivARB
#define glGetProgramStringARB get_gl_entries ()->GetProgramStringARB
#define glGetVertexAttribdvARB get_gl_entries ()->GetVertexAttribdvARB
#define glGetVertexAttribfvARB get_gl_entries ()->GetVertexAttribfvARB
#define glGetVertexAttribivARB get_gl_entries ()->GetVertexAttribivARB
#define glGetVertexAttribPointervARB get_gl_entries ()->GetVertexAttribPointervARB
#define glIsProgramARB get_gl_entries ()->IsProgramARB
#define glBindBufferARB get_gl_entries ()->BindBufferARB
#define glDeleteBuffersARB get_gl_entries ()->DeleteBuffersARB
#define glGenBuffersARB get_gl_entries ()->GenBuffersARB
#define glIsBufferARB get_gl_entries ()->IsBufferARB
#define glBufferDataARB get_gl_entries ()->BufferDataARB
#define glBufferSubDataARB get_gl_entries ()->BufferSubDataARB
#define glGetBufferSubDataARB get_gl_entries ()->GetBufferSubDataARB
#define glMapBufferARB get_gl_entries ()->MapBufferARB
#define glUnmapBufferARB get_gl_entries ()->UnmapBufferARB
#define glGetBufferParameterivARB get_gl_entries ()->GetBufferParameterivARB
#define glGetBufferPointervARB get_gl_entries ()->GetBufferPointervARB
#define glGenQueriesARB get_gl_entries ()->GenQueriesARB
#define glDeleteQueriesARB get_gl_entries ()->DeleteQueriesARB
#define glIsQueryARB get_gl_entries ()->IsQueryARB
#define glBeginQueryARB get_gl_entries ()->BeginQueryARB
#define glEndQueryARB get_gl_entries ()->EndQueryARB
#define glGetQueryivARB get_gl_entries ()->GetQueryivARB
#define glGetQueryObjectivARB get_gl_entries ()->GetQueryObjectivARB
#define glGetQueryObjectuivARB get_gl_entries ()->GetQueryObjectuivARB
#define glDeleteObjectARB get_gl_entries ()->DeleteObjectARB
#define glGetHandleARB get_gl_entries ()->GetHandleARB
#define glDetachObjectARB get_gl_entries ()->DetachObjectARB
#define glCreateShaderObjectARB get_gl_entries ()->CreateShaderObjectARB
#define glShaderSourceARB get_gl_entries ()->ShaderSourceARB
#define glCompileShaderARB get_gl_entries ()->CompileShaderARB
#define glCreateProgramObjectARB get_gl_entries ()->CreateProgramObjectARB
#define glAttachObjectARB get_gl_entries ()->AttachObjectARB
#define glLinkProgramARB get_gl_entries ()->LinkProgramARB
#define glUseProgramObjectARB get_gl_entries ()->UseProgramObjectARB
#define glValidateProgramARB get_gl_entries ()->ValidateProgramARB
#define glUniform1fARB get_gl_entries ()->Uniform1fARB
#define glUniform2fARB get_gl_entries ()->Uniform2fARB
#define glUniform3fARB get_gl_entries ()->Uniform3fARB
#define glUniform4fARB get_gl_entries ()->Uniform4fARB
#define glUniform1iARB get_gl_entries ()->Uniform1iARB
#define glUniform2iARB get_gl_entries ()->Uniform2iARB
#define glUniform3iARB get_gl_entries ()->Uniform3iARB
#define glUniform4iARB get_gl_entries ()->Uniform4iARB
#define glUniform1fvARB get_gl_entries ()->Uniform1fvARB
#define glUniform2fvARB get_gl_entries ()->Uniform2fvARB
#define glUniform3fvARB get_gl_entries ()->Uniform3fvARB
#define glUniform4fvARB get_gl_entries ()->Uniform4fvARB
#define glUniform1ivARB get_gl_entries ()->Uniform1ivARB
#define glUniform2ivARB get_gl_entries ()->Uniform2ivARB
#define glUniform3ivARB get_gl_entries ()->Uniform3ivARB
#define glUniform4ivARB get_gl_entries ()->Uniform4ivARB
#define glUniformMatrix2fvARB get_gl_entries ()->UniformMatrix2fvARB
#define glUniformMatrix3fvARB get_gl_entries ()->UniformMatrix3fvARB
#define glUniformMatrix4fvARB get_gl_entries ()->UniformMatrix4fvARB
#define glGetObjectParameterfvARB get_gl_entries ()->GetObjectParameterfvARB
#define glGetObjectParameterivARB get_gl_entries ()->GetObjectParameterivARB
#define glGetInfoLogARB get_gl_entries ()->GetInfoLogARB
#define glGetAttachedObjectsARB get_gl_entries ()->GetAttachedObjectsARB
#define glGetUniformLocationARB get_gl_entries ()->GetUniformLocationARB
#define glGetActiveUniformARB get_gl_entries ()->GetActiveUniformARB
#define glGetUniformfvARB get_gl_entries ()->GetUniformfvARB
#define glGetUniformivARB get_gl_entries ()->GetUniformivARB
#define glGetShaderSourceARB get_gl_entries ()->GetShaderSourceARB
#define glBindAttribLocationARB get_gl_entries ()->BindAttribLocationARB
#define glGetActiveAttribARB get_gl_entries ()->GetActiveAttribARB
#define glGetAttribLocationARB get_gl_entries ()->GetAttribLocationARB
#define glDrawBuffersARB get_gl_entries ()->DrawBuffersARB
#define glClampColorARB get_gl_entries ()->ClampColorARB
#define glBlendColorEXT get_gl_entries ()->BlendColorEXT
#define glPolygonOffsetEXT get_gl_entries ()->PolygonOffsetEXT
#define glTexImage3DEXT get_gl_entries ()->TexImage3DEXT
#define glTexSubImage3DEXT get_gl_entries ()->TexSubImage3DEXT
#define glGetTexFilterFuncSGIS get_gl_entries ()->GetTexFilterFuncSGIS
#define glTexFilterFuncSGIS get_gl_entries ()->TexFilterFuncSGIS
#define glTexSubImage1DEXT get_gl_entries ()->TexSubImage1DEXT
#define glTexSubImage2DEXT get_gl_entries ()->TexSubImage2DEXT
#define glCopyTexImage1DEXT get_gl_entries ()->CopyTexImage1DEXT
#define glCopyTexImage2DEXT get_gl_entries ()->CopyTexImage2DEXT
#define glCopyTexSubImage1DEXT get_gl_entries ()->CopyTexSubImage1DEXT
#define glCopyTexSubImage2DEXT get_gl_entries ()->CopyTexSubImage2DEXT
#define glCopyTexSubImage3DEXT get_gl_entries ()->CopyTexSubImage3DEXT
#define glGetHistogramEXT get_gl_entries ()->GetHistogramEXT
#define glGetHistogramParameterfvEXT get_gl_entries ()->GetHistogramParameterfvEXT
#define glGetHistogramParameterivEXT get_gl_entries ()->GetHistogramParameterivEXT
#define glGetMinmaxEXT get_gl_entries ()->GetMinmaxEXT
#define glGetMinmaxParameterfvEXT get_gl_entries ()->GetMinmaxParameterfvEXT
#define glGetMinmaxParameterivEXT get_gl_entries ()->GetMinmaxParameterivEXT
#define glHistogramEXT get_gl_entries ()->HistogramEXT
#define glMinmaxEXT get_gl_entries ()->MinmaxEXT
#define glResetHistogramEXT get_gl_entries ()->ResetHistogramEXT
#define glResetMinmaxEXT get_gl_entries ()->ResetMinmaxEXT
#define glConvolutionFilter1DEXT get_gl_entries ()->ConvolutionFilter1DEXT
#define glConvolutionFilter2DEXT get_gl_entries ()->ConvolutionFilter2DEXT
#define glConvolutionParameterfEXT get_gl_entries ()->ConvolutionParameterfEXT
#define glConvolutionParameterfvEXT get_gl_entries ()->ConvolutionParameterfvEXT
#define glConvolutionParameteriEXT get_gl_entries ()->ConvolutionParameteriEXT
#define glConvolutionParameterivEXT get_gl_entries ()->ConvolutionParameterivEXT
#define glCopyConvolutionFilter1DEXT get_gl_entries ()->CopyConvolutionFilter1DEXT
#define glCopyConvolutionFilter2DEXT get_gl_entries ()->CopyConvolutionFilter2DEXT
#define glGetConvolutionFilterEXT get_gl_entries ()->GetConvolutionFilterEXT
#define glGetConvolutionParameterfvEXT get_gl_entries ()->GetConvolutionParameterfvEXT
#define glGetConvolutionParameterivEXT get_gl_entries ()->GetConvolutionParameterivEXT
#define glGetSeparableFilterEXT get_gl_entries ()->GetSeparableFilterEXT
#define glSeparableFilter2DEXT get_gl_entries ()->SeparableFilter2DEXT
#define glColorTableSGI get_gl_entries ()->ColorTableSGI
#define glColorTableParameterfvSGI get_gl_entries ()->ColorTableParameterfvSGI
#define glColorTableParameterivSGI get_gl_entries ()->ColorTableParameterivSGI
#define glCopyColorTableSGI get_gl_entries ()->CopyColorTableSGI
#define glGetColorTableSGI get_gl_entries ()->GetColorTableSGI
#define glGetColorTableParameterfvSGI get_gl_entries ()->GetColorTableParameterfvSGI
#define glGetColorTableParameterivSGI get_gl_entries ()->GetColorTableParameterivSGI
#define glPixelTexGenSGIX get_gl_entries ()->PixelTexGenSGIX
#define glPixelTexGenParameteriSGIS get_gl_entries ()->PixelTexGenParameteriSGIS
#define glPixelTexGenParameterivSGIS get_gl_entries ()->PixelTexGenParameterivSGIS
#define glPixelTexGenParameterfSGIS get_gl_entries ()->PixelTexGenParameterfSGIS
#define glPixelTexGenParameterfvSGIS get_gl_entries ()->PixelTexGenParameterfvSGIS
#define glGetPixelTexGenParameterivSGIS get_gl_entries ()->GetPixelTexGenParameterivSGIS
#define glGetPixelTexGenParameterfvSGIS get_gl_entries ()->GetPixelTexGenParameterfvSGIS
#define glTexImage4DSGIS get_gl_entries ()->TexImage4DSGIS
#define glTexSubImage4DSGIS get_gl_entries ()->TexSubImage4DSGIS
#define glAreTexturesResidentEXT get_gl_entries ()->AreTexturesResidentEXT
#define glBindTextureEXT get_gl_entries ()->BindTextureEXT
#define glDeleteTexturesEXT get_gl_entries ()->DeleteTexturesEXT
#define glGenTexturesEXT get_gl_entries ()->GenTexturesEXT
#define glIsTextureEXT get_gl_entries ()->IsTextureEXT
#define glPrioritizeTexturesEXT get_gl_entries ()->PrioritizeTexturesEXT
#define glDetailTexFuncSGIS get_gl_entries ()->DetailTexFuncSGIS
#define glGetDetailTexFuncSGIS get_gl_entries ()->GetDetailTexFuncSGIS
#define glSharpenTexFuncSGIS get_gl_entries ()->SharpenTexFuncSGIS
#define glGetSharpenTexFuncSGIS get_gl_entries ()->GetSharpenTexFuncSGIS
#define glSampleMaskSGIS get_gl_entries ()->SampleMaskSGIS
#define glSamplePatternSGIS get_gl_entries ()->SamplePatternSGIS
#define glArrayElementEXT get_gl_entries ()->ArrayElementEXT
#define glColorPointerEXT get_gl_entries ()->ColorPointerEXT
#define glDrawArraysEXT get_gl_entries ()->DrawArraysEXT
#define glEdgeFlagPointerEXT get_gl_entries ()->EdgeFlagPointerEXT
#define glGetPointervEXT get_gl_entries ()->GetPointervEXT
#define glIndexPointerEXT get_gl_entries ()->IndexPointerEXT
#define glNormalPointerEXT get_gl_entries ()->NormalPointerEXT
#define glTexCoordPointerEXT get_gl_entries ()->TexCoordPointerEXT
#define glVertexPointerEXT get_gl_entries ()->VertexPointerEXT
#define glBlendEquationEXT get_gl_entries ()->BlendEquationEXT
#define glSpriteParameterfSGIX get_gl_entries ()->SpriteParameterfSGIX
#define glSpriteParameterfvSGIX get_gl_entries ()->SpriteParameterfvSGIX
#define glSpriteParameteriSGIX get_gl_entries ()->SpriteParameteriSGIX
#define glSpriteParameterivSGIX get_gl_entries ()->SpriteParameterivSGIX
#define glPointParameterfEXT get_gl_entries ()->PointParameterfEXT
#define glPointParameterfvEXT get_gl_entries ()->PointParameterfvEXT
#define glPointParameterfSGIS get_gl_entries ()->PointParameterfSGIS
#define glPointParameterfvSGIS get_gl_entries ()->PointParameterfvSGIS
#define glGetInstrumentsSGIX get_gl_entries ()->GetInstrumentsSGIX
#define glInstrumentsBufferSGIX get_gl_entries ()->InstrumentsBufferSGIX
#define glPollInstrumentsSGIX get_gl_entries ()->PollInstrumentsSGIX
#define glReadInstrumentsSGIX get_gl_entries ()->ReadInstrumentsSGIX
#define glStartInstrumentsSGIX get_gl_entries ()->StartInstrumentsSGIX
#define glStopInstrumentsSGIX get_gl_entries ()->StopInstrumentsSGIX
#define glFrameZoomSGIX get_gl_entries ()->FrameZoomSGIX
#define glTagSampleBufferSGIX get_gl_entries ()->TagSampleBufferSGIX
#define glDeformationMap3dSGIX get_gl_entries ()->DeformationMap3dSGIX
#define glDeformationMap3fSGIX get_gl_entries ()->DeformationMap3fSGIX
#define glDeformSGIX get_gl_entries ()->DeformSGIX
#define glLoadIdentityDeformationMapSGIX get_gl_entries ()->LoadIdentityDeformationMapSGIX
#define glReferencePlaneSGIX get_gl_entries ()->ReferencePlaneSGIX
#define glFlushRasterSGIX get_gl_entries ()->FlushRasterSGIX
#define glFogFuncSGIS get_gl_entries ()->FogFuncSGIS
#define glGetFogFuncSGIS get_gl_entries ()->GetFogFuncSGIS
#define glImageTransformParameteriHP get_gl_entries ()->ImageTransformParameteriHP
#define glImageTransformParameterfHP get_gl_entries ()->ImageTransformParameterfHP
#define glImageTransformParameterivHP get_gl_entries ()->ImageTransformParameterivHP
#define glImageTransformParameterfvHP get_gl_entries ()->ImageTransformParameterfvHP
#define glGetImageTransformParameterivHP get_gl_entries ()->GetImageTransformParameterivHP
#define glGetImageTransformParameterfvHP get_gl_entries ()->GetImageTransformParameterfvHP
#define glColorSubTableEXT get_gl_entries ()->ColorSubTableEXT
#define glCopyColorSubTableEXT get_gl_entries ()->CopyColorSubTableEXT
#define glHintPGI get_gl_entries ()->HintPGI
#define glColorTableEXT get_gl_entries ()->ColorTableEXT
#define glGetColorTableEXT get_gl_entries ()->GetColorTableEXT
#define glGetColorTableParameterivEXT get_gl_entries ()->GetColorTableParameterivEXT
#define glGetColorTableParameterfvEXT get_gl_entries ()->GetColorTableParameterfvEXT
#define glGetListParameterfvSGIX get_gl_entries ()->GetListParameterfvSGIX
#define glGetListParameterivSGIX get_gl_entries ()->GetListParameterivSGIX
#define glListParameterfSGIX get_gl_entries ()->ListParameterfSGIX
#define glListParameterfvSGIX get_gl_entries ()->ListParameterfvSGIX
#define glListParameteriSGIX get_gl_entries ()->ListParameteriSGIX
#define glListParameterivSGIX get_gl_entries ()->ListParameterivSGIX
#define glIndexMaterialEXT get_gl_entries ()->IndexMaterialEXT
#define glIndexFuncEXT get_gl_entries ()->IndexFuncEXT
#define glLockArraysEXT get_gl_entries ()->LockArraysEXT
#define glUnlockArraysEXT get_gl_entries ()->UnlockArraysEXT
#define glCullParameterdvEXT get_gl_entries ()->CullParameterdvEXT
#define glCullParameterfvEXT get_gl_entries ()->CullParameterfvEXT
#define glFragmentColorMaterialSGIX get_gl_entries ()->FragmentColorMaterialSGIX
#define glFragmentLightfSGIX get_gl_entries ()->FragmentLightfSGIX
#define glFragmentLightfvSGIX get_gl_entries ()->FragmentLightfvSGIX
#define glFragmentLightiSGIX get_gl_entries ()->FragmentLightiSGIX
#define glFragmentLightivSGIX get_gl_entries ()->FragmentLightivSGIX
#define glFragmentLightModelfSGIX get_gl_entries ()->FragmentLightModelfSGIX
#define glFragmentLightModelfvSGIX get_gl_entries ()->FragmentLightModelfvSGIX
#define glFragmentLightModeliSGIX get_gl_entries ()->FragmentLightModeliSGIX
#define glFragmentLightModelivSGIX get_gl_entries ()->FragmentLightModelivSGIX
#define glFragmentMaterialfSGIX get_gl_entries ()->FragmentMaterialfSGIX
#define glFragmentMaterialfvSGIX get_gl_entries ()->FragmentMaterialfvSGIX
#define glFragmentMaterialiSGIX get_gl_entries ()->FragmentMaterialiSGIX
#define glFragmentMaterialivSGIX get_gl_entries ()->FragmentMaterialivSGIX
#define glGetFragmentLightfvSGIX get_gl_entries ()->GetFragmentLightfvSGIX
#define glGetFragmentLightivSGIX get_gl_entries ()->GetFragmentLightivSGIX
#define glGetFragmentMaterialfvSGIX get_gl_entries ()->GetFragmentMaterialfvSGIX
#define glGetFragmentMaterialivSGIX get_gl_entries ()->GetFragmentMaterialivSGIX
#define glLightEnviSGIX get_gl_entries ()->LightEnviSGIX
#define glDrawRangeElementsEXT get_gl_entries ()->DrawRangeElementsEXT
#define glApplyTextureEXT get_gl_entries ()->ApplyTextureEXT
#define glTextureLightEXT get_gl_entries ()->TextureLightEXT
#define glTextureMaterialEXT get_gl_entries ()->TextureMaterialEXT
#define glAsyncMarkerSGIX get_gl_entries ()->AsyncMarkerSGIX
#define glFinishAsyncSGIX get_gl_entries ()->FinishAsyncSGIX
#define glPollAsyncSGIX get_gl_entries ()->PollAsyncSGIX
#define glGenAsyncMarkersSGIX get_gl_entries ()->GenAsyncMarkersSGIX
#define glDeleteAsyncMarkersSGIX get_gl_entries ()->DeleteAsyncMarkersSGIX
#define glIsAsyncMarkerSGIX get_gl_entries ()->IsAsyncMarkerSGIX
#define glVertexPointervINTEL get_gl_entries ()->VertexPointervINTEL
#define glNormalPointervINTEL get_gl_entries ()->NormalPointervINTEL
#define glColorPointervINTEL get_gl_entries ()->ColorPointervINTEL
#define glTexCoordPointervINTEL get_gl_entries ()->TexCoordPointervINTEL
#define glPixelTransformParameteriEXT get_gl_entries ()->PixelTransformParameteriEXT
#define glPixelTransformParameterfEXT get_gl_entries ()->PixelTransformParameterfEXT
#define glPixelTransformParameterivEXT get_gl_entries ()->PixelTransformParameterivEXT
#define glPixelTransformParameterfvEXT get_gl_entries ()->PixelTransformParameterfvEXT
#define glSecondaryColor3bEXT get_gl_entries ()->SecondaryColor3bEXT
#define glSecondaryColor3bvEXT get_gl_entries ()->SecondaryColor3bvEXT
#define glSecondaryColor3dEXT get_gl_entries ()->SecondaryColor3dEXT
#define glSecondaryColor3dvEXT get_gl_entries ()->SecondaryColor3dvEXT
#define glSecondaryColor3fEXT get_gl_entries ()->SecondaryColor3fEXT
#define glSecondaryColor3fvEXT get_gl_entries ()->SecondaryColor3fvEXT
#define glSecondaryColor3iEXT get_gl_entries ()->SecondaryColor3iEXT
#define glSecondaryColor3ivEXT get_gl_entries ()->SecondaryColor3ivEXT
#define glSecondaryColor3sEXT get_gl_entries ()->SecondaryColor3sEXT
#define glSecondaryColor3svEXT get_gl_entries ()->SecondaryColor3svEXT
#define glSecondaryColor3ubEXT get_gl_entries ()->SecondaryColor3ubEXT
#define glSecondaryColor3ubvEXT get_gl_entries ()->SecondaryColor3ubvEXT
#define glSecondaryColor3uiEXT get_gl_entries ()->SecondaryColor3uiEXT
#define glSecondaryColor3uivEXT get_gl_entries ()->SecondaryColor3uivEXT
#define glSecondaryColor3usEXT get_gl_entries ()->SecondaryColor3usEXT
#define glSecondaryColor3usvEXT get_gl_entries ()->SecondaryColor3usvEXT
#define glSecondaryColorPointerEXT get_gl_entries ()->SecondaryColorPointerEXT
#define glTextureNormalEXT get_gl_entries ()->TextureNormalEXT
#define glMultiDrawArraysEXT get_gl_entries ()->MultiDrawArraysEXT
#define glMultiDrawElementsEXT get_gl_entries ()->MultiDrawElementsEXT
#define glFogCoordfEXT get_gl_entries ()->FogCoordfEXT
#define glFogCoordfvEXT get_gl_entries ()->FogCoordfvEXT
#define glFogCoorddEXT get_gl_entries ()->FogCoorddEXT
#define glFogCoorddvEXT get_gl_entries ()->FogCoorddvEXT
#define glFogCoordPointerEXT get_gl_entries ()->FogCoordPointerEXT
#define glTangent3bEXT get_gl_entries ()->Tangent3bEXT
#define glTangent3bvEXT get_gl_entries ()->Tangent3bvEXT
#define glTangent3dEXT get_gl_entries ()->Tangent3dEXT
#define glTangent3dvEXT get_gl_entries ()->Tangent3dvEXT
#define glTangent3fEXT get_gl_entries ()->Tangent3fEXT
#define glTangent3fvEXT get_gl_entries ()->Tangent3fvEXT
#define glTangent3iEXT get_gl_entries ()->Tangent3iEXT
#define glTangent3ivEXT get_gl_entries ()->Tangent3ivEXT
#define glTangent3sEXT get_gl_entries ()->Tangent3sEXT
#define glTangent3svEXT get_gl_entries ()->Tangent3svEXT
#define glBinormal3bEXT get_gl_entries ()->Binormal3bEXT
#define glBinormal3bvEXT get_gl_entries ()->Binormal3bvEXT
#define glBinormal3dEXT get_gl_entries ()->Binormal3dEXT
#define glBinormal3dvEXT get_gl_entries ()->Binormal3dvEXT
#define glBinormal3fEXT get_gl_entries ()->Binormal3fEXT
#define glBinormal3fvEXT get_gl_entries ()->Binormal3fvEXT
#define glBinormal3iEXT get_gl_entries ()->Binormal3iEXT
#define glBinormal3ivEXT get_gl_entries ()->Binormal3ivEXT
#define glBinormal3sEXT get_gl_entries ()->Binormal3sEXT
#define glBinormal3svEXT get_gl_entries ()->Binormal3svEXT
#define glTangentPointerEXT get_gl_entries ()->TangentPointerEXT
#define glBinormalPointerEXT get_gl_entries ()->BinormalPointerEXT
#define glFinishTextureSUNX get_gl_entries ()->FinishTextureSUNX
#define glGlobalAlphaFactorbSUN get_gl_entries ()->GlobalAlphaFactorbSUN
#define glGlobalAlphaFactorsSUN get_gl_entries ()->GlobalAlphaFactorsSUN
#define glGlobalAlphaFactoriSUN get_gl_entries ()->GlobalAlphaFactoriSUN
#define glGlobalAlphaFactorfSUN get_gl_entries ()->GlobalAlphaFactorfSUN
#define glGlobalAlphaFactordSUN get_gl_entries ()->GlobalAlphaFactordSUN
#define glGlobalAlphaFactorubSUN get_gl_entries ()->GlobalAlphaFactorubSUN
#define glGlobalAlphaFactorusSUN get_gl_entries ()->GlobalAlphaFactorusSUN
#define glGlobalAlphaFactoruiSUN get_gl_entries ()->GlobalAlphaFactoruiSUN
#define glReplacementCodeuiSUN get_gl_entries ()->ReplacementCodeuiSUN
#define glReplacementCodeusSUN get_gl_entries ()->ReplacementCodeusSUN
#define glReplacementCodeubSUN get_gl_entries ()->ReplacementCodeubSUN
#define glReplacementCodeuivSUN get_gl_entries ()->ReplacementCodeuivSUN
#define glReplacementCodeusvSUN get_gl_entries ()->ReplacementCodeusvSUN
#define glReplacementCodeubvSUN get_gl_entries ()->ReplacementCodeubvSUN
#define glReplacementCodePointerSUN get_gl_entries ()->ReplacementCodePointerSUN
#define glColor4ubVertex2fSUN get_gl_entries ()->Color4ubVertex2fSUN
#define glColor4ubVertex2fvSUN get_gl_entries ()->Color4ubVertex2fvSUN
#define glColor4ubVertex3fSUN get_gl_entries ()->Color4ubVertex3fSUN
#define glColor4ubVertex3fvSUN get_gl_entries ()->Color4ubVertex3fvSUN
#define glColor3fVertex3fSUN get_gl_entries ()->Color3fVertex3fSUN
#define glColor3fVertex3fvSUN get_gl_entries ()->Color3fVertex3fvSUN
#define glNormal3fVertex3fSUN get_gl_entries ()->Normal3fVertex3fSUN
#define glNormal3fVertex3fvSUN get_gl_entries ()->Normal3fVertex3fvSUN
#define glColor4fNormal3fVertex3fSUN get_gl_entries ()->Color4fNormal3fVertex3fSUN
#define glColor4fNormal3fVertex3fvSUN get_gl_entries ()->Color4fNormal3fVertex3fvSUN
#define glTexCoord2fVertex3fSUN get_gl_entries ()->TexCoord2fVertex3fSUN
#define glTexCoord2fVertex3fvSUN get_gl_entries ()->TexCoord2fVertex3fvSUN
#define glTexCoord4fVertex4fSUN get_gl_entries ()->TexCoord4fVertex4fSUN
#define glTexCoord4fVertex4fvSUN get_gl_entries ()->TexCoord4fVertex4fvSUN
#define glTexCoord2fColor4ubVertex3fSUN get_gl_entries ()->TexCoord2fColor4ubVertex3fSUN
#define glTexCoord2fColor4ubVertex3fvSUN get_gl_entries ()->TexCoord2fColor4ubVertex3fvSUN
#define glTexCoord2fColor3fVertex3fSUN get_gl_entries ()->TexCoord2fColor3fVertex3fSUN
#define glTexCoord2fColor3fVertex3fvSUN get_gl_entries ()->TexCoord2fColor3fVertex3fvSUN
#define glTexCoord2fNormal3fVertex3fSUN get_gl_entries ()->TexCoord2fNormal3fVertex3fSUN
#define glTexCoord2fNormal3fVertex3fvSUN get_gl_entries ()->TexCoord2fNormal3fVertex3fvSUN
#define glTexCoord2fColor4fNormal3fVertex3fSUN get_gl_entries ()->TexCoord2fColor4fNormal3fVertex3fSUN
#define glTexCoord2fColor4fNormal3fVertex3fvSUN get_gl_entries ()->TexCoord2fColor4fNormal3fVertex3fvSUN
#define glTexCoord4fColor4fNormal3fVertex4fSUN get_gl_entries ()->TexCoord4fColor4fNormal3fVertex4fSUN
#define glTexCoord4fColor4fNormal3fVertex4fvSUN get_gl_entries ()->TexCoord4fColor4fNormal3fVertex4fvSUN
#define glReplacementCodeuiVertex3fSUN get_gl_entries ()->ReplacementCodeuiVertex3fSUN
#define glReplacementCodeuiVertex3fvSUN get_gl_entries ()->ReplacementCodeuiVertex3fvSUN
#define glReplacementCodeuiColor4ubVertex3fSUN get_gl_entries ()->ReplacementCodeuiColor4ubVertex3fSUN
#define glReplacementCodeuiColor4ubVertex3fvSUN get_gl_entries ()->ReplacementCodeuiColor4ubVertex3fvSUN
#define glReplacementCodeuiColor3fVertex3fSUN get_gl_entries ()->ReplacementCodeuiColor3fVertex3fSUN
#define glReplacementCodeuiColor3fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiColor3fVertex3fvSUN
#define glReplacementCodeuiNormal3fVertex3fSUN get_gl_entries ()->ReplacementCodeuiNormal3fVertex3fSUN
#define glReplacementCodeuiNormal3fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiNormal3fVertex3fvSUN
#define glReplacementCodeuiColor4fNormal3fVertex3fSUN get_gl_entries ()->ReplacementCodeuiColor4fNormal3fVertex3fSUN
#define glReplacementCodeuiColor4fNormal3fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiColor4fNormal3fVertex3fvSUN
#define glReplacementCodeuiTexCoord2fVertex3fSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fVertex3fSUN
#define glReplacementCodeuiTexCoord2fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fVertex3fvSUN
#define glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN
#define glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN
#define glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN
#define glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN get_gl_entries ()->ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN
#define glBlendFuncSeparateEXT get_gl_entries ()->BlendFuncSeparateEXT
#define glBlendFuncSeparateINGR get_gl_entries ()->BlendFuncSeparateINGR
#define glVertexWeightfEXT get_gl_entries ()->VertexWeightfEXT
#define glVertexWeightfvEXT get_gl_entries ()->VertexWeightfvEXT
#define glVertexWeightPointerEXT get_gl_entries ()->VertexWeightPointerEXT
#define glFlushVertexArrayRangeNV get_gl_entries ()->FlushVertexArrayRangeNV
#define glVertexArrayRangeNV get_gl_entries ()->VertexArrayRangeNV
#define glCombinerParameterfvNV get_gl_entries ()->CombinerParameterfvNV
#define glCombinerParameterfNV get_gl_entries ()->CombinerParameterfNV
#define glCombinerParameterivNV get_gl_entries ()->CombinerParameterivNV
#define glCombinerParameteriNV get_gl_entries ()->CombinerParameteriNV
#define glCombinerInputNV get_gl_entries ()->CombinerInputNV
#define glCombinerOutputNV get_gl_entries ()->CombinerOutputNV
#define glFinalCombinerInputNV get_gl_entries ()->FinalCombinerInputNV
#define glGetCombinerInputParameterfvNV get_gl_entries ()->GetCombinerInputParameterfvNV
#define glGetCombinerInputParameterivNV get_gl_entries ()->GetCombinerInputParameterivNV
#define glGetCombinerOutputParameterfvNV get_gl_entries ()->GetCombinerOutputParameterfvNV
#define glGetCombinerOutputParameterivNV get_gl_entries ()->GetCombinerOutputParameterivNV
#define glGetFinalCombinerInputParameterfvNV get_gl_entries ()->GetFinalCombinerInputParameterfvNV
#define glGetFinalCombinerInputParameterivNV get_gl_entries ()->GetFinalCombinerInputParameterivNV
#define glResizeBuffersMESA get_gl_entries ()->ResizeBuffersMESA
#define glWindowPos2dMESA get_gl_entries ()->WindowPos2dMESA
#define glWindowPos2dvMESA get_gl_entries ()->WindowPos2dvMESA
#define glWindowPos2fMESA get_gl_entries ()->WindowPos2fMESA
#define glWindowPos2fvMESA get_gl_entries ()->WindowPos2fvMESA
#define glWindowPos2iMESA get_gl_entries ()->WindowPos2iMESA
#define glWindowPos2ivMESA get_gl_entries ()->WindowPos2ivMESA
#define glWindowPos2sMESA get_gl_entries ()->WindowPos2sMESA
#define glWindowPos2svMESA get_gl_entries ()->WindowPos2svMESA
#define glWindowPos3dMESA get_gl_entries ()->WindowPos3dMESA
#define glWindowPos3dvMESA get_gl_entries ()->WindowPos3dvMESA
#define glWindowPos3fMESA get_gl_entries ()->WindowPos3fMESA
#define glWindowPos3fvMESA get_gl_entries ()->WindowPos3fvMESA
#define glWindowPos3iMESA get_gl_entries ()->WindowPos3iMESA
#define glWindowPos3ivMESA get_gl_entries ()->WindowPos3ivMESA
#define glWindowPos3sMESA get_gl_entries ()->WindowPos3sMESA
#define glWindowPos3svMESA get_gl_entries ()->WindowPos3svMESA
#define glWindowPos4dMESA get_gl_entries ()->WindowPos4dMESA
#define glWindowPos4dvMESA get_gl_entries ()->WindowPos4dvMESA
#define glWindowPos4fMESA get_gl_entries ()->WindowPos4fMESA
#define glWindowPos4fvMESA get_gl_entries ()->WindowPos4fvMESA
#define glWindowPos4iMESA get_gl_entries ()->WindowPos4iMESA
#define glWindowPos4ivMESA get_gl_entries ()->WindowPos4ivMESA
#define glWindowPos4sMESA get_gl_entries ()->WindowPos4sMESA
#define glWindowPos4svMESA get_gl_entries ()->WindowPos4svMESA
#define glMultiModeDrawArraysIBM get_gl_entries ()->MultiModeDrawArraysIBM
#define glMultiModeDrawElementsIBM get_gl_entries ()->MultiModeDrawElementsIBM
#define glColorPointerListIBM get_gl_entries ()->ColorPointerListIBM
#define glSecondaryColorPointerListIBM get_gl_entries ()->SecondaryColorPointerListIBM
#define glEdgeFlagPointerListIBM get_gl_entries ()->EdgeFlagPointerListIBM
#define glFogCoordPointerListIBM get_gl_entries ()->FogCoordPointerListIBM
#define glIndexPointerListIBM get_gl_entries ()->IndexPointerListIBM
#define glNormalPointerListIBM get_gl_entries ()->NormalPointerListIBM
#define glTexCoordPointerListIBM get_gl_entries ()->TexCoordPointerListIBM
#define glVertexPointerListIBM get_gl_entries ()->VertexPointerListIBM
#define glTbufferMask3DFX get_gl_entries ()->TbufferMask3DFX
#define glSampleMaskEXT get_gl_entries ()->SampleMaskEXT
#define glSamplePatternEXT get_gl_entries ()->SamplePatternEXT
#define glTextureColorMaskSGIS get_gl_entries ()->TextureColorMaskSGIS
#define glIglooInterfaceSGIX get_gl_entries ()->IglooInterfaceSGIX
#define glDeleteFencesNV get_gl_entries ()->DeleteFencesNV
#define glGenFencesNV get_gl_entries ()->GenFencesNV
#define glIsFenceNV get_gl_entries ()->IsFenceNV
#define glTestFenceNV get_gl_entries ()->TestFenceNV
#define glGetFenceivNV get_gl_entries ()->GetFenceivNV
#define glFinishFenceNV get_gl_entries ()->FinishFenceNV
#define glSetFenceNV get_gl_entries ()->SetFenceNV
#define glMapControlPointsNV get_gl_entries ()->MapControlPointsNV
#define glMapParameterivNV get_gl_entries ()->MapParameterivNV
#define glMapParameterfvNV get_gl_entries ()->MapParameterfvNV
#define glGetMapControlPointsNV get_gl_entries ()->GetMapControlPointsNV
#define glGetMapParameterivNV get_gl_entries ()->GetMapParameterivNV
#define glGetMapParameterfvNV get_gl_entries ()->GetMapParameterfvNV
#define glGetMapAttribParameterivNV get_gl_entries ()->GetMapAttribParameterivNV
#define glGetMapAttribParameterfvNV get_gl_entries ()->GetMapAttribParameterfvNV
#define glEvalMapsNV get_gl_entries ()->EvalMapsNV
#define glCombinerStageParameterfvNV get_gl_entries ()->CombinerStageParameterfvNV
#define glGetCombinerStageParameterfvNV get_gl_entries ()->GetCombinerStageParameterfvNV
#define glAreProgramsResidentNV get_gl_entries ()->AreProgramsResidentNV
#define glBindProgramNV get_gl_entries ()->BindProgramNV
#define glDeleteProgramsNV get_gl_entries ()->DeleteProgramsNV
#define glExecuteProgramNV get_gl_entries ()->ExecuteProgramNV
#define glGenProgramsNV get_gl_entries ()->GenProgramsNV
#define glGetProgramParameterdvNV get_gl_entries ()->GetProgramParameterdvNV
#define glGetProgramParameterfvNV get_gl_entries ()->GetProgramParameterfvNV
#define glGetProgramivNV get_gl_entries ()->GetProgramivNV
#define glGetProgramStringNV get_gl_entries ()->GetProgramStringNV
#define glGetTrackMatrixivNV get_gl_entries ()->GetTrackMatrixivNV
#define glGetVertexAttribdvNV get_gl_entries ()->GetVertexAttribdvNV
#define glGetVertexAttribfvNV get_gl_entries ()->GetVertexAttribfvNV
#define glGetVertexAttribivNV get_gl_entries ()->GetVertexAttribivNV
#define glGetVertexAttribPointervNV get_gl_entries ()->GetVertexAttribPointervNV
#define glIsProgramNV get_gl_entries ()->IsProgramNV
#define glLoadProgramNV get_gl_entries ()->LoadProgramNV
#define glProgramParameter4dNV get_gl_entries ()->ProgramParameter4dNV
#define glProgramParameter4dvNV get_gl_entries ()->ProgramParameter4dvNV
#define glProgramParameter4fNV get_gl_entries ()->ProgramParameter4fNV
#define glProgramParameter4fvNV get_gl_entries ()->ProgramParameter4fvNV
#define glProgramParameters4dvNV get_gl_entries ()->ProgramParameters4dvNV
#define glProgramParameters4fvNV get_gl_entries ()->ProgramParameters4fvNV
#define glRequestResidentProgramsNV get_gl_entries ()->RequestResidentProgramsNV
#define glTrackMatrixNV get_gl_entries ()->TrackMatrixNV
#define glVertexAttribPointerNV get_gl_entries ()->VertexAttribPointerNV
#define glVertexAttrib1dNV get_gl_entries ()->VertexAttrib1dNV
#define glVertexAttrib1dvNV get_gl_entries ()->VertexAttrib1dvNV
#define glVertexAttrib1fNV get_gl_entries ()->VertexAttrib1fNV
#define glVertexAttrib1fvNV get_gl_entries ()->VertexAttrib1fvNV
#define glVertexAttrib1sNV get_gl_entries ()->VertexAttrib1sNV
#define glVertexAttrib1svNV get_gl_entries ()->VertexAttrib1svNV
#define glVertexAttrib2dNV get_gl_entries ()->VertexAttrib2dNV
#define glVertexAttrib2dvNV get_gl_entries ()->VertexAttrib2dvNV
#define glVertexAttrib2fNV get_gl_entries ()->VertexAttrib2fNV
#define glVertexAttrib2fvNV get_gl_entries ()->VertexAttrib2fvNV
#define glVertexAttrib2sNV get_gl_entries ()->VertexAttrib2sNV
#define glVertexAttrib2svNV get_gl_entries ()->VertexAttrib2svNV
#define glVertexAttrib3dNV get_gl_entries ()->VertexAttrib3dNV
#define glVertexAttrib3dvNV get_gl_entries ()->VertexAttrib3dvNV
#define glVertexAttrib3fNV get_gl_entries ()->VertexAttrib3fNV
#define glVertexAttrib3fvNV get_gl_entries ()->VertexAttrib3fvNV
#define glVertexAttrib3sNV get_gl_entries ()->VertexAttrib3sNV
#define glVertexAttrib3svNV get_gl_entries ()->VertexAttrib3svNV
#define glVertexAttrib4dNV get_gl_entries ()->VertexAttrib4dNV
#define glVertexAttrib4dvNV get_gl_entries ()->VertexAttrib4dvNV
#define glVertexAttrib4fNV get_gl_entries ()->VertexAttrib4fNV
#define glVertexAttrib4fvNV get_gl_entries ()->VertexAttrib4fvNV
#define glVertexAttrib4sNV get_gl_entries ()->VertexAttrib4sNV
#define glVertexAttrib4svNV get_gl_entries ()->VertexAttrib4svNV
#define glVertexAttrib4ubNV get_gl_entries ()->VertexAttrib4ubNV
#define glVertexAttrib4ubvNV get_gl_entries ()->VertexAttrib4ubvNV
#define glVertexAttribs1dvNV get_gl_entries ()->VertexAttribs1dvNV
#define glVertexAttribs1fvNV get_gl_entries ()->VertexAttribs1fvNV
#define glVertexAttribs1svNV get_gl_entries ()->VertexAttribs1svNV
#define glVertexAttribs2dvNV get_gl_entries ()->VertexAttribs2dvNV
#define glVertexAttribs2fvNV get_gl_entries ()->VertexAttribs2fvNV
#define glVertexAttribs2svNV get_gl_entries ()->VertexAttribs2svNV
#define glVertexAttribs3dvNV get_gl_entries ()->VertexAttribs3dvNV
#define glVertexAttribs3fvNV get_gl_entries ()->VertexAttribs3fvNV
#define glVertexAttribs3svNV get_gl_entries ()->VertexAttribs3svNV
#define glVertexAttribs4dvNV get_gl_entries ()->VertexAttribs4dvNV
#define glVertexAttribs4fvNV get_gl_entries ()->VertexAttribs4fvNV
#define glVertexAttribs4svNV get_gl_entries ()->VertexAttribs4svNV
#define glVertexAttribs4ubvNV get_gl_entries ()->VertexAttribs4ubvNV
#define glTexBumpParameterivATI get_gl_entries ()->TexBumpParameterivATI
#define glTexBumpParameterfvATI get_gl_entries ()->TexBumpParameterfvATI
#define glGetTexBumpParameterivATI get_gl_entries ()->GetTexBumpParameterivATI
#define glGetTexBumpParameterfvATI get_gl_entries ()->GetTexBumpParameterfvATI
#define glGenFragmentShadersATI get_gl_entries ()->GenFragmentShadersATI
#define glBindFragmentShaderATI get_gl_entries ()->BindFragmentShaderATI
#define glDeleteFragmentShaderATI get_gl_entries ()->DeleteFragmentShaderATI
#define glBeginFragmentShaderATI get_gl_entries ()->BeginFragmentShaderATI
#define glEndFragmentShaderATI get_gl_entries ()->EndFragmentShaderATI
#define glPassTexCoordATI get_gl_entries ()->PassTexCoordATI
#define glSampleMapATI get_gl_entries ()->SampleMapATI
#define glColorFragmentOp1ATI get_gl_entries ()->ColorFragmentOp1ATI
#define glColorFragmentOp2ATI get_gl_entries ()->ColorFragmentOp2ATI
#define glColorFragmentOp3ATI get_gl_entries ()->ColorFragmentOp3ATI
#define glAlphaFragmentOp1ATI get_gl_entries ()->AlphaFragmentOp1ATI
#define glAlphaFragmentOp2ATI get_gl_entries ()->AlphaFragmentOp2ATI
#define glAlphaFragmentOp3ATI get_gl_entries ()->AlphaFragmentOp3ATI
#define glSetFragmentShaderConstantATI get_gl_entries ()->SetFragmentShaderConstantATI
#define glPNTrianglesiATI get_gl_entries ()->PNTrianglesiATI
#define glPNTrianglesfATI get_gl_entries ()->PNTrianglesfATI
#define glNewObjectBufferATI get_gl_entries ()->NewObjectBufferATI
#define glIsObjectBufferATI get_gl_entries ()->IsObjectBufferATI
#define glUpdateObjectBufferATI get_gl_entries ()->UpdateObjectBufferATI
#define glGetObjectBufferfvATI get_gl_entries ()->GetObjectBufferfvATI
#define glGetObjectBufferivATI get_gl_entries ()->GetObjectBufferivATI
#define glFreeObjectBufferATI get_gl_entries ()->FreeObjectBufferATI
#define glArrayObjectATI get_gl_entries ()->ArrayObjectATI
#define glGetArrayObjectfvATI get_gl_entries ()->GetArrayObjectfvATI
#define glGetArrayObjectivATI get_gl_entries ()->GetArrayObjectivATI
#define glVariantArrayObjectATI get_gl_entries ()->VariantArrayObjectATI
#define glGetVariantArrayObjectfvATI get_gl_entries ()->GetVariantArrayObjectfvATI
#define glGetVariantArrayObjectivATI get_gl_entries ()->GetVariantArrayObjectivATI
#define glBeginVertexShaderEXT get_gl_entries ()->BeginVertexShaderEXT
#define glEndVertexShaderEXT get_gl_entries ()->EndVertexShaderEXT
#define glBindVertexShaderEXT get_gl_entries ()->BindVertexShaderEXT
#define glGenVertexShadersEXT get_gl_entries ()->GenVertexShadersEXT
#define glDeleteVertexShaderEXT get_gl_entries ()->DeleteVertexShaderEXT
#define glShaderOp1EXT get_gl_entries ()->ShaderOp1EXT
#define glShaderOp2EXT get_gl_entries ()->ShaderOp2EXT
#define glShaderOp3EXT get_gl_entries ()->ShaderOp3EXT
#define glSwizzleEXT get_gl_entries ()->SwizzleEXT
#define glWriteMaskEXT get_gl_entries ()->WriteMaskEXT
#define glInsertComponentEXT get_gl_entries ()->InsertComponentEXT
#define glExtractComponentEXT get_gl_entries ()->ExtractComponentEXT
#define glGenSymbolsEXT get_gl_entries ()->GenSymbolsEXT
#define glSetInvariantEXT get_gl_entries ()->SetInvariantEXT
#define glSetLocalConstantEXT get_gl_entries ()->SetLocalConstantEXT
#define glVariantbvEXT get_gl_entries ()->VariantbvEXT
#define glVariantsvEXT get_gl_entries ()->VariantsvEXT
#define glVariantivEXT get_gl_entries ()->VariantivEXT
#define glVariantfvEXT get_gl_entries ()->VariantfvEXT
#define glVariantdvEXT get_gl_entries ()->VariantdvEXT
#define glVariantubvEXT get_gl_entries ()->VariantubvEXT
#define glVariantusvEXT get_gl_entries ()->VariantusvEXT
#define glVariantuivEXT get_gl_entries ()->VariantuivEXT
#define glVariantPointerEXT get_gl_entries ()->VariantPointerEXT
#define glEnableVariantClientStateEXT get_gl_entries ()->EnableVariantClientStateEXT
#define glDisableVariantClientStateEXT get_gl_entries ()->DisableVariantClientStateEXT
#define glBindLightParameterEXT get_gl_entries ()->BindLightParameterEXT
#define glBindMaterialParameterEXT get_gl_entries ()->BindMaterialParameterEXT
#define glBindTexGenParameterEXT get_gl_entries ()->BindTexGenParameterEXT
#define glBindTextureUnitParameterEXT get_gl_entries ()->BindTextureUnitParameterEXT
#define glBindParameterEXT get_gl_entries ()->BindParameterEXT
#define glIsVariantEnabledEXT get_gl_entries ()->IsVariantEnabledEXT
#define glGetVariantBooleanvEXT get_gl_entries ()->GetVariantBooleanvEXT
#define glGetVariantIntegervEXT get_gl_entries ()->GetVariantIntegervEXT
#define glGetVariantFloatvEXT get_gl_entries ()->GetVariantFloatvEXT
#define glGetVariantPointervEXT get_gl_entries ()->GetVariantPointervEXT
#define glGetInvariantBooleanvEXT get_gl_entries ()->GetInvariantBooleanvEXT
#define glGetInvariantIntegervEXT get_gl_entries ()->GetInvariantIntegervEXT
#define glGetInvariantFloatvEXT get_gl_entries ()->GetInvariantFloatvEXT
#define glGetLocalConstantBooleanvEXT get_gl_entries ()->GetLocalConstantBooleanvEXT
#define glGetLocalConstantIntegervEXT get_gl_entries ()->GetLocalConstantIntegervEXT
#define glGetLocalConstantFloatvEXT get_gl_entries ()->GetLocalConstantFloatvEXT
#define glVertexStream1sATI get_gl_entries ()->VertexStream1sATI
#define glVertexStream1svATI get_gl_entries ()->VertexStream1svATI
#define glVertexStream1iATI get_gl_entries ()->VertexStream1iATI
#define glVertexStream1ivATI get_gl_entries ()->VertexStream1ivATI
#define glVertexStream1fATI get_gl_entries ()->VertexStream1fATI
#define glVertexStream1fvATI get_gl_entries ()->VertexStream1fvATI
#define glVertexStream1dATI get_gl_entries ()->VertexStream1dATI
#define glVertexStream1dvATI get_gl_entries ()->VertexStream1dvATI
#define glVertexStream2sATI get_gl_entries ()->VertexStream2sATI
#define glVertexStream2svATI get_gl_entries ()->VertexStream2svATI
#define glVertexStream2iATI get_gl_entries ()->VertexStream2iATI
#define glVertexStream2ivATI get_gl_entries ()->VertexStream2ivATI
#define glVertexStream2fATI get_gl_entries ()->VertexStream2fATI
#define glVertexStream2fvATI get_gl_entries ()->VertexStream2fvATI
#define glVertexStream2dATI get_gl_entries ()->VertexStream2dATI
#define glVertexStream2dvATI get_gl_entries ()->VertexStream2dvATI
#define glVertexStream3sATI get_gl_entries ()->VertexStream3sATI
#define glVertexStream3svATI get_gl_entries ()->VertexStream3svATI
#define glVertexStream3iATI get_gl_entries ()->VertexStream3iATI
#define glVertexStream3ivATI get_gl_entries ()->VertexStream3ivATI
#define glVertexStream3fATI get_gl_entries ()->VertexStream3fATI
#define glVertexStream3fvATI get_gl_entries ()->VertexStream3fvATI
#define glVertexStream3dATI get_gl_entries ()->VertexStream3dATI
#define glVertexStream3dvATI get_gl_entries ()->VertexStream3dvATI
#define glVertexStream4sATI get_gl_entries ()->VertexStream4sATI
#define glVertexStream4svATI get_gl_entries ()->VertexStream4svATI
#define glVertexStream4iATI get_gl_entries ()->VertexStream4iATI
#define glVertexStream4ivATI get_gl_entries ()->VertexStream4ivATI
#define glVertexStream4fATI get_gl_entries ()->VertexStream4fATI
#define glVertexStream4fvATI get_gl_entries ()->VertexStream4fvATI
#define glVertexStream4dATI get_gl_entries ()->VertexStream4dATI
#define glVertexStream4dvATI get_gl_entries ()->VertexStream4dvATI
#define glNormalStream3bATI get_gl_entries ()->NormalStream3bATI
#define glNormalStream3bvATI get_gl_entries ()->NormalStream3bvATI
#define glNormalStream3sATI get_gl_entries ()->NormalStream3sATI
#define glNormalStream3svATI get_gl_entries ()->NormalStream3svATI
#define glNormalStream3iATI get_gl_entries ()->NormalStream3iATI
#define glNormalStream3ivATI get_gl_entries ()->NormalStream3ivATI
#define glNormalStream3fATI get_gl_entries ()->NormalStream3fATI
#define glNormalStream3fvATI get_gl_entries ()->NormalStream3fvATI
#define glNormalStream3dATI get_gl_entries ()->NormalStream3dATI
#define glNormalStream3dvATI get_gl_entries ()->NormalStream3dvATI
#define glClientActiveVertexStreamATI get_gl_entries ()->ClientActiveVertexStreamATI
#define glVertexBlendEnviATI get_gl_entries ()->VertexBlendEnviATI
#define glVertexBlendEnvfATI get_gl_entries ()->VertexBlendEnvfATI
#define glElementPointerATI get_gl_entries ()->ElementPointerATI
#define glDrawElementArrayATI get_gl_entries ()->DrawElementArrayATI
#define glDrawRangeElementArrayATI get_gl_entries ()->DrawRangeElementArrayATI
#define glDrawMeshArraysSUN get_gl_entries ()->DrawMeshArraysSUN
#define glGenOcclusionQueriesNV get_gl_entries ()->GenOcclusionQueriesNV
#define glDeleteOcclusionQueriesNV get_gl_entries ()->DeleteOcclusionQueriesNV
#define glIsOcclusionQueryNV get_gl_entries ()->IsOcclusionQueryNV
#define glBeginOcclusionQueryNV get_gl_entries ()->BeginOcclusionQueryNV
#define glEndOcclusionQueryNV get_gl_entries ()->EndOcclusionQueryNV
#define glGetOcclusionQueryivNV get_gl_entries ()->GetOcclusionQueryivNV
#define glGetOcclusionQueryuivNV get_gl_entries ()->GetOcclusionQueryuivNV
#define glPointParameteriNV get_gl_entries ()->PointParameteriNV
#define glPointParameterivNV get_gl_entries ()->PointParameterivNV
#define glActiveStencilFaceEXT get_gl_entries ()->ActiveStencilFaceEXT
#define glElementPointerAPPLE get_gl_entries ()->ElementPointerAPPLE
#define glDrawElementArrayAPPLE get_gl_entries ()->DrawElementArrayAPPLE
#define glDrawRangeElementArrayAPPLE get_gl_entries ()->DrawRangeElementArrayAPPLE
#define glMultiDrawElementArrayAPPLE get_gl_entries ()->MultiDrawElementArrayAPPLE
#define glMultiDrawRangeElementArrayAPPLE get_gl_entries ()->MultiDrawRangeElementArrayAPPLE
#define glGenFencesAPPLE get_gl_entries ()->GenFencesAPPLE
#define glDeleteFencesAPPLE get_gl_entries ()->DeleteFencesAPPLE
#define glSetFenceAPPLE get_gl_entries ()->SetFenceAPPLE
#define glIsFenceAPPLE get_gl_entries ()->IsFenceAPPLE
#define glTestFenceAPPLE get_gl_entries ()->TestFenceAPPLE
#define glFinishFenceAPPLE get_gl_entries ()->FinishFenceAPPLE
#define glTestObjectAPPLE get_gl_entries ()->TestObjectAPPLE
#define glFinishObjectAPPLE get_gl_entries ()->FinishObjectAPPLE
#define glBindVertexArrayAPPLE get_gl_entries ()->BindVertexArrayAPPLE
#define glDeleteVertexArraysAPPLE get_gl_entries ()->DeleteVertexArraysAPPLE
#define glGenVertexArraysAPPLE get_gl_entries ()->GenVertexArraysAPPLE
#define glIsVertexArrayAPPLE get_gl_entries ()->IsVertexArrayAPPLE
#define glVertexArrayRangeAPPLE get_gl_entries ()->VertexArrayRangeAPPLE
#define glFlushVertexArrayRangeAPPLE get_gl_entries ()->FlushVertexArrayRangeAPPLE
#define glVertexArrayParameteriAPPLE get_gl_entries ()->VertexArrayParameteriAPPLE
#define glDrawBuffersATI get_gl_entries ()->DrawBuffersATI
#define glProgramNamedParameter4fNV get_gl_entries ()->ProgramNamedParameter4fNV
#define glProgramNamedParameter4dNV get_gl_entries ()->ProgramNamedParameter4dNV
#define glProgramNamedParameter4fvNV get_gl_entries ()->ProgramNamedParameter4fvNV
#define glProgramNamedParameter4dvNV get_gl_entries ()->ProgramNamedParameter4dvNV
#define glGetProgramNamedParameterfvNV get_gl_entries ()->GetProgramNamedParameterfvNV
#define glGetProgramNamedParameterdvNV get_gl_entries ()->GetProgramNamedParameterdvNV
#define glVertex2hNV get_gl_entries ()->Vertex2hNV
#define glVertex2hvNV get_gl_entries ()->Vertex2hvNV
#define glVertex3hNV get_gl_entries ()->Vertex3hNV
#define glVertex3hvNV get_gl_entries ()->Vertex3hvNV
#define glVertex4hNV get_gl_entries ()->Vertex4hNV
#define glVertex4hvNV get_gl_entries ()->Vertex4hvNV
#define glNormal3hNV get_gl_entries ()->Normal3hNV
#define glNormal3hvNV get_gl_entries ()->Normal3hvNV
#define glColor3hNV get_gl_entries ()->Color3hNV
#define glColor3hvNV get_gl_entries ()->Color3hvNV
#define glColor4hNV get_gl_entries ()->Color4hNV
#define glColor4hvNV get_gl_entries ()->Color4hvNV
#define glTexCoord1hNV get_gl_entries ()->TexCoord1hNV
#define glTexCoord1hvNV get_gl_entries ()->TexCoord1hvNV
#define glTexCoord2hNV get_gl_entries ()->TexCoord2hNV
#define glTexCoord2hvNV get_gl_entries ()->TexCoord2hvNV
#define glTexCoord3hNV get_gl_entries ()->TexCoord3hNV
#define glTexCoord3hvNV get_gl_entries ()->TexCoord3hvNV
#define glTexCoord4hNV get_gl_entries ()->TexCoord4hNV
#define glTexCoord4hvNV get_gl_entries ()->TexCoord4hvNV
#define glMultiTexCoord1hNV get_gl_entries ()->MultiTexCoord1hNV
#define glMultiTexCoord1hvNV get_gl_entries ()->MultiTexCoord1hvNV
#define glMultiTexCoord2hNV get_gl_entries ()->MultiTexCoord2hNV
#define glMultiTexCoord2hvNV get_gl_entries ()->MultiTexCoord2hvNV
#define glMultiTexCoord3hNV get_gl_entries ()->MultiTexCoord3hNV
#define glMultiTexCoord3hvNV get_gl_entries ()->MultiTexCoord3hvNV
#define glMultiTexCoord4hNV get_gl_entries ()->MultiTexCoord4hNV
#define glMultiTexCoord4hvNV get_gl_entries ()->MultiTexCoord4hvNV
#define glFogCoordhNV get_gl_entries ()->FogCoordhNV
#define glFogCoordhvNV get_gl_entries ()->FogCoordhvNV
#define glSecondaryColor3hNV get_gl_entries ()->SecondaryColor3hNV
#define glSecondaryColor3hvNV get_gl_entries ()->SecondaryColor3hvNV
#define glVertexWeighthNV get_gl_entries ()->VertexWeighthNV
#define glVertexWeighthvNV get_gl_entries ()->VertexWeighthvNV
#define glVertexAttrib1hNV get_gl_entries ()->VertexAttrib1hNV
#define glVertexAttrib1hvNV get_gl_entries ()->VertexAttrib1hvNV
#define glVertexAttrib2hNV get_gl_entries ()->VertexAttrib2hNV
#define glVertexAttrib2hvNV get_gl_entries ()->VertexAttrib2hvNV
#define glVertexAttrib3hNV get_gl_entries ()->VertexAttrib3hNV
#define glVertexAttrib3hvNV get_gl_entries ()->VertexAttrib3hvNV
#define glVertexAttrib4hNV get_gl_entries ()->VertexAttrib4hNV
#define glVertexAttrib4hvNV get_gl_entries ()->VertexAttrib4hvNV
#define glVertexAttribs1hvNV get_gl_entries ()->VertexAttribs1hvNV
#define glVertexAttribs2hvNV get_gl_entries ()->VertexAttribs2hvNV
#define glVertexAttribs3hvNV get_gl_entries ()->VertexAttribs3hvNV
#define glVertexAttribs4hvNV get_gl_entries ()->VertexAttribs4hvNV
#define glPixelDataRangeNV get_gl_entries ()->PixelDataRangeNV
#define glFlushPixelDataRangeNV get_gl_entries ()->FlushPixelDataRangeNV
#define glPrimitiveRestartNV get_gl_entries ()->PrimitiveRestartNV
#define glPrimitiveRestartIndexNV get_gl_entries ()->PrimitiveRestartIndexNV
#define glMapObjectBufferATI get_gl_entries ()->MapObjectBufferATI
#define glUnmapObjectBufferATI get_gl_entries ()->UnmapObjectBufferATI
#define glStencilOpSeparateATI get_gl_entries ()->StencilOpSeparateATI
#define glStencilFuncSeparateATI get_gl_entries ()->StencilFuncSeparateATI
#define glVertexAttribArrayObjectATI get_gl_entries ()->VertexAttribArrayObjectATI
#define glGetVertexAttribArrayObjectfvATI get_gl_entries ()->GetVertexAttribArrayObjectfvATI
#define glGetVertexAttribArrayObjectivATI get_gl_entries ()->GetVertexAttribArrayObjectivATI
#define glDepthBoundsEXT get_gl_entries ()->DepthBoundsEXT
#define glBlendEquationSeparateEXT get_gl_entries ()->BlendEquationSeparateEXT
#define glIsRenderbufferEXT get_gl_entries ()->IsRenderbufferEXT
#define glBindRenderbufferEXT get_gl_entries ()->BindRenderbufferEXT
#define glDeleteRenderbuffersEXT get_gl_entries ()->DeleteRenderbuffersEXT
#define glGenRenderbuffersEXT get_gl_entries ()->GenRenderbuffersEXT
#define glRenderbufferStorageEXT get_gl_entries ()->RenderbufferStorageEXT
#define glGetRenderbufferParameterivEXT get_gl_entries ()->GetRenderbufferParameterivEXT
#define glIsFramebufferEXT get_gl_entries ()->IsFramebufferEXT
#define glBindFramebufferEXT get_gl_entries ()->BindFramebufferEXT
#define glDeleteFramebuffersEXT get_gl_entries ()->DeleteFramebuffersEXT
#define glGenFramebuffersEXT get_gl_entries ()->GenFramebuffersEXT
#define glCheckFramebufferStatusEXT get_gl_entries ()->CheckFramebufferStatusEXT
#define glFramebufferTexture1DEXT get_gl_entries ()->FramebufferTexture1DEXT
#define glFramebufferTexture2DEXT get_gl_entries ()->FramebufferTexture2DEXT
#define glFramebufferTexture3DEXT get_gl_entries ()->FramebufferTexture3DEXT
#define glFramebufferRenderbufferEXT get_gl_entries ()->FramebufferRenderbufferEXT
#define glGetFramebufferAttachmentParameterivEXT get_gl_entries ()->GetFramebufferAttachmentParameterivEXT
#define glGenerateMipmapEXT get_gl_entries ()->GenerateMipmapEXT
#define glStringMarkerGREMEDY get_gl_entries ()->StringMarkerGREMEDY
#define glStencilClearTagEXT get_gl_entries ()->StencilClearTagEXT
#define glBlitFramebufferEXT get_gl_entries ()->BlitFramebufferEXT
#define glRenderbufferStorageMultisampleEXT get_gl_entries ()->RenderbufferStorageMultisampleEXT
#define glGetQueryObjecti64vEXT get_gl_entries ()->GetQueryObjecti64vEXT
#define glGetQueryObjectui64vEXT get_gl_entries ()->GetQueryObjectui64vEXT
#define glProgramEnvParameters4fvEXT get_gl_entries ()->ProgramEnvParameters4fvEXT
#define glProgramLocalParameters4fvEXT get_gl_entries ()->ProgramLocalParameters4fvEXT
#define glBufferParameteriAPPLE get_gl_entries ()->BufferParameteriAPPLE
#define glFlushMappedBufferRangeAPPLE get_gl_entries ()->FlushMappedBufferRangeAPPLE
#define glProgramLocalParameterI4iNV get_gl_entries ()->ProgramLocalParameterI4iNV
#define glProgramLocalParameterI4ivNV get_gl_entries ()->ProgramLocalParameterI4ivNV
#define glProgramLocalParametersI4ivNV get_gl_entries ()->ProgramLocalParametersI4ivNV
#define glProgramLocalParameterI4uiNV get_gl_entries ()->ProgramLocalParameterI4uiNV
#define glProgramLocalParameterI4uivNV get_gl_entries ()->ProgramLocalParameterI4uivNV
#define glProgramLocalParametersI4uivNV get_gl_entries ()->ProgramLocalParametersI4uivNV
#define glProgramEnvParameterI4iNV get_gl_entries ()->ProgramEnvParameterI4iNV
#define glProgramEnvParameterI4ivNV get_gl_entries ()->ProgramEnvParameterI4ivNV
#define glProgramEnvParametersI4ivNV get_gl_entries ()->ProgramEnvParametersI4ivNV
#define glProgramEnvParameterI4uiNV get_gl_entries ()->ProgramEnvParameterI4uiNV
#define glProgramEnvParameterI4uivNV get_gl_entries ()->ProgramEnvParameterI4uivNV
#define glProgramEnvParametersI4uivNV get_gl_entries ()->ProgramEnvParametersI4uivNV
#define glGetProgramLocalParameterIivNV get_gl_entries ()->GetProgramLocalParameterIivNV
#define glGetProgramLocalParameterIuivNV get_gl_entries ()->GetProgramLocalParameterIuivNV
#define glGetProgramEnvParameterIivNV get_gl_entries ()->GetProgramEnvParameterIivNV
#define glGetProgramEnvParameterIuivNV get_gl_entries ()->GetProgramEnvParameterIuivNV
#define glProgramVertexLimitNV get_gl_entries ()->ProgramVertexLimitNV
#define glFramebufferTextureEXT get_gl_entries ()->FramebufferTextureEXT
#define glFramebufferTextureLayerEXT get_gl_entries ()->FramebufferTextureLayerEXT
#define glFramebufferTextureFaceEXT get_gl_entries ()->FramebufferTextureFaceEXT
#define glProgramParameteriEXT get_gl_entries ()->ProgramParameteriEXT
#define glVertexAttribI1iEXT get_gl_entries ()->VertexAttribI1iEXT
#define glVertexAttribI2iEXT get_gl_entries ()->VertexAttribI2iEXT
#define glVertexAttribI3iEXT get_gl_entries ()->VertexAttribI3iEXT
#define glVertexAttribI4iEXT get_gl_entries ()->VertexAttribI4iEXT
#define glVertexAttribI1uiEXT get_gl_entries ()->VertexAttribI1uiEXT
#define glVertexAttribI2uiEXT get_gl_entries ()->VertexAttribI2uiEXT
#define glVertexAttribI3uiEXT get_gl_entries ()->VertexAttribI3uiEXT
#define glVertexAttribI4uiEXT get_gl_entries ()->VertexAttribI4uiEXT
#define glVertexAttribI1ivEXT get_gl_entries ()->VertexAttribI1ivEXT
#define glVertexAttribI2ivEXT get_gl_entries ()->VertexAttribI2ivEXT
#define glVertexAttribI3ivEXT get_gl_entries ()->VertexAttribI3ivEXT
#define glVertexAttribI4ivEXT get_gl_entries ()->VertexAttribI4ivEXT
#define glVertexAttribI1uivEXT get_gl_entries ()->VertexAttribI1uivEXT
#define glVertexAttribI2uivEXT get_gl_entries ()->VertexAttribI2uivEXT
#define glVertexAttribI3uivEXT get_gl_entries ()->VertexAttribI3uivEXT
#define glVertexAttribI4uivEXT get_gl_entries ()->VertexAttribI4uivEXT
#define glVertexAttribI4bvEXT get_gl_entries ()->VertexAttribI4bvEXT
#define glVertexAttribI4svEXT get_gl_entries ()->VertexAttribI4svEXT
#define glVertexAttribI4ubvEXT get_gl_entries ()->VertexAttribI4ubvEXT
#define glVertexAttribI4usvEXT get_gl_entries ()->VertexAttribI4usvEXT
#define glVertexAttribIPointerEXT get_gl_entries ()->VertexAttribIPointerEXT
#define glGetVertexAttribIivEXT get_gl_entries ()->GetVertexAttribIivEXT
#define glGetVertexAttribIuivEXT get_gl_entries ()->GetVertexAttribIuivEXT
#define glGetUniformuivEXT get_gl_entries ()->GetUniformuivEXT
#define glBindFragDataLocationEXT get_gl_entries ()->BindFragDataLocationEXT
#define glGetFragDataLocationEXT get_gl_entries ()->GetFragDataLocationEXT
#define glUniform1uiEXT get_gl_entries ()->Uniform1uiEXT
#define glUniform2uiEXT get_gl_entries ()->Uniform2uiEXT
#define glUniform3uiEXT get_gl_entries ()->Uniform3uiEXT
#define glUniform4uiEXT get_gl_entries ()->Uniform4uiEXT
#define glUniform1uivEXT get_gl_entries ()->Uniform1uivEXT
#define glUniform2uivEXT get_gl_entries ()->Uniform2uivEXT
#define glUniform3uivEXT get_gl_entries ()->Uniform3uivEXT
#define glUniform4uivEXT get_gl_entries ()->Uniform4uivEXT
#define glDrawArraysInstancedEXT get_gl_entries ()->DrawArraysInstancedEXT
#define glDrawElementsInstancedEXT get_gl_entries ()->DrawElementsInstancedEXT
#define glTexBufferEXT get_gl_entries ()->TexBufferEXT
#define glDepthRangedNV get_gl_entries ()->DepthRangedNV
#define glClearDepthdNV get_gl_entries ()->ClearDepthdNV
#define glDepthBoundsdNV get_gl_entries ()->DepthBoundsdNV
#define glRenderbufferStorageMultisampleCoverageNV get_gl_entries ()->RenderbufferStorageMultisampleCoverageNV
#define glProgramBufferParametersfvNV get_gl_entries ()->ProgramBufferParametersfvNV
#define glProgramBufferParametersIivNV get_gl_entries ()->ProgramBufferParametersIivNV
#define glProgramBufferParametersIuivNV get_gl_entries ()->ProgramBufferParametersIuivNV
#define glColorMaskIndexedEXT get_gl_entries ()->ColorMaskIndexedEXT
#define glGetBooleanIndexedvEXT get_gl_entries ()->GetBooleanIndexedvEXT
#define glGetIntegerIndexedvEXT get_gl_entries ()->GetIntegerIndexedvEXT
#define glEnableIndexedEXT get_gl_entries ()->EnableIndexedEXT
#define glDisableIndexedEXT get_gl_entries ()->DisableIndexedEXT
#define glIsEnabledIndexedEXT get_gl_entries ()->IsEnabledIndexedEXT
#define glBeginTransformFeedbackNV get_gl_entries ()->BeginTransformFeedbackNV
#define glEndTransformFeedbackNV get_gl_entries ()->EndTransformFeedbackNV
#define glTransformFeedbackAttribsNV get_gl_entries ()->TransformFeedbackAttribsNV
#define glBindBufferRangeNV get_gl_entries ()->BindBufferRangeNV
#define glBindBufferOffsetNV get_gl_entries ()->BindBufferOffsetNV
#define glBindBufferBaseNV get_gl_entries ()->BindBufferBaseNV
#define glTransformFeedbackVaryingsNV get_gl_entries ()->TransformFeedbackVaryingsNV
#define glActiveVaryingNV get_gl_entries ()->ActiveVaryingNV
#define glGetVaryingLocationNV get_gl_entries ()->GetVaryingLocationNV
#define glGetActiveVaryingNV get_gl_entries ()->GetActiveVaryingNV
#define glGetTransformFeedbackVaryingNV get_gl_entries ()->GetTransformFeedbackVaryingNV
#define glUniformBufferEXT get_gl_entries ()->UniformBufferEXT
#define glGetUniformBufferSizeEXT get_gl_entries ()->GetUniformBufferSizeEXT
#define glGetUniformOffsetEXT get_gl_entries ()->GetUniformOffsetEXT
#define glTexParameterIivEXT get_gl_entries ()->TexParameterIivEXT
#define glTexParameterIuivEXT get_gl_entries ()->TexParameterIuivEXT
#define glGetTexParameterIivEXT get_gl_entries ()->GetTexParameterIivEXT
#define glGetTexParameterIuivEXT get_gl_entries ()->GetTexParameterIuivEXT
#define glClearColorIiEXT get_gl_entries ()->ClearColorIiEXT
#define glClearColorIuiEXT get_gl_entries ()->ClearColorIuiEXT
#define glFrameTerminatorGREMEDY get_gl_entries ()->FrameTerminatorGREMEDY

#ifdef APIENTRY_DEFINED
  #undef APIENTRY_DEFINED
  #undef APIENTRY
#endif

}

}

}

#endif
