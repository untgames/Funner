#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

void* get_uniform_pointer (const ShaderParameter& shader_parameter_layout, ConstantBufferPtr* constant_buffers)
{
  if (!constant_buffers[shader_parameter_layout.slot])
    return NULL;

  return (char*)constant_buffers[shader_parameter_layout.slot]->GetDataPointer () + shader_parameter_layout.offset;
}

}

/*
   Конструктор
*/

GlslShader::GlslShader (const ContextManager& manager, size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
  : Shader (manager), program (0)
{
  stl::string log_buffer;

  static const char* METHOD_NAME = "render::low_level::opengl::GlslShader::GlslShader";

  for (size_t i = 0; i < shaders_count; i++)
  {
    if (!shader_descs[i].profile)
      RaiseNullArgument (METHOD_NAME, "shader_descs.profile");
    
    if (!shader_descs[i].source_code)
      RaiseNullArgument (METHOD_NAME, "shader_descs.source_code");
    
    size_t shader_type = get_shader_type (shader_descs[i].profile);

    if (shader_type == -1)
      RaiseInvalidArgument (METHOD_NAME, "shader_descs.profile", shader_descs[i].profile, "Shader has uncknown type");
  }

  shader[0] = shader[1] = 0;
  
  MakeContextCurrent ();

  try
  {
    if (glCreateProgram) program = glCreateProgram ();
    else                 program = glCreateProgramObjectARB ();

    if (!program)
      RaiseError (METHOD_NAME);

    if (glCreateShader) 
    {
      shader[0] = glCreateShader (GL_FRAGMENT_SHADER);
      shader[1] = glCreateShader (GL_VERTEX_SHADER);
    }
    else
    {
      shader[0] = glCreateShaderObjectARB (GL_FRAGMENT_SHADER);
      shader[1] = glCreateShaderObjectARB (GL_VERTEX_SHADER);
    }

    if (!shader[0] || !shader[1])
      RaiseError (METHOD_NAME);

    int compile_status = 0;

    for (size_t i = 0; i < shaders_count; i++)
    {
      size_t shader_type = get_shader_type (shader_descs[i].profile);

      descs[shader_type] = shader_descs[i];

      if (glShaderSource) glShaderSource    (shader[shader_type], 1, &descs[shader_type].source_code, (int*)&descs[shader_type].source_code_size);
      else                glShaderSourceARB (shader[shader_type], 1, &descs[shader_type].source_code, (int*)&descs[shader_type].source_code_size);

      if (glCompileShader) glCompileShader    (shader[shader_type]);
      else                 glCompileShaderARB (shader[shader_type]);

      if (glGetShaderiv) glGetShaderiv             (shader[shader_type], GL_COMPILE_STATUS, &compile_status);
      else               glGetObjectParameterivARB (shader[shader_type], GL_COMPILE_STATUS, &compile_status);

      GetShaderLog (shader_type, log_buffer);

      error_log (log_buffer.c_str ());

      if (!compile_status)
        RaiseError (METHOD_NAME);

      if (!is_shader_attached[shader_type])
      {
        if (glAttachShader) glAttachShader    (program, shader[shader_type]);
        else                glAttachObjectARB (program, shader[shader_type]);

        is_shader_attached[shader_type] = true;
      }
    }

    if (glLinkProgram) glLinkProgram    (program);
    else               glLinkProgramARB (program);
    
    if (glGetProgramiv) glGetProgramiv            (program, GL_LINK_STATUS, &compile_status);
    else                glGetObjectParameterivARB (program, GL_LINK_STATUS, &compile_status);

      //Возможно нет необходимости делать валидацию
    if (compile_status)
    {
      if (glValidateProgram) glValidateProgram    (program);
      else                   glValidateProgramARB (program);

      if (glGetProgramiv) glGetProgramiv            (program, GL_OBJECT_VALIDATE_STATUS_ARB, &compile_status);
      else                glGetObjectParameterivARB (program, GL_OBJECT_VALIDATE_STATUS_ARB, &compile_status);
    }

    GetProgramLog (log_buffer);

    error_log (log_buffer.c_str ());

    if (!compile_status)
      RaiseError (METHOD_NAME);

    CheckErrors (METHOD_NAME);

    log_buffer.reserve (4096);
  }
  catch (std::exception& e)
  {
    DeletePrograms ();
    throw e;
  }
}

GlslShader::~GlslShader ()
{
  DeletePrograms ();
}

/*
   Удаление шейдеров
*/

void GlslShader::DeletePrograms ()
{
  MakeContextCurrent ();

  for (size_t i = 0; i < GlslShaderType_Num; i++)
  {
    if (!shader[i])
      continue;

    if (is_shader_attached[i])
    {
      if (glDetachShader) glDetachShader    (program, shader[i]);
      else                glDetachObjectARB (program, shader[i]);
    }

    if (glDeleteShader) glDeleteShader    (shader[i]);
    else                glDeleteObjectARB (shader[i]);
  }

  if (program)
  {
    if (glDeleteProgram) glDeleteProgram   (program);
    else                 glDeleteObjectARB (program);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::DeletePrograms");
}

/*
   Биндинг
*/

void GlslShader::Bind (ConstantBufferPtr* constant_buffers, ShaderParametersLayout* shader_parameters_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslShader::Bind";

  if (!constant_buffers)
    RaiseNullArgument (METHOD_NAME, "constant_buffers");

  if (!shader_parameters_layout)
    RaiseNullArgument (METHOD_NAME, "shader_parameters_layout");

  MakeContextCurrent ();

  PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_fn = 0;
  PFNGLUNIFORM1IPROC          glUniform1i_fn          = 0;
  PFNGLUNIFORM1FPROC          glUniform1f_fn          = 0;
  PFNGLUNIFORM2IVPROC         glUniform2iv_fn         = 0;
  PFNGLUNIFORM2FVPROC         glUniform2fv_fn         = 0;
  PFNGLUNIFORM3IVPROC         glUniform3iv_fn         = 0;
  PFNGLUNIFORM3FVPROC         glUniform3fv_fn         = 0;
  PFNGLUNIFORM4IVPROC         glUniform4iv_fn         = 0;
  PFNGLUNIFORM4FVPROC         glUniform4fv_fn         = 0;
  PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv_fn   = 0;
  PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv_fn   = 0;
  PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv_fn   = 0;
    
  if (glGetUniformLocation) glGetUniformLocation_fn = glGetUniformLocation;
  else                      glGetUniformLocation_fn = (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocationARB;

  if (glUniform1i) glUniform1i_fn = glUniform1i;
  else             glUniform1i_fn = glUniform1iARB;

  if (glUniform1f) glUniform1f_fn = glUniform1f;
  else             glUniform1f_fn = glUniform1fARB;

  if (glUniform2iv) glUniform2iv_fn = glUniform2iv;
  else              glUniform2iv_fn = glUniform2ivARB;

  if (glUniform2fv) glUniform2fv_fn = glUniform2fv;
  else              glUniform2fv_fn = glUniform2fvARB;

  if (glUniform3iv) glUniform3iv_fn = glUniform3iv;
  else              glUniform3iv_fn = glUniform3ivARB;

  if (glUniform3fv) glUniform3fv_fn = glUniform3fv;
  else              glUniform3fv_fn = glUniform3fvARB;

  if (glUniform4iv) glUniform4iv_fn = glUniform4iv;
  else              glUniform4iv_fn = glUniform4ivARB;

  if (glUniform4fv) glUniform4fv_fn = glUniform4fv;
  else              glUniform4fv_fn = glUniform4fvARB;

  if (glUniformMatrix2fv) glUniformMatrix2fv_fn = glUniformMatrix2fv;
  else                    glUniformMatrix2fv_fn = glUniformMatrix2fvARB;

  if (glUniformMatrix3fv) glUniformMatrix3fv_fn = glUniformMatrix3fv;
  else                    glUniformMatrix3fv_fn = glUniformMatrix3fvARB;

  if (glUniformMatrix4fv) glUniformMatrix4fv_fn = glUniformMatrix4fv;
  else                    glUniformMatrix4fv_fn = glUniformMatrix4fvARB;

  ShaderParametersLayoutDesc layout_desc;

  shader_parameters_layout->GetDesc (layout_desc); //!!!!

  if (glUseProgram) glUseProgram          (program);
  else              glUseProgramObjectARB (program);

  for (size_t i = 0; i < layout_desc.parameters_count; i++)
  {
    int parameter_location = glGetUniformLocation (program, layout_desc.parameters[i].name); //!!! Необходимо реализовать однократное развязывание имён

    if (parameter_location == -1)
      LogPrintf ("Unreferenced uniform parameter '%s'", layout_desc.parameters [i].name);

    void* uniform_pointer = get_uniform_pointer (layout_desc.parameters[i], constant_buffers);

    if (!uniform_pointer)
      RaiseInvalidOperation (METHOD_NAME, "No needed constant buffer %u for uniform '%s'", 
                             layout_desc.parameters[i].slot, layout_desc.parameters[i].name);

    switch (layout_desc.parameters[i].type)
    {
      case ShaderParameterType_Int:      glUniform1i_fn        (parameter_location, *(int*)uniform_pointer);        break;
      case ShaderParameterType_Float:    glUniform1f_fn        (parameter_location, *(float*)uniform_pointer);      break;
      case ShaderParameterType_Int2:     glUniform2iv_fn       (parameter_location, 1, (int*)uniform_pointer);      break;
      case ShaderParameterType_Float2:   glUniform2fv_fn       (parameter_location, 1, (float*)uniform_pointer);    break;
      case ShaderParameterType_Int3:     glUniform3iv_fn       (parameter_location, 1, (int*)uniform_pointer);      break;
      case ShaderParameterType_Float3:   glUniform3fv_fn       (parameter_location, 1, (float*)uniform_pointer);    break;
      case ShaderParameterType_Int4:     glUniform4iv_fn       (parameter_location, 1, (int*)uniform_pointer);      break;
      case ShaderParameterType_Float4:   glUniform4fv_fn       (parameter_location, 1, (float*)uniform_pointer);    break;
      case ShaderParameterType_Float2x2: glUniformMatrix2fv_fn (parameter_location, 1, 0, (float*)uniform_pointer); break;
      case ShaderParameterType_Float3x3: glUniformMatrix3fv_fn (parameter_location, 1, 0, (float*)uniform_pointer); break;
      case ShaderParameterType_Float4x4: glUniformMatrix4fv_fn (parameter_location, 1, 0, (float*)uniform_pointer); break;
    }
  }

  CheckErrors (METHOD_NAME);
}

/*
   Получение лога OpenGL
*/

void GlslShader::GetShaderLog (size_t shader_index, stl::string& log_buffer)
{
  int log_length = 0;

  MakeContextCurrent ();

  if (glGetShaderiv) glGetShaderiv             (shader [shader_index], GL_INFO_LOG_LENGTH, &log_length);
  else               glGetObjectParameterivARB (shader [shader_index], GL_INFO_LOG_LENGTH, &log_length);

  if (log_length)
  {
    int getted_log_size = 0;

    log_buffer.resize (log_length - 1);

    if (glGetShaderInfoLog) glGetShaderInfoLog (shader [shader_index], log_length, &getted_log_size, &log_buffer [0]);
    else                    glGetInfoLogARB    (shader [shader_index], log_length, &getted_log_size, &log_buffer [0]);
    
    log_buffer.resize (getted_log_size - 1);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::GetShaderLog");
}

void GlslShader::GetProgramLog (stl::string& log_buffer)
{
  int log_length = 0;

  MakeContextCurrent ();

  if (glGetProgramiv) glGetProgramiv            (program, GL_INFO_LOG_LENGTH, &log_length);
  else                glGetObjectParameterivARB (program, GL_INFO_LOG_LENGTH, &log_length);

  if (log_length)
  {
    int getted_log_size = 0;

    log_buffer.resize (log_length - 1);

    if (glGetProgramInfoLog) glGetProgramInfoLog (program, log_length, &getted_log_size, &log_buffer [0]);
    else                     glGetInfoLogARB     (program, log_length, &getted_log_size, &log_buffer [0]);
    
    log_buffer.resize (getted_log_size - 1);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::GetProgramLog");
}
