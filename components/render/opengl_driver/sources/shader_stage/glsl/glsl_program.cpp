#include "shared.h"

#include <stl/algorithm>

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

const size_t CACHED_LAYOUT_ARRAY_SIZE = 4;

/*
   Параметр шейдера
*/
struct GlslProgramParameter
{
  int                  location;  //имя константы
  ProgramParameterType type;      //тип константы
  size_t               count;     //количество элементов массива
  size_t               offset;    //смещение относительно начала константного буфера
};

struct GlslProgramParameterGroup
{
  size_t                slot;       //номер слота с константым буфером
  size_t                data_hash;  //хеш данных константного буфера
  size_t                count;      //количество элементов группы
  GlslProgramParameter* parameters; //указатель на начало области с элементами
};


typedef stl::vector<GlslProgramParameter>           ParametersArray;
typedef xtl::trackable_ptr<ProgramParametersLayout> ProgramParametersLayoutPtr;
typedef stl::vector<GlslProgramParameterGroup>      ProgramParameterGroupArray;

}

struct GlslProgram::LayoutCacheEntry
{
  ParametersArray            parameters;       //параметры программы
  ProgramParametersLayoutPtr source_layout;    //исходные параметры программы
  ProgramParameterGroupArray parameter_groups; //группы параметров
  size_t                     bind_time;        //время последнего биндинга
};

/*
   Конструктор
*/

