#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
     онструктор
*/

GlslShader::GlslShader (const ContextManager& manager, GLenum type, const ShaderDesc& desc, const LogFunction& error_log)
  : ContextObject (manager),
    handle (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslShader::GlslShader";

  try
  {
      //проверка корректности аргументов

    switch (type)
    {
      case GL_FRAGMENT_SHADER:
        if (!GetCaps ().has_arb_fragment_shader)
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create fragment shader (GL_ARB_fragment_shader not supported)");

        break;
      case GL_VERTEX_SHADER:
        if (!GetCaps ().has_arb_vertex_shader)
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create vertex shader (GL_ARB_vertex_shader not supported)");

        break;
      default:
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported shader profile '%s'", desc.profile);
    }

      //установка текущего контекста

    MakeContextCurrent ();

      //создание шейдера

    if (glCreateShader) handle = glCreateShader (type);
    else                handle = glCreateShaderObjectARB (type);

    if (!handle)
      RaiseError (METHOD_NAME);

      //компил€ци€ шейдера

    GLint compile_status = 0;

    if (glShaderSource) glShaderSource    (handle, 1, (const char**)&desc.source_code, (GLint*)&desc.source_code_size);
    else                glShaderSourceARB (handle, 1, (const char**)&desc.source_code, (GLint*)&desc.source_code_size);

    if (glCompileShader) glCompileShader    (handle);
    else                 glCompileShaderARB (handle);

    if (glGetShaderiv) glGetShaderiv             (handle, GL_COMPILE_STATUS, &compile_status);
    else               glGetObjectParameterivARB (handle, GL_COMPILE_STATUS, &compile_status);

      //протоколирование ошибок компил€ции

    GLint log_length = 0;

    if (glGetShaderiv) glGetShaderiv             (handle, GL_INFO_LOG_LENGTH, &log_length);
    else               glGetObjectParameterivARB (handle, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length)
    {
      stl::string log_buffer;

      log_buffer.resize (log_length - 1);

      GLsizei getted_log_size = 0;

      if (glGetShaderInfoLog) glGetShaderInfoLog (handle, log_length, &getted_log_size, &log_buffer [0]);
      else                    glGetInfoLogARB    (handle, log_length, &getted_log_size, &log_buffer [0]);

      if (getted_log_size)
        log_buffer.resize (getted_log_size - 1);

      if (desc.name) error_log (common::format ("%s: %s", desc.name, log_buffer.c_str ()).c_str ());
      else           error_log (log_buffer.c_str ());
    }

      //проверка состо€ни€ OpenGL

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
    if (TryMakeContextCurrent ())
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
    //подавл€ем все исключени€
  }
}

/*
   ”даление шейдеров
*/

void GlslShader::DeleteShader ()
{
  if (!handle)
    return;

    //установка текущего контекста

  MakeContextCurrent ();

    //удаление шейдера

  if (glDeleteShader) glDeleteShader    (handle);
  else                glDeleteObjectARB (handle);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::GlslShader::DeleteShader");
}
