#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ���������
*/

const size_t RESERVED_SLOTS_COUNT = 4; //������������� ���������� ������

/*
    ����������� / ����������
*/

namespace
{

const char* get_name (ShaderType type)
{
  switch (type)
  {
    case ShaderType_Compute:   return "ComputeShader";
    case ShaderType_Domain:    return "DomainShader";
    case ShaderType_Geometry:  return "GeometryShader";
    case ShaderType_Hull:      return "HullShader";
    case ShaderType_Pixel:     return "PixelShader";
    case ShaderType_Vertex:    return "VertexShader";
    default:                   throw xtl::make_argument_exception ("", "type", type);
  }
}

}

Program::Program (ShaderLibrary& library, size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
  : DeviceObject (library.GetDeviceManager ())
{
  try
  {
      //�������� ������������ ����������

    if (!shader_descs)
      throw xtl::make_null_argument_exception ("", "shader_descs");

    if (!shaders_count)
      throw xtl::make_null_argument_exception ("", "shaders_count");

      //�������� ��������

    size_t buffers_count = 0;

    for (size_t i=0; i<shaders_count; i++)
    {
      const ShaderDesc& desc = shader_descs [i];

      ShaderPtr shader = library.CreateShader (desc, error_log);

        //��������������� ���� �������

      ShaderType type = shader->GetType ();

      if (type < 0 || type >= ShaderType_Num)
        throw xtl::format_operation_exception ("", "Wrong shader type %d", type);

      ShaderSlot& slot = shaders [type];

      if (slot.shader)
        throw xtl::format_operation_exception ("", "%s has been already set", ::get_name (type));

      slot.holder = shader;
      slot.shader = &shader->GetHandle ();

      buffers_count += shader->GetConstantBufferLayoutsCount ();
    }

      //����������� ��������

    buffer_layouts.reserve (buffers_count);

    for (size_t i=0; i<ShaderType_Num; i++)
    {
      ShaderSlot& slot = shaders [i];

      if (!slot.shader)
        continue;

      for (size_t j=0, count=slot.holder->GetConstantBufferLayoutsCount (); j<count; j++)
      {
        ConstantBufferLayoutPtr layout = slot.holder->GetConstantBufferLayout (j);

        bool found = false;

        for (BufferLayoutArray::iterator iter=buffer_layouts.begin (), end=buffer_layouts.end (); iter!=end; ++iter)
        {
           ProgramBufferLayout& program_layout = **iter;

          if (&program_layout.GetLayout () == layout)
          {
            program_layout.AddSlot (ProgramBufferLayout::Slot ((ShaderType)i, j));

            found = true;

            break;
          }
        }

        if (found)
          continue;

        ProgramBufferLayoutPtr program_layout (new ProgramBufferLayout (layout, RESERVED_SLOTS_COUNT), false);

        program_layout->AddSlot (ProgramBufferLayout::Slot ((ShaderType)i, j));

        buffer_layouts.push_back (program_layout);
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Program::Program");
    throw;
  }
}

Program::~Program ()
{
}

/*
    ��������� ��������� � ��������
*/

template <class T, ShaderType Type>
inline T* Program::Get ()
{
  return static_cast<T*> (shaders [Type].shader);
}

void Program::Bind (ID3D11DeviceContext& context)
{
  context.CSSetShader (Get<ID3D11ComputeShader, ShaderType_Compute> (), 0, 0);
  context.DSSetShader (Get<ID3D11DomainShader, ShaderType_Domain> (), 0, 0);
  context.GSSetShader (Get<ID3D11GeometryShader, ShaderType_Geometry> (), 0, 0);
  context.HSSetShader (Get<ID3D11HullShader, ShaderType_Hull> (), 0, 0);
  context.PSSetShader (Get<ID3D11PixelShader, ShaderType_Pixel> (), 0, 0);
  context.VSSetShader (Get<ID3D11VertexShader, ShaderType_Vertex> (), 0, 0);
}

/*
    �������� �������� �������
*/

DxInputLayoutPtr Program::CreateInputLayout (const D3D11_INPUT_ELEMENT_DESC* descs, size_t descs_count)
{
  try
  {
      //�������� ������������ ����������

    if (!descs)
      throw xtl::make_null_argument_exception ("", "descs");

    if (!descs_count)
      throw xtl::make_null_argument_exception ("", "descs_count");

    ShaderSlot& vs_slot = shaders [ShaderType_Vertex];

    if (!vs_slot.shader)
      throw xtl::format_operation_exception ("", "Can't create layout: no vertex shader was attached");   

    ShaderCode& code = vs_slot.holder->GetShaderCode ();

      //�������� �������

    ID3D11InputLayout* dx_layout = 0;

    check_errors ("ID3D11Device::CreateInputLayout", GetDevice ().CreateInputLayout (descs, descs_count, code.GetCompiledData (), code.GetCompiledDataSize (), &dx_layout));

    if (!dx_layout)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateInputLayout failed");

    return DxInputLayoutPtr (dx_layout, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Program::CreateInputLayout");
    throw;
  }
}

/*
    ������������ ����������� �������
*/

const ProgramBufferLayout& Program::GetConstantBufferLayout (size_t index) const
{
  if (index >= buffer_layouts.size ())
    throw xtl::make_range_exception ("render::low_level::dx11::Program::GetConstantBufferLayout", "index", index, buffer_layouts.size ());

  return *buffer_layouts [index];
}