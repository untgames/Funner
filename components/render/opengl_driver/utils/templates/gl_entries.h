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
<<<GLENTRIES>>>

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
<<<GLDEFINES>>>

}

}

}

#endif
