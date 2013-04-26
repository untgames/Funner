#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

BindableProgram::BindableProgram (ShaderLibrary& library, const Program& in_program, const ProgramParametersLayout* in_layout)
  : DeviceObject (library.GetDeviceManager ())
  , program (in_program)
  , parameters_layout (in_layout)
{
  try
  {
    if (!parameters_layout && program.GetConstantBufferLayoutsCount ())
      throw xtl::format_operation_exception ("", "Null program parameters layout but program requires it (%u constant buffer layouts found)", program.GetConstantBufferLayoutsCount ());
     
      //создание прототипов буферов

    buffer_prototypes.reserve (program.GetConstantBufferLayoutsCount ());

    for (size_t i=0, count=program.GetConstantBufferLayoutsCount (); i<count; i++)
    {
      const ProgramBufferLayout& buffer_layout = program.GetConstantBufferLayout (i);

        //создание прототипа (без карты прототипов)

      TargetConstantBufferPrototypePtr prototype (new TargetConstantBufferPrototype (library, *parameters_layout, buffer_layout), false);

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

void BindableProgram::Bind 
 (ID3D11DeviceContext&          context,
  ShaderLibrary&                library,
  const SourceConstantBufferPtr src_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT],
  const InputLayout&            input_layout,
  BindableProgramContext&       program_context)
{
  try
  {
      //поиск входного лэйаута

    if (!program_context.input_layout)
    {
      program_context.input_layout = program.GetInputLayout (library, input_layout);
   
      context.IASetInputLayout (program_context.input_layout.get ());
    }

      //биндинг программы

    if (!program_context.program_binded)
    {      
      program.Bind (context);

      program_context.program_binded = true;
    }

      //поиск и биндинг буферов
    
    if (program_context.has_dirty_buffers)
    {
      ID3D11Buffer* buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];

      memset (buffers, 0, sizeof (buffers));

      for (BufferPrototypeArray::iterator iter=buffer_prototypes.begin (), end=buffer_prototypes.end (); iter!=end; ++iter)
      {
        TargetConstantBufferPrototype& prototype = **iter;

        const unsigned char* index = prototype.GetSourceBuffersIndices ();
        bool                 dirty = false;  

        for (size_t i=0, count=prototype.GetSourceBuffersCount (); i<count; i++, index++)
          if (program_context.dirty_buffers [*index])
          {
            dirty = true;
            break;
          }

        if (!dirty)
          continue;

        TargetConstantBuffer& buffer = prototype.GetBuffer (src_buffers, library);

        buffer.Bind (context, buffers);
      }

        //установка контекста

      context.CSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Compute]);
      context.DSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Domain]);
      context.GSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Geometry]);
      context.HSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Hull]);
      context.PSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Pixel]);
      context.VSSetConstantBuffers (0, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT, buffers [ShaderType_Vertex]);

      program_context.has_dirty_buffers = false;

      memset (program_context.dirty_buffers, 0, sizeof (program_context.dirty_buffers));
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::BindableProgram::Bind");
    throw;
  }
}
