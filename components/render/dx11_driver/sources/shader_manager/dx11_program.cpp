#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

namespace
{

template <class T>
inline void attach (const Shader& in_shader, T*& dst_shader, const char* type_name)
{
  if (dst_shader)
    throw xtl::format_operation_exception ("", "%s has been already set", type_name);

  dst_shader = static_cast<T*> (&in_shader.GetHandle ());
}

}

Program::Program (ShaderLibrary& library, size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
  : DeviceObject (library.GetDeviceManager ())
  , compute_shader ()
  , domain_shader ()
  , geometry_shader ()
  , hull_shader ()
  , pixel_shader ()
  , vertex_shader ()
{
  try
  {
      //проверка корректности аргументов

    if (!shader_descs)
      throw xtl::make_null_argument_exception ("", "shader_descs");

    if (!shaders_count)
      throw xtl::make_null_argument_exception ("", "shaders_count");

      //создание шейдеров

    shaders.reserve (ShaderType_Num);

    for (size_t i=0; i<shaders_count; i++)
    {
      const ShaderDesc& desc = shader_descs [i];

      ShaderPtr shader = library.CreateShader (desc, error_log);

      shaders.push_back (shader);

        //диспетчеризация типа шейдера

      switch (shader->GetType ())
      {
        case ShaderType_Compute:
          attach (*shader, compute_shader, "ComputeShader");
          break;
        case ShaderType_Domain:
          attach (*shader, domain_shader, "DomainShader");
          break;
        case ShaderType_Geometry:
          attach (*shader, geometry_shader, "GeometryShader");
          break;
        case ShaderType_Hull:
          attach (*shader, hull_shader, "HullShader");
          break;
        case ShaderType_Pixel:
          attach (*shader, pixel_shader, "PixelShader");
          break;
        case ShaderType_Vertex:
          attach (*shader, vertex_shader, "VertexShader");
          break;
        default:
          throw xtl::format_operation_exception ("", "Wrong shader type %d", shader->GetType ());
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
    Установка программы в контекст
*/

void Program::Bind (ID3D11DeviceContext& context)
{
  context.CSSetShader (compute_shader, 0, 0);
  context.DSSetShader (domain_shader, 0, 0);
  context.GSSetShader (geometry_shader, 0, 0);
  context.HSSetShader (hull_shader, 0, 0);
  context.PSSetShader (pixel_shader, 0, 0);
  context.VSSetShader (vertex_shader, 0, 0);
}
