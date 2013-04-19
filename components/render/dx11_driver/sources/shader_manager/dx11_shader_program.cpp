#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
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
      //проверка корректности аргументов

    if (!shader_descs)
      throw xtl::make_null_argument_exception ("", "shader_descs");

    if (!shaders_count)
      throw xtl::make_null_argument_exception ("", "shaders_count");

      //создание шейдеров

    for (size_t i=0; i<shaders_count; i++)
    {
      const ShaderDesc& desc = shader_descs [i];

      ShaderPtr shader = library.CreateShader (desc, error_log);

        //диспетчеризация типа шейдера

      ShaderType type = shader->GetType ();

      if (type < 0 || type >= ShaderType_Num)
        throw xtl::format_operation_exception ("", "Wrong shader type %d", type);

      ShaderSlot& slot = shaders [type];

      if (slot.shader)
        throw xtl::format_operation_exception ("", "%s has been already set", get_name (type));

      slot.holder = shader;
      slot.shader = &shader->GetHandle ();
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
    Установка программы в контекст
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
    Создание входного лэйаута
*/

DxInputLayoutPtr Program::CreateInputLayout (const D3D11_INPUT_ELEMENT_DESC* descs, size_t descs_count)
{
  try
  {
      //проверка корректности аргументов

    if (!descs)
      throw xtl::make_null_argument_exception ("", "descs");

    if (!descs_count)
      throw xtl::make_null_argument_exception ("", "descs_count");

    ShaderSlot& vs_slot = shaders [ShaderType_Vertex];

    if (!vs_slot.shader)
      throw xtl::format_operation_exception ("", "Can't create layout: no vertex shader was attached");   

    ShaderCode& code = vs_slot.holder->GetShaderCode ();

      //создание лэйаута

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
