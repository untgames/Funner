#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
     онструктор / деструктор
*/

ShaderLibrary::ShaderLibrary (const DeviceManager& device_manager)
  : DeviceObject (device_manager)
{
}

ShaderLibrary::~ShaderLibrary ()
{
}

/*
    —оздание шейдера
*/

ShaderPtr ShaderLibrary::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{
  try
  {
      //проверка корректности аргументов

    if (!desc.name)
      throw xtl::make_null_argument_exception ("", "desc.name");

    if (!desc.source_code)
      throw xtl::make_null_argument_exception ("", "desc.source_code");

    if (!desc.source_code_size)
      throw xtl::make_null_argument_exception ("", "desc.source_code_size");

    if (!desc.profile)
      throw xtl::make_null_argument_exception ("", "desc.profile");

    const char* options = desc.options ? desc.options : "";

    ShaderType type = (ShaderType)-1;

    struct Profile2ShaderType
    {
      const char* profile;
      ShaderType  type;
      const char* dx_profile;
    };

    static const Profile2ShaderType type_map [] = {
      {"hlsl.vs", ShaderType_Vertex,   "vs_4_0_level_9_3"},
      {"hlsl.ps", ShaderType_Pixel,    "ps_4_0_level_9_3"},
      {"hlsl.gs", ShaderType_Geometry, "gs_4_0_level_9_3"},
      {"hlsl.hs", ShaderType_Hull,     "hs_4_0_level_9_3"},
      {"hlsl.ds", ShaderType_Domain,   "ds_4_0_level_9_3"},
      {"hlsl.cs", ShaderType_Compute,  "cs_4_0_level_9_3"},
    };

    static const size_t type_map_size = sizeof (type_map) / sizeof (*type_map);

    const char* dx_profile = "";

    for (size_t i=0; i<type_map_size; i++)
      if (!strcmp (type_map [i].profile, desc.profile))
      {
        type       = type_map [i].type;
        dx_profile = type_map [i].dx_profile;
        break;
      }

    if (type == (ShaderType)-1)
      throw xtl::make_argument_exception ("", "desc.profile", desc.profile, "Unknown shader profile");

      //формирование хэша дл€ поиска в библиотеке

    size_t source_code_size = desc.source_code_size == (size_t)-1 ? strlen (desc.source_code) : desc.source_code_size,
           hash             = common::crc32 (desc.source_code, source_code_size, common::strhash (options));

      //поиск уже существующего шейдера

    ShaderMap::iterator iter = shaders.find (hash);

    if (iter != shaders.end ())
      return iter->second;

      //создание нового шейдера

    ShaderCodePtr code (new ShaderCode (GetDeviceManager (), desc.name, dx_profile, desc.source_code, source_code_size, options, error_log));
    ShaderPtr     shader (new Shader (type, code), false);

     //регистраци€ шейдера в библиотеке

    shader->RegisterDestroyHandler (xtl::bind (&ShaderLibrary::RemoveShaderByHash, this, hash), GetTrackable ());

    shaders.insert_pair (hash, shader.get ());

    return shader;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderLibrary::CreateShader");
    throw;
  }
}

void ShaderLibrary::RemoveShaderByHash (size_t hash)
{
  shaders.erase (hash);
}
