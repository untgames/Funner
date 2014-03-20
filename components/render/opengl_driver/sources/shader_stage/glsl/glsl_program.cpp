#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

GlslProgram::GlslProgram (const ContextManager& manager, size_t shaders_count, IShader** in_shaders, const LogFunction& error_log)
  : ContextObject (manager),
    handle (0),
    last_bindable_program_id (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslProgram::GlslProgram";

  try
  {
      //установка текущего контекста

    MakeContextCurrent ();

      //создание программы

    if (glCreateProgram)
    {
      handle = glCreateProgram ();
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      handle = glCreateProgramObjectARB ();
    }
#endif

    if (!handle)
      RaiseError (METHOD_NAME);

      //присоединение шейдеров к программе

    shaders.reserve (shaders_count);

    for (size_t i=0; i<shaders_count; i++)
    {
      ShaderPtr shader = cast_object<GlslShader> (*this, in_shaders [i], METHOD_NAME, "shaders");

      if (!shader)
        throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "shaders[%u]", i);

      if (glAttachShader)
      {
        glAttachShader (handle, shader->GetHandle ());
      }
#ifndef OPENGL_ES2_SUPPORT
      else
      {
        glAttachObjectARB (handle, shader->GetHandle ());
      }
#endif

      shaders.push_back (shader);
    }

      //линковка программы

    GLint link_status = 0;

    if (glLinkProgram)
    {
      glLinkProgram (handle);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glLinkProgramARB (handle);
    }
#endif

      //протоколирование ошибок

    if (glGetProgramiv)
    {
      glGetProgramiv (handle, GL_LINK_STATUS, &link_status);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glGetObjectParameterivARB (handle, GL_LINK_STATUS, &link_status);
    }
#endif

    stl::string log_buffer;

    GetProgramLog (log_buffer);
    error_log     (common::format ("linker: %s", log_buffer.c_str ()).c_str ());
    
      //получение описания параметров и атрибутов программы
    
    GLint parameters_count = 0, max_parameter_name_length = 0, attributes_count = 0, max_attribute_name_length = 0;
    
    if (glGetProgramiv)
    {
      glGetProgramiv (handle, GL_ACTIVE_UNIFORMS, &parameters_count);
      glGetProgramiv (handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_parameter_name_length);
      glGetProgramiv (handle, GL_ACTIVE_ATTRIBUTES, &attributes_count);
      glGetProgramiv (handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attribute_name_length);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &parameters_count);
      glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &max_parameter_name_length);
      glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &attributes_count);
      glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB, &max_attribute_name_length);
    }
