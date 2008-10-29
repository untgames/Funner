#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Описание параметра и группы параметров программы
*/

struct GlslBindableProgram::Parameter
{
  int                  location;  //имя константы
  ProgramParameterType type;      //тип константы
  size_t               count;     //количество элементов массива
  size_t               offset;    //смещение относительно начала константного буфера
};

struct GlslBindableProgram::Group
{
  size_t     slot;       //номер слота с константым буфером
  size_t     data_hash;  //хеш данных константного буфера
  size_t     count;      //количество элементов группы
  Parameter* parameters; //указатель на начало области с элементами
};

/*
    Конструктор
*/

GlslBindableProgram::GlslBindableProgram
 (const ContextManager&    manager,
  GlslProgram&             in_program,
  ProgramParametersLayout* parameters_layout)
   : ContextObject (manager),
     program (in_program)
{
    //обработка ситуации отсутствия параметров программы

  if (!parameters_layout || !parameters_layout->GetParametersCount ())
    return;

    //резервирование места для хранения параметров и групп параметров

  parameters.reserve (parameters_layout->GetParametersCount ());
  groups.reserve     (parameters_layout->GetGroupsCount ());

    //получение таблицы расширенных настроек контекста

  const ContextCaps& caps = GetCaps ();

    //сделать копирование окружения программы!!!!!!!!!
    //либо оповещение GlslProgram об изменении параметров

    //преобразование параметров

  GLhandleARB handle = program.GetHandle ();

  for (size_t i=0, count=parameters_layout->GetGroupsCount (); i<count; i++)
  {
    size_t start_parameters_count = parameters.size ();

    const ProgramParameterGroup& src_group = parameters_layout->GetGroup (i);

    for (size_t j=0; j<src_group.count; j++)
    {
      const ProgramParameter& src_parameter = src_group.parameters [j];
      Parameter               dst_parameter;

      dst_parameter.location = caps.glGetUniformLocation_fn (handle, src_parameter.name);

      if (dst_parameter.location == -1)
        continue; //пропускаем параметры, отсутствующие в программе

      dst_parameter.type   = src_parameter.type;
      dst_parameter.count  = src_parameter.count;
      dst_parameter.offset = src_parameter.offset;

        //добавление нового параметра

      parameters.push_back (dst_parameter);
    }

      //добавление новой группы

    Group dst_group;

    dst_group.slot       = src_group.slot;
    dst_group.data_hash  = 0;
    dst_group.count      = parameters.size () - start_parameters_count;
    dst_group.parameters = &parameters [start_parameters_count];

    if (!dst_group.count)
      continue; //пропускаем пустые группы

    groups.push_back (dst_group);
  }
}

/*
    Установка в контекст OpenGL
*/

void GlslBindableProgram::Bind (ConstantBufferPtr* constant_buffers)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslBindableProgram::Bind";

    //проверка корректности аргументов

  if (!constant_buffers)
    throw xtl::make_null_argument_exception (METHOD_NAME, "constant_buffers");

    //установка текущего контекста

  MakeContextCurrent ();

    //получение данных из кэша контекста

  const ContextCaps& caps            = GetCaps ();
  const size_t       current_program = GetContextCacheValue (CacheEntry_UsedProgram);

  if (current_program != program.GetId ())
  {
      //установка программы шейдинга в контекст OpenGL

    caps.glUseProgram_fn (program.GetHandle ());

    SetContextCacheValue (CacheEntry_UsedProgram, program.GetId ());
  }

    //установка параметров программы в контекст OpenGL

  for (GroupArray::iterator iter=groups.begin (), end=groups.end (); iter!=end; ++iter)
  {
    Group& group = *iter;

    if (!constant_buffers [group.slot])
      throw xtl::format_operation_exception (METHOD_NAME, "Null constant buffer #%u", group.slot);

//    if (group.data_hash == constant_buffers[group.slot]->GetDataHash ())
//      continue;

      //обновление хэша параметров группы

    group.data_hash = constant_buffers [group.slot]->GetDataHash ();

      //установка параметров

    char* buffer_base = (char*)constant_buffers [group.slot]->GetDataPointer ();

    for (size_t j=0; j<group.count; j++)
    {
      const Parameter& parameter      = group.parameters [j];
      void*            parameter_data = buffer_base + parameter.offset;

      switch (parameter.type)
      {
        case ProgramParameterType_Int:      caps.glUniform1iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);      break;
        case ProgramParameterType_Float:    caps.glUniform1fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int2:     caps.glUniform2iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);      break;
        case ProgramParameterType_Float2:   caps.glUniform2fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int3:     caps.glUniform3iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);      break;
        case ProgramParameterType_Float3:   caps.glUniform3fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int4:     caps.glUniform4iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);      break;
        case ProgramParameterType_Float4:   caps.glUniform4fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Float2x2: caps.glUniformMatrix2fv_fn (parameter.location, parameter.count, 0, (float*)parameter_data); break;
        case ProgramParameterType_Float3x3: caps.glUniformMatrix3fv_fn (parameter.location, parameter.count, 0, (float*)parameter_data); break;
        case ProgramParameterType_Float4x4: caps.glUniformMatrix4fv_fn (parameter.location, parameter.count, 0, (float*)parameter_data); break;
        default: break;
      }
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}
