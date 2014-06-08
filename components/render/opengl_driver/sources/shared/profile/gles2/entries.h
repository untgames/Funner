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

  PFNGLDEBUGMESSAGECONTROLKHRPROC DebugMessageControlKHR;
  PFNGLDEBUGMESSAGEINSERTKHRPROC DebugMessageInsertKHR;
  PFNGLDEBUGMESSAGECALLBACKKHRPROC DebugMessageCallbackKHR;
  PFNGLGETDEBUGMESSAGELOGKHRPROC GetDebugMessageLogKHR;
  PFNGLPUSHDEBUGGROUPKHRPROC PushDebugGroupKHR;
  PFNGLPOPDEBUGGROUPKHRPROC PopDebugGroupKHR;
  PFNGLOBJECTLABELKHRPROC ObjectLabelKHR;
  PFNGLGETOBJECTLABELKHRPROC GetObjectLabelKHR;
  PFNGLOBJECTPTRLABELKHRPROC ObjectPtrLabelKHR;
  PFNGLGETOBJECTPTRLABELKHRPROC GetObjectPtrLabelKHR;
  PFNGLGETPOINTERVKHRPROC GetPointervKHR;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC EGLImageTargetTexture2DOES;
  PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC EGLImageTargetRenderbufferStorageOES;
  PFNGLGETPROGRAMBINARYOESPROC GetProgramBinaryOES;
  PFNGLPROGRAMBINARYOESPROC ProgramBinaryOES;
  PFNGLMAPBUFFEROESPROC MapBufferOES;
  PFNGLUNMAPBUFFEROESPROC UnmapBufferOES;
  PFNGLGETBUFFERPOINTERVOESPROC GetBufferPointervOES;
  PFNGLTEXIMAGE3DOESPROC TexImage3DOES;
  PFNGLTEXSUBIMAGE3DOESPROC TexSubImage3DOES;
  PFNGLCOPYTEXSUBIMAGE3DOESPROC CopyTexSubImage3DOES;
  PFNGLCOMPRESSEDTEXIMAGE3DOESPROC CompressedTexImage3DOES;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC CompressedTexSubImage3DOES;
  PFNGLFRAMEBUFFERTEXTURE3DOESPROC FramebufferTexture3DOES;
  PFNGLBINDVERTEXARRAYOESPROC BindVertexArrayOES;
  PFNGLDELETEVERTEXARRAYSOESPROC DeleteVertexArraysOES;
  PFNGLGENVERTEXARRAYSOESPROC GenVertexArraysOES;
  PFNGLISVERTEXARRAYOESPROC IsVertexArrayOES;
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
  PFNGLBLITFRAMEBUFFERANGLEPROC BlitFramebufferANGLE;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC RenderbufferStorageMultisampleANGLE;
  PFNGLDRAWARRAYSINSTANCEDANGLEPROC DrawArraysInstancedANGLE;
  PFNGLDRAWELEMENTSINSTANCEDANGLEPROC DrawElementsInstancedANGLE;
  PFNGLVERTEXATTRIBDIVISORANGLEPROC VertexAttribDivisorANGLE;
  PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC GetTranslatedShaderSourceANGLE;
  PFNGLCOPYTEXTURELEVELSAPPLEPROC CopyTextureLevelsAPPLE;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC RenderbufferStorageMultisampleAPPLE;
  PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC ResolveMultisampleFramebufferAPPLE;
  PFNGLFENCESYNCAPPLEPROC FenceSyncAPPLE;
  PFNGLISSYNCAPPLEPROC IsSyncAPPLE;
  PFNGLDELETESYNCAPPLEPROC DeleteSyncAPPLE;
  PFNGLCLIENTWAITSYNCAPPLEPROC ClientWaitSyncAPPLE;
  PFNGLWAITSYNCAPPLEPROC WaitSyncAPPLE;
  PFNGLGETINTEGER64VAPPLEPROC GetInteger64vAPPLE;
  PFNGLGETSYNCIVAPPLEPROC GetSyncivAPPLE;
  PFNGLLABELOBJECTEXTPROC LabelObjectEXT;
  PFNGLGETOBJECTLABELEXTPROC GetObjectLabelEXT;
  PFNGLINSERTEVENTMARKEREXTPROC InsertEventMarkerEXT;
  PFNGLPUSHGROUPMARKEREXTPROC PushGroupMarkerEXT;
  PFNGLPOPGROUPMARKEREXTPROC PopGroupMarkerEXT;
  PFNGLDISCARDFRAMEBUFFEREXTPROC DiscardFramebufferEXT;
  PFNGLGENQUERIESEXTPROC GenQueriesEXT;
  PFNGLDELETEQUERIESEXTPROC DeleteQueriesEXT;
  PFNGLISQUERYEXTPROC IsQueryEXT;
  PFNGLBEGINQUERYEXTPROC BeginQueryEXT;
  PFNGLENDQUERYEXTPROC EndQueryEXT;
  PFNGLQUERYCOUNTEREXTPROC QueryCounterEXT;
  PFNGLGETQUERYIVEXTPROC GetQueryivEXT;
  PFNGLGETQUERYOBJECTIVEXTPROC GetQueryObjectivEXT;
  PFNGLGETQUERYOBJECTUIVEXTPROC GetQueryObjectuivEXT;
  PFNGLGETQUERYOBJECTI64VEXTPROC GetQueryObjecti64vEXT;
  PFNGLGETQUERYOBJECTUI64VEXTPROC GetQueryObjectui64vEXT;
  PFNGLDRAWBUFFERSEXTPROC DrawBuffersEXT;
  PFNGLDRAWARRAYSINSTANCEDEXTPROC DrawArraysInstancedEXT;
  PFNGLDRAWELEMENTSINSTANCEDEXTPROC DrawElementsInstancedEXT;
  PFNGLVERTEXATTRIBDIVISOREXTPROC VertexAttribDivisorEXT;
  PFNGLMAPBUFFERRANGEEXTPROC MapBufferRangeEXT;
  PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC FlushMappedBufferRangeEXT;
  PFNGLMULTIDRAWARRAYSEXTPROC MultiDrawArraysEXT;
  PFNGLMULTIDRAWELEMENTSEXTPROC MultiDrawElementsEXT;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC RenderbufferStorageMultisampleEXT;
  PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC FramebufferTexture2DMultisampleEXT;
  PFNGLREADBUFFERINDEXEDEXTPROC ReadBufferIndexedEXT;
  PFNGLDRAWBUFFERSINDEXEDEXTPROC DrawBuffersIndexedEXT;
  PFNGLGETINTEGERI_VEXTPROC GetIntegeri_vEXT;
  PFNGLGETGRAPHICSRESETSTATUSEXTPROC GetGraphicsResetStatusEXT;
  PFNGLREADNPIXELSEXTPROC ReadnPixelsEXT;
  PFNGLGETNUNIFORMFVEXTPROC GetnUniformfvEXT;
  PFNGLGETNUNIFORMIVEXTPROC GetnUniformivEXT;
  PFNGLACTIVESHADERPROGRAMEXTPROC ActiveShaderProgramEXT;
  PFNGLBINDPROGRAMPIPELINEEXTPROC BindProgramPipelineEXT;
  PFNGLCREATESHADERPROGRAMVEXTPROC CreateShaderProgramvEXT;
  PFNGLDELETEPROGRAMPIPELINESEXTPROC DeleteProgramPipelinesEXT;
  PFNGLGENPROGRAMPIPELINESEXTPROC GenProgramPipelinesEXT;
  PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC GetProgramPipelineInfoLogEXT;
  PFNGLGETPROGRAMPIPELINEIVEXTPROC GetProgramPipelineivEXT;
  PFNGLISPROGRAMPIPELINEEXTPROC IsProgramPipelineEXT;
  PFNGLPROGRAMPARAMETERIEXTPROC ProgramParameteriEXT;
  PFNGLPROGRAMUNIFORM1FEXTPROC ProgramUniform1fEXT;
  PFNGLPROGRAMUNIFORM1FVEXTPROC ProgramUniform1fvEXT;
  PFNGLPROGRAMUNIFORM1IEXTPROC ProgramUniform1iEXT;
  PFNGLPROGRAMUNIFORM1IVEXTPROC ProgramUniform1ivEXT;
  PFNGLPROGRAMUNIFORM2FEXTPROC ProgramUniform2fEXT;
  PFNGLPROGRAMUNIFORM2FVEXTPROC ProgramUniform2fvEXT;
  PFNGLPROGRAMUNIFORM2IEXTPROC ProgramUniform2iEXT;
  PFNGLPROGRAMUNIFORM2IVEXTPROC ProgramUniform2ivEXT;
  PFNGLPROGRAMUNIFORM3FEXTPROC ProgramUniform3fEXT;
  PFNGLPROGRAMUNIFORM3FVEXTPROC ProgramUniform3fvEXT;
  PFNGLPROGRAMUNIFORM3IEXTPROC ProgramUniform3iEXT;
  PFNGLPROGRAMUNIFORM3IVEXTPROC ProgramUniform3ivEXT;
  PFNGLPROGRAMUNIFORM4FEXTPROC ProgramUniform4fEXT;
  PFNGLPROGRAMUNIFORM4FVEXTPROC ProgramUniform4fvEXT;
  PFNGLPROGRAMUNIFORM4IEXTPROC ProgramUniform4iEXT;
  PFNGLPROGRAMUNIFORM4IVEXTPROC ProgramUniform4ivEXT;
  PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC ProgramUniformMatrix2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC ProgramUniformMatrix3fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC ProgramUniformMatrix4fvEXT;
  PFNGLUSEPROGRAMSTAGESEXTPROC UseProgramStagesEXT;
  PFNGLVALIDATEPROGRAMPIPELINEEXTPROC ValidateProgramPipelineEXT;
  PFNGLPROGRAMUNIFORM1UIEXTPROC ProgramUniform1uiEXT;
  PFNGLPROGRAMUNIFORM2UIEXTPROC ProgramUniform2uiEXT;
  PFNGLPROGRAMUNIFORM3UIEXTPROC ProgramUniform3uiEXT;
  PFNGLPROGRAMUNIFORM4UIEXTPROC ProgramUniform4uiEXT;
  PFNGLPROGRAMUNIFORM1UIVEXTPROC ProgramUniform1uivEXT;
  PFNGLPROGRAMUNIFORM2UIVEXTPROC ProgramUniform2uivEXT;
  PFNGLPROGRAMUNIFORM3UIVEXTPROC ProgramUniform3uivEXT;
  PFNGLPROGRAMUNIFORM4UIVEXTPROC ProgramUniform4uivEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC ProgramUniformMatrix2x3fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC ProgramUniformMatrix3x2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC ProgramUniformMatrix2x4fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC ProgramUniformMatrix4x2fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC ProgramUniformMatrix3x4fvEXT;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC ProgramUniformMatrix4x3fvEXT;
  PFNGLTEXSTORAGE1DEXTPROC TexStorage1DEXT;
  PFNGLTEXSTORAGE2DEXTPROC TexStorage2DEXT;
  PFNGLTEXSTORAGE3DEXTPROC TexStorage3DEXT;
  PFNGLTEXTURESTORAGE1DEXTPROC TextureStorage1DEXT;
  PFNGLTEXTURESTORAGE2DEXTPROC TextureStorage2DEXT;
  PFNGLTEXTURESTORAGE3DEXTPROC TextureStorage3DEXT;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC RenderbufferStorageMultisampleIMG;
  PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC FramebufferTexture2DMultisampleIMG;
  PFNGLBEGINPERFQUERYINTELPROC BeginPerfQueryINTEL;
  PFNGLCREATEPERFQUERYINTELPROC CreatePerfQueryINTEL;
  PFNGLDELETEPERFQUERYINTELPROC DeletePerfQueryINTEL;
  PFNGLENDPERFQUERYINTELPROC EndPerfQueryINTEL;
  PFNGLGETFIRSTPERFQUERYIDINTELPROC GetFirstPerfQueryIdINTEL;
  PFNGLGETNEXTPERFQUERYIDINTELPROC GetNextPerfQueryIdINTEL;
  PFNGLGETPERFCOUNTERINFOINTELPROC GetPerfCounterInfoINTEL;
  PFNGLGETPERFQUERYDATAINTELPROC GetPerfQueryDataINTEL;
  PFNGLGETPERFQUERYIDBYNAMEINTELPROC GetPerfQueryIdByNameINTEL;
  PFNGLGETPERFQUERYINFOINTELPROC GetPerfQueryInfoINTEL;
  PFNGLBLENDPARAMETERINVPROC BlendParameteriNV;
  PFNGLBLENDBARRIERNVPROC BlendBarrierNV;
  PFNGLCOPYBUFFERSUBDATANVPROC CopyBufferSubDataNV;
  PFNGLCOVERAGEMASKNVPROC CoverageMaskNV;
  PFNGLCOVERAGEOPERATIONNVPROC CoverageOperationNV;
  PFNGLDRAWBUFFERSNVPROC DrawBuffersNV;
  PFNGLDRAWARRAYSINSTANCEDNVPROC DrawArraysInstancedNV;
  PFNGLDRAWELEMENTSINSTANCEDNVPROC DrawElementsInstancedNV;
  PFNGLDELETEFENCESNVPROC DeleteFencesNV;
  PFNGLGENFENCESNVPROC GenFencesNV;
  PFNGLISFENCENVPROC IsFenceNV;
  PFNGLTESTFENCENVPROC TestFenceNV;
  PFNGLGETFENCEIVNVPROC GetFenceivNV;
  PFNGLFINISHFENCENVPROC FinishFenceNV;
  PFNGLSETFENCENVPROC SetFenceNV;
  PFNGLBLITFRAMEBUFFERNVPROC BlitFramebufferNV;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC RenderbufferStorageMultisampleNV;
  PFNGLVERTEXATTRIBDIVISORNVPROC VertexAttribDivisorNV;
  PFNGLUNIFORMMATRIX2X3FVNVPROC UniformMatrix2x3fvNV;
  PFNGLUNIFORMMATRIX3X2FVNVPROC UniformMatrix3x2fvNV;
  PFNGLUNIFORMMATRIX2X4FVNVPROC UniformMatrix2x4fvNV;
  PFNGLUNIFORMMATRIX4X2FVNVPROC UniformMatrix4x2fvNV;
  PFNGLUNIFORMMATRIX3X4FVNVPROC UniformMatrix3x4fvNV;
  PFNGLUNIFORMMATRIX4X3FVNVPROC UniformMatrix4x3fvNV;
  PFNGLREADBUFFERNVPROC ReadBufferNV;
  PFNGLALPHAFUNCQCOMPROC AlphaFuncQCOM;
  PFNGLGETDRIVERCONTROLSQCOMPROC GetDriverControlsQCOM;
  PFNGLGETDRIVERCONTROLSTRINGQCOMPROC GetDriverControlStringQCOM;
  PFNGLENABLEDRIVERCONTROLQCOMPROC EnableDriverControlQCOM;
  PFNGLDISABLEDRIVERCONTROLQCOMPROC DisableDriverControlQCOM;
  PFNGLEXTGETTEXTURESQCOMPROC ExtGetTexturesQCOM;
  PFNGLEXTGETBUFFERSQCOMPROC ExtGetBuffersQCOM;
  PFNGLEXTGETRENDERBUFFERSQCOMPROC ExtGetRenderbuffersQCOM;
  PFNGLEXTGETFRAMEBUFFERSQCOMPROC ExtGetFramebuffersQCOM;
  PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC ExtGetTexLevelParameterivQCOM;
  PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC ExtTexObjectStateOverrideiQCOM;
  PFNGLEXTGETTEXSUBIMAGEQCOMPROC ExtGetTexSubImageQCOM;
  PFNGLEXTGETBUFFERPOINTERVQCOMPROC ExtGetBufferPointervQCOM;
  PFNGLEXTGETSHADERSQCOMPROC ExtGetShadersQCOM;
  PFNGLEXTGETPROGRAMSQCOMPROC ExtGetProgramsQCOM;
  PFNGLEXTISPROGRAMBINARYQCOMPROC ExtIsProgramBinaryQCOM;
  PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC ExtGetProgramBinarySourceQCOM;
  PFNGLSTARTTILINGQCOMPROC StartTilingQCOM;
  PFNGLENDTILINGQCOMPROC EndTilingQCOM;

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