#endif

    parameters.reserve (parameters_count);        
    
    stl::string parameter_name;    

    for (GLint location=0; location<parameters_count; location++)
    {            
      parameter_name.fast_resize (max_parameter_name_length);      
      
      GLint  name_length = 0, elements_count = 0;
      GLenum type = 0;
      
      glGetActiveUniformARB (handle, location, parameter_name.size (), &name_length, &elements_count, &type, &parameter_name [0]);
      
      if ((size_t)name_length > parameter_name.size ())
        name_length = parameter_name.size ();
        
      if (name_length < 0)
        name_length = 0;
        
      parameter_name.fast_resize (name_length);

      if (strstr (parameter_name.c_str (), "[0]") == parameter_name.end () - 3)  //Обход особенности именования массивов в драйверах nVidia
        parameter_name.fast_resize (parameter_name.size () - 3);
      
      Parameter parameter;
      
      parameter.name           = parameter_name;
      parameter.name_hash      = common::strhash (parameter.name.c_str ());
      parameter.elements_count = (size_t)elements_count;      
      
      switch (type)
      {
        case GL_FLOAT:
          parameter.type = ProgramParameterType_Float;
          break;
        case GL_FLOAT_VEC2_ARB:
          parameter.type = ProgramParameterType_Float2;
          break;
        case GL_FLOAT_VEC3_ARB:
          parameter.type = ProgramParameterType_Float3;
          break;
        case GL_FLOAT_VEC4_ARB:
          parameter.type = ProgramParameterType_Float4;
          break;
        case GL_INT:
          parameter.type = ProgramParameterType_Int;
          break;
        case GL_INT_VEC2_ARB:
          parameter.type = ProgramParameterType_Int2;
          break;        
        case GL_INT_VEC3_ARB:
          parameter.type = ProgramParameterType_Int3;
          break;        
        case GL_INT_VEC4_ARB:
          parameter.type = ProgramParameterType_Int4;
          break;
        case GL_BOOL_ARB:
        case GL_BOOL_VEC2_ARB:
        case GL_BOOL_VEC3_ARB:
        case GL_BOOL_VEC4_ARB:
          throw xtl::format_not_supported_exception (METHOD_NAME, "GLboolean uniform '%s' not supported (type=%04x)", parameter.name.c_str (), type);
        case GL_FLOAT_MAT2_ARB:
          parameter.type = ProgramParameterType_Float2x2;
          break;
        case GL_FLOAT_MAT3_ARB:
          parameter.type = ProgramParameterType_Float3x3;
          break;
        case GL_FLOAT_MAT4_ARB:
          parameter.type = ProgramParameterType_Float4x4;
          break;
        case GL_SAMPLER_2D_ARB:
        case GL_SAMPLER_CUBE_ARB:
#ifndef OPENGL_ES2_SUPPORT
        case GL_SAMPLER_1D_ARB:
        case GL_SAMPLER_3D_ARB:
        case GL_SAMPLER_1D_SHADOW_ARB:
        case GL_SAMPLER_2D_SHADOW_ARB:
        case GL_SAMPLER_2D_RECT_ARB:
        case GL_SAMPLER_2D_RECT_SHADOW_ARB:
#endif
          parameter.type = ProgramParameterType_Int;
          break;                
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Unknown uniform '%s' type %04x with %u element(s)", parameter.name.c_str (), type, elements_count);
      }
      
      parameters.push_back (parameter);
    }    

    attributes.reserve (attributes_count);
    
    stl::string attribute_name;

    for (GLint location=0; location<attributes_count; location++)
    {
      attribute_name.fast_resize (max_attribute_name_length);      

      GLint  name_length = 0, elements_count = 0;
      GLenum type = 0;
      
      glGetActiveAttribARB (handle, location, attribute_name.size (), &name_length, &elements_count, &type, &attribute_name [0]);
      
      if ((size_t)name_length > attribute_name.size ())
        name_length = attribute_name.size ();
        
      if (name_length < 0)
        name_length = 0;
        
      attribute_name.fast_resize (name_length);

      if (strstr (attribute_name.c_str (), "[0]") == attribute_name.end () - 3)  //Обход особенности именования массивов в драйверах nVidia
        attribute_name.fast_resize (attribute_name.size () - 3);
      
      Attribute attribute;
      
      attribute.name      = attribute_name;
      attribute.name_hash = common::strhash (attribute.name.c_str ());

      attributes.push_back (attribute);
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
    if (TryMakeContextCurrent ())
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
    if (glDetachShader)
    {
      glDetachShader (handle, (*iter)->GetHandle ());
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glDetachObjectARB (handle, (*iter)->GetHandle ());
    }
#endif
  }

    //удаление программы

  if (glDeleteProgram)
  {
    glDeleteProgram (handle);
  }
#ifndef OPENGL_ES2_SUPPORT
  else
  {
    glDeleteObjectARB (handle);
  }
#endif

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

  if (glGetProgramiv)
  {
    glGetProgramiv (handle, GL_INFO_LOG_LENGTH, &log_length);
  }
#ifndef OPENGL_ES2_SUPPORT
  else
  {
    glGetObjectParameterivARB (handle, GL_INFO_LOG_LENGTH, &log_length);
  }
#endif

  if (!log_length)
    return;

    //чтение строки состояния

  GLsizei getted_log_size = 0;

  log_buffer.resize (log_length - 1);

  if (glGetProgramInfoLog)
  {
    glGetProgramInfoLog (handle, log_length, &getted_log_size, &log_buffer [0]);
  }
#ifndef OPENGL_ES2_SUPPORT
  else
  {
    glGetInfoLogARB (handle, log_length, &getted_log_size, &log_buffer [0]);
  }
#endif

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

/*
    Получение словаря атрибутов
*/

IVertexAttributeDictionary* GlslProgram::GetVertexAttributeDictionary ()
{
  return this;
}

/*
    Получение индекса атрибута по имени
*/

int GlslProgram::FindAttribute (const char* name)
{
  if (!name)
    return -1;

  size_t name_hash = common::strhash (name);

  for (AttributeArray::iterator iter=attributes.begin (), end=attributes.end (); iter!=end; ++iter)
    if (iter->name_hash == name_hash)
      return iter - attributes.begin ();

  return -1;
}

/*
    Получение параметров программы
*/

size_t GlslProgram::GetParametersCount ()
{
  return parameters.size ();
}

const GlslProgram::Parameter* GlslProgram::GetParameters ()
{
  return parameters.empty () ? (const Parameter*)0 : &parameters [0];
}
