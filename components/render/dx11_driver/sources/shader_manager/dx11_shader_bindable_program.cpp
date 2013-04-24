#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
     онструктор / деструктор
*/

BindableProgram::BindableProgram (ShaderLibrary& library, const Program& in_program, const ProgramParametersLayout& in_layout)
  : program (in_program)
  , parameters_layout (in_layout)
{
  try
  {
      //создание прототипов буферов

    buffer_prototypes.reserve (program.GetConstantBufferLayoutsCount ());

    for (size_t i=0, count=program.GetConstantBufferLayoutsCount (); i<count; i++)
    {
      const ProgramBufferLayout& buffer_layout = program.GetConstantBufferLayout (i);

        //создание прототипа (без карты прототипов)

      TargetConstantBufferPrototypePtr prototype (new TargetConstantBufferPrototype (library, parameters_layout, buffer_layout), false);

      buffer_prototypes.push_back (prototype);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::BindableProgram::BindableProgram");
    throw;
  }
}

BindableProgram::~BindableProgram ()
{
}

/*
    Ѕиндинг в контекст
*/

void BindableProgram::Bind (ID3D11DeviceContext& context, ShaderLibrary& library, const SourceConstantBufferPtr src_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT])
{
  try
  {
      //биндинг программы

    program.Bind (context);

      //поиск и биндинг буферов

    ID3D11Buffer* buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];

    memset (buffers, 0, sizeof (buffers));

    for (BufferPrototypeArray::iterator iter=buffer_prototypes.begin (), end=buffer_prototypes.end (); iter!=end; ++iter)
    {
      TargetConstantBufferPrototype& prototype = **iter;
      TargetConstantBuffer&          buffer    = prototype.GetBuffer (src_buffers, library);

      buffer.Bind (context, buffers);
    }

      //установка контекста

    context.CSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Compute]);
    context.DSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Domain]);
    context.GSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Geometry]);
    context.HSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Hull]);
    context.PSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Pixel]);
    context.VSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Vertex]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::BindableProgram::Bind");
    throw;
  }
}
