#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

GlslProgram::GlslProgram (const ContextManager& manager, size_t shaders_count, IShader** in_shaders, const LogFunction& error_log)
  : ContextObject (manager),
    handle (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslProgram::GlslProgram";

  try
  {
      //установка текущего контекста

    MakeContextCurrent ();

      //создание программы

    if (glCreateProgram) handle = glCreateProgram ();
    else                 handle = glCreateProgramObjectARB ();

    if (!handle)
      RaiseError (METHOD_NAME);

      //присоединение шейдеров к программе

    shaders.reserve (shaders_count);

    for (size_t i=0; i<shaders_count; i++)
    {
      ShaderPtr shader = cast_object<GlslShader> (*this, in_shaders [i], METHOD_NAME, "shaders");

      if (!shader)
        throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "shaders[%u]", i);

      if (glAttachShader) glAttachShader    (handle, shader->GetHandle ());
      else                glAttachObjectARB (handle, shader->GetHandle ());

      shaders.push_back (shader);
    }

      //линковка программы

    GLint link_status = 0;

    if (glLinkProgram) glLinkProgram    (handle);
    else               glLinkProgramARB (handle);

      //протоколирование ошибок

    if (glGetProgramiv) glGetProgramiv            (handle, GL_LINK_STATUS, &link_status);
    else                glGetObjectParameterivARB (handle, GL_LINK_STATUS, &link_status);

    stl::string log_buffer;

    GetProgramLog (log_buffer);
    error_log     (log_buffer.c_str ());

      //проверка состояния программы

    if (link_status)
    {
      if (glValidateProgram) glValidateProgram    (handle);
      else                   glValidateProgramARB (handle);

      if (glGetProgramiv) glGetProgramiv            (handle, GL_OBJECT_VALIDATE_STATUS_ARB, &link_status);
      else                glGetObjectParameterivARB (handle, GL_OBJECT_VALIDATE_STATUS_ARB, &link_status);

      GetProgramLog (log_buffer);
      error_log     (log_buffer.c_str ());
    }

      //проверка ошибок

    if (!link_status)
      RaiseError (METHOD_NAME);

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    DeleteProgram ();
    throw;
  }
}

GlslProgram::~GlslProgram ()
{
  try
  {
    DeleteProgram ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::GlslProgram::~GlslProgram");

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
    Удаление программы
*/

void GlslProgram::DeleteProgram ()
{
  if (!handle)
    return;

    //установка текущего контекста

  MakeContextCurrent ();

    //отсоединение шейдеров от программы

  for (ShaderArray::iterator iter=shaders.begin (), end=shaders.end (); iter!=end; ++iter)
  {
    if (glDetachShader) glDetachShader    (handle, (*iter)->GetHandle ());
    else                glDetachObjectARB (handle, (*iter)->GetHandle ());
  }

    //удаление программы

  if (glDeleteProgram) glDeleteProgram   (handle);
  else                 glDeleteObjectARB (handle);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::GlslProgram::DeleteProgram");
}

/*
    Получение протокола ошибок OpenGL
*/

void GlslProgram::GetProgramLog (stl::string& log_buffer)
{
    //очистка буфера

  log_buffer.clear ();

    //установка текущего контекста

  MakeContextCurrent ();

    //получение размера строки состояния программы

  GLint log_length = 0;

  if (glGetProgramiv) glGetProgramiv            (handle, GL_INFO_LOG_LENGTH, &log_length);
  else                glGetObjectParameterivARB (handle, GL_INFO_LOG_LENGTH, &log_length);

  if (!log_length)
    return;

    //чтение строки состояния

  GLsizei getted_log_size = 0;

  log_buffer.resize (log_length - 1);

  if (glGetProgramInfoLog) glGetProgramInfoLog (handle, log_length, &getted_log_size, &log_buffer [0]);
  else                     glGetInfoLogARB     (handle, log_length, &getted_log_size, &log_buffer [0]);

  if (getted_log_size)
    log_buffer.resize (getted_log_size - 1);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::GlslProgram::GetProgramLog");
}

/*
    Создание программы, устанавливаемой в контекст OpenGL
*/

IBindableProgram* GlslProgram::CreateBindableProgram (ProgramParametersLayout* layout)
{
  try
  {
    return new GlslBindableProgram (GetContextManager (), *this, layout);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::GlslProgram::CreateBindableProgram");
    throw;
  }
}
