#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

BindableProgram::BindableProgram (ShaderLibrary& library, Program& in_program, ProgramParametersLayout& in_layout)
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
    Биндинг в контекст
*/

void BindableProgram::Bind (ID3D11DeviceContext& context)
{
  try
  {
      //биндинг программы

    program.Bind (context);

      //поиск буферов

    //???????

      //биндинг буферов

    ID3D11Buffer* buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];

    memset (buffers, 0, sizeof (buffers));

    //??????????

      //установка контекста

    context.CSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Compute]);
    context.DSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Domain]);
    context.GSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Geometry]);
    context.HSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Hull]);
    context.PSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Pixel]);
    context.VSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Vertex]);

    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::BindableProgram::Bind");
    throw;
  }
}
