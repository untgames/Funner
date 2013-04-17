#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

Shader::Shader (ShaderType in_shader_type, const ShaderCode& code)
  : DeviceObject (code.GetDeviceManager ())
  , type (in_shader_type)
{
  try
  {
    ID3D11Device& device = GetDevice ();

    switch (type)
    {
      case ShaderType_Compute:
      {
        ID3D11ComputeShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateComputeShader", device.CreateComputeShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateComputeShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Domain:
      {
        ID3D11DomainShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateDomainShader", device.CreateDomainShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateDomainShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Geometry:
      {
        ID3D11GeometryShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateGeometryShader", device.CreateGeometryShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateGeometryShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Hull:
      {
        ID3D11HullShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateHullShader", device.CreateHullShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateHullShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Pixel:
      {
        ID3D11PixelShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreatePixelShader", device.CreatePixelShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreatePixelShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      case ShaderType_Vertex:
      {
        ID3D11VertexShader* dx_shader = 0;

        check_errors ("ID3D11Device::CreateVertexShader", device.CreateVertexShader (code.GetCompiledData (), code.GetCompiledDataSize (), 0, &dx_shader));

        if (dx_shader)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateVertexShader failed");
        
        shader = DxDeviceChildPtr (dx_shader, false);

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "shader_type");
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
