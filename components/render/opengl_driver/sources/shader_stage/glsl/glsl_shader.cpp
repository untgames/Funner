#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

//Константы
const char*  DEFINE_TEXT              = "#define ";
const size_t DEFINE_ADDITIONAL_LENGTH = xtl::xstrlen (DEFINE_TEXT) + 1;

}

/*
    Конструктор
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

    if (glCreateShader)
    {
      handle = glCreateShader (type);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      handle = glCreateShaderObjectARB (type);
    }
#endif

    if (!handle)
      RaiseError (METHOD_NAME);

      //компиляция шейдера

    GLint compile_status = 0;

    common::StringArray defines = common::split (desc.options);

    stl::string defines_string;

#ifdef OPENGL_ES2_SUPPORT
    defines_string = "#define OPENGL_ES2_SUPPORT\n";
#endif

    defines_string.reserve (defines_string.length () + xtl::xstrlen (desc.options) + defines.Size () * DEFINE_ADDITIONAL_LENGTH);

    for (size_t i = 0, count = defines.Size (); i < count; i++)
      defines_string += common::format ("%s %s\n", DEFINE_TEXT, defines [i]);

      //replace '=' with ' '
    for (char* current_symbol = defines_string.begin (); *current_symbol; current_symbol++)
    {
      if (*current_symbol == '=')
        *current_symbol = ' ';
    }

    const char* source_codes []       = { defines_string.c_str (), desc.source_code };
    GLint       source_codes_sizes [] = { defines_string.length (), desc.source_code_size };

    if (glShaderSource)
    {
      glShaderSource (handle, 2, source_codes, source_codes_sizes);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glShaderSourceARB (handle, 2, source_codes, source_codes_sizes);
    }
#endif

    if (glCompileShader)
    {
      glCompileShader (handle);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glCompileShaderARB (handle);
    }
#endif

    if (glGetShaderiv)
    {
      glGetShaderiv (handle, GL_COMPILE_STATUS, &compile_status);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glGetObjectParameterivARB (handle, GL_COMPILE_STATUS, &compile_status);
    }
#endif

      //протоколирование ошибок компиляции

    GLint log_length = 0;

    if (glGetShaderiv)
    {
      glGetShaderiv (handle, GL_INFO_LOG_LENGTH, &log_length);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glGetObjectParameterivARB (handle, GL_INFO_LOG_LENGTH, &log_length);
    }
#endif

    if (log_length)
    {
      stl::string log_buffer;

      log_buffer.resize (log_length - 1);

      GLsizei getted_log_size = 0;

      if (glGetShaderInfoLog)
      {
        glGetShaderInfoLog (handle, log_length, &getted_log_size, &log_buffer [0]);
      }
#ifndef OPENGL_ES2_SUPPORT
      else
      {
        glGetInfoLogARB (handle, log_length, &getted_log_size, &log_buffer [0]);
      }
#endif

      if (getted_log_size)
        log_buffer.resize (getted_log_size - 1);

      if (desc.name) error_log (common::format ("%s: %s", desc.name, log_buffer.c_str ()).c_str ());
      else           error_log (log_buffer.c_str ());
    }

      //проверка состояния OpenGL

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
    //подавляем все исключения
  }
}

/*
   Удаление шейдеров
*/

void GlslShader::DeleteShader ()
{
  if (!handle)
    return;

    //установка текущего контекста

  MakeContextCurrent ();

    //удаление шейдера

  if (glDeleteShader)
  {
    glDeleteShader (handle);
  }
#ifndef OPENGL_ES2_SUPPORT
  else
  {
    glDeleteObjectARB (handle);
  }
#endif

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::GlslShader::DeleteShader");
}
