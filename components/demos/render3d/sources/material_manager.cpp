#include "shared.h"

MaterialManager::MaterialManager (Test& in_test)
  : test (in_test)
{
}

void MaterialManager::SetReflectionTexture (const char* name)
{
  default_reflection_texture = LoadTexture (name);
}

void MaterialManager::LoadMaterials (const char* file_name)
{
  if (!default_sampler)
  {
    default_sampler = CreateSampler ();
  }

  media::rfx::MaterialLibrary materials (file_name);
  
  for (media::rfx::MaterialLibrary::Iterator iter=materials.CreateIterator (); iter; ++iter)
  {
    try
    {
      LoadMaterial (materials.ItemId (iter), *iter);
    }
    catch (xtl::exception& e)
    {
      e.touch ("Model::LoadMaterial(name='%s')", materials.ItemId (iter));
      throw;
    }
  }
}

void MaterialManager::LoadMaterial (const char* id, const media::rfx::Material& src_mtl)
{
  ModelMaterialPtr dst_mtl (new ModelMaterial);
  
  const common::PropertyMap& properties = src_mtl.Properties ();
  
  if (properties.IndexOf ("DiffuseTexture") != -1)
  {    
    dst_mtl->texmaps [SamplerChannel_Diffuse].texture = LoadTexture (properties.GetString ("DiffuseTexture"));
  }
  
  if (properties.IndexOf ("SpecularTexture") != -1)
  {    
    dst_mtl->texmaps [SamplerChannel_Specular].texture = LoadTexture (properties.GetString ("SpecularTexture"));
  }
  
  if (properties.IndexOf ("AmbientTexture") != -1)
  {    
    dst_mtl->texmaps [SamplerChannel_Ambient].texture = LoadTexture (properties.GetString ("AmbientTexture"));
  }
  
  if (properties.IndexOf ("BumpTexture") != -1)
  {    
    dst_mtl->texmaps [SamplerChannel_Bump].texture = LoadTexture (properties.GetString ("BumpTexture"));
  }
  
  if (properties.IndexOf ("EmissionTexture") != -1)
  {    
    dst_mtl->texmaps [SamplerChannel_Emission].texture = LoadTexture (properties.GetString ("EmissionTexture"));
  }
  
  dst_mtl->texmaps [SamplerChannel_Reflection].texture = default_reflection_texture;
  
  for (int i=0; i<SamplerChannel_Num; i++)
    dst_mtl->texmaps [i].sampler = default_sampler;
  
  const char* shader_type_string = properties.GetString ("ShaderType");
  
  ShaderType shader_type = (ShaderType)-1;
  
  struct Name2Map 
  {
    const char* name;
    ShaderType  type;
  };
  
  static const Name2Map shader_type_map [] = {
    {"Phong", ShaderType_Phong},
    {"Lambert", ShaderType_Lambert},
    {"Constant", ShaderType_Constant},
  };
  
  static const size_t shader_type_map_size = sizeof (shader_type_map) / sizeof (*shader_type_map);
  
  for (size_t i=0; i<shader_type_map_size; i++)
    if (!strcmp (shader_type_map [i].name, shader_type_string))
    {
      shader_type = shader_type_map [i].type;
      break;
    }
    
  if (shader_type == (ShaderType)-1)
  {
    printf ("Bad material '%s' shader type '%s'\n", id, shader_type_string);
    return;
  }
  
  MaterialShaderParams params;
  
  params.shader_type              = shader_type;
  params.reflectivity             = properties.GetFloat ("Reflectivity");
  params.transparency             = properties.GetFloat ("Transparency");
  params.shininess                = properties.GetFloat ("Shininess");
  params.bump_amount              = properties.IndexOf ("BumpAmount") != -1 ? properties.GetFloat ("BumpAmount") : 0.0f;
  params.diffuse_color            = properties.GetVector ("Diffuse");
  params.ambient_color            = properties.GetVector ("Ambient");
  params.specular_color           = properties.GetVector ("Specular");
  params.emission_color           = properties.GetVector ("Emission");
  params.bump_texture_channel     = properties.IndexOf ("BumpTextureChannel") != -1 ? properties.GetInteger ("BumpTextureChannel") : 0;
  params.diffuse_texture_channel  = properties.IndexOf ("DiffuseTextureChannel") != -1 ? properties.GetInteger ("DiffuseTextureChannel") : 0;
  params.ambient_texture_channel  = properties.IndexOf ("AmbientTextureChannel") != -1 ? properties.GetInteger ("AmbientTextureChannel") : 0;
  params.specular_texture_channel = properties.IndexOf ("SpecularTextureChannel") != -1 ? properties.GetInteger ("SpecularTextureChannel") : 0;
  params.emission_texture_channel = properties.IndexOf ("EmissionTextureChannel") != -1 ? properties.GetInteger ("EmissionTextureChannel") : 0;
  
  BufferDesc cb_desc;

  memset (&cb_desc, 0, sizeof cb_desc);

  cb_desc.size         = sizeof (MaterialShaderParams);
  cb_desc.usage_mode   = UsageMode_Default;
  cb_desc.bind_flags   = BindFlag_ConstantBuffer;
  cb_desc.access_flags = AccessFlag_ReadWrite;

  BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

  cb->SetData (0, sizeof (MaterialShaderParams), &params);
  
  dst_mtl->constant_buffer = cb;

  dst_mtl->shader = test.shader_manager.GetShader (src_mtl.Effect ());

  materials.insert_pair (id, dst_mtl);
}