GlslProgram::GlslProgram (const ContextManager& manager, size_t shaders_count, ShaderPtr* in_shaders, const LogFunction& error_log)
  : Program (manager), program (0), layouts_cache (CACHED_LAYOUT_ARRAY_SIZE)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslProgram::GlslProgram";

  try
  {
    MakeContextCurrent ();

    if (glCreateProgram) program = glCreateProgram ();
    else                 program = glCreateProgramObjectARB ();

    if (!program)
      RaiseError (METHOD_NAME);

    shaders.reserve (shaders_count);

    for (size_t i = 0; i < shaders_count; i++)
    {
      GlslShaderPtr shader = cast_object<GlslShader> (*this, in_shaders [i].get (), METHOD_NAME, "shaders"); 

      if (glAttachShader) glAttachShader    (program, shader->GetShaderObject ());
      else                glAttachObjectARB (program, shader->GetShaderObject ());

      shaders.push_back (shader);
    }

    int link_status = 0;

    if (glLinkProgram) glLinkProgram    (program);
    else               glLinkProgramARB (program);

    if (glGetProgramiv) glGetProgramiv            (program, GL_LINK_STATUS, &link_status);
    else                glGetObjectParameterivARB (program, GL_LINK_STATUS, &link_status);

    stl::string log_buffer;   

    GetProgramLog (log_buffer);

    error_log (log_buffer.c_str ());

      //Возможно нет необходимости делать валидацию

    if (link_status)
    {
      if (glValidateProgram) glValidateProgram    (program);
      else                   glValidateProgramARB (program);

      if (glGetProgramiv) glGetProgramiv            (program, GL_OBJECT_VALIDATE_STATUS_ARB, &link_status);
      else                glGetObjectParameterivARB (program, GL_OBJECT_VALIDATE_STATUS_ARB, &link_status);

      GetProgramLog (log_buffer);

      error_log (log_buffer.c_str ());
    }

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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::GlslProgram::~GlslProgram");

    LogPrintf ("%s", exception.Message ());
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

void GlslProgram::DeleteProgram ()
{
  if (!program)
    return;

  MakeContextCurrent ();

  for (ShaderArray::iterator iter=shaders.begin (), end=shaders.end (); iter!=end; ++iter)
  {
    if (glDetachShader) glDetachShader    (program, (*iter)->GetShaderObject ());
    else                glDetachObjectARB (program, (*iter)->GetShaderObject ());
  }

  if (glDeleteProgram) glDeleteProgram   (program);
  else                 glDeleteObjectARB (program);

  CheckErrors ("render::low_level::opengl::GlslProgram::DeletePrograms");
}

/*
   Биндинг
*/

void GlslProgram::Bind (ConstantBufferPtr* constant_buffers, ProgramParametersLayout* parameters_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslProgram::Bind";

  if (!constant_buffers)
    RaiseNullArgument (METHOD_NAME, "constant_buffers");

  if (!parameters_layout)
    RaiseNullArgument (METHOD_NAME, "parameters_layout");

  MakeContextCurrent ();

  const ContextCaps& caps = GetCaps ();

  size_t& current_program = GetContextDataTable (Stage_Shading)[ShaderStageCache_UsedProgram];
  
  if (current_program != GetId ())
  {
    caps.glUseProgram_fn (program);
  
    //установка кэш-переменной

    current_program = GetId ();
  }
  
  //Поиск в кэше

  LayoutCacheEntry *hit_layout = NULL;

  for (size_t i = 0; i < CACHED_LAYOUT_ARRAY_SIZE; i++)
  {
    if (layouts_cache[i].source_layout == parameters_layout)
      hit_layout = &layouts_cache[i];
  }

  if (!hit_layout)
  {
    LayoutCacheEntry *oldest_entry = &layouts_cache[0];

    for (size_t i = 1; i < CACHED_LAYOUT_ARRAY_SIZE; i++)
    {
      if ((current_time - layouts_cache[i].bind_time) > (current_time - oldest_entry->bind_time))
        oldest_entry = &layouts_cache[i];
    }

    oldest_entry->source_layout = NULL;

    oldest_entry->parameters.clear ();
    oldest_entry->parameters.reserve (parameters_layout->ParametersCount ());

    GlslProgramParameter temp_parameter;

    oldest_entry->parameter_groups.clear ();
    oldest_entry->parameter_groups.reserve (parameters_layout->GroupsCount ());

    for (size_t i = 0, group_count = parameters_layout->GroupsCount (); i < group_count; i++)
    {
      ProgramParameterGroup &current_group = parameters_layout->ParametersGroup (i);

      GlslProgramParameterGroup new_group = {current_group.slot, 0, current_group.count, NULL};

      for (size_t j = 0; j < current_group.count; j++)
      {
        ProgramParameter &current_parameter = current_group.parameters [j];
        
        int parameter_location = caps.glGetUniformLocation_fn (program, current_parameter.name);

        if (parameter_location == -1)
        {
          LogPrintf ("Unreferenced uniform parameter '%s'", current_parameter.name);
          continue;
        }

        temp_parameter.location = parameter_location;
        temp_parameter.type     = current_parameter.type;
        temp_parameter.count    = current_parameter.count;
        temp_parameter.offset   = current_parameter.offset;

        oldest_entry->parameters.push_back (temp_parameter);
      }

      new_group.parameters = oldest_entry->parameters.end () - new_group.count;

      oldest_entry->parameter_groups.push_back (new_group);
    }

    oldest_entry->source_layout = parameters_layout;

    hit_layout = oldest_entry;
  }

  //Установка юниформов

  for (size_t i = 0; i < hit_layout->parameter_groups.size (); i++)
  {
    GlslProgramParameterGroup& group = hit_layout->parameter_groups [i];

    if (!constant_buffers[group.slot])
      RaiseInvalidOperation (METHOD_NAME, "No needed constant buffer %u", group.slot);

    if ((current_time - hit_layout->bind_time == 1) && (group.data_hash == constant_buffers[group.slot]->GetDataHash ()))
      continue;

    group.data_hash = constant_buffers[group.slot]->GetDataHash ();

    char* buffer_base = (char*)constant_buffers[group.slot]->GetDataPointer ();

    for (size_t j = 0; j < group.count; j++)
    {
      const GlslProgramParameter& parameter       = group.parameters [j];
      void*                       uniform_pointer = buffer_base + parameter.offset;

      switch (parameter.type)
      {
        case ProgramParameterType_Int:      caps.glUniform1iv_fn       (parameter.location, parameter.count, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float:    caps.glUniform1fv_fn       (parameter.location, parameter.count, (float*)uniform_pointer);    break;
        case ProgramParameterType_Int2:     caps.glUniform2iv_fn       (parameter.location, parameter.count, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float2:   caps.glUniform2fv_fn       (parameter.location, parameter.count, (float*)uniform_pointer);    break;
        case ProgramParameterType_Int3:     caps.glUniform3iv_fn       (parameter.location, parameter.count, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float3:   caps.glUniform3fv_fn       (parameter.location, parameter.count, (float*)uniform_pointer);    break;
        case ProgramParameterType_Int4:     caps.glUniform4iv_fn       (parameter.location, parameter.count, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float4:   caps.glUniform4fv_fn       (parameter.location, parameter.count, (float*)uniform_pointer);    break;
        case ProgramParameterType_Float2x2: caps.glUniformMatrix2fv_fn (parameter.location, parameter.count, 0, (float*)uniform_pointer); break;
        case ProgramParameterType_Float3x3: caps.glUniformMatrix3fv_fn (parameter.location, parameter.count, 0, (float*)uniform_pointer); break;
        case ProgramParameterType_Float4x4: caps.glUniformMatrix4fv_fn (parameter.location, parameter.count, 0, (float*)uniform_pointer); break;
      }
    }
  }

  hit_layout->bind_time = current_time++;

  CheckErrors (METHOD_NAME);
}

/*
   Получение лога OpenGL
*/

void GlslProgram::GetProgramLog (stl::string& log_buffer)
{
  int log_length = 0;

  log_buffer.clear ();

  MakeContextCurrent ();

  if (glGetProgramiv) glGetProgramiv            (program, GL_INFO_LOG_LENGTH, &log_length);
  else                glGetObjectParameterivARB (program, GL_INFO_LOG_LENGTH, &log_length);

  if (log_length)
  {
    int getted_log_size = 0;

    log_buffer.resize (log_length - 1);

    if (glGetProgramInfoLog) glGetProgramInfoLog (program, log_length, &getted_log_size, &log_buffer [0]);
    else                     glGetInfoLogARB     (program, log_length, &getted_log_size, &log_buffer [0]);
    
    if (getted_log_size)
      log_buffer.resize (getted_log_size - 1);
  }

  CheckErrors ("render::low_level::opengl::GlslProgram::GetProgramLog");
}
