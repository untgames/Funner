#ifndef RENDER_GL_DRIVER_GLES2_ENTRIES_HEADER
#define RENDER_GL_DRIVER_GLES2_ENTRIES_HEADER

#include <shared/profile/gles2/gl_types.h>

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
      Точки входа OpenGL ES 2.0
  */

  void         (GL_APIENTRY* ActiveTexture)(GLenum texture);
  void         (GL_APIENTRY* AttachShader)(GLuint program, GLuint shader);
  void         (GL_APIENTRY* BindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
  void         (GL_APIENTRY* BindBuffer)(GLenum target, GLuint buffer);
  void         (GL_APIENTRY* BindFramebuffer)(GLenum target, GLuint framebuffer);
  void         (GL_APIENTRY* BindRenderbuffer)(GLenum target, GLuint renderbuffer);
  void         (GL_APIENTRY* BindTexture)(GLenum target, GLuint texture);
  void         (GL_APIENTRY* BlendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void         (GL_APIENTRY* BlendEquation)( GLenum mode );
  void         (GL_APIENTRY* BlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
  void         (GL_APIENTRY* BlendFunc)(GLenum sfactor, GLenum dfactor);
  void         (GL_APIENTRY* BlendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
  void         (GL_APIENTRY* BufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
  void         (GL_APIENTRY* BufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
  GLenum       (GL_APIENTRY* CheckFramebufferStatus)(GLenum target);
  void         (GL_APIENTRY* Clear)(GLbitfield mask);
  void         (GL_APIENTRY* ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void         (GL_APIENTRY* ClearDepthf)(GLclampf depth);
  void         (GL_APIENTRY* ClearStencil)(GLint s);
  void         (GL_APIENTRY* ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
  void         (GL_APIENTRY* CompileShader)(GLuint shader);
  void         (GL_APIENTRY* CompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);
  void         (GL_APIENTRY* CompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);
  void         (GL_APIENTRY* CopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
  void         (GL_APIENTRY* CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  GLuint       (GL_APIENTRY* CreateProgram)(void);
  GLuint       (GL_APIENTRY* CreateShader)(GLenum type);
  void         (GL_APIENTRY* CullFace)(GLenum mode);
  void         (GL_APIENTRY* DeleteBuffers)(GLsizei n, const GLuint* buffers);
  void         (GL_APIENTRY* DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
  void         (GL_APIENTRY* DeleteProgram)(GLuint program);
  void         (GL_APIENTRY* DeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
  void         (GL_APIENTRY* DeleteShader)(GLuint shader);
  void         (GL_APIENTRY* DeleteTextures)(GLsizei n, const GLuint* textures);
  void         (GL_APIENTRY* DepthFunc)(GLenum func);
  void         (GL_APIENTRY* DepthMask)(GLboolean flag);
  void         (GL_APIENTRY* DepthRangef)(GLclampf zNear, GLclampf zFar);
  void         (GL_APIENTRY* DetachShader)(GLuint program, GLuint shader);
  void         (GL_APIENTRY* Disable)(GLenum cap);
  void         (GL_APIENTRY* DisableVertexAttribArray)(GLuint index);
  void         (GL_APIENTRY* DrawArrays)(GLenum mode, GLint first, GLsizei count);
  void         (GL_APIENTRY* DrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
  void         (GL_APIENTRY* Enable)(GLenum cap);
  void         (GL_APIENTRY* EnableVertexAttribArray)(GLuint index);
  void         (GL_APIENTRY* Finish)(void);
  void         (GL_APIENTRY* Flush)(void);
  void         (GL_APIENTRY* FramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
  void         (GL_APIENTRY* FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  void         (GL_APIENTRY* FrontFace)(GLenum mode);
  void         (GL_APIENTRY* GenBuffers)(GLsizei n, GLuint* buffers);
  void         (GL_APIENTRY* GenerateMipmap)(GLenum target);
  void         (GL_APIENTRY* GenFramebuffers)(GLsizei n, GLuint* framebuffers);
  void         (GL_APIENTRY* GenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
  void         (GL_APIENTRY* GenTextures)(GLsizei n, GLuint* textures);
  void         (GL_APIENTRY* GetActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
  void         (GL_APIENTRY* GetActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
  void         (GL_APIENTRY* GetAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
  int          (GL_APIENTRY* GetAttribLocation)(GLuint program, const GLchar* name);
  void         (GL_APIENTRY* GetBooleanv)(GLenum pname, GLboolean* params);
  void         (GL_APIENTRY* GetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
  GLenum       (GL_APIENTRY* GetError)(void);
  void         (GL_APIENTRY* GetFloatv)(GLenum pname, GLfloat* params);
  void         (GL_APIENTRY* GetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetIntegerv)(GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetProgramiv)(GLuint program, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
  void         (GL_APIENTRY* GetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetShaderiv)(GLuint shader, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
  void         (GL_APIENTRY* GetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
  void         (GL_APIENTRY* GetShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
  const GLubyte* (GL_APIENTRY* GetString)(GLenum name);
  void         (GL_APIENTRY* GetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
  void         (GL_APIENTRY* GetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetUniformfv)(GLuint program, GLint location, GLfloat* params);
  void         (GL_APIENTRY* GetUniformiv)(GLuint program, GLint location, GLint* params);
  int          (GL_APIENTRY* GetUniformLocation)(GLuint program, const GLchar* name);
  void         (GL_APIENTRY* GetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
  void         (GL_APIENTRY* GetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
  void         (GL_APIENTRY* GetVertexAttribPointerv)(GLuint index, GLenum pname, GLvoid** pointer);
  void         (GL_APIENTRY* Hint)(GLenum target, GLenum mode);
  GLboolean    (GL_APIENTRY* IsBuffer)(GLuint buffer);
  GLboolean    (GL_APIENTRY* IsEnabled)(GLenum cap);
  GLboolean    (GL_APIENTRY* IsFramebuffer)(GLuint framebuffer);
  GLboolean    (GL_APIENTRY* IsProgram)(GLuint program);
  GLboolean    (GL_APIENTRY* IsRenderbuffer)(GLuint renderbuffer);
  GLboolean    (GL_APIENTRY* IsShader)(GLuint shader);
  GLboolean    (GL_APIENTRY* IsTexture)(GLuint texture);
  void         (GL_APIENTRY* LineWidth)(GLfloat width);
  void         (GL_APIENTRY* LinkProgram)(GLuint program);
  void         (GL_APIENTRY* PixelStorei)(GLenum pname, GLint param);
  void         (GL_APIENTRY* PolygonOffset)(GLfloat factor, GLfloat units);
  void         (GL_APIENTRY* ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
  void         (GL_APIENTRY* ReleaseShaderCompiler)(void);
  void         (GL_APIENTRY* RenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
  void         (GL_APIENTRY* SampleCoverage)(GLclampf value, GLboolean invert);
  void         (GL_APIENTRY* Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
  void         (GL_APIENTRY* ShaderBinary)(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
  void         (GL_APIENTRY* ShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
  void         (GL_APIENTRY* StencilFunc)(GLenum func, GLint ref, GLuint mask);
  void         (GL_APIENTRY* StencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
  void         (GL_APIENTRY* StencilMask)(GLuint mask);
  void         (GL_APIENTRY* StencilMaskSeparate)(GLenum face, GLuint mask);
  void         (GL_APIENTRY* StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
  void         (GL_APIENTRY* StencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
  void         (GL_APIENTRY* TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
  void         (GL_APIENTRY* TexParameterf)(GLenum target, GLenum pname, GLfloat param);
  void         (GL_APIENTRY* TexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
  void         (GL_APIENTRY* TexParameteri)(GLenum target, GLenum pname, GLint param);
  void         (GL_APIENTRY* TexParameteriv)(GLenum target, GLenum pname, const GLint* params);
  void         (GL_APIENTRY* TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
  void         (GL_APIENTRY* Uniform1f)(GLint location, GLfloat x);
  void         (GL_APIENTRY* Uniform1fv)(GLint location, GLsizei count, const GLfloat* v);
  void         (GL_APIENTRY* Uniform1i)(GLint location, GLint x);
  void         (GL_APIENTRY* Uniform1iv)(GLint location, GLsizei count, const GLint* v);
  void         (GL_APIENTRY* Uniform2f)(GLint location, GLfloat x, GLfloat y);
  void         (GL_APIENTRY* Uniform2fv)(GLint location, GLsizei count, const GLfloat* v);
  void         (GL_APIENTRY* Uniform2i)(GLint location, GLint x, GLint y);
  void         (GL_APIENTRY* Uniform2iv)(GLint location, GLsizei count, const GLint* v);
  void         (GL_APIENTRY* Uniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);
  void         (GL_APIENTRY* Uniform3fv)(GLint location, GLsizei count, const GLfloat* v);
  void         (GL_APIENTRY* Uniform3i)(GLint location, GLint x, GLint y, GLint z);
  void         (GL_APIENTRY* Uniform3iv)(GLint location, GLsizei count, const GLint* v);
  void         (GL_APIENTRY* Uniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void         (GL_APIENTRY* Uniform4fv)(GLint location, GLsizei count, const GLfloat* v);
  void         (GL_APIENTRY* Uniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);
  void         (GL_APIENTRY* Uniform4iv)(GLint location, GLsizei count, const GLint* v);
  void         (GL_APIENTRY* UniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
  void         (GL_APIENTRY* UniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
  void         (GL_APIENTRY* UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
  void         (GL_APIENTRY* UseProgram)(GLuint program);
  void         (GL_APIENTRY* ValidateProgram)(GLuint program);
  void         (GL_APIENTRY* VertexAttrib1f)(GLuint indx, GLfloat x);
  void         (GL_APIENTRY* VertexAttrib1fv)(GLuint indx, const GLfloat* values);
  void         (GL_APIENTRY* VertexAttrib2f)(GLuint indx, GLfloat x, GLfloat y);
  void         (GL_APIENTRY* VertexAttrib2fv)(GLuint indx, const GLfloat* values);
  void         (GL_APIENTRY* VertexAttrib3f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
  void         (GL_APIENTRY* VertexAttrib3fv)(GLuint indx, const GLfloat* values);
  void         (GL_APIENTRY* VertexAttrib4f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void         (GL_APIENTRY* VertexAttrib4fv)(GLuint indx, const GLfloat* values);
  void         (GL_APIENTRY* VertexAttribPointer)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
  void         (GL_APIENTRY* Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);
  
  /*
      Точки входа расширений OpenGL
  */
<<<GLES2ENTRIES>>>

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
    Переопределения OpenGL ES 2.0
*/

#define glActiveTexture get_gl_entries ()->ActiveTexture
#define glAttachShader get_gl_entries ()->AttachShader
#define glBindAttribLocation get_gl_entries ()->BindAttribLocation
#define glBindBuffer get_gl_entries ()->BindBuffer
#define glBindFramebuffer get_gl_entries ()->BindFramebuffer
#define glBindRenderbuffer get_gl_entries ()->BindRenderbuffer
#define glBindTexture get_gl_entries ()->BindTexture
#define glBlendColor get_gl_entries ()->BlendColor
#define glBlendEquation get_gl_entries ()->BlendEquation
#define glBlendEquationSeparate get_gl_entries ()->BlendEquationSeparate
#define glBlendFunc get_gl_entries ()->BlendFunc
#define glBlendFuncSeparate get_gl_entries ()->BlendFuncSeparate
#define glBufferData get_gl_entries ()->BufferData
#define glBufferSubData get_gl_entries ()->BufferSubData
#define glCheckFramebufferStatus get_gl_entries ()->CheckFramebufferStatus
#define glClear get_gl_entries ()->Clear
#define glClearColor get_gl_entries ()->ClearColor
#define glClearDepthf get_gl_entries ()->ClearDepthf
#define glClearStencil get_gl_entries ()->ClearStencil
#define glColorMask get_gl_entries ()->ColorMask
#define glCompileShader get_gl_entries ()->CompileShader
#define glCompressedTexImage2D get_gl_entries ()->CompressedTexImage2D
#define glCompressedTexSubImage2D get_gl_entries ()->CompressedTexSubImage2D
#define glCopyTexImage2D get_gl_entries ()->CopyTexImage2D
#define glCopyTexSubImage2D get_gl_entries ()->CopyTexSubImage2D
#define glCreateProgram get_gl_entries ()->CreateProgram
#define glCreateShader get_gl_entries ()->CreateShader
#define glCullFace get_gl_entries ()->CullFace
#define glDeleteBuffers get_gl_entries ()->DeleteBuffers
#define glDeleteFramebuffers get_gl_entries ()->DeleteFramebuffers
#define glDeleteProgram get_gl_entries ()->DeleteProgram
#define glDeleteRenderbuffers get_gl_entries ()->DeleteRenderbuffers
#define glDeleteShader get_gl_entries ()->DeleteShader
#define glDeleteTextures get_gl_entries ()->DeleteTextures
#define glDepthFunc get_gl_entries ()->DepthFunc
#define glDepthMask get_gl_entries ()->DepthMask
#define glDepthRangef get_gl_entries ()->DepthRangef
#define glDetachShader get_gl_entries ()->DetachShader
#define glDisable get_gl_entries ()->Disable
#define glDisableVertexAttribArray get_gl_entries ()->DisableVertexAttribArray
#define glDrawArrays get_gl_entries ()->DrawArrays
#define glDrawElements get_gl_entries ()->DrawElements
#define glEnable get_gl_entries ()->Enable
#define glEnableVertexAttribArray get_gl_entries ()->EnableVertexAttribArray
#define glFinish get_gl_entries ()->Finish
#define glFlush get_gl_entries ()->Flush
#define glFramebufferRenderbuffer get_gl_entries ()->FramebufferRenderbuffer
#define glFramebufferTexture2D get_gl_entries ()->FramebufferTexture2D
#define glFrontFace get_gl_entries ()->FrontFace
#define glGenBuffers get_gl_entries ()->GenBuffers
#define glGenerateMipmap get_gl_entries ()->GenerateMipmap
#define glGenFramebuffers get_gl_entries ()->GenFramebuffers
#define glGenRenderbuffers get_gl_entries ()->GenRenderbuffers
#define glGenTextures get_gl_entries ()->GenTextures
#define glGetActiveAttrib get_gl_entries ()->GetActiveAttrib
#define glGetActiveUniform get_gl_entries ()->GetActiveUniform
#define glGetAttachedShaders get_gl_entries ()->GetAttachedShaders
#define glGetAttribLocation get_gl_entries ()->GetAttribLocation
#define glGetBooleanv get_gl_entries ()->GetBooleanv
#define glGetBufferParameteriv get_gl_entries ()->GetBufferParameteriv
#define glGetError get_gl_entries ()->GetError
#define glGetFloatv get_gl_entries ()->GetFloatv
#define glGetFramebufferAttachmentParameteriv get_gl_entries ()->GetFramebufferAttachmentParameteriv
#define glGetIntegerv get_gl_entries ()->GetIntegerv
#define glGetProgramiv get_gl_entries ()->GetProgramiv
#define glGetProgramInfoLog get_gl_entries ()->GetProgramInfoLog
#define glGetRenderbufferParameteriv get_gl_entries ()->GetRenderbufferParameteriv
#define glGetShaderiv get_gl_entries ()->GetShaderiv
#define glGetShaderInfoLog get_gl_entries ()->GetShaderInfoLog
#define glGetShaderPrecisionFormat get_gl_entries ()->GetShaderPrecisionFormat
#define glGetShaderSource get_gl_entries ()->GetShaderSource
#define glGetString get_gl_entries ()->GetString
#define glGetTexParameterfv get_gl_entries ()->GetTexParameterfv
#define glGetTexParameteriv get_gl_entries ()->GetTexParameteriv
#define glGetUniformfv get_gl_entries ()->GetUniformfv
#define glGetUniformiv get_gl_entries ()->GetUniformiv
#define glGetUniformLocation get_gl_entries ()->GetUniformLocation
#define glGetVertexAttribfv get_gl_entries ()->GetVertexAttribfv
#define glGetVertexAttribiv get_gl_entries ()->GetVertexAttribiv
#define glGetVertexAttribPointerv get_gl_entries ()->GetVertexAttribPointerv
#define glHint get_gl_entries ()->Hint
#define glIsBuffer get_gl_entries ()->IsBuffer
#define glIsEnabled get_gl_entries ()->IsEnabled
#define glIsFramebuffer get_gl_entries ()->IsFramebuffer
#define glIsProgram get_gl_entries ()->IsProgram
#define glIsRenderbuffer get_gl_entries ()->IsRenderbuffer
#define glIsShader get_gl_entries ()->IsShader
#define glIsTexture get_gl_entries ()->IsTexture
#define glLineWidth get_gl_entries ()->LineWidth
#define glLinkProgram get_gl_entries ()->LinkProgram
#define glPixelStorei get_gl_entries ()->PixelStorei
#define glPolygonOffset get_gl_entries ()->PolygonOffset
#define glReadPixels get_gl_entries ()->ReadPixels
#define glReleaseShaderCompiler get_gl_entries ()->ReleaseShaderCompiler
#define glRenderbufferStorage get_gl_entries ()->RenderbufferStorage
#define glSampleCoverage get_gl_entries ()->SampleCoverage
#define glScissor get_gl_entries ()->Scissor
#define glShaderBinary get_gl_entries ()->ShaderBinary
#define glShaderSource get_gl_entries ()->ShaderSource
#define glStencilFunc get_gl_entries ()->StencilFunc
#define glStencilFuncSeparate get_gl_entries ()->StencilFuncSeparate
#define glStencilMask get_gl_entries ()->StencilMask
#define glStencilMaskSeparate get_gl_entries ()->StencilMaskSeparate
#define glStencilOp get_gl_entries ()->StencilOp
#define glStencilOpSeparate get_gl_entries ()->StencilOpSeparate
#define glTexImage2D get_gl_entries ()->TexImage2D
#define glTexParameterf get_gl_entries ()->TexParameterf
#define glTexParameterfv get_gl_entries ()->TexParameterfv
#define glTexParameteri get_gl_entries ()->TexParameteri
#define glTexParameteriv get_gl_entries ()->TexParameteriv
#define glTexSubImage2D get_gl_entries ()->TexSubImage2D
#define glUniform1f get_gl_entries ()->Uniform1f
#define glUniform1fv get_gl_entries ()->Uniform1fv
#define glUniform1i get_gl_entries ()->Uniform1i
#define glUniform1iv get_gl_entries ()->Uniform1iv
#define glUniform2f get_gl_entries ()->Uniform2f
#define glUniform2fv get_gl_entries ()->Uniform2fv
#define glUniform2i get_gl_entries ()->Uniform2i
#define glUniform2iv get_gl_entries ()->Uniform2iv
#define glUniform3f get_gl_entries ()->Uniform3f
#define glUniform3fv get_gl_entries ()->Uniform3fv
#define glUniform3i get_gl_entries ()->Uniform3i
#define glUniform3iv get_gl_entries ()->Uniform3iv
#define glUniform4f get_gl_entries ()->Uniform4f
#define glUniform4fv get_gl_entries ()->Uniform4fv
#define glUniform4i get_gl_entries ()->Uniform4i
#define glUniform4iv get_gl_entries ()->Uniform4iv
#define glUniformMatrix2fv get_gl_entries ()->UniformMatrix2fv
#define glUniformMatrix3fv get_gl_entries ()->UniformMatrix3fv
#define glUniformMatrix4fv get_gl_entries ()->UniformMatrix4fv
#define glUseProgram get_gl_entries ()->UseProgram
#define glValidateProgram get_gl_entries ()->ValidateProgram
#define glVertexAttrib1f get_gl_entries ()->VertexAttrib1f
#define glVertexAttrib1fv get_gl_entries ()->VertexAttrib1fv
#define glVertexAttrib2f get_gl_entries ()->VertexAttrib2f
#define glVertexAttrib2fv get_gl_entries ()->VertexAttrib2fv
#define glVertexAttrib3f get_gl_entries ()->VertexAttrib3f
#define glVertexAttrib3fv get_gl_entries ()->VertexAttrib3fv
#define glVertexAttrib4f get_gl_entries ()->VertexAttrib4f
#define glVertexAttrib4fv get_gl_entries ()->VertexAttrib4fv
#define glVertexAttribPointer get_gl_entries ()->VertexAttribPointer
#define glViewport get_gl_entries ()->Viewport

/*
    Переопределения расширений OpenGL
*/
<<<GLES2DEFINES>>>

}

}

}

#endif
