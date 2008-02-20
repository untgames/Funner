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
  size_t               offset;    //смещение относительно начала константного буфера
};

struct GlslProgramParameterGroup
{
  size_t                slot;       //номер слота с константым буфером
  size_t                data_hash;  //хеш данных константного буфера
  size_t                count;      //количество элементов группы
  GlslProgramParameter* parameters; //указатель на начало области с элементами
};

struct ProgramParameterComparator
{
  bool operator () (const ProgramParameter* parameter1, const ProgramParameter* parameter2) const
  {
    return parameter1->slot < parameter2->slot;
  }
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
  : ContextObject (manager), program (0), layouts_cache (CACHED_LAYOUT_ARRAY_SIZE)
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

  ProgramParametersLayoutDesc &layout_desc = parameters_layout->GetDesc ();

  size_t& current_program = GetContextDataTable (Stage_Shading)[ShaderStageCache_UsedProgram];
  
  if (current_program != program)
  {
    if (glUseProgram) glUseProgram          (program);
    else              glUseProgramObjectARB (program);
  
    //установка кэш-переменной

    current_program = program;
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

    ProgramParametersLayoutDesc &layout_desc = parameters_layout->GetDesc ();

    oldest_entry->source_layout = NULL;

    oldest_entry->parameters.clear ();
    oldest_entry->parameters.reserve (layout_desc.parameters_count);

    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_fn = 0;
    GlslProgramParameter temp_parameter;

    if (glGetUniformLocation) glGetUniformLocation_fn = glGetUniformLocation;
    else                      glGetUniformLocation_fn = (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocationARB;
    
    stl::vector<const ProgramParameter*> temp_parameters (layout_desc.parameters_count);

    for (size_t i = 0; i < layout_desc.parameters_count; i++)
      temp_parameters[i] = &layout_desc.parameters [i];

    stl::sort (temp_parameters.begin (), temp_parameters.end (), ProgramParameterComparator ());

    size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT + 1; //номер текущего слота (для отслеживания изменения группы параметров)
    
    oldest_entry->parameter_groups.reserve (DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    for (size_t i = 0; i < temp_parameters.size (); i++)
    {
      int parameter_location = glGetUniformLocation_fn (program, temp_parameters[i]->name);

      if (parameter_location == -1)
      {
        LogPrintf ("Unreferenced uniform parameter '%s'", temp_parameters[i]->name);
        continue;
      }

      temp_parameter.location = parameter_location;
      temp_parameter.type     = temp_parameters [i]->type;
      temp_parameter.offset   = temp_parameters [i]->offset;

      oldest_entry->parameters.push_back (temp_parameter);

        //отслеживание появления новой группы

      if (current_slot != temp_parameters [i]->slot)
      {
        oldest_entry->parameter_groups.push_back ();

        GlslProgramParameterGroup& new_group = oldest_entry->parameter_groups.back ();
        
        new_group.slot       = temp_parameters [i]->slot;
        new_group.parameters = &oldest_entry->parameters.back ();
        new_group.count      = 0;
        new_group.data_hash  = 0;
        current_slot         = temp_parameters [i]->slot;
      }
    }

    for (ProgramParameterGroupArray::iterator iter = oldest_entry->parameter_groups.begin (), end = oldest_entry->parameter_groups.end ()-1; iter != end; ++iter)
      iter->count = iter [1].parameters - iter [0].parameters;

    oldest_entry->parameter_groups.back ().count = oldest_entry->parameters.end () - oldest_entry->parameter_groups.back ().parameters;    

    oldest_entry->source_layout = parameters_layout;

    hit_layout = oldest_entry;
  }

  //Установка юниформов

  PFNGLUNIFORM1IPROC        glUniform1i_fn        = 0;
  PFNGLUNIFORM1FPROC        glUniform1f_fn        = 0;
  PFNGLUNIFORM2IVPROC       glUniform2iv_fn       = 0;
  PFNGLUNIFORM2FVPROC       glUniform2fv_fn       = 0;
  PFNGLUNIFORM3IVPROC       glUniform3iv_fn       = 0;
  PFNGLUNIFORM3FVPROC       glUniform3fv_fn       = 0;
  PFNGLUNIFORM4IVPROC       glUniform4iv_fn       = 0;
  PFNGLUNIFORM4FVPROC       glUniform4fv_fn       = 0;
  PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv_fn = 0;
  PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv_fn = 0;
  PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_fn = 0;
    
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
        case ProgramParameterType_Int:      glUniform1i_fn        (parameter.location, *(int*)uniform_pointer);        break;
        case ProgramParameterType_Float:    glUniform1f_fn        (parameter.location, *(float*)uniform_pointer);      break;
        case ProgramParameterType_Int2:     glUniform2iv_fn       (parameter.location, 1, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float2:   glUniform2fv_fn       (parameter.location, 1, (float*)uniform_pointer);    break;
        case ProgramParameterType_Int3:     glUniform3iv_fn       (parameter.location, 1, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float3:   glUniform3fv_fn       (parameter.location, 1, (float*)uniform_pointer);    break;
        case ProgramParameterType_Int4:     glUniform4iv_fn       (parameter.location, 1, (int*)uniform_pointer);      break;
        case ProgramParameterType_Float4:   glUniform4fv_fn       (parameter.location, 1, (float*)uniform_pointer);    break;
        case ProgramParameterType_Float2x2: glUniformMatrix2fv_fn (parameter.location, 1, 0, (float*)uniform_pointer); break;
        case ProgramParameterType_Float3x3: glUniformMatrix3fv_fn (parameter.location, 1, 0, (float*)uniform_pointer); break;
        case ProgramParameterType_Float4x4: glUniformMatrix4fv_fn (parameter.location, 1, 0, (float*)uniform_pointer); break;
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

  CheckErrors ("render::low_level::opengl::GlslProgram::GetProgramLog");
}
