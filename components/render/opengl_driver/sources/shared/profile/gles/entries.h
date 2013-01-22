#ifndef RENDER_GL_DRIVER_GLES_ENTRIES_HEADER
#define RENDER_GL_DRIVER_GLES_ENTRIES_HEADER

#include <shared/profile/gles/gl_types.h>
#include <shared/profile/gles/glext.h>

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
      Точки входа OpenGL ES 1.1  
  */

  void (GL_APIENTRY* AlphaFunc)(GLenum func, GLclampf ref);
  void (GL_APIENTRY* ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void (GL_APIENTRY* ClearDepthf)(GLclampf depth);
  void (GL_APIENTRY* ClipPlanef)(GLenum plane, const GLfloat *equation);
  void (GL_APIENTRY* Color4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  void (GL_APIENTRY* DepthRangef)(GLclampf zNear, GLclampf zFar);
  void (GL_APIENTRY* Fogf)(GLenum pname, GLfloat param);
  void (GL_APIENTRY* Fogfv)(GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* Frustumf)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  void (GL_APIENTRY* GetClipPlanef)(GLenum pname, GLfloat eqn[4]);
  void (GL_APIENTRY* GetFloatv)(GLenum pname, GLfloat *params);
  void (GL_APIENTRY* GetLightfv)(GLenum light, GLenum pname, GLfloat *params);
  void (GL_APIENTRY* GetMaterialfv)(GLenum face, GLenum pname, GLfloat *params);
  void (GL_APIENTRY* GetTexEnvfv)(GLenum env, GLenum pname, GLfloat *params);
  void (GL_APIENTRY* GetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
  void (GL_APIENTRY* LightModelf)(GLenum pname, GLfloat param);
  void (GL_APIENTRY* LightModelfv)(GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* Lightf)(GLenum light, GLenum pname, GLfloat param);
  void (GL_APIENTRY* Lightfv)(GLenum light, GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* LineWidth)(GLfloat width);
  void (GL_APIENTRY* LoadMatrixf)(const GLfloat *m);
  void (GL_APIENTRY* Materialf)(GLenum face, GLenum pname, GLfloat param);
  void (GL_APIENTRY* Materialfv)(GLenum face, GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* MultMatrixf)(const GLfloat *m);
  void (GL_APIENTRY* MultiTexCoord4f)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  void (GL_APIENTRY* Normal3f)(GLfloat nx, GLfloat ny, GLfloat nz);
  void (GL_APIENTRY* Orthof)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  void (GL_APIENTRY* PointParameterf)(GLenum pname, GLfloat param);
  void (GL_APIENTRY* PointParameterfv)(GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* PointSize)(GLfloat size);
  void (GL_APIENTRY* PolygonOffset)(GLfloat factor, GLfloat units);
  void (GL_APIENTRY* Rotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void (GL_APIENTRY* Scalef)(GLfloat x, GLfloat y, GLfloat z);
  void (GL_APIENTRY* TexEnvf)(GLenum target, GLenum pname, GLfloat param);
  void (GL_APIENTRY* TexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* TexParameterf)(GLenum target, GLenum pname, GLfloat param);
  void (GL_APIENTRY* TexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
  void (GL_APIENTRY* Translatef)(GLfloat x, GLfloat y, GLfloat z);

  void (GL_APIENTRY* ActiveTexture)(GLenum texture);
  void (GL_APIENTRY* AlphaFuncx)(GLenum func, GLclampx ref);
  void (GL_APIENTRY* BindBuffer)(GLenum target, GLuint buffer);
  void (GL_APIENTRY* BindTexture)(GLenum target, GLuint texture);
  void (GL_APIENTRY* BlendFunc)(GLenum sfactor, GLenum dfactor);
  void (GL_APIENTRY* BufferData)(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
  void (GL_APIENTRY* BufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
  void (GL_APIENTRY* Clear)(GLbitfield mask);
  void (GL_APIENTRY* ClearColorx)(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  void (GL_APIENTRY* ClearDepthx)(GLclampx depth);
  void (GL_APIENTRY* ClearStencil)(GLint s);
  void (GL_APIENTRY* ClientActiveTexture)(GLenum texture);
  void (GL_APIENTRY* ClipPlanex)(GLenum plane, const GLfixed *equation);
  void (GL_APIENTRY* Color4ub)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  void (GL_APIENTRY* Color4x)(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  void (GL_APIENTRY* ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
  void (GL_APIENTRY* ColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (GL_APIENTRY* CompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
  void (GL_APIENTRY* CompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
  void (GL_APIENTRY* CopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
  void (GL_APIENTRY* CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  void (GL_APIENTRY* CullFace)(GLenum mode);
  void (GL_APIENTRY* DeleteBuffers)(GLsizei n, const GLuint *buffers);
  void (GL_APIENTRY* DeleteTextures)(GLsizei n, const GLuint *textures);
  void (GL_APIENTRY* DepthFunc)(GLenum func);
  void (GL_APIENTRY* DepthMask)(GLboolean flag);
  void (GL_APIENTRY* DepthRangex)(GLclampx zNear, GLclampx zFar);
  void (GL_APIENTRY* Disable)(GLenum cap);
  void (GL_APIENTRY* DisableClientState)(GLenum array);
  void (GL_APIENTRY* DrawArrays)(GLenum mode, GLint first, GLsizei count);
  void (GL_APIENTRY* DrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
  void (GL_APIENTRY* Enable)(GLenum cap);
  void (GL_APIENTRY* EnableClientState)(GLenum array);
  void (GL_APIENTRY* Finish)(void);
  void (GL_APIENTRY* Flush)(void);
  void (GL_APIENTRY* Fogx)(GLenum pname, GLfixed param);
  void (GL_APIENTRY* Fogxv)(GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* FrontFace)(GLenum mode);
  void (GL_APIENTRY* Frustumx)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  void (GL_APIENTRY* GetBooleanv)(GLenum pname, GLboolean *params);
  void (GL_APIENTRY* GetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
  void (GL_APIENTRY* GetClipPlanex)(GLenum pname, GLfixed eqn[4]);
  void (GL_APIENTRY* GenBuffers)(GLsizei n, GLuint *buffers);
  void (GL_APIENTRY* GenTextures)(GLsizei n, GLuint *textures);
  GLenum (GL_APIENTRY* GetError)(void);
  void (GL_APIENTRY* GetFixedv)(GLenum pname, GLfixed *params);
  void (GL_APIENTRY* GetIntegerv)(GLenum pname, GLint *params);
  void (GL_APIENTRY* GetLightxv)(GLenum light, GLenum pname, GLfixed *params);
  void (GL_APIENTRY* GetMaterialxv)(GLenum face, GLenum pname, GLfixed *params);
  void (GL_APIENTRY* GetPointerv)(GLenum pname, void **params);
  const GLubyte * (GL_APIENTRY* GetString)(GLenum name);
  void (GL_APIENTRY* GetTexEnviv)(GLenum env, GLenum pname, GLint *params);
  void (GL_APIENTRY* GetTexEnvxv)(GLenum env, GLenum pname, GLfixed *params);
  void (GL_APIENTRY* GetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
  void (GL_APIENTRY* GetTexParameterxv)(GLenum target, GLenum pname, GLfixed *params);
  void (GL_APIENTRY* Hint)(GLenum target, GLenum mode);
  GLboolean (GL_APIENTRY* IsBuffer)(GLuint buffer);
  GLboolean (GL_APIENTRY* IsEnabled)(GLenum cap);
  GLboolean (GL_APIENTRY* IsTexture)(GLuint texture);
  void (GL_APIENTRY* LightModelx)(GLenum pname, GLfixed param);
  void (GL_APIENTRY* LightModelxv)(GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* Lightx)(GLenum light, GLenum pname, GLfixed param);
  void (GL_APIENTRY* Lightxv)(GLenum light, GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* LineWidthx)(GLfixed width);
  void (GL_APIENTRY* LoadIdentity)(void);
  void (GL_APIENTRY* LoadMatrixx)(const GLfixed *m);
  void (GL_APIENTRY* LogicOp)(GLenum opcode);
  void (GL_APIENTRY* Materialx)(GLenum face, GLenum pname, GLfixed param);
  void (GL_APIENTRY* Materialxv)(GLenum face, GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* MatrixMode)(GLenum mode);
  void (GL_APIENTRY* MultMatrixx)(const GLfixed *m);
  void (GL_APIENTRY* MultiTexCoord4x)(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
  void (GL_APIENTRY* Normal3x)(GLfixed nx, GLfixed ny, GLfixed nz);
  void (GL_APIENTRY* NormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
  void (GL_APIENTRY* Orthox)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  void (GL_APIENTRY* PixelStorei)(GLenum pname, GLint param);
  void (GL_APIENTRY* PointParameterx)(GLenum pname, GLfixed param);
  void (GL_APIENTRY* PointParameterxv)(GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* PointSizex)(GLfixed size);
  void (GL_APIENTRY* PolygonOffsetx)(GLfixed factor, GLfixed units);
  void (GL_APIENTRY* PopMatrix)(void);
  void (GL_APIENTRY* PushMatrix)(void);
  void (GL_APIENTRY* ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
  void (GL_APIENTRY* Rotatex)(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  void (GL_APIENTRY* SampleCoverage)(GLclampf value, GLboolean invert);
  void (GL_APIENTRY* SampleCoveragex)(GLclampx value, GLboolean invert);
  void (GL_APIENTRY* Scalex)(GLfixed x, GLfixed y, GLfixed z);
  void (GL_APIENTRY* Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
  void (GL_APIENTRY* ShadeModel)(GLenum mode);
  void (GL_APIENTRY* StencilFunc)(GLenum func, GLint ref, GLuint mask);
  void (GL_APIENTRY* StencilMask)(GLuint mask);
  void (GL_APIENTRY* StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
  void (GL_APIENTRY* TexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (GL_APIENTRY* TexEnvi)(GLenum target, GLenum pname, GLint param);
  void (GL_APIENTRY* TexEnvx)(GLenum target, GLenum pname, GLfixed param);
  void (GL_APIENTRY* TexEnviv)(GLenum target, GLenum pname, const GLint *params);
  void (GL_APIENTRY* TexEnvxv)(GLenum target, GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  void (GL_APIENTRY* TexParameteri)(GLenum target, GLenum pname, GLint param);
  void (GL_APIENTRY* TexParameterx)(GLenum target, GLenum pname, GLfixed param);
  void (GL_APIENTRY* TexParameteriv)(GLenum target, GLenum pname, const GLint *params);
  void (GL_APIENTRY* TexParameterxv)(GLenum target, GLenum pname, const GLfixed *params);
  void (GL_APIENTRY* TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  void (GL_APIENTRY* Translatex)(GLfixed x, GLfixed y, GLfixed z);
  void (GL_APIENTRY* VertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  void (GL_APIENTRY* Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);

//Required OES extension functions
  void (GL_APIENTRY* PointSizePointerOES)(GLenum type, GLsizei stride, const GLvoid *pointer);
  
  /*
      Точки входа расширений OpenGL
  */

  PFNGLBLENDEQUATIONSEPARATEOESPROC BlendEquationSeparateOES;
  PFNGLBLENDFUNCSEPARATEOESPROC BlendFuncSeparateOES;
  PFNGLBLENDEQUATIONOESPROC BlendEquationOES;
  PFNGLDRAWTEXSOESPROC DrawTexsOES;
  PFNGLDRAWTEXIOESPROC DrawTexiOES;
  PFNGLDRAWTEXXOESPROC DrawTexxOES;
  PFNGLDRAWTEXSVOESPROC DrawTexsvOES;
  PFNGLDRAWTEXIVOESPROC DrawTexivOES;
  PFNGLDRAWTEXXVOESPROC DrawTexxvOES;
  PFNGLDRAWTEXFOESPROC DrawTexfOES;
  PFNGLDRAWTEXFVOESPROC DrawTexfvOES;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC EGLImageTargetTexture2DOES;
  PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC EGLImageTargetRenderbufferStorageOES;
  PFNGLALPHAFUNCXOESPROC AlphaFuncxOES;
  PFNGLCLEARCOLORXOESPROC ClearColorxOES;
  PFNGLCLEARDEPTHXOESPROC ClearDepthxOES;
  PFNGLCLIPPLANEXOESPROC ClipPlanexOES;
  PFNGLCOLOR4XOESPROC Color4xOES;
  PFNGLDEPTHRANGEXOESPROC DepthRangexOES;
  PFNGLFOGXOESPROC FogxOES;
  PFNGLFOGXVOESPROC FogxvOES;
  PFNGLFRUSTUMXOESPROC FrustumxOES;
  PFNGLGETCLIPPLANEXOESPROC GetClipPlanexOES;
  PFNGLGETFIXEDVOESPROC GetFixedvOES;
  PFNGLGETLIGHTXVOESPROC GetLightxvOES;
  PFNGLGETMATERIALXVOESPROC GetMaterialxvOES;
  PFNGLGETTEXENVXVOESPROC GetTexEnvxvOES;
  PFNGLGETTEXPARAMETERXVOESPROC GetTexParameterxvOES;
  PFNGLLIGHTMODELXOESPROC LightModelxOES;
  PFNGLLIGHTMODELXVOESPROC LightModelxvOES;
  PFNGLLIGHTXOESPROC LightxOES;
  PFNGLLIGHTXVOESPROC LightxvOES;
  PFNGLLINEWIDTHXOESPROC LineWidthxOES;
  PFNGLLOADMATRIXXOESPROC LoadMatrixxOES;
  PFNGLMATERIALXOESPROC MaterialxOES;
  PFNGLMATERIALXVOESPROC MaterialxvOES;
  PFNGLMULTMATRIXXOESPROC MultMatrixxOES;
  PFNGLMULTITEXCOORD4XOESPROC MultiTexCoord4xOES;
  PFNGLNORMAL3XOESPROC Normal3xOES;
  PFNGLORTHOXOESPROC OrthoxOES;
  PFNGLPOINTPARAMETERXOESPROC PointParameterxOES;
  PFNGLPOINTPARAMETERXVOESPROC PointParameterxvOES;
  PFNGLPOINTSIZEXOESPROC PointSizexOES;
  PFNGLPOLYGONOFFSETXOESPROC PolygonOffsetxOES;
  PFNGLROTATEXOESPROC RotatexOES;
  PFNGLSAMPLECOVERAGEXOESPROC SampleCoveragexOES;
  PFNGLSCALEXOESPROC ScalexOES;
  PFNGLTEXENVXOESPROC TexEnvxOES;
  PFNGLTEXENVXVOESPROC TexEnvxvOES;
  PFNGLTEXPARAMETERXOESPROC TexParameterxOES;
  PFNGLTEXPARAMETERXVOESPROC TexParameterxvOES;
  PFNGLTRANSLATEXOESPROC TranslatexOES;
  PFNGLISRENDERBUFFEROESPROC IsRenderbufferOES;
  PFNGLBINDRENDERBUFFEROESPROC BindRenderbufferOES;
  PFNGLDELETERENDERBUFFERSOESPROC DeleteRenderbuffersOES;
  PFNGLGENRENDERBUFFERSOESPROC GenRenderbuffersOES;
  PFNGLRENDERBUFFERSTORAGEOESPROC RenderbufferStorageOES;
  PFNGLGETRENDERBUFFERPARAMETERIVOESPROC GetRenderbufferParameterivOES;
  PFNGLISFRAMEBUFFEROESPROC IsFramebufferOES;
  PFNGLBINDFRAMEBUFFEROESPROC BindFramebufferOES;
  PFNGLDELETEFRAMEBUFFERSOESPROC DeleteFramebuffersOES;
  PFNGLGENFRAMEBUFFERSOESPROC GenFramebuffersOES;
  PFNGLCHECKFRAMEBUFFERSTATUSOESPROC CheckFramebufferStatusOES;
  PFNGLFRAMEBUFFERRENDERBUFFEROESPROC FramebufferRenderbufferOES;
  PFNGLFRAMEBUFFERTEXTURE2DOESPROC FramebufferTexture2DOES;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC GetFramebufferAttachmentParameterivOES;
  PFNGLGENERATEMIPMAPOESPROC GenerateMipmapOES;
  PFNGLMAPBUFFEROESPROC MapBufferOES;
  PFNGLUNMAPBUFFEROESPROC UnmapBufferOES;
  PFNGLGETBUFFERPOINTERVOESPROC GetBufferPointervOES;
  PFNGLCURRENTPALETTEMATRIXOESPROC CurrentPaletteMatrixOES;
  PFNGLLOADPALETTEFROMMODELVIEWMATRIXOESPROC LoadPaletteFromModelViewMatrixOES;
  PFNGLMATRIXINDEXPOINTEROESPROC MatrixIndexPointerOES;
  PFNGLWEIGHTPOINTEROESPROC WeightPointerOES;
  PFNGLQUERYMATRIXXOESPROC QueryMatrixxOES;
  PFNGLDEPTHRANGEFOESPROC DepthRangefOES;
  PFNGLFRUSTUMFOESPROC FrustumfOES;
  PFNGLORTHOFOESPROC OrthofOES;
  PFNGLCLIPPLANEFOESPROC ClipPlanefOES;
  PFNGLGETCLIPPLANEFOESPROC GetClipPlanefOES;
  PFNGLCLEARDEPTHFOESPROC ClearDepthfOES;
  PFNGLTEXGENFOESPROC TexGenfOES;
  PFNGLTEXGENFVOESPROC TexGenfvOES;
  PFNGLTEXGENIOESPROC TexGeniOES;
  PFNGLTEXGENIVOESPROC TexGenivOES;
  PFNGLTEXGENXOESPROC TexGenxOES;
  PFNGLTEXGENXVOESPROC TexGenxvOES;
  PFNGLGETTEXGENFVOESPROC GetTexGenfvOES;
  PFNGLGETTEXGENIVOESPROC GetTexGenivOES;
  PFNGLGETTEXGENXVOESPROC GetTexGenxvOES;
  PFNGLCLIPPLANEFIMGPROC ClipPlanefIMG;
  PFNGLCLIPPLANEXIMGPROC ClipPlanexIMG;
  PFNGLDELETEFENCESNVPROC DeleteFencesNV;
  PFNGLGENFENCESNVPROC GenFencesNV;
  PFNGLISFENCENVPROC IsFenceNV;
  PFNGLTESTFENCENVPROC TestFenceNV;
  PFNGLGETFENCEIVNVPROC GetFenceivNV;
  PFNGLFINISHFENCENVPROC FinishFenceNV;
  PFNGLSETFENCENVPROC SetFenceNV;
  PFNGLGETDRIVERCONTROLSQCOMPROC GetDriverControlsQCOM;
  PFNGLGETDRIVERCONTROLSTRINGQCOMPROC GetDriverControlStringQCOM;
  PFNGLENABLEDRIVERCONTROLQCOMPROC EnableDriverControlQCOM;
  PFNGLDISABLEDRIVERCONTROLQCOMPROC DisableDriverControlQCOM;

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
    Переопределения OpenGL ES 1.1
*/

#define glAlphaFunc get_gl_entries ()->AlphaFunc
#define glClearColor get_gl_entries ()->ClearColor
#define glClearDepthf get_gl_entries ()->ClearDepthf
#define glClipPlanef get_gl_entries ()->ClipPlanef
#define glColor4f get_gl_entries ()->Color4f
#define glDepthRangef get_gl_entries ()->DepthRangef
#define glFogf get_gl_entries ()->Fogf
#define glFogfv get_gl_entries ()->Fogfv
#define glFrustumf get_gl_entries ()->Frustumf
#define glGetClipPlanef get_gl_entries ()->GetClipPlanef
#define glGetFloatv get_gl_entries ()->GetFloatv
#define glGetLightfv get_gl_entries ()->GetLightfv
#define glGetMaterialfv get_gl_entries ()->GetMaterialfv
#define glGetTexEnvfv get_gl_entries ()->GetTexEnvfv
#define glGetTexParameterfv get_gl_entries ()->GetTexParameterfv
#define glLightModelf get_gl_entries ()->LightModelf
#define glLightModelfv get_gl_entries ()->LightModelfv
#define glLightf get_gl_entries ()->Lightf
#define glLightfv get_gl_entries ()->Lightfv
#define glLineWidth get_gl_entries ()->LineWidth
#define glLoadMatrixf get_gl_entries ()->LoadMatrixf
#define glMaterialf get_gl_entries ()->Materialf
#define glMaterialfv get_gl_entries ()->Materialfv
#define glMultMatrixf get_gl_entries ()->MultMatrixf
#define glMultiTexCoord4f get_gl_entries ()->MultiTexCoord4f
#define glNormal3f get_gl_entries ()->Normal3f
#define glOrthof get_gl_entries ()->Orthof
#define glPointParameterf get_gl_entries ()->PointParameterf
#define glPointParameterfv get_gl_entries ()->PointParameterfv
#define glPointSize get_gl_entries ()->PointSize
#define glPolygonOffset get_gl_entries ()->PolygonOffset
#define glRotatef get_gl_entries ()->Rotatef
#define glScalef get_gl_entries ()->Scalef
#define glTexEnvf get_gl_entries ()->TexEnvf
#define glTexEnvfv get_gl_entries ()->TexEnvfv
#define glTexParameterf get_gl_entries ()->TexParameterf
#define glTexParameterfv get_gl_entries ()->TexParameterfv
#define glTranslatef get_gl_entries ()->Translatef
#define glActiveTexture get_gl_entries ()->ActiveTexture
#define glAlphaFuncx get_gl_entries ()->AlphaFuncx
#define glBindBuffer get_gl_entries ()->BindBuffer
#define glBindTexture get_gl_entries ()->BindTexture
#define glBlendFunc get_gl_entries ()->BlendFunc
#define glBufferData get_gl_entries ()->BufferData
#define glBufferSubData get_gl_entries ()->BufferSubData
#define glClear get_gl_entries ()->Clear
#define glClearColorx get_gl_entries ()->ClearColorx
#define glClearDepthx get_gl_entries ()->ClearDepthx
#define glClearStencil get_gl_entries ()->ClearStencil
#define glClientActiveTexture get_gl_entries ()->ClientActiveTexture
#define glClipPlanex get_gl_entries ()->ClipPlanex
#define glColor4ub get_gl_entries ()->Color4ub
#define glColor4x get_gl_entries ()->Color4x
#define glColorMask get_gl_entries ()->ColorMask
#define glColorPointer get_gl_entries ()->ColorPointer
#define glCompressedTexImage2D get_gl_entries ()->CompressedTexImage2D
#define glCompressedTexSubImage2D get_gl_entries ()->CompressedTexSubImage2D
#define glCopyTexImage2D get_gl_entries ()->CopyTexImage2D
#define glCopyTexSubImage2D get_gl_entries ()->CopyTexSubImage2D
#define glCullFace get_gl_entries ()->CullFace
#define glDeleteBuffers get_gl_entries ()->DeleteBuffers
#define glDeleteTextures get_gl_entries ()->DeleteTextures
#define glDepthFunc get_gl_entries ()->DepthFunc
#define glDepthMask get_gl_entries ()->DepthMask
#define glDepthRangex get_gl_entries ()->DepthRangex
#define glDisable get_gl_entries ()->Disable
#define glDisableClientState get_gl_entries ()->DisableClientState
#define glDrawArrays get_gl_entries ()->DrawArrays
#define glDrawElements get_gl_entries ()->DrawElements
#define glEnable get_gl_entries ()->Enable
#define glEnableClientState get_gl_entries ()->EnableClientState
#define glFinish get_gl_entries ()->Finish
#define glFlush get_gl_entries ()->Flush
#define glFogx get_gl_entries ()->Fogx
#define glFogxv get_gl_entries ()->Fogxv
#define glFrontFace get_gl_entries ()->FrontFace
#define glFrustumx get_gl_entries ()->Frustumx
#define glGetBooleanv get_gl_entries ()->GetBooleanv
#define glGetBufferParameteriv get_gl_entries ()->GetBufferParameteriv
#define glGetClipPlanex get_gl_entries ()->GetClipPlanex
#define glGenBuffers get_gl_entries ()->GenBuffers
#define glGenTextures get_gl_entries ()->GenTextures
#define glGetError get_gl_entries ()->GetError
#define glGetFixedv get_gl_entries ()->GetFixedv
#define glGetIntegerv get_gl_entries ()->GetIntegerv
#define glGetLightxv get_gl_entries ()->GetLightxv
#define glGetMaterialxv get_gl_entries ()->GetMaterialxv
#define glGetPointerv get_gl_entries ()->GetPointerv
#define glGetString get_gl_entries ()->GetString
#define glGetTexEnviv get_gl_entries ()->GetTexEnviv
#define glGetTexEnvxv get_gl_entries ()->GetTexEnvxv
#define glGetTexParameteriv get_gl_entries ()->GetTexParameteriv
#define glGetTexParameterxv get_gl_entries ()->GetTexParameterxv
#define glHint get_gl_entries ()->Hint
#define glIsBuffer get_gl_entries ()->IsBuffer
#define glIsEnabled get_gl_entries ()->IsEnabled
#define glIsTexture get_gl_entries ()->IsTexture
#define glLightModelx get_gl_entries ()->LightModelx
#define glLightModelxv get_gl_entries ()->LightModelxv
#define glLightx get_gl_entries ()->Lightx
#define glLightxv get_gl_entries ()->Lightxv
#define glLineWidthx get_gl_entries ()->LineWidthx
#define glLoadIdentity get_gl_entries ()->LoadIdentity
#define glLoadMatrixx get_gl_entries ()->LoadMatrixx
#define glLogicOp get_gl_entries ()->LogicOp
#define glMaterialx get_gl_entries ()->Materialx
#define glMaterialxv get_gl_entries ()->Materialxv
#define glMatrixMode get_gl_entries ()->MatrixMode
#define glMultMatrixx get_gl_entries ()->MultMatrixx
#define glMultiTexCoord4x get_gl_entries ()->MultiTexCoord4x
#define glNormal3x get_gl_entries ()->Normal3x
#define glNormalPointer get_gl_entries ()->NormalPointer
#define glOrthox get_gl_entries ()->Orthox
#define glPixelStorei get_gl_entries ()->PixelStorei
#define glPointParameterx get_gl_entries ()->PointParameterx
#define glPointParameterxv get_gl_entries ()->PointParameterxv
#define glPointSizex get_gl_entries ()->PointSizex
#define glPolygonOffsetx get_gl_entries ()->PolygonOffsetx
#define glPopMatrix get_gl_entries ()->PopMatrix
#define glPushMatrix get_gl_entries ()->PushMatrix
#define glReadPixels get_gl_entries ()->ReadPixels
#define glRotatex get_gl_entries ()->Rotatex
#define glSampleCoverage get_gl_entries ()->SampleCoverage
#define glSampleCoveragex get_gl_entries ()->SampleCoveragex
#define glScalex get_gl_entries ()->Scalex
#define glScissor get_gl_entries ()->Scissor
#define glShadeModel get_gl_entries ()->ShadeModel
#define glStencilFunc get_gl_entries ()->StencilFunc
#define glStencilMask get_gl_entries ()->StencilMask
#define glStencilOp get_gl_entries ()->StencilOp
#define glTexCoordPointer get_gl_entries ()->TexCoordPointer
#define glTexEnvi get_gl_entries ()->TexEnvi
#define glTexEnvx get_gl_entries ()->TexEnvx
#define glTexEnviv get_gl_entries ()->TexEnviv
#define glTexEnvxv get_gl_entries ()->TexEnvxv
#define glTexImage2D get_gl_entries ()->TexImage2D
#define glTexParameteri get_gl_entries ()->TexParameteri
#define glTexParameterx get_gl_entries ()->TexParameterx
#define glTexParameteriv get_gl_entries ()->TexParameteriv
#define glTexParameterxv get_gl_entries ()->TexParameterxv
#define glTexSubImage2D get_gl_entries ()->TexSubImage2D
#define glTranslatex get_gl_entries ()->Translatex
#define glVertexPointer get_gl_entries ()->VertexPointer
#define glViewport get_gl_entries ()->Viewport
#define glPointSizePointerOES get_gl_entries ()->PointSizePointerOES

/*
    Переопределения расширений OpenGL
*/

#define glBlendEquationSeparateOES get_gl_entries ()->BlendEquationSeparateOES
#define glBlendFuncSeparateOES get_gl_entries ()->BlendFuncSeparateOES
#define glBlendEquationOES get_gl_entries ()->BlendEquationOES
#define glDrawTexsOES get_gl_entries ()->DrawTexsOES
#define glDrawTexiOES get_gl_entries ()->DrawTexiOES
#define glDrawTexxOES get_gl_entries ()->DrawTexxOES
#define glDrawTexsvOES get_gl_entries ()->DrawTexsvOES
#define glDrawTexivOES get_gl_entries ()->DrawTexivOES
#define glDrawTexxvOES get_gl_entries ()->DrawTexxvOES
#define glDrawTexfOES get_gl_entries ()->DrawTexfOES
#define glDrawTexfvOES get_gl_entries ()->DrawTexfvOES
#define glEGLImageTargetTexture2DOES get_gl_entries ()->EGLImageTargetTexture2DOES
#define glEGLImageTargetRenderbufferStorageOES get_gl_entries ()->EGLImageTargetRenderbufferStorageOES
#define glAlphaFuncxOES get_gl_entries ()->AlphaFuncxOES
#define glClearColorxOES get_gl_entries ()->ClearColorxOES
#define glClearDepthxOES get_gl_entries ()->ClearDepthxOES
#define glClipPlanexOES get_gl_entries ()->ClipPlanexOES
#define glColor4xOES get_gl_entries ()->Color4xOES
#define glDepthRangexOES get_gl_entries ()->DepthRangexOES
#define glFogxOES get_gl_entries ()->FogxOES
#define glFogxvOES get_gl_entries ()->FogxvOES
#define glFrustumxOES get_gl_entries ()->FrustumxOES
#define glGetClipPlanexOES get_gl_entries ()->GetClipPlanexOES
#define glGetFixedvOES get_gl_entries ()->GetFixedvOES
#define glGetLightxvOES get_gl_entries ()->GetLightxvOES
#define glGetMaterialxvOES get_gl_entries ()->GetMaterialxvOES
#define glGetTexEnvxvOES get_gl_entries ()->GetTexEnvxvOES
#define glGetTexParameterxvOES get_gl_entries ()->GetTexParameterxvOES
#define glLightModelxOES get_gl_entries ()->LightModelxOES
#define glLightModelxvOES get_gl_entries ()->LightModelxvOES
#define glLightxOES get_gl_entries ()->LightxOES
#define glLightxvOES get_gl_entries ()->LightxvOES
#define glLineWidthxOES get_gl_entries ()->LineWidthxOES
#define glLoadMatrixxOES get_gl_entries ()->LoadMatrixxOES
#define glMaterialxOES get_gl_entries ()->MaterialxOES
#define glMaterialxvOES get_gl_entries ()->MaterialxvOES
#define glMultMatrixxOES get_gl_entries ()->MultMatrixxOES
#define glMultiTexCoord4xOES get_gl_entries ()->MultiTexCoord4xOES
#define glNormal3xOES get_gl_entries ()->Normal3xOES
#define glOrthoxOES get_gl_entries ()->OrthoxOES
#define glPointParameterxOES get_gl_entries ()->PointParameterxOES
#define glPointParameterxvOES get_gl_entries ()->PointParameterxvOES
#define glPointSizexOES get_gl_entries ()->PointSizexOES
#define glPolygonOffsetxOES get_gl_entries ()->PolygonOffsetxOES
#define glRotatexOES get_gl_entries ()->RotatexOES
#define glSampleCoveragexOES get_gl_entries ()->SampleCoveragexOES
#define glScalexOES get_gl_entries ()->ScalexOES
#define glTexEnvxOES get_gl_entries ()->TexEnvxOES
#define glTexEnvxvOES get_gl_entries ()->TexEnvxvOES
#define glTexParameterxOES get_gl_entries ()->TexParameterxOES
#define glTexParameterxvOES get_gl_entries ()->TexParameterxvOES
#define glTranslatexOES get_gl_entries ()->TranslatexOES
#define glIsRenderbufferOES get_gl_entries ()->IsRenderbufferOES
#define glBindRenderbufferOES get_gl_entries ()->BindRenderbufferOES
#define glDeleteRenderbuffersOES get_gl_entries ()->DeleteRenderbuffersOES
#define glGenRenderbuffersOES get_gl_entries ()->GenRenderbuffersOES
#define glRenderbufferStorageOES get_gl_entries ()->RenderbufferStorageOES
#define glGetRenderbufferParameterivOES get_gl_entries ()->GetRenderbufferParameterivOES
#define glIsFramebufferOES get_gl_entries ()->IsFramebufferOES
#define glBindFramebufferOES get_gl_entries ()->BindFramebufferOES
#define glDeleteFramebuffersOES get_gl_entries ()->DeleteFramebuffersOES
#define glGenFramebuffersOES get_gl_entries ()->GenFramebuffersOES
#define glCheckFramebufferStatusOES get_gl_entries ()->CheckFramebufferStatusOES
#define glFramebufferRenderbufferOES get_gl_entries ()->FramebufferRenderbufferOES
#define glFramebufferTexture2DOES get_gl_entries ()->FramebufferTexture2DOES
#define glGetFramebufferAttachmentParameterivOES get_gl_entries ()->GetFramebufferAttachmentParameterivOES
#define glGenerateMipmapOES get_gl_entries ()->GenerateMipmapOES
#define glMapBufferOES get_gl_entries ()->MapBufferOES
#define glUnmapBufferOES get_gl_entries ()->UnmapBufferOES
#define glGetBufferPointervOES get_gl_entries ()->GetBufferPointervOES
#define glCurrentPaletteMatrixOES get_gl_entries ()->CurrentPaletteMatrixOES
#define glLoadPaletteFromModelViewMatrixOES get_gl_entries ()->LoadPaletteFromModelViewMatrixOES
#define glMatrixIndexPointerOES get_gl_entries ()->MatrixIndexPointerOES
#define glWeightPointerOES get_gl_entries ()->WeightPointerOES
#define glQueryMatrixxOES get_gl_entries ()->QueryMatrixxOES
#define glDepthRangefOES get_gl_entries ()->DepthRangefOES
#define glFrustumfOES get_gl_entries ()->FrustumfOES
#define glOrthofOES get_gl_entries ()->OrthofOES
#define glClipPlanefOES get_gl_entries ()->ClipPlanefOES
#define glGetClipPlanefOES get_gl_entries ()->GetClipPlanefOES
#define glClearDepthfOES get_gl_entries ()->ClearDepthfOES
#define glTexGenfOES get_gl_entries ()->TexGenfOES
#define glTexGenfvOES get_gl_entries ()->TexGenfvOES
#define glTexGeniOES get_gl_entries ()->TexGeniOES
#define glTexGenivOES get_gl_entries ()->TexGenivOES
#define glTexGenxOES get_gl_entries ()->TexGenxOES
#define glTexGenxvOES get_gl_entries ()->TexGenxvOES
#define glGetTexGenfvOES get_gl_entries ()->GetTexGenfvOES
#define glGetTexGenivOES get_gl_entries ()->GetTexGenivOES
#define glGetTexGenxvOES get_gl_entries ()->GetTexGenxvOES
#define glClipPlanefIMG get_gl_entries ()->ClipPlanefIMG
#define glClipPlanexIMG get_gl_entries ()->ClipPlanexIMG
#define glDeleteFencesNV get_gl_entries ()->DeleteFencesNV
#define glGenFencesNV get_gl_entries ()->GenFencesNV
#define glIsFenceNV get_gl_entries ()->IsFenceNV
#define glTestFenceNV get_gl_entries ()->TestFenceNV
#define glGetFenceivNV get_gl_entries ()->GetFenceivNV
#define glFinishFenceNV get_gl_entries ()->FinishFenceNV
#define glSetFenceNV get_gl_entries ()->SetFenceNV
#define glGetDriverControlsQCOM get_gl_entries ()->GetDriverControlsQCOM
#define glGetDriverControlStringQCOM get_gl_entries ()->GetDriverControlStringQCOM
#define glEnableDriverControlQCOM get_gl_entries ()->EnableDriverControlQCOM
#define glDisableDriverControlQCOM get_gl_entries ()->DisableDriverControlQCOM

}

}

}

#endif
