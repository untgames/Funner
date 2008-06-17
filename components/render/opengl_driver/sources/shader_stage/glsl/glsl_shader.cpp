#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор
*/

GlslShader::GlslShader (const ContextManager& manager, const ShaderDesc& shader_desc, const LogFunction& error_log)
  : ContextObject (manager), shader (0)
{
  stl::string log_buffer;

  static const char* METHOD_NAME = "render::low_level::opengl::GlslShader::GlslShader";

  if (!shader_desc.profile)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shader_desc.profile");
  
  if (!shader_desc.source_code)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shader_desc.source_code");
  
  size_t shader_type = get_shader_type (shader_desc.profile);

  if (shader_type == -1)
    throw xtl::make_argument_exception (METHOD_NAME, "shader_desc.profile", shader_desc.profile, "Shader has uncknown type");

  MakeContextCurrent ();

  try
  {
    GLenum gl_shader_type;

    switch (shader_type)
    {
      case GlslShaderType_Fragment: 
        if (GetCaps ().has_arb_fragment_shader) 
          gl_shader_type = GL_FRAGMENT_SHADER;
        else
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create fragment shader ('GL_ARB_fragment_shader' extension not supported)");
        
        break;
      case GlslShaderType_Vertex: 
        if (GetCaps ().has_arb_vertex_shader) 
          gl_shader_type = GL_VERTEX_SHADER;
        else
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create vertex shader ('GL_ARB_vertex_shader' extension not supported)");

        break;
      default: throw xtl::format_exception<xtl::not_implemented_exception> (METHOD_NAME, "Shaders with profile '%s' not implemented", shader_desc.profile);
    }

    if (glCreateShader) shader = glCreateShader (gl_shader_type);
    else                shader = glCreateShaderObjectARB (gl_shader_type);

    if (!shader)
      RaiseError (METHOD_NAME);

    int compile_status = 0;

    if (glShaderSource) glShaderSource    (shader, 1, &((ShaderDesc&)shader_desc).source_code, (int*)&shader_desc.source_code_size);
    else                glShaderSourceARB (shader, 1, &((ShaderDesc&)shader_desc).source_code, (int*)&shader_desc.source_code_size);

    if (glCompileShader) glCompileShader    (shader);
    else                 glCompileShaderARB (shader);

    if (glGetShaderiv) glGetShaderiv             (shader, GL_COMPILE_STATUS, &compile_status);
    else               glGetObjectParameterivARB (shader, GL_COMPILE_STATUS, &compile_status);

    GetShaderLog (log_buffer);

    error_log (log_buffer.c_str ());

    if (!compile_status)
      RaiseError (METHOD_NAME);

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    DeleteShader ();
    throw;
  }
}

GlslShader::~GlslShader ()
{
  try
  {
    DeleteShader ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::GlslShader::~GlslShader");

    LogPrintf ("%s", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
   Удаление шейдеров
*/

void GlslShader::DeleteShader ()
{
  MakeContextCurrent ();

  if (shader)
  {
    if (glDeleteShader) glDeleteShader    (shader);
    else                glDeleteObjectARB (shader);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::DeleteShader");
}

/*
   Получение лога OpenGL
*/

void GlslShader::GetShaderLog (stl::string& log_buffer)
{
  int log_length = 0;

  log_buffer.clear ();

  MakeContextCurrent ();

  if (glGetShaderiv) glGetShaderiv             (shader, GL_INFO_LOG_LENGTH, &log_length);
  else               glGetObjectParameterivARB (shader, GL_INFO_LOG_LENGTH, &log_length);

  if (log_length)
  {
    int getted_log_size = 0;

    log_buffer.resize (log_length - 1);

    if (glGetShaderInfoLog) glGetShaderInfoLog (shader, log_length, &getted_log_size, &log_buffer [0]);
    else                    glGetInfoLogARB    (shader, log_length, &getted_log_size, &log_buffer [0]);
    
    if (getted_log_size)
      log_buffer.resize (getted_log_size - 1);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::GetShaderLog");
}