#define glDebugMessageControlKHR get_gl_entries ()->DebugMessageControlKHR
#define glDebugMessageInsertKHR get_gl_entries ()->DebugMessageInsertKHR
#define glDebugMessageCallbackKHR get_gl_entries ()->DebugMessageCallbackKHR
#define glGetDebugMessageLogKHR get_gl_entries ()->GetDebugMessageLogKHR
#define glPushDebugGroupKHR get_gl_entries ()->PushDebugGroupKHR
#define glPopDebugGroupKHR get_gl_entries ()->PopDebugGroupKHR
#define glObjectLabelKHR get_gl_entries ()->ObjectLabelKHR
#define glGetObjectLabelKHR get_gl_entries ()->GetObjectLabelKHR
#define glObjectPtrLabelKHR get_gl_entries ()->ObjectPtrLabelKHR
#define glGetObjectPtrLabelKHR get_gl_entries ()->GetObjectPtrLabelKHR
#define glGetPointervKHR get_gl_entries ()->GetPointervKHR
#define glEGLImageTargetTexture2DOES get_gl_entries ()->EGLImageTargetTexture2DOES
#define glEGLImageTargetRenderbufferStorageOES get_gl_entries ()->EGLImageTargetRenderbufferStorageOES
#define glGetProgramBinaryOES get_gl_entries ()->GetProgramBinaryOES
#define glProgramBinaryOES get_gl_entries ()->ProgramBinaryOES
#define glMapBufferOES get_gl_entries ()->MapBufferOES
#define glUnmapBufferOES get_gl_entries ()->UnmapBufferOES
#define glGetBufferPointervOES get_gl_entries ()->GetBufferPointervOES
#define glTexImage3DOES get_gl_entries ()->TexImage3DOES
#define glTexSubImage3DOES get_gl_entries ()->TexSubImage3DOES
#define glCopyTexSubImage3DOES get_gl_entries ()->CopyTexSubImage3DOES
#define glCompressedTexImage3DOES get_gl_entries ()->CompressedTexImage3DOES
#define glCompressedTexSubImage3DOES get_gl_entries ()->CompressedTexSubImage3DOES
#define glFramebufferTexture3DOES get_gl_entries ()->FramebufferTexture3DOES
#define glBindVertexArrayOES get_gl_entries ()->BindVertexArrayOES
#define glDeleteVertexArraysOES get_gl_entries ()->DeleteVertexArraysOES
#define glGenVertexArraysOES get_gl_entries ()->GenVertexArraysOES
#define glIsVertexArrayOES get_gl_entries ()->IsVertexArrayOES
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
#define glBlitFramebufferANGLE get_gl_entries ()->BlitFramebufferANGLE
#define glRenderbufferStorageMultisampleANGLE get_gl_entries ()->RenderbufferStorageMultisampleANGLE
#define glDrawArraysInstancedANGLE get_gl_entries ()->DrawArraysInstancedANGLE
#define glDrawElementsInstancedANGLE get_gl_entries ()->DrawElementsInstancedANGLE
#define glVertexAttribDivisorANGLE get_gl_entries ()->VertexAttribDivisorANGLE
#define glGetTranslatedShaderSourceANGLE get_gl_entries ()->GetTranslatedShaderSourceANGLE
#define glCopyTextureLevelsAPPLE get_gl_entries ()->CopyTextureLevelsAPPLE
#define glRenderbufferStorageMultisampleAPPLE get_gl_entries ()->RenderbufferStorageMultisampleAPPLE
#define glResolveMultisampleFramebufferAPPLE get_gl_entries ()->ResolveMultisampleFramebufferAPPLE
#define glFenceSyncAPPLE get_gl_entries ()->FenceSyncAPPLE
#define glIsSyncAPPLE get_gl_entries ()->IsSyncAPPLE
#define glDeleteSyncAPPLE get_gl_entries ()->DeleteSyncAPPLE
#define glClientWaitSyncAPPLE get_gl_entries ()->ClientWaitSyncAPPLE
#define glWaitSyncAPPLE get_gl_entries ()->WaitSyncAPPLE
#define glGetInteger64vAPPLE get_gl_entries ()->GetInteger64vAPPLE
#define glGetSyncivAPPLE get_gl_entries ()->GetSyncivAPPLE
#define glLabelObjectEXT get_gl_entries ()->LabelObjectEXT
#define glGetObjectLabelEXT get_gl_entries ()->GetObjectLabelEXT
#define glInsertEventMarkerEXT get_gl_entries ()->InsertEventMarkerEXT
#define glPushGroupMarkerEXT get_gl_entries ()->PushGroupMarkerEXT
#define glPopGroupMarkerEXT get_gl_entries ()->PopGroupMarkerEXT
#define glDiscardFramebufferEXT get_gl_entries ()->DiscardFramebufferEXT
#define glGenQueriesEXT get_gl_entries ()->GenQueriesEXT
#define glDeleteQueriesEXT get_gl_entries ()->DeleteQueriesEXT
#define glIsQueryEXT get_gl_entries ()->IsQueryEXT
#define glBeginQueryEXT get_gl_entries ()->BeginQueryEXT
#define glEndQueryEXT get_gl_entries ()->EndQueryEXT
#define glQueryCounterEXT get_gl_entries ()->QueryCounterEXT
#define glGetQueryivEXT get_gl_entries ()->GetQueryivEXT
#define glGetQueryObjectivEXT get_gl_entries ()->GetQueryObjectivEXT
#define glGetQueryObjectuivEXT get_gl_entries ()->GetQueryObjectuivEXT
#define glGetQueryObjecti64vEXT get_gl_entries ()->GetQueryObjecti64vEXT
#define glGetQueryObjectui64vEXT get_gl_entries ()->GetQueryObjectui64vEXT
#define glDrawBuffersEXT get_gl_entries ()->DrawBuffersEXT
#define glDrawArraysInstancedEXT get_gl_entries ()->DrawArraysInstancedEXT
#define glDrawElementsInstancedEXT get_gl_entries ()->DrawElementsInstancedEXT
#define glVertexAttribDivisorEXT get_gl_entries ()->VertexAttribDivisorEXT
#define glMapBufferRangeEXT get_gl_entries ()->MapBufferRangeEXT
#define glFlushMappedBufferRangeEXT get_gl_entries ()->FlushMappedBufferRangeEXT
#define glMultiDrawArraysEXT get_gl_entries ()->MultiDrawArraysEXT
#define glMultiDrawElementsEXT get_gl_entries ()->MultiDrawElementsEXT
#define glRenderbufferStorageMultisampleEXT get_gl_entries ()->RenderbufferStorageMultisampleEXT
#define glFramebufferTexture2DMultisampleEXT get_gl_entries ()->FramebufferTexture2DMultisampleEXT
#define glReadBufferIndexedEXT get_gl_entries ()->ReadBufferIndexedEXT
#define glDrawBuffersIndexedEXT get_gl_entries ()->DrawBuffersIndexedEXT
#define glGetIntegeri_vEXT get_gl_entries ()->GetIntegeri_vEXT
#define glGetGraphicsResetStatusEXT get_gl_entries ()->GetGraphicsResetStatusEXT
#define glReadnPixelsEXT get_gl_entries ()->ReadnPixelsEXT
#define glGetnUniformfvEXT get_gl_entries ()->GetnUniformfvEXT
#define glGetnUniformivEXT get_gl_entries ()->GetnUniformivEXT
#define glActiveShaderProgramEXT get_gl_entries ()->ActiveShaderProgramEXT
#define glBindProgramPipelineEXT get_gl_entries ()->BindProgramPipelineEXT
#define glCreateShaderProgramvEXT get_gl_entries ()->CreateShaderProgramvEXT
#define glDeleteProgramPipelinesEXT get_gl_entries ()->DeleteProgramPipelinesEXT
#define glGenProgramPipelinesEXT get_gl_entries ()->GenProgramPipelinesEXT
#define glGetProgramPipelineInfoLogEXT get_gl_entries ()->GetProgramPipelineInfoLogEXT
#define glGetProgramPipelineivEXT get_gl_entries ()->GetProgramPipelineivEXT
#define glIsProgramPipelineEXT get_gl_entries ()->IsProgramPipelineEXT
#define glProgramParameteriEXT get_gl_entries ()->ProgramParameteriEXT
#define glProgramUniform1fEXT get_gl_entries ()->ProgramUniform1fEXT
#define glProgramUniform1fvEXT get_gl_entries ()->ProgramUniform1fvEXT
#define glProgramUniform1iEXT get_gl_entries ()->ProgramUniform1iEXT
#define glProgramUniform1ivEXT get_gl_entries ()->ProgramUniform1ivEXT
#define glProgramUniform2fEXT get_gl_entries ()->ProgramUniform2fEXT
#define glProgramUniform2fvEXT get_gl_entries ()->ProgramUniform2fvEXT
#define glProgramUniform2iEXT get_gl_entries ()->ProgramUniform2iEXT
#define glProgramUniform2ivEXT get_gl_entries ()->ProgramUniform2ivEXT
#define glProgramUniform3fEXT get_gl_entries ()->ProgramUniform3fEXT
#define glProgramUniform3fvEXT get_gl_entries ()->ProgramUniform3fvEXT
#define glProgramUniform3iEXT get_gl_entries ()->ProgramUniform3iEXT
#define glProgramUniform3ivEXT get_gl_entries ()->ProgramUniform3ivEXT
#define glProgramUniform4fEXT get_gl_entries ()->ProgramUniform4fEXT
#define glProgramUniform4fvEXT get_gl_entries ()->ProgramUniform4fvEXT
#define glProgramUniform4iEXT get_gl_entries ()->ProgramUniform4iEXT
#define glProgramUniform4ivEXT get_gl_entries ()->ProgramUniform4ivEXT
#define glProgramUniformMatrix2fvEXT get_gl_entries ()->ProgramUniformMatrix2fvEXT
#define glProgramUniformMatrix3fvEXT get_gl_entries ()->ProgramUniformMatrix3fvEXT
#define glProgramUniformMatrix4fvEXT get_gl_entries ()->ProgramUniformMatrix4fvEXT
#define glUseProgramStagesEXT get_gl_entries ()->UseProgramStagesEXT
#define glValidateProgramPipelineEXT get_gl_entries ()->ValidateProgramPipelineEXT
#define glProgramUniform1uiEXT get_gl_entries ()->ProgramUniform1uiEXT
#define glProgramUniform2uiEXT get_gl_entries ()->ProgramUniform2uiEXT
#define glProgramUniform3uiEXT get_gl_entries ()->ProgramUniform3uiEXT
#define glProgramUniform4uiEXT get_gl_entries ()->ProgramUniform4uiEXT
#define glProgramUniform1uivEXT get_gl_entries ()->ProgramUniform1uivEXT
#define glProgramUniform2uivEXT get_gl_entries ()->ProgramUniform2uivEXT
#define glProgramUniform3uivEXT get_gl_entries ()->ProgramUniform3uivEXT
#define glProgramUniform4uivEXT get_gl_entries ()->ProgramUniform4uivEXT
#define glProgramUniformMatrix2x3fvEXT get_gl_entries ()->ProgramUniformMatrix2x3fvEXT
#define glProgramUniformMatrix3x2fvEXT get_gl_entries ()->ProgramUniformMatrix3x2fvEXT
#define glProgramUniformMatrix2x4fvEXT get_gl_entries ()->ProgramUniformMatrix2x4fvEXT
#define glProgramUniformMatrix4x2fvEXT get_gl_entries ()->ProgramUniformMatrix4x2fvEXT
#define glProgramUniformMatrix3x4fvEXT get_gl_entries ()->ProgramUniformMatrix3x4fvEXT
#define glProgramUniformMatrix4x3fvEXT get_gl_entries ()->ProgramUniformMatrix4x3fvEXT
#define glTexStorage1DEXT get_gl_entries ()->TexStorage1DEXT
#define glTexStorage2DEXT get_gl_entries ()->TexStorage2DEXT
#define glTexStorage3DEXT get_gl_entries ()->TexStorage3DEXT
#define glTextureStorage1DEXT get_gl_entries ()->TextureStorage1DEXT
#define glTextureStorage2DEXT get_gl_entries ()->TextureStorage2DEXT
#define glTextureStorage3DEXT get_gl_entries ()->TextureStorage3DEXT
#define glRenderbufferStorageMultisampleIMG get_gl_entries ()->RenderbufferStorageMultisampleIMG
#define glFramebufferTexture2DMultisampleIMG get_gl_entries ()->FramebufferTexture2DMultisampleIMG
#define glBeginPerfQueryINTEL get_gl_entries ()->BeginPerfQueryINTEL
#define glCreatePerfQueryINTEL get_gl_entries ()->CreatePerfQueryINTEL
#define glDeletePerfQueryINTEL get_gl_entries ()->DeletePerfQueryINTEL
#define glEndPerfQueryINTEL get_gl_entries ()->EndPerfQueryINTEL
#define glGetFirstPerfQueryIdINTEL get_gl_entries ()->GetFirstPerfQueryIdINTEL
#define glGetNextPerfQueryIdINTEL get_gl_entries ()->GetNextPerfQueryIdINTEL
#define glGetPerfCounterInfoINTEL get_gl_entries ()->GetPerfCounterInfoINTEL
#define glGetPerfQueryDataINTEL get_gl_entries ()->GetPerfQueryDataINTEL
#define glGetPerfQueryIdByNameINTEL get_gl_entries ()->GetPerfQueryIdByNameINTEL
#define glGetPerfQueryInfoINTEL get_gl_entries ()->GetPerfQueryInfoINTEL
#define glBlendParameteriNV get_gl_entries ()->BlendParameteriNV
#define glBlendBarrierNV get_gl_entries ()->BlendBarrierNV
#define glCopyBufferSubDataNV get_gl_entries ()->CopyBufferSubDataNV
#define glCoverageMaskNV get_gl_entries ()->CoverageMaskNV
#define glCoverageOperationNV get_gl_entries ()->CoverageOperationNV
#define glDrawBuffersNV get_gl_entries ()->DrawBuffersNV
#define glDrawArraysInstancedNV get_gl_entries ()->DrawArraysInstancedNV
#define glDrawElementsInstancedNV get_gl_entries ()->DrawElementsInstancedNV
#define glDeleteFencesNV get_gl_entries ()->DeleteFencesNV
#define glGenFencesNV get_gl_entries ()->GenFencesNV
#define glIsFenceNV get_gl_entries ()->IsFenceNV
#define glTestFenceNV get_gl_entries ()->TestFenceNV
#define glGetFenceivNV get_gl_entries ()->GetFenceivNV
#define glFinishFenceNV get_gl_entries ()->FinishFenceNV
#define glSetFenceNV get_gl_entries ()->SetFenceNV
#define glBlitFramebufferNV get_gl_entries ()->BlitFramebufferNV
#define glRenderbufferStorageMultisampleNV get_gl_entries ()->RenderbufferStorageMultisampleNV
#define glVertexAttribDivisorNV get_gl_entries ()->VertexAttribDivisorNV
#define glUniformMatrix2x3fvNV get_gl_entries ()->UniformMatrix2x3fvNV
#define glUniformMatrix3x2fvNV get_gl_entries ()->UniformMatrix3x2fvNV
#define glUniformMatrix2x4fvNV get_gl_entries ()->UniformMatrix2x4fvNV
#define glUniformMatrix4x2fvNV get_gl_entries ()->UniformMatrix4x2fvNV
#define glUniformMatrix3x4fvNV get_gl_entries ()->UniformMatrix3x4fvNV
#define glUniformMatrix4x3fvNV get_gl_entries ()->UniformMatrix4x3fvNV
#define glReadBufferNV get_gl_entries ()->ReadBufferNV
#define glAlphaFuncQCOM get_gl_entries ()->AlphaFuncQCOM
#define glGetDriverControlsQCOM get_gl_entries ()->GetDriverControlsQCOM
#define glGetDriverControlStringQCOM get_gl_entries ()->GetDriverControlStringQCOM
#define glEnableDriverControlQCOM get_gl_entries ()->EnableDriverControlQCOM
#define glDisableDriverControlQCOM get_gl_entries ()->DisableDriverControlQCOM
#define glExtGetTexturesQCOM get_gl_entries ()->ExtGetTexturesQCOM
#define glExtGetBuffersQCOM get_gl_entries ()->ExtGetBuffersQCOM
#define glExtGetRenderbuffersQCOM get_gl_entries ()->ExtGetRenderbuffersQCOM
#define glExtGetFramebuffersQCOM get_gl_entries ()->ExtGetFramebuffersQCOM
#define glExtGetTexLevelParameterivQCOM get_gl_entries ()->ExtGetTexLevelParameterivQCOM
#define glExtTexObjectStateOverrideiQCOM get_gl_entries ()->ExtTexObjectStateOverrideiQCOM
#define glExtGetTexSubImageQCOM get_gl_entries ()->ExtGetTexSubImageQCOM
#define glExtGetBufferPointervQCOM get_gl_entries ()->ExtGetBufferPointervQCOM
#define glExtGetShadersQCOM get_gl_entries ()->ExtGetShadersQCOM
#define glExtGetProgramsQCOM get_gl_entries ()->ExtGetProgramsQCOM
#define glExtIsProgramBinaryQCOM get_gl_entries ()->ExtIsProgramBinaryQCOM
#define glExtGetProgramBinarySourceQCOM get_gl_entries ()->ExtGetProgramBinarySourceQCOM
#define glStartTilingQCOM get_gl_entries ()->StartTilingQCOM
#define glEndTilingQCOM get_gl_entries ()->EndTilingQCOM

}

}

}

#endif
