#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

Shader::Shader (ShaderType in_shader_type, const ShaderCodePtr& in_code, ShaderLibrary& library)
  : DeviceObject (in_code->GetDeviceManager ())
  , type (in_shader_type)
  , code (in_code)
{
  try
  {
    if (!code)
      throw xtl::make_null_argument_exception ("", "code");

    ID3D11Device& device = GetDevice ();

      //компиляция шейдера

    switch (type)
    {
      case ShaderType_Compute:
      {
        ID3D11ComputeShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateComputeShader", device.CreateComputeShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateComputeShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Domain:
      {
        ID3D11DomainShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateDomainShader", device.CreateDomainShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateDomainShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Geometry:
      {
        ID3D11GeometryShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateGeometryShader", device.CreateGeometryShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateGeometryShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Hull:
      {
        ID3D11HullShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateHullShader", device.CreateHullShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateHullShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Pixel:
      {
        ID3D11PixelShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreatePixelShader", device.CreatePixelShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreatePixelShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Vertex:
      {
        ID3D11VertexShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateVertexShader", device.CreateVertexShader (code->GetCompiledData (), code->GetCompiledDataSize (), 0, &dx_shader));

        if (!dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateVertexShader failed");

        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "shader_type");
    }

      //получение информации о шейдере

    ID3D11ShaderReflection* reflector = 0;
   
    check_errors ("D3DReflect", GetDeviceManager ().GetLibrary ().D3DReflect (code->GetCompiledData (), code->GetCompiledDataSize (), IID_ID3D11ShaderReflection, (void**)&reflector));

    if (!reflector)
      throw xtl::format_operation_exception ("", "D3DReflect failed");

    D3D11_SHADER_DESC desc;

    check_errors ("ID3D11ShaderReflection::GetDesc", reflector->GetDesc (&desc));

      //получение информации о константных буферах

    buffer_layouts.reserve (desc.ConstantBuffers);

    for (size_t i=0; i<desc.ConstantBuffers; i++)
    {
      ID3D11ShaderReflectionConstantBuffer* buffer = reflector->GetConstantBufferByIndex (i);

      if (!buffer)
        throw xtl::format_operation_exception ("", "ID3D11ShaderReflection::GetConstantBufferByIndex failed");

      ConstantBufferLayoutPtr layout (new ConstantBufferLayout (*buffer), false);

      layout = library.AddConstantBufferLayout (layout);

      buffer_layouts.push_back (layout);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Shader::Shader");
    throw;
  }
}

Shader::~Shader ()
{
}

/*
    Перечисление константных буферов
*/

ConstantBufferLayoutPtr Shader::GetConstantBufferLayout (size_t index) const
{
  if (index >= buffer_layouts.size ())
    throw xtl::make_range_exception ("render::low_level::dx11:Shader::GetConstantBufferLayout", "index", index, buffer_layouts.size ());

  return buffer_layouts [index];
}