ModelMaterialPtr MaterialManager::FindMaterial (const char* name)
{
  if (!name)
    return ModelMaterialPtr ();
    
  ModelMaterialMap::iterator iter = materials.find (name);
  
  if (iter == materials.end ())
    return ModelMaterialPtr ();
    
  return iter->second;
}

render::low_level::PixelFormat MaterialManager::GetPixelFormat (media::Image& image)
{
  switch (image.Format ())
  {
    case media::PixelFormat_RGB8:  return render::low_level::PixelFormat_RGB8;
    case media::PixelFormat_RGBA8: return render::low_level::PixelFormat_RGBA8;
    case media::PixelFormat_A8:
    case media::PixelFormat_L8:    return render::low_level::PixelFormat_A8;
    case media::PixelFormat_LA8:   return render::low_level::PixelFormat_LA8;
    default:
      printf ("Unsupported image format %s", media::get_format_name (image.Format ()));
      image.Convert (media::PixelFormat_RGBA8);
      return render::low_level::PixelFormat_RGBA8;
  }
}

TexturePtr MaterialManager::LoadTexture (const char* name)
{
  TextureMap::iterator iter = textures.find (name);
  
  if (iter != textures.end ())
    return iter->second;
    
  media::Image image (name);
  
  TextureDesc tex_desc;

  memset (&tex_desc, 0, sizeof (tex_desc));
  
  switch (image.Depth ())
  {
    case 1:
      tex_desc.dimension = TextureDimension_2D;
      break;
    case 6:
      tex_desc.dimension = TextureDimension_Cubemap;
      break;
    default:
      tex_desc.dimension = TextureDimension_3D;
      break;
  }

  tex_desc.width                = image.Width ();
  tex_desc.height               = image.Height ();
  tex_desc.layers               = image.Depth ();
  tex_desc.format               = GetPixelFormat (image);
  tex_desc.generate_mips_enable = true;
  tex_desc.bind_flags           = BindFlag_Texture;
  tex_desc.access_flags         = AccessFlag_ReadWrite;

  TexturePtr texture = TexturePtr (test.device->CreateTexture (tex_desc), false);

  for (int i=0, count=image.Depth (); i<count; i++)
    texture->SetData (i, 0, 0, 0, tex_desc.width, tex_desc.height, tex_desc.format, image.Bitmap (i));

  textures.insert_pair (name, texture);
  
  return texture;
}

//создание сэмплера
SamplerStatePtr MaterialManager::CreateSampler ()
{
  SamplerDesc sampler_desc;

  memset (&sampler_desc, 0, sizeof (sampler_desc));

  sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
  sampler_desc.mag_filter           = TexMagFilter_Linear;
  sampler_desc.max_anisotropy       = 1;
  sampler_desc.wrap_u               = TexcoordWrap_Clamp;
  sampler_desc.wrap_v               = TexcoordWrap_Clamp;
//    sampler_desc.wrap_u               = TexcoordWrap_Repeat;
//    sampler_desc.wrap_v               = TexcoordWrap_Repeat;
  sampler_desc.comparision_function = CompareMode_AlwaysPass;
  sampler_desc.min_lod              = 0;
  sampler_desc.max_lod              = FLT_MAX;

  return SamplerStatePtr (test.device->CreateSamplerState (sampler_desc), false);
}
