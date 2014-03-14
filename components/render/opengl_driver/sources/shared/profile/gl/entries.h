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

#include <shared/profile/gl/gl_types.h>
#include <shared/profile/gl/glext.h>

namespace render
{

namespace low_level
{

namespace opengl
{

//forward declaration
class ILibrary;

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
  PFNGLTEXIMAGE3DPROC TexImage3D;
  PFNGLTEXSUBIMAGE3DPROC TexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC CopyTexSubImage3D;
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
  PFNGLACTIVETEXTUREPROC ActiveTexture;
  PFNGLSAMPLECOVERAGEPROC SampleCoverage;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC CompressedTexImage3D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC CompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC CompressedTexImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC CompressedTexSubImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC CompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC CompressedTexSubImage1D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC GetCompressedTexImage;
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
  PFNGLBLENDFUNCSEPARATEPROC BlendFuncSeparate;
  PFNGLMULTIDRAWARRAYSPROC MultiDrawArrays;
  PFNGLMULTIDRAWELEMENTSPROC MultiDrawElements;
  PFNGLPOINTPARAMETERFPROC PointParameterf;
  PFNGLPOINTPARAMETERFVPROC PointParameterfv;
  PFNGLPOINTPARAMETERIPROC PointParameteri;
  PFNGLPOINTPARAMETERIVPROC PointParameteriv;
  PFNGLFOGCOORDFPROC FogCoordf;
  PFNGLFOGCOORDFVPROC FogCoordfv;
  PFNGLFOGCOORDDPROC FogCoordd;
  PFNGLFOGCOORDDVPROC FogCoorddv;
  PFNGLFOGCOORDPOINTERPROC FogCoordPointer;
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
  PFNGLCOLORMASKIPROC ColorMaski;
  PFNGLGETBOOLEANI_VPROC GetBooleani_v;
  PFNGLGETINTEGERI_VPROC GetIntegeri_v;
  PFNGLENABLEIPROC Enablei;
  PFNGLDISABLEIPROC Disablei;
  PFNGLISENABLEDIPROC IsEnabledi;
  PFNGLBEGINTRANSFORMFEEDBACKPROC BeginTransformFeedback;
  PFNGLENDTRANSFORMFEEDBACKPROC EndTransformFeedback;
  PFNGLBINDBUFFERRANGEPROC BindBufferRange;
  PFNGLBINDBUFFERBASEPROC BindBufferBase;
  PFNGLTRANSFORMFEEDBACKVARYINGSPROC TransformFeedbackVaryings;
  PFNGLGETTRANSFORMFEEDBACKVARYINGPROC GetTransformFeedbackVarying;
  PFNGLCLAMPCOLORPROC ClampColor;
  PFNGLBEGINCONDITIONALRENDERPROC BeginConditionalRender;
  PFNGLENDCONDITIONALRENDERPROC EndConditionalRender;
  PFNGLVERTEXATTRIBIPOINTERPROC VertexAttribIPointer;
  PFNGLGETVERTEXATTRIBIIVPROC GetVertexAttribIiv;
  PFNGLGETVERTEXATTRIBIUIVPROC GetVertexAttribIuiv;
  PFNGLVERTEXATTRIBI1IPROC VertexAttribI1i;
  PFNGLVERTEXATTRIBI2IPROC VertexAttribI2i;
  PFNGLVERTEXATTRIBI3IPROC VertexAttribI3i;
  PFNGLVERTEXATTRIBI4IPROC VertexAttribI4i;
  PFNGLVERTEXATTRIBI1UIPROC VertexAttribI1ui;
  PFNGLVERTEXATTRIBI2UIPROC VertexAttribI2ui;
  PFNGLVERTEXATTRIBI3UIPROC VertexAttribI3ui;
  PFNGLVERTEXATTRIBI4UIPROC VertexAttribI4ui;
  PFNGLVERTEXATTRIBI1IVPROC VertexAttribI1iv;
  PFNGLVERTEXATTRIBI2IVPROC VertexAttribI2iv;
  PFNGLVERTEXATTRIBI3IVPROC VertexAttribI3iv;
  PFNGLVERTEXATTRIBI4IVPROC VertexAttribI4iv;
  PFNGLVERTEXATTRIBI1UIVPROC VertexAttribI1uiv;
  PFNGLVERTEXATTRIBI2UIVPROC VertexAttribI2uiv;
  PFNGLVERTEXATTRIBI3UIVPROC VertexAttribI3uiv;
  PFNGLVERTEXATTRIBI4UIVPROC VertexAttribI4uiv;
  PFNGLVERTEXATTRIBI4BVPROC VertexAttribI4bv;
  PFNGLVERTEXATTRIBI4SVPROC VertexAttribI4sv;
  PFNGLVERTEXATTRIBI4UBVPROC VertexAttribI4ubv;
  PFNGLVERTEXATTRIBI4USVPROC VertexAttribI4usv;
  PFNGLGETUNIFORMUIVPROC GetUniformuiv;
  PFNGLBINDFRAGDATALOCATIONPROC BindFragDataLocation;
  PFNGLGETFRAGDATALOCATIONPROC GetFragDataLocation;
  PFNGLUNIFORM1UIPROC Uniform1ui;
  PFNGLUNIFORM2UIPROC Uniform2ui;
  PFNGLUNIFORM3UIPROC Uniform3ui;
  PFNGLUNIFORM4UIPROC Uniform4ui;
  PFNGLUNIFORM1UIVPROC Uniform1uiv;
  PFNGLUNIFORM2UIVPROC Uniform2uiv;
  PFNGLUNIFORM3UIVPROC Uniform3uiv;
  PFNGLUNIFORM4UIVPROC Uniform4uiv;
  PFNGLTEXPARAMETERIIVPROC TexParameterIiv;
  PFNGLTEXPARAMETERIUIVPROC TexParameterIuiv;
  PFNGLGETTEXPARAMETERIIVPROC GetTexParameterIiv;
  PFNGLGETTEXPARAMETERIUIVPROC GetTexParameterIuiv;
  PFNGLCLEARBUFFERIVPROC ClearBufferiv;
  PFNGLCLEARBUFFERUIVPROC ClearBufferuiv;
  PFNGLCLEARBUFFERFVPROC ClearBufferfv;
  PFNGLCLEARBUFFERFIPROC ClearBufferfi;
  PFNGLGETSTRINGIPROC GetStringi;
  PFNGLDRAWARRAYSINSTANCEDPROC DrawArraysInstanced;
  PFNGLDRAWELEMENTSINSTANCEDPROC DrawElementsInstanced;
  PFNGLTEXBUFFERPROC TexBuffer;
  PFNGLPRIMITIVERESTARTINDEXPROC PrimitiveRestartIndex;
  PFNGLGETINTEGER64I_VPROC GetInteger64i_v;
  PFNGLGETBUFFERPARAMETERI64VPROC GetBufferParameteri64v;
  PFNGLFRAMEBUFFERTEXTUREPROC FramebufferTexture;
  PFNGLVERTEXATTRIBDIVISORPROC VertexAttribDivisor;
  PFNGLMINSAMPLESHADINGPROC MinSampleShading;
  PFNGLBLENDEQUATIONIPROC BlendEquationi;
  PFNGLBLENDEQUATIONSEPARATEIPROC BlendEquationSeparatei;
  PFNGLBLENDFUNCIPROC BlendFunci;
  PFNGLBLENDFUNCSEPARATEIPROC BlendFuncSeparatei;
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
  PFNGLDRAWARRAYSINSTANCEDARBPROC DrawArraysInstancedARB;
  PFNGLDRAWELEMENTSINSTANCEDARBPROC DrawElementsInstancedARB;
  PFNGLISRENDERBUFFERPROC IsRenderbuffer;
  PFNGLBINDRENDERBUFFERPROC BindRenderbuffer;
  PFNGLDELETERENDERBUFFERSPROC DeleteRenderbuffers;
  PFNGLGENRENDERBUFFERSPROC GenRenderbuffers;
  PFNGLRENDERBUFFERSTORAGEPROC RenderbufferStorage;
  PFNGLGETRENDERBUFFERPARAMETERIVPROC GetRenderbufferParameteriv;
  PFNGLISFRAMEBUFFERPROC IsFramebuffer;
  PFNGLBINDFRAMEBUFFERPROC BindFramebuffer;
  PFNGLDELETEFRAMEBUFFERSPROC DeleteFramebuffers;
  PFNGLGENFRAMEBUFFERSPROC GenFramebuffers;
  PFNGLCHECKFRAMEBUFFERSTATUSPROC CheckFramebufferStatus;
  PFNGLFRAMEBUFFERTEXTURE1DPROC FramebufferTexture1D;
  PFNGLFRAMEBUFFERTEXTURE2DPROC FramebufferTexture2D;
  PFNGLFRAMEBUFFERTEXTURE3DPROC FramebufferTexture3D;
  PFNGLFRAMEBUFFERRENDERBUFFERPROC FramebufferRenderbuffer;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC GetFramebufferAttachmentParameteriv;
  PFNGLGENERATEMIPMAPPROC GenerateMipmap;
  PFNGLBLITFRAMEBUFFERPROC BlitFramebuffer;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC RenderbufferStorageMultisample;
  PFNGLFRAMEBUFFERTEXTURELAYERPROC FramebufferTextureLayer;
  PFNGLPROGRAMPARAMETERIARBPROC ProgramParameteriARB;
  PFNGLFRAMEBUFFERTEXTUREARBPROC FramebufferTextureARB;
  PFNGLFRAMEBUFFERTEXTURELAYERARBPROC FramebufferTextureLayerARB;
  PFNGLFRAMEBUFFERTEXTUREFACEARBPROC FramebufferTextureFaceARB;
  PFNGLVERTEXATTRIBDIVISORARBPROC VertexAttribDivisorARB;
  PFNGLMAPBUFFERRANGEPROC MapBufferRange;
  PFNGLFLUSHMAPPEDBUFFERRANGEPROC FlushMappedBufferRange;
  PFNGLTEXBUFFERARBPROC TexBufferARB;
  PFNGLBINDVERTEXARRAYPROC BindVertexArray;
  PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;
  PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
  PFNGLISVERTEXARRAYPROC IsVertexArray;
  PFNGLGETUNIFORMINDICESPROC GetUniformIndices;
  PFNGLGETACTIVEUNIFORMSIVPROC GetActiveUniformsiv;
  PFNGLGETACTIVEUNIFORMNAMEPROC GetActiveUniformName;
  PFNGLGETUNIFORMBLOCKINDEXPROC GetUniformBlockIndex;
  PFNGLGETACTIVEUNIFORMBLOCKIVPROC GetActiveUniformBlockiv;
  PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC GetActiveUniformBlockName;
  PFNGLUNIFORMBLOCKBINDINGPROC UniformBlockBinding;
  PFNGLCOPYBUFFERSUBDATAPROC CopyBufferSubData;
  PFNGLDRAWELEMENTSBASEVERTEXPROC DrawElementsBaseVertex;
  PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC DrawRangeElementsBaseVertex;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC DrawElementsInstancedBaseVertex;
  PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC MultiDrawElementsBaseVertex;
  PFNGLPROVOKINGVERTEXPROC ProvokingVertex;
  PFNGLFENCESYNCPROC FenceSync;
  PFNGLISSYNCPROC IsSync;
  PFNGLDELETESYNCPROC DeleteSync;
  PFNGLCLIENTWAITSYNCPROC ClientWaitSync;
  PFNGLWAITSYNCPROC WaitSync;
  PFNGLGETINTEGER64VPROC GetInteger64v;
  PFNGLGETSYNCIVPROC GetSynciv;
  PFNGLTEXIMAGE2DMULTISAMPLEPROC TexImage2DMultisample;
  PFNGLTEXIMAGE3DMULTISAMPLEPROC TexImage3DMultisample;
  PFNGLGETMULTISAMPLEFVPROC GetMultisamplefv;
  PFNGLSAMPLEMASKIPROC SampleMaski;
  PFNGLBLENDEQUATIONIARBPROC BlendEquationiARB;
  PFNGLBLENDEQUATIONSEPARATEIARBPROC BlendEquationSeparateiARB;
  PFNGLBLENDFUNCIARBPROC BlendFunciARB;
  PFNGLBLENDFUNCSEPARATEIARBPROC BlendFuncSeparateiARB;
  PFNGLMINSAMPLESHADINGARBPROC MinSampleShadingARB;
  PFNGLNAMEDSTRINGARBPROC NamedStringARB;
  PFNGLDELETENAMEDSTRINGARBPROC DeleteNamedStringARB;
  PFNGLCOMPILESHADERINCLUDEARBPROC CompileShaderIncludeARB;
  PFNGLISNAMEDSTRINGARBPROC IsNamedStringARB;
  PFNGLGETNAMEDSTRINGARBPROC GetNamedStringARB;
  PFNGLGETNAMEDSTRINGIVARBPROC GetNamedStringivARB;
  PFNGLBINDFRAGDATALOCATIONINDEXEDPROC BindFragDataLocationIndexed;
  PFNGLGETFRAGDATAINDEXPROC GetFragDataIndex;
  PFNGLGENSAMPLERSPROC GenSamplers;
  PFNGLDELETESAMPLERSPROC DeleteSamplers;
  PFNGLISSAMPLERPROC IsSampler;
  PFNGLBINDSAMPLERPROC BindSampler;
  PFNGLSAMPLERPARAMETERIPROC SamplerParameteri;
  PFNGLSAMPLERPARAMETERIVPROC SamplerParameteriv;
  PFNGLSAMPLERPARAMETERFPROC SamplerParameterf;
  PFNGLSAMPLERPARAMETERFVPROC SamplerParameterfv;
  PFNGLSAMPLERPARAMETERIIVPROC SamplerParameterIiv;
  PFNGLSAMPLERPARAMETERIUIVPROC SamplerParameterIuiv;
  PFNGLGETSAMPLERPARAMETERIVPROC GetSamplerParameteriv;
  PFNGLGETSAMPLERPARAMETERIIVPROC GetSamplerParameterIiv;
  PFNGLGETSAMPLERPARAMETERFVPROC GetSamplerParameterfv;
  PFNGLGETSAMPLERPARAMETERIUIVPROC GetSamplerParameterIuiv;
  PFNGLQUERYCOUNTERPROC QueryCounter;
  PFNGLGETQUERYOBJECTI64VPROC GetQueryObjecti64v;
  PFNGLGETQUERYOBJECTUI64VPROC GetQueryObjectui64v;
  PFNGLVERTEXP2UIPROC VertexP2ui;
  PFNGLVERTEXP2UIVPROC VertexP2uiv;
  PFNGLVERTEXP3UIPROC VertexP3ui;
  PFNGLVERTEXP3UIVPROC VertexP3uiv;
  PFNGLVERTEXP4UIPROC VertexP4ui;
  PFNGLVERTEXP4UIVPROC VertexP4uiv;
  PFNGLTEXCOORDP1UIPROC TexCoordP1ui;
  PFNGLTEXCOORDP1UIVPROC TexCoordP1uiv;
  PFNGLTEXCOORDP2UIPROC TexCoordP2ui;
  PFNGLTEXCOORDP2UIVPROC TexCoordP2uiv;
  PFNGLTEXCOORDP3UIPROC TexCoordP3ui;
  PFNGLTEXCOORDP3UIVPROC TexCoordP3uiv;
  PFNGLTEXCOORDP4UIPROC TexCoordP4ui;
  PFNGLTEXCOORDP4UIVPROC TexCoordP4uiv;
  PFNGLMULTITEXCOORDP1UIPROC MultiTexCoordP1ui;
  PFNGLMULTITEXCOORDP1UIVPROC MultiTexCoordP1uiv;
  PFNGLMULTITEXCOORDP2UIPROC MultiTexCoordP2ui;
  PFNGLMULTITEXCOORDP2UIVPROC MultiTexCoordP2uiv;
  PFNGLMULTITEXCOORDP3UIPROC MultiTexCoordP3ui;
  PFNGLMULTITEXCOORDP3UIVPROC MultiTexCoordP3uiv;
  PFNGLMULTITEXCOORDP4UIPROC MultiTexCoordP4ui;
  PFNGLMULTITEXCOORDP4UIVPROC MultiTexCoordP4uiv;
  PFNGLNORMALP3UIPROC NormalP3ui;
  PFNGLNORMALP3UIVPROC NormalP3uiv;
  PFNGLCOLORP3UIPROC ColorP3ui;
  PFNGLCOLORP3UIVPROC ColorP3uiv;
  PFNGLCOLORP4UIPROC ColorP4ui;
  PFNGLCOLORP4UIVPROC ColorP4uiv;
  PFNGLSECONDARYCOLORP3UIPROC SecondaryColorP3ui;
  PFNGLSECONDARYCOLORP3UIVPROC SecondaryColorP3uiv;
  PFNGLVERTEXATTRIBP1UIPROC VertexAttribP1ui;
  PFNGLVERTEXATTRIBP1UIVPROC VertexAttribP1uiv;
  PFNGLVERTEXATTRIBP2UIPROC VertexAttribP2ui;
  PFNGLVERTEXATTRIBP2UIVPROC VertexAttribP2uiv;
  PFNGLVERTEXATTRIBP3UIPROC VertexAttribP3ui;
  PFNGLVERTEXATTRIBP3UIVPROC VertexAttribP3uiv;
  PFNGLVERTEXATTRIBP4UIPROC VertexAttribP4ui;
  PFNGLVERTEXATTRIBP4UIVPROC VertexAttribP4uiv;
  PFNGLDRAWARRAYSINDIRECTPROC DrawArraysIndirect;
  PFNGLDRAWELEMENTSINDIRECTPROC DrawElementsIndirect;
  PFNGLUNIFORM1DPROC Uniform1d;
  PFNGLUNIFORM2DPROC Uniform2d;
  PFNGLUNIFORM3DPROC Uniform3d;
  PFNGLUNIFORM4DPROC Uniform4d;
  PFNGLUNIFORM1DVPROC Uniform1dv;
  PFNGLUNIFORM2DVPROC Uniform2dv;
  PFNGLUNIFORM3DVPROC Uniform3dv;
  PFNGLUNIFORM4DVPROC Uniform4dv;
  PFNGLUNIFORMMATRIX2DVPROC UniformMatrix2dv;
  PFNGLUNIFORMMATRIX3DVPROC UniformMatrix3dv;
  PFNGLUNIFORMMATRIX4DVPROC UniformMatrix4dv;
  PFNGLUNIFORMMATRIX2X3DVPROC UniformMatrix2x3dv;
  PFNGLUNIFORMMATRIX2X4DVPROC UniformMatrix2x4dv;
  PFNGLUNIFORMMATRIX3X2DVPROC UniformMatrix3x2dv;
  PFNGLUNIFORMMATRIX3X4DVPROC UniformMatrix3x4dv;
  PFNGLUNIFORMMATRIX4X2DVPROC UniformMatrix4x2dv;
  PFNGLUNIFORMMATRIX4X3DVPROC UniformMatrix4x3dv;
  PFNGLGETUNIFORMDVPROC GetUniformdv;
  PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC GetSubroutineUniformLocation;
  PFNGLGETSUBROUTINEINDEXPROC GetSubroutineIndex;
  PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC GetActiveSubroutineUniformiv;
  PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC GetActiveSubroutineUniformName;
  PFNGLGETACTIVESUBROUTINENAMEPROC GetActiveSubroutineName;
  PFNGLUNIFORMSUBROUTINESUIVPROC UniformSubroutinesuiv;
  PFNGLGETUNIFORMSUBROUTINEUIVPROC GetUniformSubroutineuiv;
  PFNGLGETPROGRAMSTAGEIVPROC GetProgramStageiv;
  PFNGLPATCHPARAMETERIPROC PatchParameteri;
  PFNGLPATCHPARAMETERFVPROC PatchParameterfv;
  PFNGLBINDTRANSFORMFEEDBACKPROC BindTransformFeedback;
  PFNGLDELETETRANSFORMFEEDBACKSPROC DeleteTransformFeedbacks;
  PFNGLGENTRANSFORMFEEDBACKSPROC GenTransformFeedbacks;
  PFNGLISTRANSFORMFEEDBACKPROC IsTransformFeedback;
  PFNGLPAUSETRANSFORMFEEDBACKPROC PauseTransformFeedback;
  PFNGLRESUMETRANSFORMFEEDBACKPROC ResumeTransformFeedback;
  PFNGLDRAWTRANSFORMFEEDBACKPROC DrawTransformFeedback;
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC DrawTransformFeedbackStream;
  PFNGLBEGINQUERYINDEXEDPROC BeginQueryIndexed;
  PFNGLENDQUERYINDEXEDPROC EndQueryIndexed;
  PFNGLGETQUERYINDEXEDIVPROC GetQueryIndexediv;
  PFNGLRELEASESHADERCOMPILERPROC ReleaseShaderCompiler;
  PFNGLSHADERBINARYPROC ShaderBinary;
  PFNGLGETSHADERPRECISIONFORMATPROC GetShaderPrecisionFormat;
  PFNGLDEPTHRANGEFPROC DepthRangef;
  PFNGLCLEARDEPTHFPROC ClearDepthf;
  PFNGLGETPROGRAMBINARYPROC GetProgramBinary;
  PFNGLPROGRAMBINARYPROC ProgramBinary;
  PFNGLPROGRAMPARAMETERIPROC ProgramParameteri;
  PFNGLUSEPROGRAMSTAGESPROC UseProgramStages;
  PFNGLACTIVESHADERPROGRAMPROC ActiveShaderProgram;
  PFNGLCREATESHADERPROGRAMVPROC CreateShaderProgramv;
  PFNGLBINDPROGRAMPIPELINEPROC BindProgramPipeline;
  PFNGLDELETEPROGRAMPIPELINESPROC DeleteProgramPipelines;
  PFNGLGENPROGRAMPIPELINESPROC GenProgramPipelines;
  PFNGLISPROGRAMPIPELINEPROC IsProgramPipeline;
  PFNGLGETPROGRAMPIPELINEIVPROC GetProgramPipelineiv;
  PFNGLPROGRAMUNIFORM1IPROC ProgramUniform1i;
  PFNGLPROGRAMUNIFORM1IVPROC ProgramUniform1iv;
  PFNGLPROGRAMUNIFORM1FPROC ProgramUniform1f;
  PFNGLPROGRAMUNIFORM1FVPROC ProgramUniform1fv;
  PFNGLPROGRAMUNIFORM1DPROC ProgramUniform1d;
  PFNGLPROGRAMUNIFORM1DVPROC ProgramUniform1dv;
  PFNGLPROGRAMUNIFORM1UIPROC ProgramUniform1ui;
  PFNGLPROGRAMUNIFORM1UIVPROC ProgramUniform1uiv;
  PFNGLPROGRAMUNIFORM2IPROC ProgramUniform2i;
  PFNGLPROGRAMUNIFORM2IVPROC ProgramUniform2iv;
  PFNGLPROGRAMUNIFORM2FPROC ProgramUniform2f;
  PFNGLPROGRAMUNIFORM2FVPROC ProgramUniform2fv;
  PFNGLPROGRAMUNIFORM2DPROC ProgramUniform2d;
  PFNGLPROGRAMUNIFORM2DVPROC ProgramUniform2dv;
  PFNGLPROGRAMUNIFORM2UIPROC ProgramUniform2ui;
  PFNGLPROGRAMUNIFORM2UIVPROC ProgramUniform2uiv;
  PFNGLPROGRAMUNIFORM3IPROC ProgramUniform3i;
  PFNGLPROGRAMUNIFORM3IVPROC ProgramUniform3iv;
  PFNGLPROGRAMUNIFORM3FPROC ProgramUniform3f;
  PFNGLPROGRAMUNIFORM3FVPROC ProgramUniform3fv;
  PFNGLPROGRAMUNIFORM3DPROC ProgramUniform3d;
  PFNGLPROGRAMUNIFORM3DVPROC ProgramUniform3dv;
  PFNGLPROGRAMUNIFORM3UIPROC ProgramUniform3ui;
  PFNGLPROGRAMUNIFORM3UIVPROC ProgramUniform3uiv;
  PFNGLPROGRAMUNIFORM4IPROC ProgramUniform4i;
  PFNGLPROGRAMUNIFORM4IVPROC ProgramUniform4iv;
  PFNGLPROGRAMUNIFORM4FPROC ProgramUniform4f;
  PFNGLPROGRAMUNIFORM4FVPROC ProgramUniform4fv;
  PFNGLPROGRAMUNIFORM4DPROC ProgramUniform4d;
  PFNGLPROGRAMUNIFORM4DVPROC ProgramUniform4dv;
  PFNGLPROGRAMUNIFORM4UIPROC ProgramUniform4ui;
  PFNGLPROGRAMUNIFORM4UIVPROC ProgramUniform4uiv;
  PFNGLPROGRAMUNIFORMMATRIX2FVPROC ProgramUniformMatrix2fv;
  PFNGLPROGRAMUNIFORMMATRIX3FVPROC ProgramUniformMatrix3fv;
  PFNGLPROGRAMUNIFORMMATRIX4FVPROC ProgramUniformMatrix4fv;
  PFNGLPROGRAMUNIFORMMATRIX2DVPROC ProgramUniformMatrix2dv;
  PFNGLPROGRAMUNIFORMMATRIX3DVPROC ProgramUniformMatrix3dv;
  PFNGLPROGRAMUNIFORMMATRIX4DVPROC ProgramUniformMatrix4dv;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC ProgramUniformMatrix2x3fv;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC ProgramUniformMatrix3x2fv;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC ProgramUniformMatrix2x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC ProgramUniformMatrix4x2fv;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC ProgramUniformMatrix3x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC ProgramUniformMatrix4x3fv;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC ProgramUniformMatrix2x3dv;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC ProgramUniformMatrix3x2dv;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC ProgramUniformMatrix2x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC ProgramUniformMatrix4x2dv;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC ProgramUniformMatrix3x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC ProgramUniformMatrix4x3dv;
  PFNGLVALIDATEPROGRAMPIPELINEPROC ValidateProgramPipeline;
  PFNGLGETPROGRAMPIPELINEINFOLOGPROC GetProgramPipelineInfoLog;
  PFNGLVERTEXATTRIBL1DPROC VertexAttribL1d;
  PFNGLVERTEXATTRIBL2DPROC VertexAttribL2d;
  PFNGLVERTEXATTRIBL3DPROC VertexAttribL3d;
  PFNGLVERTEXATTRIBL4DPROC VertexAttribL4d;
  PFNGLVERTEXATTRIBL1DVPROC VertexAttribL1dv;
  PFNGLVERTEXATTRIBL2DVPROC VertexAttribL2dv;
  PFNGLVERTEXATTRIBL3DVPROC VertexAttribL3dv;
  PFNGLVERTEXATTRIBL4DVPROC VertexAttribL4dv;
  PFNGLVERTEXATTRIBLPOINTERPROC VertexAttribLPointer;
  PFNGLGETVERTEXATTRIBLDVPROC GetVertexAttribLdv;
  PFNGLVIEWPORTARRAYVPROC ViewportArrayv;
  PFNGLVIEWPORTINDEXEDFPROC ViewportIndexedf;
  PFNGLVIEWPORTINDEXEDFVPROC ViewportIndexedfv;
  PFNGLSCISSORARRAYVPROC ScissorArrayv;
  PFNGLSCISSORINDEXEDPROC ScissorIndexed;
  PFNGLSCISSORINDEXEDVPROC ScissorIndexedv;
  PFNGLDEPTHRANGEARRAYVPROC DepthRangeArrayv;
  PFNGLDEPTHRANGEINDEXEDPROC DepthRangeIndexed;
  PFNGLGETFLOATI_VPROC GetFloati_v;
  PFNGLGETDOUBLEI_VPROC GetDoublei_v;
  PFNGLCREATESYNCFROMCLEVENTARBPROC CreateSyncFromCLeventARB;
  PFNGLDEBUGMESSAGECONTROLARBPROC DebugMessageControlARB;
  PFNGLDEBUGMESSAGEINSERTARBPROC DebugMessageInsertARB;
  PFNGLDEBUGMESSAGECALLBACKARBPROC DebugMessageCallbackARB;
  PFNGLGETDEBUGMESSAGELOGARBPROC GetDebugMessageLogARB;
  PFNGLGETGRAPHICSRESETSTATUSARBPROC GetGraphicsResetStatusARB;
  PFNGLGETNMAPDVARBPROC GetnMapdvARB;
  PFNGLGETNMAPFVARBPROC GetnMapfvARB;
  PFNGLGETNMAPIVARBPROC GetnMapivARB;
  PFNGLGETNPIXELMAPFVARBPROC GetnPixelMapfvARB;
  PFNGLGETNPIXELMAPUIVARBPROC GetnPixelMapuivARB;
  PFNGLGETNPIXELMAPUSVARBPROC GetnPixelMapusvARB;
  PFNGLGETNPOLYGONSTIPPLEARBPROC GetnPolygonStippleARB;
  PFNGLGETNCOLORTABLEARBPROC GetnColorTableARB;
  PFNGLGETNCONVOLUTIONFILTERARBPROC GetnConvolutionFilterARB;
  PFNGLGETNSEPARABLEFILTERARBPROC GetnSeparableFilterARB;
  PFNGLGETNHISTOGRAMARBPROC GetnHistogramARB;
  PFNGLGETNMINMAXARBPROC GetnMinmaxARB;
  PFNGLGETNTEXIMAGEARBPROC GetnTexImageARB;
  PFNGLREADNPIXELSARBPROC ReadnPixelsARB;
  PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC GetnCompressedTexImageARB;
  PFNGLGETNUNIFORMFVARBPROC GetnUniformfvARB;
  PFNGLGETNUNIFORMIVARBPROC GetnUniformivARB;
  PFNGLGETNUNIFORMUIVARBPROC GetnUniformuivARB;
  PFNGLGETNUNIFORMDVARBPROC GetnUniformdvARB;
  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC DrawArraysInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC DrawElementsInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC DrawElementsInstancedBaseVertexBaseInstance;
  PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC DrawTransformFeedbackInstanced;
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC DrawTransformFeedbackStreamInstanced;
  PFNGLGETINTERNALFORMATIVPROC GetInternalformativ;
  PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC GetActiveAtomicCounterBufferiv;
  PFNGLBINDIMAGETEXTUREPROC BindImageTexture;
  PFNGLMEMORYBARRIERPROC MemoryBarrier;
  PFNGLTEXSTORAGE1DPROC TexStorage1D;
  PFNGLTEXSTORAGE2DPROC TexStorage2D;
  PFNGLTEXSTORAGE3DPROC TexStorage3D;
  PFNGLTEXTURESTORAGE1DEXTPROC TextureStorage1DEXT;
  PFNGLTEXTURESTORAGE2DEXTPROC TextureStorage2DEXT;
  PFNGLTEXTURESTORAGE3DEXTPROC TextureStorage3DEXT;
  PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl;
  PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert;
  PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback;
  PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog;
  PFNGLPUSHDEBUGGROUPPROC PushDebugGroup;
  PFNGLPOPDEBUGGROUPPROC PopDebugGroup;
  PFNGLOBJECTLABELPROC ObjectLabel;
  PFNGLGETOBJECTLABELPROC GetObjectLabel;
  PFNGLOBJECTPTRLABELPROC ObjectPtrLabel;
  PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel;
  PFNGLCLEARBUFFERDATAPROC ClearBufferData;
  PFNGLCLEARBUFFERSUBDATAPROC ClearBufferSubData;
  PFNGLCLEARNAMEDBUFFERDATAEXTPROC ClearNamedBufferDataEXT;
  PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC ClearNamedBufferSubDataEXT;
  PFNGLDISPATCHCOMPUTEPROC DispatchCompute;
  PFNGLDISPATCHCOMPUTEINDIRECTPROC DispatchComputeIndirect;
  PFNGLCOPYIMAGESUBDATAPROC CopyImageSubData;
  PFNGLTEXTUREVIEWPROC TextureView;
  PFNGLBINDVERTEXBUFFERPROC BindVertexBuffer;
  PFNGLVERTEXATTRIBFORMATPROC VertexAttribFormat;
  PFNGLVERTEXATTRIBIFORMATPROC VertexAttribIFormat;
  PFNGLVERTEXATTRIBLFORMATPROC VertexAttribLFormat;
  PFNGLVERTEXATTRIBBINDINGPROC VertexAttribBinding;
  PFNGLVERTEXBINDINGDIVISORPROC VertexBindingDivisor;
  PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC VertexArrayBindVertexBufferEXT;
  PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC VertexArrayVertexAttribFormatEXT;
  PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC VertexArrayVertexAttribIFormatEXT;
  PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC VertexArrayVertexAttribLFormatEXT;
  PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC VertexArrayVertexAttribBindingEXT;
  PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC VertexArrayVertexBindingDivisorEXT;
  PFNGLFRAMEBUFFERPARAMETERIPROC FramebufferParameteri;
  PFNGLGETFRAMEBUFFERPARAMETERIVPROC GetFramebufferParameteriv;
  PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC NamedFramebufferParameteriEXT;
  PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC GetNamedFramebufferParameterivEXT;
  PFNGLGETINTERNALFORMATI64VPROC GetInternalformati64v;
  PFNGLINVALIDATETEXSUBIMAGEPROC InvalidateTexSubImage;
  PFNGLINVALIDATETEXIMAGEPROC InvalidateTexImage;
  PFNGLINVALIDATEBUFFERSUBDATAPROC InvalidateBufferSubData;
  PFNGLINVALIDATEBUFFERDATAPROC InvalidateBufferData;
  PFNGLINVALIDATEFRAMEBUFFERPROC InvalidateFramebuffer;
  PFNGLINVALIDATESUBFRAMEBUFFERPROC InvalidateSubFramebuffer;
  PFNGLMULTIDRAWARRAYSINDIRECTPROC MultiDrawArraysIndirect;
  PFNGLMULTIDRAWELEMENTSINDIRECTPROC MultiDrawElementsIndirect;
  PFNGLGETPROGRAMINTERFACEIVPROC GetProgramInterfaceiv;
  PFNGLGETPROGRAMRESOURCEINDEXPROC GetProgramResourceIndex;
  PFNGLGETPROGRAMRESOURCENAMEPROC GetProgramResourceName;
  PFNGLGETPROGRAMRESOURCEIVPROC GetProgramResourceiv;
  PFNGLGETPROGRAMRESOURCELOCATIONPROC GetProgramResourceLocation;
  PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC GetProgramResourceLocationIndex;
  PFNGLSHADERSTORAGEBLOCKBINDINGPROC ShaderStorageBlockBinding;
  PFNGLTEXBUFFERRANGEPROC TexBufferRange;
  PFNGLTEXTUREBUFFERRANGEEXTPROC TextureBufferRangeEXT;
  PFNGLTEXSTORAGE2DMULTISAMPLEPROC TexStorage2DMultisample;
  PFNGLTEXSTORAGE3DMULTISAMPLEPROC TexStorage3DMultisample;
  PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC TextureStorage2DMultisampleEXT;
  PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC TextureStorage3DMultisampleEXT;
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
  PFNGLGETPIXELTRANSFORMPARAMETERIVEXTPROC GetPixelTransformParameterivEXT;
  PFNGLGETPIXELTRANSFORMPARAMETERFVEXTPROC GetPixelTransformParameterfvEXT;
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
  PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC TransformFeedbackStreamAttribsNV;
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
  PFNGLBEGINCONDITIONALRENDERNVPROC BeginConditionalRenderNV;
  PFNGLENDCONDITIONALRENDERNVPROC EndConditionalRenderNV;
  PFNGLPRESENTFRAMEKEYEDNVPROC PresentFrameKeyedNV;
  PFNGLPRESENTFRAMEDUALFILLNVPROC PresentFrameDualFillNV;
  PFNGLGETVIDEOIVNVPROC GetVideoivNV;
  PFNGLGETVIDEOUIVNVPROC GetVideouivNV;
  PFNGLGETVIDEOI64VNVPROC GetVideoi64vNV;
  PFNGLGETVIDEOUI64VNVPROC GetVideoui64vNV;
  PFNGLBEGINTRANSFORMFEEDBACKEXTPROC BeginTransformFeedbackEXT;
  PFNGLENDTRANSFORMFEEDBACKEXTPROC EndTransformFeedbackEXT;
  PFNGLBINDBUFFERRANGEEXTPROC BindBufferRangeEXT;
  PFNGLBINDBUFFEROFFSETEXTPROC BindBufferOffsetEXT;
  PFNGLBINDBUFFERBASEEXTPROC BindBufferBaseEXT;
  PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC TransformFeedbackVaryingsEXT;
  PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC GetTransformFeedbackVaryingEXT;
  PFNGLCLIENTATTRIBDEFAULTEXTPROC ClientAttribDefaultEXT;
  PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC PushClientAttribDefaultEXT;
  PFNGLMATRIXLOADFEXTPROC MatrixLoadfEXT;
  PFNGLMATRIXLOADDEXTPROC MatrixLoaddEXT;
  PFNGLMATRIXMULTFEXTPROC MatrixMultfEXT;
  PFNGLMATRIXMULTDEXTPROC MatrixMultdEXT;
  PFNGLMATRIXLOADIDENTITYEXTPROC MatrixLoadIdentityEXT;
  PFNGLMATRIXROTATEFEXTPROC MatrixRotatefEXT;
  PFNGLMATRIXROTATEDEXTPROC MatrixRotatedEXT;
  PFNGLMATRIXSCALEFEXTPROC MatrixScalefEXT;
  PFNGLMATRIXSCALEDEXTPROC MatrixScaledEXT;
  PFNGLMATRIXTRANSLATEFEXTPROC MatrixTranslatefEXT;
  PFNGLMATRIXTRANSLATEDEXTPROC MatrixTranslatedEXT;
  PFNGLMATRIXFRUSTUMEXTPROC MatrixFrustumEXT;
  PFNGLMATRIXORTHOEXTPROC MatrixOrthoEXT;
  PFNGLMATRIXPOPEXTPROC MatrixPopEXT;
  PFNGLMATRIXPUSHEXTPROC MatrixPushEXT;
  PFNGLMATRIXLOADTRANSPOSEFEXTPROC MatrixLoadTransposefEXT;
  PFNGLMATRIXLOADTRANSPOSEDEXTPROC MatrixLoadTransposedEXT;
  PFNGLMATRIXMULTTRANSPOSEFEXTPROC MatrixMultTransposefEXT;
  PFNGLMATRIXMULTTRANSPOSEDEXTPROC MatrixMultTransposedEXT;
  PFNGLTEXTUREPARAMETERFEXTPROC TextureParameterfEXT;
  PFNGLTEXTUREPARAMETERFVEXTPROC TextureParameterfvEXT;
  PFNGLTEXTUREPARAMETERIEXTPROC TextureParameteriEXT;
  PFNGLTEXTUREPARAMETERIVEXTPROC TextureParameterivEXT;
  PFNGLTEXTUREIMAGE1DEXTPROC TextureImage1DEXT;
  PFNGLTEXTUREIMAGE2DEXTPROC TextureImage2DEXT;
  PFNGLTEXTURESUBIMAGE1DEXTPROC TextureSubImage1DEXT;
  PFNGLTEXTURESUBIMAGE2DEXTPROC TextureSubImage2DEXT;
  PFNGLCOPYTEXTUREIMAGE1DEXTPROC CopyTextureImage1DEXT;
  PFNGLCOPYTEXTUREIMAGE2DEXTPROC CopyTextureImage2DEXT;
  PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC CopyTextureSubImage1DEXT;
  PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC CopyTextureSubImage2DEXT;
  PFNGLGETTEXTUREIMAGEEXTPROC GetTextureImageEXT;
  PFNGLGETTEXTUREPARAMETERFVEXTPROC GetTextureParameterfvEXT;
  PFNGLGETTEXTUREPARAMETERIVEXTPROC GetTextureParameterivEXT;
  PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC GetTextureLevelParameterfvEXT;
  PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC GetTextureLevelParameterivEXT;
  PFNGLTEXTUREIMAGE3DEXTPROC TextureImage3DEXT;
  PFNGLTEXTURESUBIMAGE3DEXTPROC TextureSubImage3DEXT;
  PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC CopyTextureSubImage3DEXT;
  PFNGLMULTITEXPARAMETERFEXTPROC MultiTexParameterfEXT;
  PFNGLMULTITEXPARAMETERFVEXTPROC MultiTexParameterfvEXT;
  PFNGLMULTITEXPARAMETERIEXTPROC MultiTexParameteriEXT;
  PFNGLMULTITEXPARAMETERIVEXTPROC MultiTexParameterivEXT;
  PFNGLMULTITEXIMAGE1DEXTPROC MultiTexImage1DEXT;
  PFNGLMULTITEXIMAGE2DEXTPROC MultiTexImage2DEXT;
  PFNGLMULTITEXSUBIMAGE1DEXTPROC MultiTexSubImage1DEXT;
  PFNGLMULTITEXSUBIMAGE2DEXTPROC MultiTexSubImage2DEXT;
  PFNGLCOPYMULTITEXIMAGE1DEXTPROC CopyMultiTexImage1DEXT;
  PFNGLCOPYMULTITEXIMAGE2DEXTPROC CopyMultiTexImage2DEXT;
  PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC CopyMultiTexSubImage1DEXT;
  PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC CopyMultiTexSubImage2DEXT;
  PFNGLGETMULTITEXIMAGEEXTPROC GetMultiTexImageEXT;
  PFNGLGETMULTITEXPARAMETERFVEXTPROC GetMultiTexParameterfvEXT;
  PFNGLGETMULTITEXPARAMETERIVEXTPROC GetMultiTexParameterivEXT;
  PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC GetMultiTexLevelParameterfvEXT;
  PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC GetMultiTexLevelParameterivEXT;
  PFNGLMULTITEXIMAGE3DEXTPROC MultiTexImage3DEXT;
  PFNGLMULTITEXSUBIMAGE3DEXTPROC MultiTexSubImage3DEXT;
  PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC CopyMultiTexSubImage3DEXT;
  PFNGLBINDMULTITEXTUREEXTPROC BindMultiTextureEXT;
  PFNGLENABLECLIENTSTATEINDEXEDEXTPROC EnableClientStateIndexedEXT;
  PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC DisableClientStateIndexedEXT;
  PFNGLMULTITEXCOORDPOINTEREXTPROC MultiTexCoordPointerEXT;
  PFNGLMULTITEXENVFEXTPROC MultiTexEnvfEXT;
  PFNGLMULTITEXENVFVEXTPROC MultiTexEnvfvEXT;
  PFNGLMULTITEXENVIEXTPROC MultiTexEnviEXT;
  PFNGLMULTITEXENVIVEXTPROC MultiTexEnvivEXT;
  PFNGLMULTITEXGENDEXTPROC MultiTexGendEXT;
  PFNGLMULTITEXGENDVEXTPROC MultiTexGendvEXT;
  PFNGLMULTITEXGENFEXTPROC MultiTexGenfEXT;
  PFNGLMULTITEXGENFVEXTPROC MultiTexGenfvEXT;
  PFNGLMULTITEXGENIEXTPROC MultiTexGeniEXT;
  PFNGLMULTITEXGENIVEXTPROC MultiTexGenivEXT;
  PFNGLGETMULTITEXENVFVEXTPROC GetMultiTexEnvfvEXT;
  PFNGLGETMULTITEXENVIVEXTPROC GetMultiTexEnvivEXT;
  PFNGLGETMULTITEXGENDVEXTPROC GetMultiTexGendvEXT;
  PFNGLGETMULTITEXGENFVEXTPROC GetMultiTexGenfvEXT;
  PFNGLGETMULTITEXGENIVEXTPROC GetMultiTexGenivEXT;
  PFNGLGETFLOATINDEXEDVEXTPROC GetFloatIndexedvEXT;
  PFNGLGETDOUBLEINDEXEDVEXTPROC GetDoubleIndexedvEXT;
  PFNGLGETPOINTERINDEXEDVEXTPROC GetPointerIndexedvEXT;
  PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC CompressedTextureImage3DEXT;
  PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC CompressedTextureImage2DEXT;
  PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC CompressedTextureImage1DEXT;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC CompressedTextureSubImage3DEXT;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC CompressedTextureSubImage2DEXT;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC CompressedTextureSubImage1DEXT;
  PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC GetCompressedTextureImageEXT;
  PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC CompressedMultiTexImage3DEXT;
  PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC CompressedMultiTexImage2DEXT;
  PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC CompressedMultiTexImage1DEXT;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC CompressedMultiTexSubImage3DEXT;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC CompressedMultiTexSubImage2DEXT;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC CompressedMultiTexSubImage1DEXT;
  PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC GetCompressedMultiTexImageEXT;
  PFNGLNAMEDPROGRAMSTRINGEXTPROC NamedProgramStringEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC NamedProgramLocalParameter4dEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC NamedProgramLocalParameter4dvEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC NamedProgramLocalParameter4fEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC NamedProgramLocalParameter4fvEXT;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC GetNamedProgramLocalParameterdvEXT;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC GetNamedProgramLocalParameterfvEXT;
  PFNGLGETNAMEDPROGRAMIVEXTPROC GetNamedProgramivEXT;
  PFNGLGETNAMEDPROGRAMSTRINGEXTPROC GetNamedProgramStringEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC NamedProgramLocalParameters4fvEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC NamedProgramLocalParameterI4iEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC NamedProgramLocalParameterI4ivEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC NamedProgramLocalParametersI4ivEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC NamedProgramLocalParameterI4uiEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC NamedProgramLocalParameterI4uivEXT;
  PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC NamedProgramLocalParametersI4uivEXT;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC GetNamedProgramLocalParameterIivEXT;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC GetNamedProgramLocalParameterIuivEXT;
  PFNGLTEXTUREPARAMETERIIVEXTPROC TextureParameterIivEXT;
  PFNGLTEXTUREPARAMETERIUIVEXTPROC TextureParameterIuivEXT;
  PFNGLGETTEXTUREPARAMETERIIVEXTPROC GetTextureParameterIivEXT;
  PFNGLGETTEXTUREPARAMETERIUIVEXTPROC GetTextureParameterIuivEXT;
  PFNGLMULTITEXPARAMETERIIVEXTPROC MultiTexParameterIivEXT;
  PFNGLMULTITEXPARAMETERIUIVEXTPROC MultiTexParameterIuivEXT;
  PFNGLGETMULTITEXPARAMETERIIVEXTPROC GetMultiTexParameterIivEXT;
  PFNGLGETMULTITEXPARAMETERIUIVEXTPROC GetMultiTexParameterIuivEXT;
  PFNGLPROGRAMUNIFORM1FEXTPROC ProgramUniform1fEXT;
  PFNGLPROGRAMUNIFORM2FEXTPROC ProgramUniform2fEXT;
  PFNGLPROGRAMUNIFORM3FEXTPROC ProgramUniform3fEXT;
  PFNGLPROGRAMUNIFORM4FEXTPROC ProgramUniform4fEXT;
  PFNGLPROGRAMUNIFORM1IEXTPROC ProgramUniform1iEXT;
  PFNGLPROGRAMUNIFORM2IEXTPROC ProgramUniform2iEXT;
  PFNGLPROGRAMUNIFORM3IEXTPROC ProgramUniform3iEXT;
  PFNGLPROGRAMUNIFORM4IEXTPROC ProgramUniform4iEXT;
  PFNGLPROGRAMUNIFORM1FVEXTPROC ProgramUniform1fvEXT;
  PFNGLPROGRAMUNIFORM2FVEXTPROC ProgramUniform2fvEXT;
  PFNGLPROGRAMUNIFORM3FVEXTPROC ProgramUniform3fvEXT;
  PFNGLPROGRAMUNIFORM4FVEXTPROC ProgramUniform4fvEXT;
  PFNGLPROGRAMUNIFORM1IVEXTPROC ProgramUniform1ivEXT;
  PFNGLPROGRAMUNIFORM2IVEXTPROC ProgramUniform2ivEXT;
  PFNGLPROGRAMUNIFORM3IVEXTPROC ProgramUniform3ivEXT;
  PFNGLPROGRAMUNIFORM4IVEXTPROC ProgramUniform4ivEXT;
  PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC ProgramUniformMatrix2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC ProgramUniformMatrix3fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC ProgramUniformMatrix4fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC ProgramUniformMatrix2x3fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC ProgramUniformMatrix3x2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC ProgramUniformMatrix2x4fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC ProgramUniformMatrix4x2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC ProgramUniformMatrix3x4fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC ProgramUniformMatrix4x3fvEXT;
  PFNGLPROGRAMUNIFORM1UIEXTPROC ProgramUniform1uiEXT;
  PFNGLPROGRAMUNIFORM2UIEXTPROC ProgramUniform2uiEXT;
  PFNGLPROGRAMUNIFORM3UIEXTPROC ProgramUniform3uiEXT;
  PFNGLPROGRAMUNIFORM4UIEXTPROC ProgramUniform4uiEXT;
  PFNGLPROGRAMUNIFORM1UIVEXTPROC ProgramUniform1uivEXT;
  PFNGLPROGRAMUNIFORM2UIVEXTPROC ProgramUniform2uivEXT;
  PFNGLPROGRAMUNIFORM3UIVEXTPROC ProgramUniform3uivEXT;
  PFNGLPROGRAMUNIFORM4UIVEXTPROC ProgramUniform4uivEXT;
  PFNGLNAMEDBUFFERDATAEXTPROC NamedBufferDataEXT;
  PFNGLNAMEDBUFFERSUBDATAEXTPROC NamedBufferSubDataEXT;
  PFNGLMAPNAMEDBUFFEREXTPROC MapNamedBufferEXT;
  PFNGLUNMAPNAMEDBUFFEREXTPROC UnmapNamedBufferEXT;
  PFNGLMAPNAMEDBUFFERRANGEEXTPROC MapNamedBufferRangeEXT;
  PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC FlushMappedNamedBufferRangeEXT;
  PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC NamedCopyBufferSubDataEXT;
  PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC GetNamedBufferParameterivEXT;
  PFNGLGETNAMEDBUFFERPOINTERVEXTPROC GetNamedBufferPointervEXT;
  PFNGLGETNAMEDBUFFERSUBDATAEXTPROC GetNamedBufferSubDataEXT;
  PFNGLTEXTUREBUFFEREXTPROC TextureBufferEXT;
  PFNGLMULTITEXBUFFEREXTPROC MultiTexBufferEXT;
  PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC NamedRenderbufferStorageEXT;
  PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC GetNamedRenderbufferParameterivEXT;
  PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC CheckNamedFramebufferStatusEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC NamedFramebufferTexture1DEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC NamedFramebufferTexture2DEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC NamedFramebufferTexture3DEXT;
  PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC NamedFramebufferRenderbufferEXT;
  PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC GetNamedFramebufferAttachmentParameterivEXT;
  PFNGLGENERATETEXTUREMIPMAPEXTPROC GenerateTextureMipmapEXT;
  PFNGLGENERATEMULTITEXMIPMAPEXTPROC GenerateMultiTexMipmapEXT;
  PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC FramebufferDrawBufferEXT;
  PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC FramebufferDrawBuffersEXT;
  PFNGLFRAMEBUFFERREADBUFFEREXTPROC FramebufferReadBufferEXT;
  PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC GetFramebufferParameterivEXT;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC NamedRenderbufferStorageMultisampleEXT;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC NamedRenderbufferStorageMultisampleCoverageEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC NamedFramebufferTextureEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC NamedFramebufferTextureLayerEXT;
  PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC NamedFramebufferTextureFaceEXT;
  PFNGLTEXTURERENDERBUFFEREXTPROC TextureRenderbufferEXT;
  PFNGLMULTITEXRENDERBUFFEREXTPROC MultiTexRenderbufferEXT;
  PFNGLPROGRAMUNIFORM1DEXTPROC ProgramUniform1dEXT;
  PFNGLPROGRAMUNIFORM2DEXTPROC ProgramUniform2dEXT;
  PFNGLPROGRAMUNIFORM3DEXTPROC ProgramUniform3dEXT;
  PFNGLPROGRAMUNIFORM4DEXTPROC ProgramUniform4dEXT;
  PFNGLPROGRAMUNIFORM1DVEXTPROC ProgramUniform1dvEXT;
  PFNGLPROGRAMUNIFORM2DVEXTPROC ProgramUniform2dvEXT;
  PFNGLPROGRAMUNIFORM3DVEXTPROC ProgramUniform3dvEXT;
  PFNGLPROGRAMUNIFORM4DVEXTPROC ProgramUniform4dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC ProgramUniformMatrix2dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC ProgramUniformMatrix3dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC ProgramUniformMatrix4dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC ProgramUniformMatrix2x3dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC ProgramUniformMatrix2x4dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC ProgramUniformMatrix3x2dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC ProgramUniformMatrix3x4dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC ProgramUniformMatrix4x2dvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC ProgramUniformMatrix4x3dvEXT;
  PFNGLGETMULTISAMPLEFVNVPROC GetMultisamplefvNV;
  PFNGLSAMPLEMASKINDEXEDNVPROC SampleMaskIndexedNV;
  PFNGLTEXRENDERBUFFERNVPROC TexRenderbufferNV;
  PFNGLBINDTRANSFORMFEEDBACKNVPROC BindTransformFeedbackNV;
  PFNGLDELETETRANSFORMFEEDBACKSNVPROC DeleteTransformFeedbacksNV;
  PFNGLGENTRANSFORMFEEDBACKSNVPROC GenTransformFeedbacksNV;
  PFNGLISTRANSFORMFEEDBACKNVPROC IsTransformFeedbackNV;
  PFNGLPAUSETRANSFORMFEEDBACKNVPROC PauseTransformFeedbackNV;
  PFNGLRESUMETRANSFORMFEEDBACKNVPROC ResumeTransformFeedbackNV;
  PFNGLDRAWTRANSFORMFEEDBACKNVPROC DrawTransformFeedbackNV;
  PFNGLGETPERFMONITORGROUPSAMDPROC GetPerfMonitorGroupsAMD;
  PFNGLGETPERFMONITORCOUNTERSAMDPROC GetPerfMonitorCountersAMD;
  PFNGLGETPERFMONITORGROUPSTRINGAMDPROC GetPerfMonitorGroupStringAMD;
  PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC GetPerfMonitorCounterStringAMD;
  PFNGLGETPERFMONITORCOUNTERINFOAMDPROC GetPerfMonitorCounterInfoAMD;
  PFNGLGENPERFMONITORSAMDPROC GenPerfMonitorsAMD;
  PFNGLDELETEPERFMONITORSAMDPROC DeletePerfMonitorsAMD;
  PFNGLSELECTPERFMONITORCOUNTERSAMDPROC SelectPerfMonitorCountersAMD;
  PFNGLBEGINPERFMONITORAMDPROC BeginPerfMonitorAMD;
  PFNGLENDPERFMONITORAMDPROC EndPerfMonitorAMD;
  PFNGLGETPERFMONITORCOUNTERDATAAMDPROC GetPerfMonitorCounterDataAMD;
  PFNGLTESSELLATIONFACTORAMDPROC TessellationFactorAMD;
  PFNGLTESSELLATIONMODEAMDPROC TessellationModeAMD;
  PFNGLPROVOKINGVERTEXEXTPROC ProvokingVertexEXT;
  PFNGLBLENDFUNCINDEXEDAMDPROC BlendFuncIndexedAMD;
  PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC BlendFuncSeparateIndexedAMD;
  PFNGLBLENDEQUATIONINDEXEDAMDPROC BlendEquationIndexedAMD;
  PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC BlendEquationSeparateIndexedAMD;
  PFNGLTEXTURERANGEAPPLEPROC TextureRangeAPPLE;
  PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC GetTexParameterPointervAPPLE;
  PFNGLENABLEVERTEXATTRIBAPPLEPROC EnableVertexAttribAPPLE;
  PFNGLDISABLEVERTEXATTRIBAPPLEPROC DisableVertexAttribAPPLE;
  PFNGLISVERTEXATTRIBENABLEDAPPLEPROC IsVertexAttribEnabledAPPLE;
  PFNGLMAPVERTEXATTRIB1DAPPLEPROC MapVertexAttrib1dAPPLE;
  PFNGLMAPVERTEXATTRIB1FAPPLEPROC MapVertexAttrib1fAPPLE;
  PFNGLMAPVERTEXATTRIB2DAPPLEPROC MapVertexAttrib2dAPPLE;
  PFNGLMAPVERTEXATTRIB2FAPPLEPROC MapVertexAttrib2fAPPLE;
  PFNGLOBJECTPURGEABLEAPPLEPROC ObjectPurgeableAPPLE;
  PFNGLOBJECTUNPURGEABLEAPPLEPROC ObjectUnpurgeableAPPLE;
  PFNGLGETOBJECTPARAMETERIVAPPLEPROC GetObjectParameterivAPPLE;
  PFNGLBEGINVIDEOCAPTURENVPROC BeginVideoCaptureNV;
  PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC BindVideoCaptureStreamBufferNV;
  PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC BindVideoCaptureStreamTextureNV;
  PFNGLENDVIDEOCAPTURENVPROC EndVideoCaptureNV;
  PFNGLGETVIDEOCAPTUREIVNVPROC GetVideoCaptureivNV;
  PFNGLGETVIDEOCAPTURESTREAMIVNVPROC GetVideoCaptureStreamivNV;
  PFNGLGETVIDEOCAPTURESTREAMFVNVPROC GetVideoCaptureStreamfvNV;
  PFNGLGETVIDEOCAPTURESTREAMDVNVPROC GetVideoCaptureStreamdvNV;
  PFNGLVIDEOCAPTURENVPROC VideoCaptureNV;
  PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC VideoCaptureStreamParameterivNV;
  PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC VideoCaptureStreamParameterfvNV;
  PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC VideoCaptureStreamParameterdvNV;
  PFNGLCOPYIMAGESUBDATANVPROC CopyImageSubDataNV;
  PFNGLUSESHADERPROGRAMEXTPROC UseShaderProgramEXT;
  PFNGLACTIVEPROGRAMEXTPROC ActiveProgramEXT;
  PFNGLCREATESHADERPROGRAMEXTPROC CreateShaderProgramEXT;
  PFNGLMAKEBUFFERRESIDENTNVPROC MakeBufferResidentNV;
  PFNGLMAKEBUFFERNONRESIDENTNVPROC MakeBufferNonResidentNV;
  PFNGLISBUFFERRESIDENTNVPROC IsBufferResidentNV;
  PFNGLMAKENAMEDBUFFERRESIDENTNVPROC MakeNamedBufferResidentNV;
  PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC MakeNamedBufferNonResidentNV;
  PFNGLISNAMEDBUFFERRESIDENTNVPROC IsNamedBufferResidentNV;
  PFNGLGETBUFFERPARAMETERUI64VNVPROC GetBufferParameterui64vNV;
  PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC GetNamedBufferParameterui64vNV;
  PFNGLGETINTEGERUI64VNVPROC GetIntegerui64vNV;
  PFNGLUNIFORMUI64NVPROC Uniformui64NV;
  PFNGLUNIFORMUI64VNVPROC Uniformui64vNV;
  PFNGLGETUNIFORMUI64VNVPROC GetUniformui64vNV;
  PFNGLPROGRAMUNIFORMUI64NVPROC ProgramUniformui64NV;
  PFNGLPROGRAMUNIFORMUI64VNVPROC ProgramUniformui64vNV;
  PFNGLBUFFERADDRESSRANGENVPROC BufferAddressRangeNV;
  PFNGLVERTEXFORMATNVPROC VertexFormatNV;
  PFNGLNORMALFORMATNVPROC NormalFormatNV;
  PFNGLCOLORFORMATNVPROC ColorFormatNV;
  PFNGLINDEXFORMATNVPROC IndexFormatNV;
  PFNGLTEXCOORDFORMATNVPROC TexCoordFormatNV;
  PFNGLEDGEFLAGFORMATNVPROC EdgeFlagFormatNV;
  PFNGLSECONDARYCOLORFORMATNVPROC SecondaryColorFormatNV;
  PFNGLFOGCOORDFORMATNVPROC FogCoordFormatNV;
  PFNGLVERTEXATTRIBFORMATNVPROC VertexAttribFormatNV;
  PFNGLVERTEXATTRIBIFORMATNVPROC VertexAttribIFormatNV;
  PFNGLGETINTEGERUI64I_VNVPROC GetIntegerui64i_vNV;
  PFNGLTEXTUREBARRIERNVPROC TextureBarrierNV;
  PFNGLBINDIMAGETEXTUREEXTPROC BindImageTextureEXT;
  PFNGLMEMORYBARRIEREXTPROC MemoryBarrierEXT;
  PFNGLVERTEXATTRIBL1DEXTPROC VertexAttribL1dEXT;
  PFNGLVERTEXATTRIBL2DEXTPROC VertexAttribL2dEXT;
  PFNGLVERTEXATTRIBL3DEXTPROC VertexAttribL3dEXT;
  PFNGLVERTEXATTRIBL4DEXTPROC VertexAttribL4dEXT;
  PFNGLVERTEXATTRIBL1DVEXTPROC VertexAttribL1dvEXT;
  PFNGLVERTEXATTRIBL2DVEXTPROC VertexAttribL2dvEXT;
  PFNGLVERTEXATTRIBL3DVEXTPROC VertexAttribL3dvEXT;
  PFNGLVERTEXATTRIBL4DVEXTPROC VertexAttribL4dvEXT;
  PFNGLVERTEXATTRIBLPOINTEREXTPROC VertexAttribLPointerEXT;
  PFNGLGETVERTEXATTRIBLDVEXTPROC GetVertexAttribLdvEXT;
  PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC VertexArrayVertexAttribLOffsetEXT;
  PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC ProgramSubroutineParametersuivNV;
  PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC GetProgramSubroutineParameteruivNV;
  PFNGLUNIFORM1I64NVPROC Uniform1i64NV;
  PFNGLUNIFORM2I64NVPROC Uniform2i64NV;
  PFNGLUNIFORM3I64NVPROC Uniform3i64NV;
  PFNGLUNIFORM4I64NVPROC Uniform4i64NV;
  PFNGLUNIFORM1I64VNVPROC Uniform1i64vNV;
  PFNGLUNIFORM2I64VNVPROC Uniform2i64vNV;
  PFNGLUNIFORM3I64VNVPROC Uniform3i64vNV;
  PFNGLUNIFORM4I64VNVPROC Uniform4i64vNV;
  PFNGLUNIFORM1UI64NVPROC Uniform1ui64NV;
  PFNGLUNIFORM2UI64NVPROC Uniform2ui64NV;
  PFNGLUNIFORM3UI64NVPROC Uniform3ui64NV;
  PFNGLUNIFORM4UI64NVPROC Uniform4ui64NV;
  PFNGLUNIFORM1UI64VNVPROC Uniform1ui64vNV;
  PFNGLUNIFORM2UI64VNVPROC Uniform2ui64vNV;
  PFNGLUNIFORM3UI64VNVPROC Uniform3ui64vNV;
  PFNGLUNIFORM4UI64VNVPROC Uniform4ui64vNV;
  PFNGLGETUNIFORMI64VNVPROC GetUniformi64vNV;
  PFNGLPROGRAMUNIFORM1I64NVPROC ProgramUniform1i64NV;
  PFNGLPROGRAMUNIFORM2I64NVPROC ProgramUniform2i64NV;
  PFNGLPROGRAMUNIFORM3I64NVPROC ProgramUniform3i64NV;
  PFNGLPROGRAMUNIFORM4I64NVPROC ProgramUniform4i64NV;
  PFNGLPROGRAMUNIFORM1I64VNVPROC ProgramUniform1i64vNV;
  PFNGLPROGRAMUNIFORM2I64VNVPROC ProgramUniform2i64vNV;
  PFNGLPROGRAMUNIFORM3I64VNVPROC ProgramUniform3i64vNV;
  PFNGLPROGRAMUNIFORM4I64VNVPROC ProgramUniform4i64vNV;
  PFNGLPROGRAMUNIFORM1UI64NVPROC ProgramUniform1ui64NV;
  PFNGLPROGRAMUNIFORM2UI64NVPROC ProgramUniform2ui64NV;
  PFNGLPROGRAMUNIFORM3UI64NVPROC ProgramUniform3ui64NV;
  PFNGLPROGRAMUNIFORM4UI64NVPROC ProgramUniform4ui64NV;
  PFNGLPROGRAMUNIFORM1UI64VNVPROC ProgramUniform1ui64vNV;
  PFNGLPROGRAMUNIFORM2UI64VNVPROC ProgramUniform2ui64vNV;
  PFNGLPROGRAMUNIFORM3UI64VNVPROC ProgramUniform3ui64vNV;
  PFNGLPROGRAMUNIFORM4UI64VNVPROC ProgramUniform4ui64vNV;
  PFNGLVERTEXATTRIBL1I64NVPROC VertexAttribL1i64NV;
  PFNGLVERTEXATTRIBL2I64NVPROC VertexAttribL2i64NV;
  PFNGLVERTEXATTRIBL3I64NVPROC VertexAttribL3i64NV;
  PFNGLVERTEXATTRIBL4I64NVPROC VertexAttribL4i64NV;
  PFNGLVERTEXATTRIBL1I64VNVPROC VertexAttribL1i64vNV;
  PFNGLVERTEXATTRIBL2I64VNVPROC VertexAttribL2i64vNV;
  PFNGLVERTEXATTRIBL3I64VNVPROC VertexAttribL3i64vNV;
  PFNGLVERTEXATTRIBL4I64VNVPROC VertexAttribL4i64vNV;
  PFNGLVERTEXATTRIBL1UI64NVPROC VertexAttribL1ui64NV;
  PFNGLVERTEXATTRIBL2UI64NVPROC VertexAttribL2ui64NV;
  PFNGLVERTEXATTRIBL3UI64NVPROC VertexAttribL3ui64NV;
  PFNGLVERTEXATTRIBL4UI64NVPROC VertexAttribL4ui64NV;
  PFNGLVERTEXATTRIBL1UI64VNVPROC VertexAttribL1ui64vNV;
  PFNGLVERTEXATTRIBL2UI64VNVPROC VertexAttribL2ui64vNV;
  PFNGLVERTEXATTRIBL3UI64VNVPROC VertexAttribL3ui64vNV;
  PFNGLVERTEXATTRIBL4UI64VNVPROC VertexAttribL4ui64vNV;
  PFNGLGETVERTEXATTRIBLI64VNVPROC GetVertexAttribLi64vNV;
  PFNGLGETVERTEXATTRIBLUI64VNVPROC GetVertexAttribLui64vNV;
  PFNGLVERTEXATTRIBLFORMATNVPROC VertexAttribLFormatNV;
  PFNGLGENNAMESAMDPROC GenNamesAMD;
  PFNGLDELETENAMESAMDPROC DeleteNamesAMD;
  PFNGLISNAMEAMDPROC IsNameAMD;
  PFNGLDEBUGMESSAGEENABLEAMDPROC DebugMessageEnableAMD;
  PFNGLDEBUGMESSAGEINSERTAMDPROC DebugMessageInsertAMD;
  PFNGLDEBUGMESSAGECALLBACKAMDPROC DebugMessageCallbackAMD;
  PFNGLGETDEBUGMESSAGELOGAMDPROC GetDebugMessageLogAMD;
  PFNGLVDPAUINITNVPROC VDPAUInitNV;
  PFNGLVDPAUFININVPROC VDPAUFiniNV;
  PFNGLVDPAUREGISTERVIDEOSURFACENVPROC VDPAURegisterVideoSurfaceNV;
  PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC VDPAURegisterOutputSurfaceNV;
  PFNGLVDPAUISSURFACENVPROC VDPAUIsSurfaceNV;
  PFNGLVDPAUUNREGISTERSURFACENVPROC VDPAUUnregisterSurfaceNV;
  PFNGLVDPAUGETSURFACEIVNVPROC VDPAUGetSurfaceivNV;
  PFNGLVDPAUSURFACEACCESSNVPROC VDPAUSurfaceAccessNV;
  PFNGLVDPAUMAPSURFACESNVPROC VDPAUMapSurfacesNV;
  PFNGLVDPAUUNMAPSURFACESNVPROC VDPAUUnmapSurfacesNV;
  PFNGLTEXIMAGE2DMULTISAMPLECOVERAGENVPROC TexImage2DMultisampleCoverageNV;
  PFNGLTEXIMAGE3DMULTISAMPLECOVERAGENVPROC TexImage3DMultisampleCoverageNV;
  PFNGLTEXTUREIMAGE2DMULTISAMPLENVPROC TextureImage2DMultisampleNV;
  PFNGLTEXTUREIMAGE3DMULTISAMPLENVPROC TextureImage3DMultisampleNV;
  PFNGLTEXTUREIMAGE2DMULTISAMPLECOVERAGENVPROC TextureImage2DMultisampleCoverageNV;
  PFNGLTEXTUREIMAGE3DMULTISAMPLECOVERAGENVPROC TextureImage3DMultisampleCoverageNV;
  PFNGLSETMULTISAMPLEFVAMDPROC SetMultisamplefvAMD;
  PFNGLIMPORTSYNCEXTPROC ImportSyncEXT;
  PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC MultiDrawArraysIndirectAMD;
  PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC MultiDrawElementsIndirectAMD;
  PFNGLGENPATHSNVPROC GenPathsNV;
  PFNGLDELETEPATHSNVPROC DeletePathsNV;
  PFNGLISPATHNVPROC IsPathNV;
  PFNGLPATHCOMMANDSNVPROC PathCommandsNV;
  PFNGLPATHCOORDSNVPROC PathCoordsNV;
  PFNGLPATHSUBCOMMANDSNVPROC PathSubCommandsNV;
  PFNGLPATHSUBCOORDSNVPROC PathSubCoordsNV;
  PFNGLPATHSTRINGNVPROC PathStringNV;
  PFNGLPATHGLYPHSNVPROC PathGlyphsNV;
  PFNGLPATHGLYPHRANGENVPROC PathGlyphRangeNV;
  PFNGLWEIGHTPATHSNVPROC WeightPathsNV;
  PFNGLCOPYPATHNVPROC CopyPathNV;
  PFNGLINTERPOLATEPATHSNVPROC InterpolatePathsNV;
  PFNGLTRANSFORMPATHNVPROC TransformPathNV;
  PFNGLPATHPARAMETERIVNVPROC PathParameterivNV;
  PFNGLPATHPARAMETERINVPROC PathParameteriNV;
  PFNGLPATHPARAMETERFVNVPROC PathParameterfvNV;
  PFNGLPATHPARAMETERFNVPROC PathParameterfNV;
  PFNGLPATHDASHARRAYNVPROC PathDashArrayNV;
  PFNGLPATHSTENCILFUNCNVPROC PathStencilFuncNV;
  PFNGLPATHSTENCILDEPTHOFFSETNVPROC PathStencilDepthOffsetNV;
  PFNGLSTENCILFILLPATHNVPROC StencilFillPathNV;
  PFNGLSTENCILSTROKEPATHNVPROC StencilStrokePathNV;
  PFNGLSTENCILFILLPATHINSTANCEDNVPROC StencilFillPathInstancedNV;
  PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC StencilStrokePathInstancedNV;
  PFNGLPATHCOVERDEPTHFUNCNVPROC PathCoverDepthFuncNV;
  PFNGLPATHCOLORGENNVPROC PathColorGenNV;
  PFNGLPATHTEXGENNVPROC PathTexGenNV;
  PFNGLPATHFOGGENNVPROC PathFogGenNV;
  PFNGLCOVERFILLPATHNVPROC CoverFillPathNV;
  PFNGLCOVERSTROKEPATHNVPROC CoverStrokePathNV;
  PFNGLCOVERFILLPATHINSTANCEDNVPROC CoverFillPathInstancedNV;
  PFNGLCOVERSTROKEPATHINSTANCEDNVPROC CoverStrokePathInstancedNV;
  PFNGLGETPATHPARAMETERIVNVPROC GetPathParameterivNV;
  PFNGLGETPATHPARAMETERFVNVPROC GetPathParameterfvNV;
  PFNGLGETPATHCOMMANDSNVPROC GetPathCommandsNV;
  PFNGLGETPATHCOORDSNVPROC GetPathCoordsNV;
  PFNGLGETPATHDASHARRAYNVPROC GetPathDashArrayNV;
  PFNGLGETPATHMETRICSNVPROC GetPathMetricsNV;
  PFNGLGETPATHMETRICRANGENVPROC GetPathMetricRangeNV;
  PFNGLGETPATHSPACINGNVPROC GetPathSpacingNV;
  PFNGLGETPATHCOLORGENIVNVPROC GetPathColorGenivNV;
  PFNGLGETPATHCOLORGENFVNVPROC GetPathColorGenfvNV;
  PFNGLGETPATHTEXGENIVNVPROC GetPathTexGenivNV;
  PFNGLGETPATHTEXGENFVNVPROC GetPathTexGenfvNV;
  PFNGLISPOINTINFILLPATHNVPROC IsPointInFillPathNV;
  PFNGLISPOINTINSTROKEPATHNVPROC IsPointInStrokePathNV;
  PFNGLGETPATHLENGTHNVPROC GetPathLengthNV;
  PFNGLPOINTALONGPATHNVPROC PointAlongPathNV;
  PFNGLSTENCILOPVALUEAMDPROC StencilOpValueAMD;
  PFNGLGETTEXTUREHANDLENVPROC GetTextureHandleNV;
  PFNGLGETTEXTURESAMPLERHANDLENVPROC GetTextureSamplerHandleNV;
  PFNGLMAKETEXTUREHANDLERESIDENTNVPROC MakeTextureHandleResidentNV;
  PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC MakeTextureHandleNonResidentNV;
  PFNGLGETIMAGEHANDLENVPROC GetImageHandleNV;
  PFNGLMAKEIMAGEHANDLERESIDENTNVPROC MakeImageHandleResidentNV;
  PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC MakeImageHandleNonResidentNV;
  PFNGLUNIFORMHANDLEUI64NVPROC UniformHandleui64NV;
  PFNGLUNIFORMHANDLEUI64VNVPROC UniformHandleui64vNV;
  PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC ProgramUniformHandleui64NV;
  PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC ProgramUniformHandleui64vNV;
  PFNGLISTEXTUREHANDLERESIDENTNVPROC IsTextureHandleResidentNV;
  PFNGLISIMAGEHANDLERESIDENTNVPROC IsImageHandleResidentNV;
  PFNGLTEXSTORAGESPARSEAMDPROC TexStorageSparseAMD;
  PFNGLTEXTURESTORAGESPARSEAMDPROC TextureStorageSparseAMD;

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
#define glTexImage3D get_gl_entries ()->TexImage3D
#define glTexSubImage3D get_gl_entries ()->TexSubImage3D
#define glCopyTexSubImage3D get_gl_entries ()->CopyTexSubImage3D
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
#define glActiveTexture get_gl_entries ()->ActiveTexture
#define glSampleCoverage get_gl_entries ()->SampleCoverage
#define glCompressedTexImage3D get_gl_entries ()->CompressedTexImage3D
#define glCompressedTexImage2D get_gl_entries ()->CompressedTexImage2D
#define glCompressedTexImage1D get_gl_entries ()->CompressedTexImage1D
#define glCompressedTexSubImage3D get_gl_entries ()->CompressedTexSubImage3D
#define glCompressedTexSubImage2D get_gl_entries ()->CompressedTexSubImage2D
#define glCompressedTexSubImage1D get_gl_entries ()->CompressedTexSubImage1D
#define glGetCompressedTexImage get_gl_entries ()->GetCompressedTexImage
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
#define glBlendFuncSeparate get_gl_entries ()->BlendFuncSeparate
#define glMultiDrawArrays get_gl_entries ()->MultiDrawArrays
#define glMultiDrawElements get_gl_entries ()->MultiDrawElements
#define glPointParameterf get_gl_entries ()->PointParameterf
#define glPointParameterfv get_gl_entries ()->PointParameterfv
#define glPointParameteri get_gl_entries ()->PointParameteri
#define glPointParameteriv get_gl_entries ()->PointParameteriv
#define glFogCoordf get_gl_entries ()->FogCoordf
#define glFogCoordfv get_gl_entries ()->FogCoordfv
#define glFogCoordd get_gl_entries ()->FogCoordd
#define glFogCoorddv get_gl_entries ()->FogCoorddv
#define glFogCoordPointer get_gl_entries ()->FogCoordPointer
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
#define glColorMaski get_gl_entries ()->ColorMaski
#define glGetBooleani_v get_gl_entries ()->GetBooleani_v
#define glGetIntegeri_v get_gl_entries ()->GetIntegeri_v
#define glEnablei get_gl_entries ()->Enablei
#define glDisablei get_gl_entries ()->Disablei
#define glIsEnabledi get_gl_entries ()->IsEnabledi
#define glBeginTransformFeedback get_gl_entries ()->BeginTransformFeedback
#define glEndTransformFeedback get_gl_entries ()->EndTransformFeedback
#define glBindBufferRange get_gl_entries ()->BindBufferRange
#define glBindBufferBase get_gl_entries ()->BindBufferBase
#define glTransformFeedbackVaryings get_gl_entries ()->TransformFeedbackVaryings
#define glGetTransformFeedbackVarying get_gl_entries ()->GetTransformFeedbackVarying
#define glClampColor get_gl_entries ()->ClampColor
#define glBeginConditionalRender get_gl_entries ()->BeginConditionalRender
#define glEndConditionalRender get_gl_entries ()->EndConditionalRender
#define glVertexAttribIPointer get_gl_entries ()->VertexAttribIPointer
#define glGetVertexAttribIiv get_gl_entries ()->GetVertexAttribIiv
#define glGetVertexAttribIuiv get_gl_entries ()->GetVertexAttribIuiv
#define glVertexAttribI1i get_gl_entries ()->VertexAttribI1i
#define glVertexAttribI2i get_gl_entries ()->VertexAttribI2i
#define glVertexAttribI3i get_gl_entries ()->VertexAttribI3i
#define glVertexAttribI4i get_gl_entries ()->VertexAttribI4i
#define glVertexAttribI1ui get_gl_entries ()->VertexAttribI1ui
#define glVertexAttribI2ui get_gl_entries ()->VertexAttribI2ui
#define glVertexAttribI3ui get_gl_entries ()->VertexAttribI3ui
#define glVertexAttribI4ui get_gl_entries ()->VertexAttribI4ui
#define glVertexAttribI1iv get_gl_entries ()->VertexAttribI1iv
#define glVertexAttribI2iv get_gl_entries ()->VertexAttribI2iv
#define glVertexAttribI3iv get_gl_entries ()->VertexAttribI3iv
#define glVertexAttribI4iv get_gl_entries ()->VertexAttribI4iv
#define glVertexAttribI1uiv get_gl_entries ()->VertexAttribI1uiv
#define glVertexAttribI2uiv get_gl_entries ()->VertexAttribI2uiv
#define glVertexAttribI3uiv get_gl_entries ()->VertexAttribI3uiv
#define glVertexAttribI4uiv get_gl_entries ()->VertexAttribI4uiv
#define glVertexAttribI4bv get_gl_entries ()->VertexAttribI4bv
#define glVertexAttribI4sv get_gl_entries ()->VertexAttribI4sv
#define glVertexAttribI4ubv get_gl_entries ()->VertexAttribI4ubv
#define glVertexAttribI4usv get_gl_entries ()->VertexAttribI4usv
#define glGetUniformuiv get_gl_entries ()->GetUniformuiv
#define glBindFragDataLocation get_gl_entries ()->BindFragDataLocation
#define glGetFragDataLocation get_gl_entries ()->GetFragDataLocation
#define glUniform1ui get_gl_entries ()->Uniform1ui
#define glUniform2ui get_gl_entries ()->Uniform2ui
#define glUniform3ui get_gl_entries ()->Uniform3ui
#define glUniform4ui get_gl_entries ()->Uniform4ui
#define glUniform1uiv get_gl_entries ()->Uniform1uiv
#define glUniform2uiv get_gl_entries ()->Uniform2uiv
#define glUniform3uiv get_gl_entries ()->Uniform3uiv
#define glUniform4uiv get_gl_entries ()->Uniform4uiv
#define glTexParameterIiv get_gl_entries ()->TexParameterIiv
#define glTexParameterIuiv get_gl_entries ()->TexParameterIuiv
#define glGetTexParameterIiv get_gl_entries ()->GetTexParameterIiv
#define glGetTexParameterIuiv get_gl_entries ()->GetTexParameterIuiv
#define glClearBufferiv get_gl_entries ()->ClearBufferiv
#define glClearBufferuiv get_gl_entries ()->ClearBufferuiv
#define glClearBufferfv get_gl_entries ()->ClearBufferfv
#define glClearBufferfi get_gl_entries ()->ClearBufferfi
#define glGetStringi get_gl_entries ()->GetStringi
#define glDrawArraysInstanced get_gl_entries ()->DrawArraysInstanced
#define glDrawElementsInstanced get_gl_entries ()->DrawElementsInstanced
#define glTexBuffer get_gl_entries ()->TexBuffer
#define glPrimitiveRestartIndex get_gl_entries ()->PrimitiveRestartIndex
#define glGetInteger64i_v get_gl_entries ()->GetInteger64i_v
#define glGetBufferParameteri64v get_gl_entries ()->GetBufferParameteri64v
#define glFramebufferTexture get_gl_entries ()->FramebufferTexture
#define glVertexAttribDivisor get_gl_entries ()->VertexAttribDivisor
#define glMinSampleShading get_gl_entries ()->MinSampleShading
#define glBlendEquationi get_gl_entries ()->BlendEquationi
#define glBlendEquationSeparatei get_gl_entries ()->BlendEquationSeparatei
#define glBlendFunci get_gl_entries ()->BlendFunci
#define glBlendFuncSeparatei get_gl_entries ()->BlendFuncSeparatei
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
#define glDrawArraysInstancedARB get_gl_entries ()->DrawArraysInstancedARB
#define glDrawElementsInstancedARB get_gl_entries ()->DrawElementsInstancedARB
#define glIsRenderbuffer get_gl_entries ()->IsRenderbuffer
#define glBindRenderbuffer get_gl_entries ()->BindRenderbuffer
#define glDeleteRenderbuffers get_gl_entries ()->DeleteRenderbuffers
#define glGenRenderbuffers get_gl_entries ()->GenRenderbuffers
#define glRenderbufferStorage get_gl_entries ()->RenderbufferStorage
#define glGetRenderbufferParameteriv get_gl_entries ()->GetRenderbufferParameteriv
#define glIsFramebuffer get_gl_entries ()->IsFramebuffer
#define glBindFramebuffer get_gl_entries ()->BindFramebuffer
#define glDeleteFramebuffers get_gl_entries ()->DeleteFramebuffers
#define glGenFramebuffers get_gl_entries ()->GenFramebuffers
#define glCheckFramebufferStatus get_gl_entries ()->CheckFramebufferStatus
#define glFramebufferTexture1D get_gl_entries ()->FramebufferTexture1D
#define glFramebufferTexture2D get_gl_entries ()->FramebufferTexture2D
#define glFramebufferTexture3D get_gl_entries ()->FramebufferTexture3D
#define glFramebufferRenderbuffer get_gl_entries ()->FramebufferRenderbuffer
#define glGetFramebufferAttachmentParameteriv get_gl_entries ()->GetFramebufferAttachmentParameteriv
#define glGenerateMipmap get_gl_entries ()->GenerateMipmap
#define glBlitFramebuffer get_gl_entries ()->BlitFramebuffer
#define glRenderbufferStorageMultisample get_gl_entries ()->RenderbufferStorageMultisample
#define glFramebufferTextureLayer get_gl_entries ()->FramebufferTextureLayer
#define glProgramParameteriARB get_gl_entries ()->ProgramParameteriARB
#define glFramebufferTextureARB get_gl_entries ()->FramebufferTextureARB
#define glFramebufferTextureLayerARB get_gl_entries ()->FramebufferTextureLayerARB
#define glFramebufferTextureFaceARB get_gl_entries ()->FramebufferTextureFaceARB
#define glVertexAttribDivisorARB get_gl_entries ()->VertexAttribDivisorARB
#define glMapBufferRange get_gl_entries ()->MapBufferRange
#define glFlushMappedBufferRange get_gl_entries ()->FlushMappedBufferRange
#define glTexBufferARB get_gl_entries ()->TexBufferARB
#define glBindVertexArray get_gl_entries ()->BindVertexArray
#define glDeleteVertexArrays get_gl_entries ()->DeleteVertexArrays
#define glGenVertexArrays get_gl_entries ()->GenVertexArrays
#define glIsVertexArray get_gl_entries ()->IsVertexArray
#define glGetUniformIndices get_gl_entries ()->GetUniformIndices
#define glGetActiveUniformsiv get_gl_entries ()->GetActiveUniformsiv
#define glGetActiveUniformName get_gl_entries ()->GetActiveUniformName
#define glGetUniformBlockIndex get_gl_entries ()->GetUniformBlockIndex
#define glGetActiveUniformBlockiv get_gl_entries ()->GetActiveUniformBlockiv
#define glGetActiveUniformBlockName get_gl_entries ()->GetActiveUniformBlockName
#define glUniformBlockBinding get_gl_entries ()->UniformBlockBinding
#define glCopyBufferSubData get_gl_entries ()->CopyBufferSubData
#define glDrawElementsBaseVertex get_gl_entries ()->DrawElementsBaseVertex
#define glDrawRangeElementsBaseVertex get_gl_entries ()->DrawRangeElementsBaseVertex
#define glDrawElementsInstancedBaseVertex get_gl_entries ()->DrawElementsInstancedBaseVertex
#define glMultiDrawElementsBaseVertex get_gl_entries ()->MultiDrawElementsBaseVertex
#define glProvokingVertex get_gl_entries ()->ProvokingVertex
#define glFenceSync get_gl_entries ()->FenceSync
#define glIsSync get_gl_entries ()->IsSync
#define glDeleteSync get_gl_entries ()->DeleteSync
#define glClientWaitSync get_gl_entries ()->ClientWaitSync
#define glWaitSync get_gl_entries ()->WaitSync
#define glGetInteger64v get_gl_entries ()->GetInteger64v
#define glGetSynciv get_gl_entries ()->GetSynciv
#define glTexImage2DMultisample get_gl_entries ()->TexImage2DMultisample
#define glTexImage3DMultisample get_gl_entries ()->TexImage3DMultisample
#define glGetMultisamplefv get_gl_entries ()->GetMultisamplefv
#define glSampleMaski get_gl_entries ()->SampleMaski
#define glBlendEquationiARB get_gl_entries ()->BlendEquationiARB
#define glBlendEquationSeparateiARB get_gl_entries ()->BlendEquationSeparateiARB
#define glBlendFunciARB get_gl_entries ()->BlendFunciARB
#define glBlendFuncSeparateiARB get_gl_entries ()->BlendFuncSeparateiARB
#define glMinSampleShadingARB get_gl_entries ()->MinSampleShadingARB
#define glNamedStringARB get_gl_entries ()->NamedStringARB
#define glDeleteNamedStringARB get_gl_entries ()->DeleteNamedStringARB
#define glCompileShaderIncludeARB get_gl_entries ()->CompileShaderIncludeARB
#define glIsNamedStringARB get_gl_entries ()->IsNamedStringARB
#define glGetNamedStringARB get_gl_entries ()->GetNamedStringARB
#define glGetNamedStringivARB get_gl_entries ()->GetNamedStringivARB
#define glBindFragDataLocationIndexed get_gl_entries ()->BindFragDataLocationIndexed
#define glGetFragDataIndex get_gl_entries ()->GetFragDataIndex
#define glGenSamplers get_gl_entries ()->GenSamplers
#define glDeleteSamplers get_gl_entries ()->DeleteSamplers
#define glIsSampler get_gl_entries ()->IsSampler
#define glBindSampler get_gl_entries ()->BindSampler
#define glSamplerParameteri get_gl_entries ()->SamplerParameteri
#define glSamplerParameteriv get_gl_entries ()->SamplerParameteriv
#define glSamplerParameterf get_gl_entries ()->SamplerParameterf
#define glSamplerParameterfv get_gl_entries ()->SamplerParameterfv
#define glSamplerParameterIiv get_gl_entries ()->SamplerParameterIiv
#define glSamplerParameterIuiv get_gl_entries ()->SamplerParameterIuiv
#define glGetSamplerParameteriv get_gl_entries ()->GetSamplerParameteriv
#define glGetSamplerParameterIiv get_gl_entries ()->GetSamplerParameterIiv
#define glGetSamplerParameterfv get_gl_entries ()->GetSamplerParameterfv
#define glGetSamplerParameterIuiv get_gl_entries ()->GetSamplerParameterIuiv
#define glQueryCounter get_gl_entries ()->QueryCounter
#define glGetQueryObjecti64v get_gl_entries ()->GetQueryObjecti64v
#define glGetQueryObjectui64v get_gl_entries ()->GetQueryObjectui64v
#define glVertexP2ui get_gl_entries ()->VertexP2ui
#define glVertexP2uiv get_gl_entries ()->VertexP2uiv
#define glVertexP3ui get_gl_entries ()->VertexP3ui
#define glVertexP3uiv get_gl_entries ()->VertexP3uiv
#define glVertexP4ui get_gl_entries ()->VertexP4ui
#define glVertexP4uiv get_gl_entries ()->VertexP4uiv
#define glTexCoordP1ui get_gl_entries ()->TexCoordP1ui
#define glTexCoordP1uiv get_gl_entries ()->TexCoordP1uiv
#define glTexCoordP2ui get_gl_entries ()->TexCoordP2ui
#define glTexCoordP2uiv get_gl_entries ()->TexCoordP2uiv
#define glTexCoordP3ui get_gl_entries ()->TexCoordP3ui
#define glTexCoordP3uiv get_gl_entries ()->TexCoordP3uiv
#define glTexCoordP4ui get_gl_entries ()->TexCoordP4ui
#define glTexCoordP4uiv get_gl_entries ()->TexCoordP4uiv
#define glMultiTexCoordP1ui get_gl_entries ()->MultiTexCoordP1ui
#define glMultiTexCoordP1uiv get_gl_entries ()->MultiTexCoordP1uiv
#define glMultiTexCoordP2ui get_gl_entries ()->MultiTexCoordP2ui
#define glMultiTexCoordP2uiv get_gl_entries ()->MultiTexCoordP2uiv
#define glMultiTexCoordP3ui get_gl_entries ()->MultiTexCoordP3ui
#define glMultiTexCoordP3uiv get_gl_entries ()->MultiTexCoordP3uiv
#define glMultiTexCoordP4ui get_gl_entries ()->MultiTexCoordP4ui
#define glMultiTexCoordP4uiv get_gl_entries ()->MultiTexCoordP4uiv
#define glNormalP3ui get_gl_entries ()->NormalP3ui
#define glNormalP3uiv get_gl_entries ()->NormalP3uiv
#define glColorP3ui get_gl_entries ()->ColorP3ui
#define glColorP3uiv get_gl_entries ()->ColorP3uiv
#define glColorP4ui get_gl_entries ()->ColorP4ui
#define glColorP4uiv get_gl_entries ()->ColorP4uiv
#define glSecondaryColorP3ui get_gl_entries ()->SecondaryColorP3ui
#define glSecondaryColorP3uiv get_gl_entries ()->SecondaryColorP3uiv
#define glVertexAttribP1ui get_gl_entries ()->VertexAttribP1ui
#define glVertexAttribP1uiv get_gl_entries ()->VertexAttribP1uiv
#define glVertexAttribP2ui get_gl_entries ()->VertexAttribP2ui
#define glVertexAttribP2uiv get_gl_entries ()->VertexAttribP2uiv
#define glVertexAttribP3ui get_gl_entries ()->VertexAttribP3ui
#define glVertexAttribP3uiv get_gl_entries ()->VertexAttribP3uiv
#define glVertexAttribP4ui get_gl_entries ()->VertexAttribP4ui
#define glVertexAttribP4uiv get_gl_entries ()->VertexAttribP4uiv
#define glDrawArraysIndirect get_gl_entries ()->DrawArraysIndirect
#define glDrawElementsIndirect get_gl_entries ()->DrawElementsIndirect
#define glUniform1d get_gl_entries ()->Uniform1d
#define glUniform2d get_gl_entries ()->Uniform2d
#define glUniform3d get_gl_entries ()->Uniform3d
#define glUniform4d get_gl_entries ()->Uniform4d
#define glUniform1dv get_gl_entries ()->Uniform1dv
#define glUniform2dv get_gl_entries ()->Uniform2dv
#define glUniform3dv get_gl_entries ()->Uniform3dv
#define glUniform4dv get_gl_entries ()->Uniform4dv
#define glUniformMatrix2dv get_gl_entries ()->UniformMatrix2dv
#define glUniformMatrix3dv get_gl_entries ()->UniformMatrix3dv
#define glUniformMatrix4dv get_gl_entries ()->UniformMatrix4dv
#define glUniformMatrix2x3dv get_gl_entries ()->UniformMatrix2x3dv
#define glUniformMatrix2x4dv get_gl_entries ()->UniformMatrix2x4dv
#define glUniformMatrix3x2dv get_gl_entries ()->UniformMatrix3x2dv
#define glUniformMatrix3x4dv get_gl_entries ()->UniformMatrix3x4dv
#define glUniformMatrix4x2dv get_gl_entries ()->UniformMatrix4x2dv
#define glUniformMatrix4x3dv get_gl_entries ()->UniformMatrix4x3dv
#define glGetUniformdv get_gl_entries ()->GetUniformdv
#define glGetSubroutineUniformLocation get_gl_entries ()->GetSubroutineUniformLocation
#define glGetSubroutineIndex get_gl_entries ()->GetSubroutineIndex
#define glGetActiveSubroutineUniformiv get_gl_entries ()->GetActiveSubroutineUniformiv
#define glGetActiveSubroutineUniformName get_gl_entries ()->GetActiveSubroutineUniformName
#define glGetActiveSubroutineName get_gl_entries ()->GetActiveSubroutineName
#define glUniformSubroutinesuiv get_gl_entries ()->UniformSubroutinesuiv
#define glGetUniformSubroutineuiv get_gl_entries ()->GetUniformSubroutineuiv
#define glGetProgramStageiv get_gl_entries ()->GetProgramStageiv
#define glPatchParameteri get_gl_entries ()->PatchParameteri
#define glPatchParameterfv get_gl_entries ()->PatchParameterfv
#define glBindTransformFeedback get_gl_entries ()->BindTransformFeedback
#define glDeleteTransformFeedbacks get_gl_entries ()->DeleteTransformFeedbacks
#define glGenTransformFeedbacks get_gl_entries ()->GenTransformFeedbacks
#define glIsTransformFeedback get_gl_entries ()->IsTransformFeedback
#define glPauseTransformFeedback get_gl_entries ()->PauseTransformFeedback
#define glResumeTransformFeedback get_gl_entries ()->ResumeTransformFeedback
#define glDrawTransformFeedback get_gl_entries ()->DrawTransformFeedback
#define glDrawTransformFeedbackStream get_gl_entries ()->DrawTransformFeedbackStream
#define glBeginQueryIndexed get_gl_entries ()->BeginQueryIndexed
#define glEndQueryIndexed get_gl_entries ()->EndQueryIndexed
#define glGetQueryIndexediv get_gl_entries ()->GetQueryIndexediv
#define glReleaseShaderCompiler get_gl_entries ()->ReleaseShaderCompiler
#define glShaderBinary get_gl_entries ()->ShaderBinary
#define glGetShaderPrecisionFormat get_gl_entries ()->GetShaderPrecisionFormat
#define glDepthRangef get_gl_entries ()->DepthRangef
#define glClearDepthf get_gl_entries ()->ClearDepthf
#define glGetProgramBinary get_gl_entries ()->GetProgramBinary
#define glProgramBinary get_gl_entries ()->ProgramBinary
#define glProgramParameteri get_gl_entries ()->ProgramParameteri
#define glUseProgramStages get_gl_entries ()->UseProgramStages
#define glActiveShaderProgram get_gl_entries ()->ActiveShaderProgram
#define glCreateShaderProgramv get_gl_entries ()->CreateShaderProgramv
#define glBindProgramPipeline get_gl_entries ()->BindProgramPipeline
#define glDeleteProgramPipelines get_gl_entries ()->DeleteProgramPipelines
#define glGenProgramPipelines get_gl_entries ()->GenProgramPipelines
#define glIsProgramPipeline get_gl_entries ()->IsProgramPipeline
#define glGetProgramPipelineiv get_gl_entries ()->GetProgramPipelineiv
#define glProgramUniform1i get_gl_entries ()->ProgramUniform1i
#define glProgramUniform1iv get_gl_entries ()->ProgramUniform1iv
#define glProgramUniform1f get_gl_entries ()->ProgramUniform1f
#define glProgramUniform1fv get_gl_entries ()->ProgramUniform1fv
#define glProgramUniform1d get_gl_entries ()->ProgramUniform1d
#define glProgramUniform1dv get_gl_entries ()->ProgramUniform1dv
#define glProgramUniform1ui get_gl_entries ()->ProgramUniform1ui
#define glProgramUniform1uiv get_gl_entries ()->ProgramUniform1uiv
#define glProgramUniform2i get_gl_entries ()->ProgramUniform2i
#define glProgramUniform2iv get_gl_entries ()->ProgramUniform2iv
#define glProgramUniform2f get_gl_entries ()->ProgramUniform2f
#define glProgramUniform2fv get_gl_entries ()->ProgramUniform2fv
#define glProgramUniform2d get_gl_entries ()->ProgramUniform2d
#define glProgramUniform2dv get_gl_entries ()->ProgramUniform2dv
#define glProgramUniform2ui get_gl_entries ()->ProgramUniform2ui
#define glProgramUniform2uiv get_gl_entries ()->ProgramUniform2uiv
#define glProgramUniform3i get_gl_entries ()->ProgramUniform3i
#define glProgramUniform3iv get_gl_entries ()->ProgramUniform3iv
#define glProgramUniform3f get_gl_entries ()->ProgramUniform3f
#define glProgramUniform3fv get_gl_entries ()->ProgramUniform3fv
#define glProgramUniform3d get_gl_entries ()->ProgramUniform3d
#define glProgramUniform3dv get_gl_entries ()->ProgramUniform3dv
#define glProgramUniform3ui get_gl_entries ()->ProgramUniform3ui
#define glProgramUniform3uiv get_gl_entries ()->ProgramUniform3uiv
#define glProgramUniform4i get_gl_entries ()->ProgramUniform4i
#define glProgramUniform4iv get_gl_entries ()->ProgramUniform4iv
#define glProgramUniform4f get_gl_entries ()->ProgramUniform4f
#define glProgramUniform4fv get_gl_entries ()->ProgramUniform4fv
#define glProgramUniform4d get_gl_entries ()->ProgramUniform4d
#define glProgramUniform4dv get_gl_entries ()->ProgramUniform4dv
#define glProgramUniform4ui get_gl_entries ()->ProgramUniform4ui
#define glProgramUniform4uiv get_gl_entries ()->ProgramUniform4uiv
#define glProgramUniformMatrix2fv get_gl_entries ()->ProgramUniformMatrix2fv
#define glProgramUniformMatrix3fv get_gl_entries ()->ProgramUniformMatrix3fv
#define glProgramUniformMatrix4fv get_gl_entries ()->ProgramUniformMatrix4fv
#define glProgramUniformMatrix2dv get_gl_entries ()->ProgramUniformMatrix2dv
#define glProgramUniformMatrix3dv get_gl_entries ()->ProgramUniformMatrix3dv
#define glProgramUniformMatrix4dv get_gl_entries ()->ProgramUniformMatrix4dv
#define glProgramUniformMatrix2x3fv get_gl_entries ()->ProgramUniformMatrix2x3fv
#define glProgramUniformMatrix3x2fv get_gl_entries ()->ProgramUniformMatrix3x2fv
#define glProgramUniformMatrix2x4fv get_gl_entries ()->ProgramUniformMatrix2x4fv
#define glProgramUniformMatrix4x2fv get_gl_entries ()->ProgramUniformMatrix4x2fv
#define glProgramUniformMatrix3x4fv get_gl_entries ()->ProgramUniformMatrix3x4fv
#define glProgramUniformMatrix4x3fv get_gl_entries ()->ProgramUniformMatrix4x3fv
#define glProgramUniformMatrix2x3dv get_gl_entries ()->ProgramUniformMatrix2x3dv
#define glProgramUniformMatrix3x2dv get_gl_entries ()->ProgramUniformMatrix3x2dv
#define glProgramUniformMatrix2x4dv get_gl_entries ()->ProgramUniformMatrix2x4dv
#define glProgramUniformMatrix4x2dv get_gl_entries ()->ProgramUniformMatrix4x2dv
#define glProgramUniformMatrix3x4dv get_gl_entries ()->ProgramUniformMatrix3x4dv
#define glProgramUniformMatrix4x3dv get_gl_entries ()->ProgramUniformMatrix4x3dv
#define glValidateProgramPipeline get_gl_entries ()->ValidateProgramPipeline
#define glGetProgramPipelineInfoLog get_gl_entries ()->GetProgramPipelineInfoLog
#define glVertexAttribL1d get_gl_entries ()->VertexAttribL1d
#define glVertexAttribL2d get_gl_entries ()->VertexAttribL2d
#define glVertexAttribL3d get_gl_entries ()->VertexAttribL3d
#define glVertexAttribL4d get_gl_entries ()->VertexAttribL4d
#define glVertexAttribL1dv get_gl_entries ()->VertexAttribL1dv
#define glVertexAttribL2dv get_gl_entries ()->VertexAttribL2dv
#define glVertexAttribL3dv get_gl_entries ()->VertexAttribL3dv
#define glVertexAttribL4dv get_gl_entries ()->VertexAttribL4dv
#define glVertexAttribLPointer get_gl_entries ()->VertexAttribLPointer
#define glGetVertexAttribLdv get_gl_entries ()->GetVertexAttribLdv
#define glViewportArrayv get_gl_entries ()->ViewportArrayv
#define glViewportIndexedf get_gl_entries ()->ViewportIndexedf
#define glViewportIndexedfv get_gl_entries ()->ViewportIndexedfv
#define glScissorArrayv get_gl_entries ()->ScissorArrayv
#define glScissorIndexed get_gl_entries ()->ScissorIndexed
#define glScissorIndexedv get_gl_entries ()->ScissorIndexedv
#define glDepthRangeArrayv get_gl_entries ()->DepthRangeArrayv
#define glDepthRangeIndexed get_gl_entries ()->DepthRangeIndexed
#define glGetFloati_v get_gl_entries ()->GetFloati_v
#define glGetDoublei_v get_gl_entries ()->GetDoublei_v
#define glCreateSyncFromCLeventARB get_gl_entries ()->CreateSyncFromCLeventARB
#define glDebugMessageControlARB get_gl_entries ()->DebugMessageControlARB
#define glDebugMessageInsertARB get_gl_entries ()->DebugMessageInsertARB
#define glDebugMessageCallbackARB get_gl_entries ()->DebugMessageCallbackARB
#define glGetDebugMessageLogARB get_gl_entries ()->GetDebugMessageLogARB
#define glGetGraphicsResetStatusARB get_gl_entries ()->GetGraphicsResetStatusARB
#define glGetnMapdvARB get_gl_entries ()->GetnMapdvARB
#define glGetnMapfvARB get_gl_entries ()->GetnMapfvARB
#define glGetnMapivARB get_gl_entries ()->GetnMapivARB
#define glGetnPixelMapfvARB get_gl_entries ()->GetnPixelMapfvARB
#define glGetnPixelMapuivARB get_gl_entries ()->GetnPixelMapuivARB
#define glGetnPixelMapusvARB get_gl_entries ()->GetnPixelMapusvARB
#define glGetnPolygonStippleARB get_gl_entries ()->GetnPolygonStippleARB
#define glGetnColorTableARB get_gl_entries ()->GetnColorTableARB
#define glGetnConvolutionFilterARB get_gl_entries ()->GetnConvolutionFilterARB
#define glGetnSeparableFilterARB get_gl_entries ()->GetnSeparableFilterARB
#define glGetnHistogramARB get_gl_entries ()->GetnHistogramARB
#define glGetnMinmaxARB get_gl_entries ()->GetnMinmaxARB
#define glGetnTexImageARB get_gl_entries ()->GetnTexImageARB
#define glReadnPixelsARB get_gl_entries ()->ReadnPixelsARB
#define glGetnCompressedTexImageARB get_gl_entries ()->GetnCompressedTexImageARB
#define glGetnUniformfvARB get_gl_entries ()->GetnUniformfvARB
#define glGetnUniformivARB get_gl_entries ()->GetnUniformivARB
#define glGetnUniformuivARB get_gl_entries ()->GetnUniformuivARB
#define glGetnUniformdvARB get_gl_entries ()->GetnUniformdvARB
#define glDrawArraysInstancedBaseInstance get_gl_entries ()->DrawArraysInstancedBaseInstance
#define glDrawElementsInstancedBaseInstance get_gl_entries ()->DrawElementsInstancedBaseInstance
#define glDrawElementsInstancedBaseVertexBaseInstance get_gl_entries ()->DrawElementsInstancedBaseVertexBaseInstance
#define glDrawTransformFeedbackInstanced get_gl_entries ()->DrawTransformFeedbackInstanced
#define glDrawTransformFeedbackStreamInstanced get_gl_entries ()->DrawTransformFeedbackStreamInstanced
#define glGetInternalformativ get_gl_entries ()->GetInternalformativ
#define glGetActiveAtomicCounterBufferiv get_gl_entries ()->GetActiveAtomicCounterBufferiv
#define glBindImageTexture get_gl_entries ()->BindImageTexture
#define glMemoryBarrier get_gl_entries ()->MemoryBarrier
#define glTexStorage1D get_gl_entries ()->TexStorage1D
#define glTexStorage2D get_gl_entries ()->TexStorage2D
#define glTexStorage3D get_gl_entries ()->TexStorage3D
#define glTextureStorage1DEXT get_gl_entries ()->TextureStorage1DEXT
#define glTextureStorage2DEXT get_gl_entries ()->TextureStorage2DEXT
#define glTextureStorage3DEXT get_gl_entries ()->TextureStorage3DEXT
#define glDebugMessageControl get_gl_entries ()->DebugMessageControl
#define glDebugMessageInsert get_gl_entries ()->DebugMessageInsert
#define glDebugMessageCallback get_gl_entries ()->DebugMessageCallback
#define glGetDebugMessageLog get_gl_entries ()->GetDebugMessageLog
#define glPushDebugGroup get_gl_entries ()->PushDebugGroup
#define glPopDebugGroup get_gl_entries ()->PopDebugGroup
#define glObjectLabel get_gl_entries ()->ObjectLabel
#define glGetObjectLabel get_gl_entries ()->GetObjectLabel
#define glObjectPtrLabel get_gl_entries ()->ObjectPtrLabel
#define glGetObjectPtrLabel get_gl_entries ()->GetObjectPtrLabel
#define glClearBufferData get_gl_entries ()->ClearBufferData
#define glClearBufferSubData get_gl_entries ()->ClearBufferSubData
#define glClearNamedBufferDataEXT get_gl_entries ()->ClearNamedBufferDataEXT
#define glClearNamedBufferSubDataEXT get_gl_entries ()->ClearNamedBufferSubDataEXT
#define glDispatchCompute get_gl_entries ()->DispatchCompute
#define glDispatchComputeIndirect get_gl_entries ()->DispatchComputeIndirect
#define glCopyImageSubData get_gl_entries ()->CopyImageSubData
#define glTextureView get_gl_entries ()->TextureView
#define glBindVertexBuffer get_gl_entries ()->BindVertexBuffer
#define glVertexAttribFormat get_gl_entries ()->VertexAttribFormat
#define glVertexAttribIFormat get_gl_entries ()->VertexAttribIFormat
#define glVertexAttribLFormat get_gl_entries ()->VertexAttribLFormat
#define glVertexAttribBinding get_gl_entries ()->VertexAttribBinding
#define glVertexBindingDivisor get_gl_entries ()->VertexBindingDivisor
#define glVertexArrayBindVertexBufferEXT get_gl_entries ()->VertexArrayBindVertexBufferEXT
#define glVertexArrayVertexAttribFormatEXT get_gl_entries ()->VertexArrayVertexAttribFormatEXT
#define glVertexArrayVertexAttribIFormatEXT get_gl_entries ()->VertexArrayVertexAttribIFormatEXT
#define glVertexArrayVertexAttribLFormatEXT get_gl_entries ()->VertexArrayVertexAttribLFormatEXT
#define glVertexArrayVertexAttribBindingEXT get_gl_entries ()->VertexArrayVertexAttribBindingEXT
#define glVertexArrayVertexBindingDivisorEXT get_gl_entries ()->VertexArrayVertexBindingDivisorEXT
#define glFramebufferParameteri get_gl_entries ()->FramebufferParameteri
#define glGetFramebufferParameteriv get_gl_entries ()->GetFramebufferParameteriv
#define glNamedFramebufferParameteriEXT get_gl_entries ()->NamedFramebufferParameteriEXT
#define glGetNamedFramebufferParameterivEXT get_gl_entries ()->GetNamedFramebufferParameterivEXT
#define glGetInternalformati64v get_gl_entries ()->GetInternalformati64v
#define glInvalidateTexSubImage get_gl_entries ()->InvalidateTexSubImage
#define glInvalidateTexImage get_gl_entries ()->InvalidateTexImage
#define glInvalidateBufferSubData get_gl_entries ()->InvalidateBufferSubData
#define glInvalidateBufferData get_gl_entries ()->InvalidateBufferData
#define glInvalidateFramebuffer get_gl_entries ()->InvalidateFramebuffer
#define glInvalidateSubFramebuffer get_gl_entries ()->InvalidateSubFramebuffer
#define glMultiDrawArraysIndirect get_gl_entries ()->MultiDrawArraysIndirect
#define glMultiDrawElementsIndirect get_gl_entries ()->MultiDrawElementsIndirect
#define glGetProgramInterfaceiv get_gl_entries ()->GetProgramInterfaceiv
#define glGetProgramResourceIndex get_gl_entries ()->GetProgramResourceIndex
#define glGetProgramResourceName get_gl_entries ()->GetProgramResourceName
#define glGetProgramResourceiv get_gl_entries ()->GetProgramResourceiv
#define glGetProgramResourceLocation get_gl_entries ()->GetProgramResourceLocation
#define glGetProgramResourceLocationIndex get_gl_entries ()->GetProgramResourceLocationIndex
#define glShaderStorageBlockBinding get_gl_entries ()->ShaderStorageBlockBinding
#define glTexBufferRange get_gl_entries ()->TexBufferRange
#define glTextureBufferRangeEXT get_gl_entries ()->TextureBufferRangeEXT
#define glTexStorage2DMultisample get_gl_entries ()->TexStorage2DMultisample
#define glTexStorage3DMultisample get_gl_entries ()->TexStorage3DMultisample
#define glTextureStorage2DMultisampleEXT get_gl_entries ()->TextureStorage2DMultisampleEXT
#define glTextureStorage3DMultisampleEXT get_gl_entries ()->TextureStorage3DMultisampleEXT
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
#define glGetPixelTransformParameterivEXT get_gl_entries ()->GetPixelTransformParameterivEXT
#define glGetPixelTransformParameterfvEXT get_gl_entries ()->GetPixelTransformParameterfvEXT
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
#define glTransformFeedbackStreamAttribsNV get_gl_entries ()->TransformFeedbackStreamAttribsNV
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
#define glBeginConditionalRenderNV get_gl_entries ()->BeginConditionalRenderNV
#define glEndConditionalRenderNV get_gl_entries ()->EndConditionalRenderNV
#define glPresentFrameKeyedNV get_gl_entries ()->PresentFrameKeyedNV
#define glPresentFrameDualFillNV get_gl_entries ()->PresentFrameDualFillNV
#define glGetVideoivNV get_gl_entries ()->GetVideoivNV
#define glGetVideouivNV get_gl_entries ()->GetVideouivNV
#define glGetVideoi64vNV get_gl_entries ()->GetVideoi64vNV
#define glGetVideoui64vNV get_gl_entries ()->GetVideoui64vNV
#define glBeginTransformFeedbackEXT get_gl_entries ()->BeginTransformFeedbackEXT
#define glEndTransformFeedbackEXT get_gl_entries ()->EndTransformFeedbackEXT
#define glBindBufferRangeEXT get_gl_entries ()->BindBufferRangeEXT
#define glBindBufferOffsetEXT get_gl_entries ()->BindBufferOffsetEXT
#define glBindBufferBaseEXT get_gl_entries ()->BindBufferBaseEXT
#define glTransformFeedbackVaryingsEXT get_gl_entries ()->TransformFeedbackVaryingsEXT
#define glGetTransformFeedbackVaryingEXT get_gl_entries ()->GetTransformFeedbackVaryingEXT
#define glClientAttribDefaultEXT get_gl_entries ()->ClientAttribDefaultEXT
#define glPushClientAttribDefaultEXT get_gl_entries ()->PushClientAttribDefaultEXT
#define glMatrixLoadfEXT get_gl_entries ()->MatrixLoadfEXT
#define glMatrixLoaddEXT get_gl_entries ()->MatrixLoaddEXT
#define glMatrixMultfEXT get_gl_entries ()->MatrixMultfEXT
#define glMatrixMultdEXT get_gl_entries ()->MatrixMultdEXT
#define glMatrixLoadIdentityEXT get_gl_entries ()->MatrixLoadIdentityEXT
#define glMatrixRotatefEXT get_gl_entries ()->MatrixRotatefEXT
#define glMatrixRotatedEXT get_gl_entries ()->MatrixRotatedEXT
#define glMatrixScalefEXT get_gl_entries ()->MatrixScalefEXT
#define glMatrixScaledEXT get_gl_entries ()->MatrixScaledEXT
#define glMatrixTranslatefEXT get_gl_entries ()->MatrixTranslatefEXT
#define glMatrixTranslatedEXT get_gl_entries ()->MatrixTranslatedEXT
#define glMatrixFrustumEXT get_gl_entries ()->MatrixFrustumEXT
#define glMatrixOrthoEXT get_gl_entries ()->MatrixOrthoEXT
#define glMatrixPopEXT get_gl_entries ()->MatrixPopEXT
#define glMatrixPushEXT get_gl_entries ()->MatrixPushEXT
#define glMatrixLoadTransposefEXT get_gl_entries ()->MatrixLoadTransposefEXT
#define glMatrixLoadTransposedEXT get_gl_entries ()->MatrixLoadTransposedEXT
#define glMatrixMultTransposefEXT get_gl_entries ()->MatrixMultTransposefEXT
#define glMatrixMultTransposedEXT get_gl_entries ()->MatrixMultTransposedEXT
#define glTextureParameterfEXT get_gl_entries ()->TextureParameterfEXT
#define glTextureParameterfvEXT get_gl_entries ()->TextureParameterfvEXT
#define glTextureParameteriEXT get_gl_entries ()->TextureParameteriEXT
#define glTextureParameterivEXT get_gl_entries ()->TextureParameterivEXT
#define glTextureImage1DEXT get_gl_entries ()->TextureImage1DEXT
#define glTextureImage2DEXT get_gl_entries ()->TextureImage2DEXT
#define glTextureSubImage1DEXT get_gl_entries ()->TextureSubImage1DEXT
#define glTextureSubImage2DEXT get_gl_entries ()->TextureSubImage2DEXT
#define glCopyTextureImage1DEXT get_gl_entries ()->CopyTextureImage1DEXT
#define glCopyTextureImage2DEXT get_gl_entries ()->CopyTextureImage2DEXT
#define glCopyTextureSubImage1DEXT get_gl_entries ()->CopyTextureSubImage1DEXT
#define glCopyTextureSubImage2DEXT get_gl_entries ()->CopyTextureSubImage2DEXT
#define glGetTextureImageEXT get_gl_entries ()->GetTextureImageEXT
#define glGetTextureParameterfvEXT get_gl_entries ()->GetTextureParameterfvEXT
#define glGetTextureParameterivEXT get_gl_entries ()->GetTextureParameterivEXT
#define glGetTextureLevelParameterfvEXT get_gl_entries ()->GetTextureLevelParameterfvEXT
#define glGetTextureLevelParameterivEXT get_gl_entries ()->GetTextureLevelParameterivEXT
#define glTextureImage3DEXT get_gl_entries ()->TextureImage3DEXT
#define glTextureSubImage3DEXT get_gl_entries ()->TextureSubImage3DEXT
#define glCopyTextureSubImage3DEXT get_gl_entries ()->CopyTextureSubImage3DEXT
#define glMultiTexParameterfEXT get_gl_entries ()->MultiTexParameterfEXT
#define glMultiTexParameterfvEXT get_gl_entries ()->MultiTexParameterfvEXT
#define glMultiTexParameteriEXT get_gl_entries ()->MultiTexParameteriEXT
#define glMultiTexParameterivEXT get_gl_entries ()->MultiTexParameterivEXT
#define glMultiTexImage1DEXT get_gl_entries ()->MultiTexImage1DEXT
#define glMultiTexImage2DEXT get_gl_entries ()->MultiTexImage2DEXT
#define glMultiTexSubImage1DEXT get_gl_entries ()->MultiTexSubImage1DEXT
#define glMultiTexSubImage2DEXT get_gl_entries ()->MultiTexSubImage2DEXT
#define glCopyMultiTexImage1DEXT get_gl_entries ()->CopyMultiTexImage1DEXT
#define glCopyMultiTexImage2DEXT get_gl_entries ()->CopyMultiTexImage2DEXT
#define glCopyMultiTexSubImage1DEXT get_gl_entries ()->CopyMultiTexSubImage1DEXT
#define glCopyMultiTexSubImage2DEXT get_gl_entries ()->CopyMultiTexSubImage2DEXT
#define glGetMultiTexImageEXT get_gl_entries ()->GetMultiTexImageEXT
#define glGetMultiTexParameterfvEXT get_gl_entries ()->GetMultiTexParameterfvEXT
#define glGetMultiTexParameterivEXT get_gl_entries ()->GetMultiTexParameterivEXT
#define glGetMultiTexLevelParameterfvEXT get_gl_entries ()->GetMultiTexLevelParameterfvEXT
#define glGetMultiTexLevelParameterivEXT get_gl_entries ()->GetMultiTexLevelParameterivEXT
#define glMultiTexImage3DEXT get_gl_entries ()->MultiTexImage3DEXT
#define glMultiTexSubImage3DEXT get_gl_entries ()->MultiTexSubImage3DEXT
#define glCopyMultiTexSubImage3DEXT get_gl_entries ()->CopyMultiTexSubImage3DEXT
#define glBindMultiTextureEXT get_gl_entries ()->BindMultiTextureEXT
#define glEnableClientStateIndexedEXT get_gl_entries ()->EnableClientStateIndexedEXT
#define glDisableClientStateIndexedEXT get_gl_entries ()->DisableClientStateIndexedEXT
#define glMultiTexCoordPointerEXT get_gl_entries ()->MultiTexCoordPointerEXT
#define glMultiTexEnvfEXT get_gl_entries ()->MultiTexEnvfEXT
#define glMultiTexEnvfvEXT get_gl_entries ()->MultiTexEnvfvEXT
#define glMultiTexEnviEXT get_gl_entries ()->MultiTexEnviEXT
#define glMultiTexEnvivEXT get_gl_entries ()->MultiTexEnvivEXT
#define glMultiTexGendEXT get_gl_entries ()->MultiTexGendEXT
#define glMultiTexGendvEXT get_gl_entries ()->MultiTexGendvEXT
#define glMultiTexGenfEXT get_gl_entries ()->MultiTexGenfEXT
#define glMultiTexGenfvEXT get_gl_entries ()->MultiTexGenfvEXT
#define glMultiTexGeniEXT get_gl_entries ()->MultiTexGeniEXT
#define glMultiTexGenivEXT get_gl_entries ()->MultiTexGenivEXT
#define glGetMultiTexEnvfvEXT get_gl_entries ()->GetMultiTexEnvfvEXT
#define glGetMultiTexEnvivEXT get_gl_entries ()->GetMultiTexEnvivEXT
#define glGetMultiTexGendvEXT get_gl_entries ()->GetMultiTexGendvEXT
#define glGetMultiTexGenfvEXT get_gl_entries ()->GetMultiTexGenfvEXT
#define glGetMultiTexGenivEXT get_gl_entries ()->GetMultiTexGenivEXT
#define glGetFloatIndexedvEXT get_gl_entries ()->GetFloatIndexedvEXT
#define glGetDoubleIndexedvEXT get_gl_entries ()->GetDoubleIndexedvEXT
#define glGetPointerIndexedvEXT get_gl_entries ()->GetPointerIndexedvEXT
#define glCompressedTextureImage3DEXT get_gl_entries ()->CompressedTextureImage3DEXT
#define glCompressedTextureImage2DEXT get_gl_entries ()->CompressedTextureImage2DEXT
#define glCompressedTextureImage1DEXT get_gl_entries ()->CompressedTextureImage1DEXT
#define glCompressedTextureSubImage3DEXT get_gl_entries ()->CompressedTextureSubImage3DEXT
#define glCompressedTextureSubImage2DEXT get_gl_entries ()->CompressedTextureSubImage2DEXT
#define glCompressedTextureSubImage1DEXT get_gl_entries ()->CompressedTextureSubImage1DEXT
#define glGetCompressedTextureImageEXT get_gl_entries ()->GetCompressedTextureImageEXT
#define glCompressedMultiTexImage3DEXT get_gl_entries ()->CompressedMultiTexImage3DEXT
#define glCompressedMultiTexImage2DEXT get_gl_entries ()->CompressedMultiTexImage2DEXT
#define glCompressedMultiTexImage1DEXT get_gl_entries ()->CompressedMultiTexImage1DEXT
#define glCompressedMultiTexSubImage3DEXT get_gl_entries ()->CompressedMultiTexSubImage3DEXT
#define glCompressedMultiTexSubImage2DEXT get_gl_entries ()->CompressedMultiTexSubImage2DEXT
#define glCompressedMultiTexSubImage1DEXT get_gl_entries ()->CompressedMultiTexSubImage1DEXT
#define glGetCompressedMultiTexImageEXT get_gl_entries ()->GetCompressedMultiTexImageEXT
#define glNamedProgramStringEXT get_gl_entries ()->NamedProgramStringEXT
#define glNamedProgramLocalParameter4dEXT get_gl_entries ()->NamedProgramLocalParameter4dEXT
#define glNamedProgramLocalParameter4dvEXT get_gl_entries ()->NamedProgramLocalParameter4dvEXT
#define glNamedProgramLocalParameter4fEXT get_gl_entries ()->NamedProgramLocalParameter4fEXT
#define glNamedProgramLocalParameter4fvEXT get_gl_entries ()->NamedProgramLocalParameter4fvEXT
#define glGetNamedProgramLocalParameterdvEXT get_gl_entries ()->GetNamedProgramLocalParameterdvEXT
#define glGetNamedProgramLocalParameterfvEXT get_gl_entries ()->GetNamedProgramLocalParameterfvEXT
#define glGetNamedProgramivEXT get_gl_entries ()->GetNamedProgramivEXT
#define glGetNamedProgramStringEXT get_gl_entries ()->GetNamedProgramStringEXT
#define glNamedProgramLocalParameters4fvEXT get_gl_entries ()->NamedProgramLocalParameters4fvEXT
#define glNamedProgramLocalParameterI4iEXT get_gl_entries ()->NamedProgramLocalParameterI4iEXT
#define glNamedProgramLocalParameterI4ivEXT get_gl_entries ()->NamedProgramLocalParameterI4ivEXT
#define glNamedProgramLocalParametersI4ivEXT get_gl_entries ()->NamedProgramLocalParametersI4ivEXT
#define glNamedProgramLocalParameterI4uiEXT get_gl_entries ()->NamedProgramLocalParameterI4uiEXT
#define glNamedProgramLocalParameterI4uivEXT get_gl_entries ()->NamedProgramLocalParameterI4uivEXT
#define glNamedProgramLocalParametersI4uivEXT get_gl_entries ()->NamedProgramLocalParametersI4uivEXT
#define glGetNamedProgramLocalParameterIivEXT get_gl_entries ()->GetNamedProgramLocalParameterIivEXT
#define glGetNamedProgramLocalParameterIuivEXT get_gl_entries ()->GetNamedProgramLocalParameterIuivEXT
#define glTextureParameterIivEXT get_gl_entries ()->TextureParameterIivEXT
#define glTextureParameterIuivEXT get_gl_entries ()->TextureParameterIuivEXT
#define glGetTextureParameterIivEXT get_gl_entries ()->GetTextureParameterIivEXT
#define glGetTextureParameterIuivEXT get_gl_entries ()->GetTextureParameterIuivEXT
#define glMultiTexParameterIivEXT get_gl_entries ()->MultiTexParameterIivEXT
#define glMultiTexParameterIuivEXT get_gl_entries ()->MultiTexParameterIuivEXT
#define glGetMultiTexParameterIivEXT get_gl_entries ()->GetMultiTexParameterIivEXT
#define glGetMultiTexParameterIuivEXT get_gl_entries ()->GetMultiTexParameterIuivEXT
#define glProgramUniform1fEXT get_gl_entries ()->ProgramUniform1fEXT
#define glProgramUniform2fEXT get_gl_entries ()->ProgramUniform2fEXT
#define glProgramUniform3fEXT get_gl_entries ()->ProgramUniform3fEXT
#define glProgramUniform4fEXT get_gl_entries ()->ProgramUniform4fEXT
#define glProgramUniform1iEXT get_gl_entries ()->ProgramUniform1iEXT
#define glProgramUniform2iEXT get_gl_entries ()->ProgramUniform2iEXT
#define glProgramUniform3iEXT get_gl_entries ()->ProgramUniform3iEXT
#define glProgramUniform4iEXT get_gl_entries ()->ProgramUniform4iEXT
#define glProgramUniform1fvEXT get_gl_entries ()->ProgramUniform1fvEXT
#define glProgramUniform2fvEXT get_gl_entries ()->ProgramUniform2fvEXT
#define glProgramUniform3fvEXT get_gl_entries ()->ProgramUniform3fvEXT
#define glProgramUniform4fvEXT get_gl_entries ()->ProgramUniform4fvEXT
#define glProgramUniform1ivEXT get_gl_entries ()->ProgramUniform1ivEXT
#define glProgramUniform2ivEXT get_gl_entries ()->ProgramUniform2ivEXT
#define glProgramUniform3ivEXT get_gl_entries ()->ProgramUniform3ivEXT
#define glProgramUniform4ivEXT get_gl_entries ()->ProgramUniform4ivEXT
#define glProgramUniformMatrix2fvEXT get_gl_entries ()->ProgramUniformMatrix2fvEXT
#define glProgramUniformMatrix3fvEXT get_gl_entries ()->ProgramUniformMatrix3fvEXT
#define glProgramUniformMatrix4fvEXT get_gl_entries ()->ProgramUniformMatrix4fvEXT
#define glProgramUniformMatrix2x3fvEXT get_gl_entries ()->ProgramUniformMatrix2x3fvEXT
#define glProgramUniformMatrix3x2fvEXT get_gl_entries ()->ProgramUniformMatrix3x2fvEXT
#define glProgramUniformMatrix2x4fvEXT get_gl_entries ()->ProgramUniformMatrix2x4fvEXT
#define glProgramUniformMatrix4x2fvEXT get_gl_entries ()->ProgramUniformMatrix4x2fvEXT
#define glProgramUniformMatrix3x4fvEXT get_gl_entries ()->ProgramUniformMatrix3x4fvEXT
#define glProgramUniformMatrix4x3fvEXT get_gl_entries ()->ProgramUniformMatrix4x3fvEXT
#define glProgramUniform1uiEXT get_gl_entries ()->ProgramUniform1uiEXT
#define glProgramUniform2uiEXT get_gl_entries ()->ProgramUniform2uiEXT
#define glProgramUniform3uiEXT get_gl_entries ()->ProgramUniform3uiEXT
#define glProgramUniform4uiEXT get_gl_entries ()->ProgramUniform4uiEXT
#define glProgramUniform1uivEXT get_gl_entries ()->ProgramUniform1uivEXT
#define glProgramUniform2uivEXT get_gl_entries ()->ProgramUniform2uivEXT
#define glProgramUniform3uivEXT get_gl_entries ()->ProgramUniform3uivEXT
#define glProgramUniform4uivEXT get_gl_entries ()->ProgramUniform4uivEXT
#define glNamedBufferDataEXT get_gl_entries ()->NamedBufferDataEXT
#define glNamedBufferSubDataEXT get_gl_entries ()->NamedBufferSubDataEXT
#define glMapNamedBufferEXT get_gl_entries ()->MapNamedBufferEXT
#define glUnmapNamedBufferEXT get_gl_entries ()->UnmapNamedBufferEXT
#define glMapNamedBufferRangeEXT get_gl_entries ()->MapNamedBufferRangeEXT
#define glFlushMappedNamedBufferRangeEXT get_gl_entries ()->FlushMappedNamedBufferRangeEXT
#define glNamedCopyBufferSubDataEXT get_gl_entries ()->NamedCopyBufferSubDataEXT
#define glGetNamedBufferParameterivEXT get_gl_entries ()->GetNamedBufferParameterivEXT
#define glGetNamedBufferPointervEXT get_gl_entries ()->GetNamedBufferPointervEXT
#define glGetNamedBufferSubDataEXT get_gl_entries ()->GetNamedBufferSubDataEXT
#define glTextureBufferEXT get_gl_entries ()->TextureBufferEXT
#define glMultiTexBufferEXT get_gl_entries ()->MultiTexBufferEXT
#define glNamedRenderbufferStorageEXT get_gl_entries ()->NamedRenderbufferStorageEXT
#define glGetNamedRenderbufferParameterivEXT get_gl_entries ()->GetNamedRenderbufferParameterivEXT
#define glCheckNamedFramebufferStatusEXT get_gl_entries ()->CheckNamedFramebufferStatusEXT
#define glNamedFramebufferTexture1DEXT get_gl_entries ()->NamedFramebufferTexture1DEXT
#define glNamedFramebufferTexture2DEXT get_gl_entries ()->NamedFramebufferTexture2DEXT
#define glNamedFramebufferTexture3DEXT get_gl_entries ()->NamedFramebufferTexture3DEXT
#define glNamedFramebufferRenderbufferEXT get_gl_entries ()->NamedFramebufferRenderbufferEXT
#define glGetNamedFramebufferAttachmentParameterivEXT get_gl_entries ()->GetNamedFramebufferAttachmentParameterivEXT
#define glGenerateTextureMipmapEXT get_gl_entries ()->GenerateTextureMipmapEXT
#define glGenerateMultiTexMipmapEXT get_gl_entries ()->GenerateMultiTexMipmapEXT
#define glFramebufferDrawBufferEXT get_gl_entries ()->FramebufferDrawBufferEXT
#define glFramebufferDrawBuffersEXT get_gl_entries ()->FramebufferDrawBuffersEXT
#define glFramebufferReadBufferEXT get_gl_entries ()->FramebufferReadBufferEXT
#define glGetFramebufferParameterivEXT get_gl_entries ()->GetFramebufferParameterivEXT
#define glNamedRenderbufferStorageMultisampleEXT get_gl_entries ()->NamedRenderbufferStorageMultisampleEXT
#define glNamedRenderbufferStorageMultisampleCoverageEXT get_gl_entries ()->NamedRenderbufferStorageMultisampleCoverageEXT
#define glNamedFramebufferTextureEXT get_gl_entries ()->NamedFramebufferTextureEXT
#define glNamedFramebufferTextureLayerEXT get_gl_entries ()->NamedFramebufferTextureLayerEXT
#define glNamedFramebufferTextureFaceEXT get_gl_entries ()->NamedFramebufferTextureFaceEXT
#define glTextureRenderbufferEXT get_gl_entries ()->TextureRenderbufferEXT
#define glMultiTexRenderbufferEXT get_gl_entries ()->MultiTexRenderbufferEXT
#define glProgramUniform1dEXT get_gl_entries ()->ProgramUniform1dEXT
#define glProgramUniform2dEXT get_gl_entries ()->ProgramUniform2dEXT
#define glProgramUniform3dEXT get_gl_entries ()->ProgramUniform3dEXT
#define glProgramUniform4dEXT get_gl_entries ()->ProgramUniform4dEXT
#define glProgramUniform1dvEXT get_gl_entries ()->ProgramUniform1dvEXT
#define glProgramUniform2dvEXT get_gl_entries ()->ProgramUniform2dvEXT
#define glProgramUniform3dvEXT get_gl_entries ()->ProgramUniform3dvEXT
#define glProgramUniform4dvEXT get_gl_entries ()->ProgramUniform4dvEXT
#define glProgramUniformMatrix2dvEXT get_gl_entries ()->ProgramUniformMatrix2dvEXT
#define glProgramUniformMatrix3dvEXT get_gl_entries ()->ProgramUniformMatrix3dvEXT
#define glProgramUniformMatrix4dvEXT get_gl_entries ()->ProgramUniformMatrix4dvEXT
#define glProgramUniformMatrix2x3dvEXT get_gl_entries ()->ProgramUniformMatrix2x3dvEXT
#define glProgramUniformMatrix2x4dvEXT get_gl_entries ()->ProgramUniformMatrix2x4dvEXT
#define glProgramUniformMatrix3x2dvEXT get_gl_entries ()->ProgramUniformMatrix3x2dvEXT
#define glProgramUniformMatrix3x4dvEXT get_gl_entries ()->ProgramUniformMatrix3x4dvEXT
#define glProgramUniformMatrix4x2dvEXT get_gl_entries ()->ProgramUniformMatrix4x2dvEXT
#define glProgramUniformMatrix4x3dvEXT get_gl_entries ()->ProgramUniformMatrix4x3dvEXT
#define glGetMultisamplefvNV get_gl_entries ()->GetMultisamplefvNV
#define glSampleMaskIndexedNV get_gl_entries ()->SampleMaskIndexedNV
#define glTexRenderbufferNV get_gl_entries ()->TexRenderbufferNV
#define glBindTransformFeedbackNV get_gl_entries ()->BindTransformFeedbackNV
#define glDeleteTransformFeedbacksNV get_gl_entries ()->DeleteTransformFeedbacksNV
#define glGenTransformFeedbacksNV get_gl_entries ()->GenTransformFeedbacksNV
#define glIsTransformFeedbackNV get_gl_entries ()->IsTransformFeedbackNV
#define glPauseTransformFeedbackNV get_gl_entries ()->PauseTransformFeedbackNV
#define glResumeTransformFeedbackNV get_gl_entries ()->ResumeTransformFeedbackNV
#define glDrawTransformFeedbackNV get_gl_entries ()->DrawTransformFeedbackNV
#define glGetPerfMonitorGroupsAMD get_gl_entries ()->GetPerfMonitorGroupsAMD
#define glGetPerfMonitorCountersAMD get_gl_entries ()->GetPerfMonitorCountersAMD
#define glGetPerfMonitorGroupStringAMD get_gl_entries ()->GetPerfMonitorGroupStringAMD
#define glGetPerfMonitorCounterStringAMD get_gl_entries ()->GetPerfMonitorCounterStringAMD
#define glGetPerfMonitorCounterInfoAMD get_gl_entries ()->GetPerfMonitorCounterInfoAMD
#define glGenPerfMonitorsAMD get_gl_entries ()->GenPerfMonitorsAMD
#define glDeletePerfMonitorsAMD get_gl_entries ()->DeletePerfMonitorsAMD
#define glSelectPerfMonitorCountersAMD get_gl_entries ()->SelectPerfMonitorCountersAMD
#define glBeginPerfMonitorAMD get_gl_entries ()->BeginPerfMonitorAMD
#define glEndPerfMonitorAMD get_gl_entries ()->EndPerfMonitorAMD
#define glGetPerfMonitorCounterDataAMD get_gl_entries ()->GetPerfMonitorCounterDataAMD
#define glTessellationFactorAMD get_gl_entries ()->TessellationFactorAMD
#define glTessellationModeAMD get_gl_entries ()->TessellationModeAMD
#define glProvokingVertexEXT get_gl_entries ()->ProvokingVertexEXT
#define glBlendFuncIndexedAMD get_gl_entries ()->BlendFuncIndexedAMD
#define glBlendFuncSeparateIndexedAMD get_gl_entries ()->BlendFuncSeparateIndexedAMD
#define glBlendEquationIndexedAMD get_gl_entries ()->BlendEquationIndexedAMD
#define glBlendEquationSeparateIndexedAMD get_gl_entries ()->BlendEquationSeparateIndexedAMD
#define glTextureRangeAPPLE get_gl_entries ()->TextureRangeAPPLE
#define glGetTexParameterPointervAPPLE get_gl_entries ()->GetTexParameterPointervAPPLE
#define glEnableVertexAttribAPPLE get_gl_entries ()->EnableVertexAttribAPPLE
#define glDisableVertexAttribAPPLE get_gl_entries ()->DisableVertexAttribAPPLE
#define glIsVertexAttribEnabledAPPLE get_gl_entries ()->IsVertexAttribEnabledAPPLE
#define glMapVertexAttrib1dAPPLE get_gl_entries ()->MapVertexAttrib1dAPPLE
#define glMapVertexAttrib1fAPPLE get_gl_entries ()->MapVertexAttrib1fAPPLE
#define glMapVertexAttrib2dAPPLE get_gl_entries ()->MapVertexAttrib2dAPPLE
#define glMapVertexAttrib2fAPPLE get_gl_entries ()->MapVertexAttrib2fAPPLE
#define glObjectPurgeableAPPLE get_gl_entries ()->ObjectPurgeableAPPLE
#define glObjectUnpurgeableAPPLE get_gl_entries ()->ObjectUnpurgeableAPPLE
#define glGetObjectParameterivAPPLE get_gl_entries ()->GetObjectParameterivAPPLE
#define glBeginVideoCaptureNV get_gl_entries ()->BeginVideoCaptureNV
#define glBindVideoCaptureStreamBufferNV get_gl_entries ()->BindVideoCaptureStreamBufferNV
#define glBindVideoCaptureStreamTextureNV get_gl_entries ()->BindVideoCaptureStreamTextureNV
#define glEndVideoCaptureNV get_gl_entries ()->EndVideoCaptureNV
#define glGetVideoCaptureivNV get_gl_entries ()->GetVideoCaptureivNV
#define glGetVideoCaptureStreamivNV get_gl_entries ()->GetVideoCaptureStreamivNV
#define glGetVideoCaptureStreamfvNV get_gl_entries ()->GetVideoCaptureStreamfvNV
#define glGetVideoCaptureStreamdvNV get_gl_entries ()->GetVideoCaptureStreamdvNV
#define glVideoCaptureNV get_gl_entries ()->VideoCaptureNV
#define glVideoCaptureStreamParameterivNV get_gl_entries ()->VideoCaptureStreamParameterivNV
#define glVideoCaptureStreamParameterfvNV get_gl_entries ()->VideoCaptureStreamParameterfvNV
#define glVideoCaptureStreamParameterdvNV get_gl_entries ()->VideoCaptureStreamParameterdvNV
#define glCopyImageSubDataNV get_gl_entries ()->CopyImageSubDataNV
#define glUseShaderProgramEXT get_gl_entries ()->UseShaderProgramEXT
#define glActiveProgramEXT get_gl_entries ()->ActiveProgramEXT
#define glCreateShaderProgramEXT get_gl_entries ()->CreateShaderProgramEXT
#define glMakeBufferResidentNV get_gl_entries ()->MakeBufferResidentNV
#define glMakeBufferNonResidentNV get_gl_entries ()->MakeBufferNonResidentNV
#define glIsBufferResidentNV get_gl_entries ()->IsBufferResidentNV
#define glMakeNamedBufferResidentNV get_gl_entries ()->MakeNamedBufferResidentNV
#define glMakeNamedBufferNonResidentNV get_gl_entries ()->MakeNamedBufferNonResidentNV
#define glIsNamedBufferResidentNV get_gl_entries ()->IsNamedBufferResidentNV
#define glGetBufferParameterui64vNV get_gl_entries ()->GetBufferParameterui64vNV
#define glGetNamedBufferParameterui64vNV get_gl_entries ()->GetNamedBufferParameterui64vNV
#define glGetIntegerui64vNV get_gl_entries ()->GetIntegerui64vNV
#define glUniformui64NV get_gl_entries ()->Uniformui64NV
#define glUniformui64vNV get_gl_entries ()->Uniformui64vNV
#define glGetUniformui64vNV get_gl_entries ()->GetUniformui64vNV
#define glProgramUniformui64NV get_gl_entries ()->ProgramUniformui64NV
#define glProgramUniformui64vNV get_gl_entries ()->ProgramUniformui64vNV
#define glBufferAddressRangeNV get_gl_entries ()->BufferAddressRangeNV
#define glVertexFormatNV get_gl_entries ()->VertexFormatNV
#define glNormalFormatNV get_gl_entries ()->NormalFormatNV
#define glColorFormatNV get_gl_entries ()->ColorFormatNV
#define glIndexFormatNV get_gl_entries ()->IndexFormatNV
#define glTexCoordFormatNV get_gl_entries ()->TexCoordFormatNV
#define glEdgeFlagFormatNV get_gl_entries ()->EdgeFlagFormatNV
#define glSecondaryColorFormatNV get_gl_entries ()->SecondaryColorFormatNV
#define glFogCoordFormatNV get_gl_entries ()->FogCoordFormatNV
#define glVertexAttribFormatNV get_gl_entries ()->VertexAttribFormatNV
#define glVertexAttribIFormatNV get_gl_entries ()->VertexAttribIFormatNV
#define glGetIntegerui64i_vNV get_gl_entries ()->GetIntegerui64i_vNV
#define glTextureBarrierNV get_gl_entries ()->TextureBarrierNV
#define glBindImageTextureEXT get_gl_entries ()->BindImageTextureEXT
#define glMemoryBarrierEXT get_gl_entries ()->MemoryBarrierEXT
#define glVertexAttribL1dEXT get_gl_entries ()->VertexAttribL1dEXT
#define glVertexAttribL2dEXT get_gl_entries ()->VertexAttribL2dEXT
#define glVertexAttribL3dEXT get_gl_entries ()->VertexAttribL3dEXT
#define glVertexAttribL4dEXT get_gl_entries ()->VertexAttribL4dEXT
#define glVertexAttribL1dvEXT get_gl_entries ()->VertexAttribL1dvEXT
#define glVertexAttribL2dvEXT get_gl_entries ()->VertexAttribL2dvEXT
#define glVertexAttribL3dvEXT get_gl_entries ()->VertexAttribL3dvEXT
#define glVertexAttribL4dvEXT get_gl_entries ()->VertexAttribL4dvEXT
#define glVertexAttribLPointerEXT get_gl_entries ()->VertexAttribLPointerEXT
#define glGetVertexAttribLdvEXT get_gl_entries ()->GetVertexAttribLdvEXT
#define glVertexArrayVertexAttribLOffsetEXT get_gl_entries ()->VertexArrayVertexAttribLOffsetEXT
#define glProgramSubroutineParametersuivNV get_gl_entries ()->ProgramSubroutineParametersuivNV
#define glGetProgramSubroutineParameteruivNV get_gl_entries ()->GetProgramSubroutineParameteruivNV
#define glUniform1i64NV get_gl_entries ()->Uniform1i64NV
#define glUniform2i64NV get_gl_entries ()->Uniform2i64NV
#define glUniform3i64NV get_gl_entries ()->Uniform3i64NV
#define glUniform4i64NV get_gl_entries ()->Uniform4i64NV
#define glUniform1i64vNV get_gl_entries ()->Uniform1i64vNV
#define glUniform2i64vNV get_gl_entries ()->Uniform2i64vNV
#define glUniform3i64vNV get_gl_entries ()->Uniform3i64vNV
#define glUniform4i64vNV get_gl_entries ()->Uniform4i64vNV
#define glUniform1ui64NV get_gl_entries ()->Uniform1ui64NV
#define glUniform2ui64NV get_gl_entries ()->Uniform2ui64NV
#define glUniform3ui64NV get_gl_entries ()->Uniform3ui64NV
#define glUniform4ui64NV get_gl_entries ()->Uniform4ui64NV
#define glUniform1ui64vNV get_gl_entries ()->Uniform1ui64vNV
#define glUniform2ui64vNV get_gl_entries ()->Uniform2ui64vNV
#define glUniform3ui64vNV get_gl_entries ()->Uniform3ui64vNV
#define glUniform4ui64vNV get_gl_entries ()->Uniform4ui64vNV
#define glGetUniformi64vNV get_gl_entries ()->GetUniformi64vNV
#define glProgramUniform1i64NV get_gl_entries ()->ProgramUniform1i64NV
#define glProgramUniform2i64NV get_gl_entries ()->ProgramUniform2i64NV
#define glProgramUniform3i64NV get_gl_entries ()->ProgramUniform3i64NV
#define glProgramUniform4i64NV get_gl_entries ()->ProgramUniform4i64NV
#define glProgramUniform1i64vNV get_gl_entries ()->ProgramUniform1i64vNV
#define glProgramUniform2i64vNV get_gl_entries ()->ProgramUniform2i64vNV
#define glProgramUniform3i64vNV get_gl_entries ()->ProgramUniform3i64vNV
#define glProgramUniform4i64vNV get_gl_entries ()->ProgramUniform4i64vNV
#define glProgramUniform1ui64NV get_gl_entries ()->ProgramUniform1ui64NV
#define glProgramUniform2ui64NV get_gl_entries ()->ProgramUniform2ui64NV
#define glProgramUniform3ui64NV get_gl_entries ()->ProgramUniform3ui64NV
#define glProgramUniform4ui64NV get_gl_entries ()->ProgramUniform4ui64NV
#define glProgramUniform1ui64vNV get_gl_entries ()->ProgramUniform1ui64vNV
#define glProgramUniform2ui64vNV get_gl_entries ()->ProgramUniform2ui64vNV
#define glProgramUniform3ui64vNV get_gl_entries ()->ProgramUniform3ui64vNV
#define glProgramUniform4ui64vNV get_gl_entries ()->ProgramUniform4ui64vNV
#define glVertexAttribL1i64NV get_gl_entries ()->VertexAttribL1i64NV
#define glVertexAttribL2i64NV get_gl_entries ()->VertexAttribL2i64NV
#define glVertexAttribL3i64NV get_gl_entries ()->VertexAttribL3i64NV
#define glVertexAttribL4i64NV get_gl_entries ()->VertexAttribL4i64NV
#define glVertexAttribL1i64vNV get_gl_entries ()->VertexAttribL1i64vNV
#define glVertexAttribL2i64vNV get_gl_entries ()->VertexAttribL2i64vNV
#define glVertexAttribL3i64vNV get_gl_entries ()->VertexAttribL3i64vNV
#define glVertexAttribL4i64vNV get_gl_entries ()->VertexAttribL4i64vNV
#define glVertexAttribL1ui64NV get_gl_entries ()->VertexAttribL1ui64NV
#define glVertexAttribL2ui64NV get_gl_entries ()->VertexAttribL2ui64NV
#define glVertexAttribL3ui64NV get_gl_entries ()->VertexAttribL3ui64NV
#define glVertexAttribL4ui64NV get_gl_entries ()->VertexAttribL4ui64NV
#define glVertexAttribL1ui64vNV get_gl_entries ()->VertexAttribL1ui64vNV
#define glVertexAttribL2ui64vNV get_gl_entries ()->VertexAttribL2ui64vNV
#define glVertexAttribL3ui64vNV get_gl_entries ()->VertexAttribL3ui64vNV
#define glVertexAttribL4ui64vNV get_gl_entries ()->VertexAttribL4ui64vNV
#define glGetVertexAttribLi64vNV get_gl_entries ()->GetVertexAttribLi64vNV
#define glGetVertexAttribLui64vNV get_gl_entries ()->GetVertexAttribLui64vNV
#define glVertexAttribLFormatNV get_gl_entries ()->VertexAttribLFormatNV
#define glGenNamesAMD get_gl_entries ()->GenNamesAMD
#define glDeleteNamesAMD get_gl_entries ()->DeleteNamesAMD
#define glIsNameAMD get_gl_entries ()->IsNameAMD
#define glDebugMessageEnableAMD get_gl_entries ()->DebugMessageEnableAMD
#define glDebugMessageInsertAMD get_gl_entries ()->DebugMessageInsertAMD
#define glDebugMessageCallbackAMD get_gl_entries ()->DebugMessageCallbackAMD
#define glGetDebugMessageLogAMD get_gl_entries ()->GetDebugMessageLogAMD
#define glVDPAUInitNV get_gl_entries ()->VDPAUInitNV
#define glVDPAUFiniNV get_gl_entries ()->VDPAUFiniNV
#define glVDPAURegisterVideoSurfaceNV get_gl_entries ()->VDPAURegisterVideoSurfaceNV
#define glVDPAURegisterOutputSurfaceNV get_gl_entries ()->VDPAURegisterOutputSurfaceNV
#define glVDPAUIsSurfaceNV get_gl_entries ()->VDPAUIsSurfaceNV
#define glVDPAUUnregisterSurfaceNV get_gl_entries ()->VDPAUUnregisterSurfaceNV
#define glVDPAUGetSurfaceivNV get_gl_entries ()->VDPAUGetSurfaceivNV
#define glVDPAUSurfaceAccessNV get_gl_entries ()->VDPAUSurfaceAccessNV
#define glVDPAUMapSurfacesNV get_gl_entries ()->VDPAUMapSurfacesNV
#define glVDPAUUnmapSurfacesNV get_gl_entries ()->VDPAUUnmapSurfacesNV
#define glTexImage2DMultisampleCoverageNV get_gl_entries ()->TexImage2DMultisampleCoverageNV
#define glTexImage3DMultisampleCoverageNV get_gl_entries ()->TexImage3DMultisampleCoverageNV
#define glTextureImage2DMultisampleNV get_gl_entries ()->TextureImage2DMultisampleNV
#define glTextureImage3DMultisampleNV get_gl_entries ()->TextureImage3DMultisampleNV
#define glTextureImage2DMultisampleCoverageNV get_gl_entries ()->TextureImage2DMultisampleCoverageNV
#define glTextureImage3DMultisampleCoverageNV get_gl_entries ()->TextureImage3DMultisampleCoverageNV
#define glSetMultisamplefvAMD get_gl_entries ()->SetMultisamplefvAMD
#define glImportSyncEXT get_gl_entries ()->ImportSyncEXT
#define glMultiDrawArraysIndirectAMD get_gl_entries ()->MultiDrawArraysIndirectAMD
#define glMultiDrawElementsIndirectAMD get_gl_entries ()->MultiDrawElementsIndirectAMD
#define glGenPathsNV get_gl_entries ()->GenPathsNV
#define glDeletePathsNV get_gl_entries ()->DeletePathsNV
#define glIsPathNV get_gl_entries ()->IsPathNV
#define glPathCommandsNV get_gl_entries ()->PathCommandsNV
#define glPathCoordsNV get_gl_entries ()->PathCoordsNV
#define glPathSubCommandsNV get_gl_entries ()->PathSubCommandsNV
#define glPathSubCoordsNV get_gl_entries ()->PathSubCoordsNV
#define glPathStringNV get_gl_entries ()->PathStringNV
#define glPathGlyphsNV get_gl_entries ()->PathGlyphsNV
#define glPathGlyphRangeNV get_gl_entries ()->PathGlyphRangeNV
#define glWeightPathsNV get_gl_entries ()->WeightPathsNV
#define glCopyPathNV get_gl_entries ()->CopyPathNV
#define glInterpolatePathsNV get_gl_entries ()->InterpolatePathsNV
#define glTransformPathNV get_gl_entries ()->TransformPathNV
#define glPathParameterivNV get_gl_entries ()->PathParameterivNV
#define glPathParameteriNV get_gl_entries ()->PathParameteriNV
#define glPathParameterfvNV get_gl_entries ()->PathParameterfvNV
#define glPathParameterfNV get_gl_entries ()->PathParameterfNV
#define glPathDashArrayNV get_gl_entries ()->PathDashArrayNV
#define glPathStencilFuncNV get_gl_entries ()->PathStencilFuncNV
#define glPathStencilDepthOffsetNV get_gl_entries ()->PathStencilDepthOffsetNV
#define glStencilFillPathNV get_gl_entries ()->StencilFillPathNV
#define glStencilStrokePathNV get_gl_entries ()->StencilStrokePathNV
#define glStencilFillPathInstancedNV get_gl_entries ()->StencilFillPathInstancedNV
#define glStencilStrokePathInstancedNV get_gl_entries ()->StencilStrokePathInstancedNV
#define glPathCoverDepthFuncNV get_gl_entries ()->PathCoverDepthFuncNV
#define glPathColorGenNV get_gl_entries ()->PathColorGenNV
#define glPathTexGenNV get_gl_entries ()->PathTexGenNV
#define glPathFogGenNV get_gl_entries ()->PathFogGenNV
#define glCoverFillPathNV get_gl_entries ()->CoverFillPathNV
#define glCoverStrokePathNV get_gl_entries ()->CoverStrokePathNV
#define glCoverFillPathInstancedNV get_gl_entries ()->CoverFillPathInstancedNV
#define glCoverStrokePathInstancedNV get_gl_entries ()->CoverStrokePathInstancedNV
#define glGetPathParameterivNV get_gl_entries ()->GetPathParameterivNV
#define glGetPathParameterfvNV get_gl_entries ()->GetPathParameterfvNV
#define glGetPathCommandsNV get_gl_entries ()->GetPathCommandsNV
#define glGetPathCoordsNV get_gl_entries ()->GetPathCoordsNV
#define glGetPathDashArrayNV get_gl_entries ()->GetPathDashArrayNV
#define glGetPathMetricsNV get_gl_entries ()->GetPathMetricsNV
#define glGetPathMetricRangeNV get_gl_entries ()->GetPathMetricRangeNV
#define glGetPathSpacingNV get_gl_entries ()->GetPathSpacingNV
#define glGetPathColorGenivNV get_gl_entries ()->GetPathColorGenivNV
#define glGetPathColorGenfvNV get_gl_entries ()->GetPathColorGenfvNV
#define glGetPathTexGenivNV get_gl_entries ()->GetPathTexGenivNV
#define glGetPathTexGenfvNV get_gl_entries ()->GetPathTexGenfvNV
#define glIsPointInFillPathNV get_gl_entries ()->IsPointInFillPathNV
#define glIsPointInStrokePathNV get_gl_entries ()->IsPointInStrokePathNV
#define glGetPathLengthNV get_gl_entries ()->GetPathLengthNV
#define glPointAlongPathNV get_gl_entries ()->PointAlongPathNV
#define glStencilOpValueAMD get_gl_entries ()->StencilOpValueAMD
#define glGetTextureHandleNV get_gl_entries ()->GetTextureHandleNV
#define glGetTextureSamplerHandleNV get_gl_entries ()->GetTextureSamplerHandleNV
#define glMakeTextureHandleResidentNV get_gl_entries ()->MakeTextureHandleResidentNV
#define glMakeTextureHandleNonResidentNV get_gl_entries ()->MakeTextureHandleNonResidentNV
#define glGetImageHandleNV get_gl_entries ()->GetImageHandleNV
#define glMakeImageHandleResidentNV get_gl_entries ()->MakeImageHandleResidentNV
#define glMakeImageHandleNonResidentNV get_gl_entries ()->MakeImageHandleNonResidentNV
#define glUniformHandleui64NV get_gl_entries ()->UniformHandleui64NV
#define glUniformHandleui64vNV get_gl_entries ()->UniformHandleui64vNV
#define glProgramUniformHandleui64NV get_gl_entries ()->ProgramUniformHandleui64NV
#define glProgramUniformHandleui64vNV get_gl_entries ()->ProgramUniformHandleui64vNV
#define glIsTextureHandleResidentNV get_gl_entries ()->IsTextureHandleResidentNV
#define glIsImageHandleResidentNV get_gl_entries ()->IsImageHandleResidentNV
#define glTexStorageSparseAMD get_gl_entries ()->TexStorageSparseAMD
#define glTextureStorageSparseAMD get_gl_entries ()->TextureStorageSparseAMD

}

}

}

#endif
