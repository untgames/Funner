#ifndef RENDER_GL_DRIVER_GLES2_PROFILE_HEADER
#define RENDER_GL_DRIVER_GLES2_PROFILE_HEADER

#include <shared/profile/gles2/gl_types.h>
#include <shared/profile/gles2/gl2ext.h>
#include <shared/profile/gles2/entries.h>

#define glBlendEquationOES glBlendEquation
#define glBlendEquationSeparateOES glBlendEquationSeparate
#define glBlendFuncSeparateOES glBlendFuncSeparate
#define glBindFramebufferOES glBindFramebuffer
#define glIsRenderbufferOES glIsRenderbuffer
#define glBindRenderbufferOES glBindRenderbuffer
#define glDeleteRenderbuffersOES glDeleteRenderbuffers
#define glGenRenderbuffersOES glGenRenderbuffers
#define glRenderbufferStorageOES glRenderbufferStorage
#define glGetRenderbufferParameterivOES glGetRenderbufferParameteriv
#define glIsFramebufferOES glIsFramebuffer
#define glBindFramebufferOES glBindFramebuffer
#define glDeleteFramebuffersOES glDeleteFramebuffers
#define glGenFramebuffersOES glGenFramebuffers
#define glCheckFramebufferStatusOES glCheckFramebufferStatus
#define glFramebufferRenderbufferOES glFramebufferRenderbuffer
#define glFramebufferTexture2DOES glFramebufferTexture2D
#define glGetFramebufferAttachmentParameterivOES glGetFramebufferAttachmentParameteriv
#define glGenerateMipmapOES glGenerateMipmap

#define glTexImage3D glTexImage3DOES
#define glTexSubImage3D glTexSubImage3DOES
#define glCopyTexSubImage3D glCopyTexSubImage3DOES
#define glCompressedTexImage3D glCompressedTexImage3DOES
#define glCompressedTexSubImage3D glCompressedTexSubImage3DOES
#define glFramebufferTexture3D glFramebufferTexture3DOES
#define glTexImage3DEXT glTexImage3DOES
#define glTexSubImage3DEXT glTexSubImage3DOES
#define glCopyTexSubImage3DEXT glCopyTexSubImage3DOES
#define glCompressedTexImage3DEXT glCompressedTexImage3DOES
#define glCompressedTexSubImage3DEXT glCompressedTexSubImage3DOES
#define glFramebufferTexture3DEXT glFramebufferTexture3DOES

#define glStencilOpSeparateATI glStencilOpSeparate
#define glStencilFuncSeparateATI glStencilFuncSeparate

#define glGetActiveUniformARB glGetActiveUniform

#define GL_OBJECT_VALIDATE_STATUS_ARB GL_VALIDATE_STATUS
#define GL_FLOAT_VEC2_ARB GL_FLOAT_VEC2
#define GL_FLOAT_VEC3_ARB GL_FLOAT_VEC3
#define GL_FLOAT_VEC4_ARB GL_FLOAT_VEC4
#define GL_INT_VEC2_ARB GL_INT_VEC2
#define GL_INT_VEC3_ARB GL_INT_VEC3
#define GL_INT_VEC4_ARB GL_INT_VEC4
#define GL_BOOL_ARB GL_BOOL
#define GL_BOOL_VEC2_ARB GL_BOOL_VEC2
#define GL_BOOL_VEC3_ARB GL_BOOL_VEC3
#define GL_BOOL_VEC4_ARB GL_BOOL_VEC4
#define GL_FLOAT_MAT2_ARB GL_FLOAT_MAT2
#define GL_FLOAT_MAT3_ARB GL_FLOAT_MAT3
#define GL_FLOAT_MAT4_ARB GL_FLOAT_MAT4
#define GL_SAMPLER_2D_ARB GL_SAMPLER_2D
#define GL_SAMPLER_CUBE_ARB GL_SAMPLER_CUBE

#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB GL_MAX_TEXTURE_IMAGE_UNITS
#define GL_STACK_OVERFLOW GL_STACK_OVERFLOW_KHR
#define GL_STACK_UNDERFLOW GL_STACK_UNDERFLOW_KHR
#define GL_STENCIL_INDEX8_OES GL_STENCIL_INDEX8

#define GL_TEXTURE_WRAP_R GL_TEXTURE_WRAP_R_OES
#define GL_TEXTURE_3D GL_TEXTURE_3D_OES
#define GL_TEXTURE_BINDING_3D GL_TEXTURE_BINDING_3D_OES
#define GL_MAX_3D_TEXTURE_SIZE GL_MAX_3D_TEXTURE_SIZE_OES
#define GL_SAMPLER_3D GL_SAMPLER_3D_OES
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES
#define GL_TEXTURE_WRAP_R_EXT GL_TEXTURE_WRAP_R_OES
#define GL_TEXTURE_3D_EXT GL_TEXTURE_3D_OES
#define GL_TEXTURE_BINDING_3D_EXT GL_TEXTURE_BINDING_3D_OES
#define GL_MAX_3D_TEXTURE_SIZE_EXT GL_MAX_3D_TEXTURE_SIZE_OES
#define GL_SAMPLER_3D_EXT GL_SAMPLER_3D_OES
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES
#define GL_FUNC_ADD_EXT GL_FUNC_ADD
#define GL_BLEND_EQUATION_EXT GL_BLEND_EQUATION
#define GL_BLEND_EQUATION_RGB_EXT GL_BLEND_EQUATION_RGB
#define GL_BLEND_EQUATION_ALPHA_EXT GL_BLEND_EQUATION_ALPHA
#define GL_FUNC_SUBTRACT_EXT GL_FUNC_SUBTRACT
#define GL_FUNC_REVERSE_SUBTRACT_EXT GL_FUNC_REVERSE_SUBTRACT

typedef GLuint GLhandleARB;

/*
    ѕереопределени€ дл€ упрощени€ портируемости
*/

typedef PFNGLFRAMEBUFFERTEXTURE3DOESPROC PFNGLFRAMEBUFFERTEXTURE3DPROC;

typedef void (GL_APIENTRY* PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (GL_APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (GL_APIENTRY* PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void (GL_APIENTRY* PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (GL_APIENTRY* PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (GL_APIENTRY* PFNGLBLENDFUNCSEPARATEPROC)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (GL_APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (GL_APIENTRY* PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (GL_APIENTRY* PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (GL_APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (GL_APIENTRY* PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint* renderbuffers);
typedef void (GL_APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (GL_APIENTRY* PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint* renderbuffers);
typedef void (GL_APIENTRY* PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef GLboolean (GL_APIENTRY* PFNGLISRENDERBUFFERPROC)(GLuint renderbuffer);
typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY* PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef GLboolean (GL_APIENTRY* PFNGLISFRAMEBUFFERPROC)(GLuint framebuffer);
typedef void (GL_APIENTRY* PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (GL_APIENTRY* PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
typedef void (GL_APIENTRY* PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef GLenum (GL_APIENTRY* PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (GL_APIENTRY* PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
typedef void (GL_APIENTRY* PFNGLGENERATEMIPMAPPROC)(GLenum target);

typedef int  (GL_APIENTRY* PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (GL_APIENTRY* PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* v);
typedef void (GL_APIENTRY* PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint* v);
typedef void (GL_APIENTRY* PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (GL_APIENTRY* PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (GL_APIENTRY* PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (GL_APIENTRY* PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (GL_APIENTRY* PFNGLVERTEXATTRIBPOINTERPROC)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
typedef void (GL_APIENTRY* PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (GL_APIENTRY* PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);

#endif
