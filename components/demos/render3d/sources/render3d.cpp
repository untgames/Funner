#include "shared.h"

const char* VERTEX_SHADER_FILE_NAME  = "data/phong.vert";
const char* PIXEL_SHADER_FILE_NAME   = "data/phong.frag";
const char* MODEL_NAME               = "data/meshes.xmesh";
const char* MATERIAL_LIBRARY         = "data/materials.xmtl";
const char* SCENE_NAME               = "data/scene.xscene";
const char* REFLECTION_TEXTURE       = "env/EnvGala_000_D2.tga";

const float EPS = 0.001;

enum ConstantBufferSemantic
{
  ConstantBufferSemantic_Common,
  ConstantBufferSemantic_Material,
};

#pragma pack (1)

enum SamplerChannel
{
  SamplerChannel_Diffuse,
  SamplerChannel_Specular,
  SamplerChannel_Bump,
  SamplerChannel_Ambient,
  SamplerChannel_Emission,
  SamplerChannel_Reflection,
  
  SamplerChannel_Num
};

struct CommonShaderParams
{
  math::mat4f object_tm;
  math::mat4f view_tm;
  math::mat4f proj_tm;
  math::mat4f model_view_tm;
  math::mat4f model_view_proj_tm;
  math::vec3f light_pos;
  math::vec3f light_dir;
  int         diffuse_sampler;
  int         specular_sampler;
  int         bump_sampler;
  int         ambient_sampler;
  int         emission_sampler;
  int         reflection_sampler;
};

enum ShaderType
{
  ShaderType_Phong,
  ShaderType_Lambert,
  ShaderType_Constant
};

struct MaterialShaderParams
{
  ShaderType  shader_type;
  float       reflectivity;
  float       transparency;
  float       shininess;
  float       bump_amount;
  int         bump_texture_channel;
  math::vec4f diffuse_color;
  int         diffuse_texture_channel;
  math::vec4f ambient_color;
  int         ambient_texture_channel;  
  math::vec4f specular_color;
  int         specular_texture_channel;
  math::vec4f emission_color;
  int         emission_texture_channel;
};

struct ModelTexmap
{
  TexturePtr      texture;
  SamplerStatePtr sampler;
};

struct ModelMaterial
{
  ModelTexmap texmaps [SamplerChannel_Num];
  BufferPtr   constant_buffer;
};

typedef xtl::shared_ptr<ModelMaterial>  ModelMaterialPtr;
typedef stl::hash_map<stl::hash_key<const char*>, ModelMaterialPtr> ModelMaterialMap;

typedef stl::vector<BufferPtr> BufferArray;

struct ModelVertexBuffer
{
  BufferArray    vertex_streams;
  InputLayoutPtr input_layout;
  size_t         id;
};

typedef xtl::shared_ptr<ModelVertexBuffer>     VertexBufferPtr;
typedef stl::hash_map<size_t, VertexBufferPtr> VertexBufferMap;
typedef stl::hash_map<size_t, BufferPtr>       BufferMap;
typedef stl::vector<VertexBufferPtr>           VertexBufferArray;

struct ModelPrimitive
{
  PrimitiveType    type;          //тип примитива
  ModelMaterialPtr material;      //материал
  VertexBufferPtr  vertex_buffer; //вершинный буфер
  size_t           first;         //индекс первой вершины/индекса
  size_t           count;         //количество примитивов
};

typedef stl::vector<ModelPrimitive> PrimitiveArray;

struct ModelMesh
{
  VertexBufferArray vertex_buffers;
  BufferPtr         index_buffer;
  PrimitiveArray    primitives;
  stl::string       name;

  ModelMesh (const char* in_name)
    : name (in_name)
    {}
};

typedef xtl::shared_ptr<ModelMesh>                            ModelMeshPtr;
typedef stl::vector<ModelMeshPtr>                             ModelMeshArray;
typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr> TextureMap;

namespace math
{

inline bool read (const char* string, math::vec4f& result)
{
  common::StringArray tokens = common::split (string);

  switch (tokens.Size ())
  {
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::vec4f (value);

      return true;
    }
    case 2:
    case 3:
    case 4:
    {
      math::vec4f v (0.0f);

      for (size_t i=0, count=tokens.Size (); i<count; i++)
        if (!xtl::io::read (tokens [i], v [i]))
          return false;

      result = v;

      return true;
    }
    default:
      return false;
  }
}

inline bool read (const char* string, math::mat4f& result)
{
  typedef xtl::io::token_iterator<const char*> token_iterator;

  common::StringArray tokens = common::split (string);

  switch (tokens.Size ())
  {
    case 1:
    {
      float value = 0.0f;

      if (!xtl::io::read (tokens [0], value))
        return false;

      result = math::mat4f (value);

      return true;
    }
    case 16:
    {
      token_iterator iter = xtl::io::make_token_iterator (tokens.Data (), tokens.Data () + tokens.Size ());

      return read (iter, result);
    }
    default:
      return false;
  }
}

}

//декомпозиция аффинных преобразований из матрицы преобразований
void affine_decompose (const math::mat4f& matrix, math::vec3f& position, math::vec3f& rotation, math::vec3f& scale)
{
  math::mat4f local_matrix (transpose (matrix)); //копия матрицы преобразований

  //выделение преобразования переноса  
  for (size_t i = 0; i < 3; i++)
  {
    position [i] = local_matrix [3][i];
    local_matrix [3][i] = 0;
  }

  //выделение преобразования масштабирования
  for (size_t i = 0; i < 3; i++)
  {
    //определение длины вектора-столбца преобразований
    float square_length = 0;

    for (size_t j = 0; j < 3; j++)
      square_length += local_matrix [j][i] * local_matrix [j][i];

    scale [i] = sqrt (square_length);

    //нормирование
    for (size_t j = 0; j < 3; j++)
      local_matrix [j][i] /= scale [i];
  }

  //выделение преобразования поворота
  rotation [1] = asin (-local_matrix [0][2]);

  if (cos (rotation [1]) != 0.0)
  {
    rotation [0] = atan2 (local_matrix [1][2], local_matrix [2][2]);
    rotation [2] = atan2 (local_matrix [0][1], local_matrix [0][0]);
  }
  else
  {
    rotation [0] = atan2 (-local_matrix [2][0], local_matrix [1][1]);
    rotation [2] = 0;
  }
}

struct Model : public xtl::visitor<void, scene_graph::VisualModel>
{
  Test&                        test;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;
  TextureMap                   textures;
  SamplerStatePtr              default_sampler;
  ModelMaterialMap             materials;
  ModelMeshArray               meshes;
  TexturePtr                   default_reflection_texture;

  Model (Test& in_test, const char* name) : test (in_test), library (name)
  {
    default_sampler = CreateSampler ();
    default_reflection_texture = LoadTexture (REFLECTION_TEXTURE);
    
    LoadMaterials ();
    LoadVertexBuffers ();
    LoadMeshes ();
    LoadScene ();
  }

  void ReadMeshInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent)
  {
    scene_graph::VisualModel::Pointer model = scene_graph::VisualModel::Create ();

    model->SetMeshName (common::get<const char*> (*node, "name"));

    model->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);
  }

  void ReadNodeInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent)
  {
    scene_graph::Node::Pointer new_node = scene_graph::Node::Create ();

    const char* transform_string = common::get<const char*> (*node, "transform");

    math::mat4f transform;

    if (!read (transform_string, transform))
      throw xtl::format_operation_exception ("ReadNodeInfo", "Invalid transform format at line %u", node->LineNumber ());

    math::vec3f translation, rotation, scale;

    affine_decompose (transform, translation, rotation, scale);

    new_node->SetPosition    (translation);
    new_node->SetOrientation (math::radian (rotation.x), math::radian (rotation.y), math::radian (rotation.z));
    new_node->SetScale       (scale);

    new_node->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);

    for_each_child (*node, "mesh", xtl::bind (&Model::ReadMeshInfo, this, _1, new_node));
    for_each_child (*node, "node", xtl::bind (&Model::ReadNodeInfo, this, _1, new_node));
  }

  void LoadScene ()
  {
    common::Parser   p (SCENE_NAME);
    common::ParseLog log = p.Log ();

    for_each_child (p.Root (), "scene.node", xtl::bind (&Model::ReadNodeInfo, this, _1, scene_graph::Node::Pointer (&test.scene.Root ())));

    for (size_t i = 0; i < log.MessagesCount (); i++)
      switch (log.MessageType (i))
      {
        case common::ParseLogMessageType_Error:
        case common::ParseLogMessageType_FatalError:
          throw xtl::format_operation_exception ("LoadScene", log.Message (i));
        default:
          break;
      }
  }
  
  void LoadMeshes ()
  {
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;

      printf ("Load mesh '%s'\n", mesh.Name ());

      ModelMeshPtr model_mesh (new ModelMesh (mesh.Name ()));

      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);

        VertexBufferMap::iterator iter = vertex_buffers.find (vb.Id ());

        if (iter == vertex_buffers.end ())
        {
          printf ("Internal error: VB#%u not found\n", vb.Id ());
          abort ();
        }

        VertexBufferPtr model_vb = iter->second;

        model_mesh->vertex_buffers.push_back (model_vb);
      }

      const media::geometry::IndexBuffer& ib = mesh.IndexBuffer ();

      BufferMap::iterator iter = index_buffers.find (ib.Id ());
      BufferPtr ib_buffer;

      if (iter == index_buffers.end ())
      {
        if (ib.Size ())
        {
          BufferDesc desc;

          memset (&desc, 0, sizeof desc);

          desc.size         = ib.Size () * sizeof (unsigned short);
          desc.usage_mode   = UsageMode_Default;
          desc.bind_flags   = BindFlag_IndexBuffer;
          desc.access_flags = AccessFlag_ReadWrite;
          
          stl::vector<unsigned short> index_data (ib.Size ());
          
          const size_t*   src_index = ib.Data ();
          unsigned short* dst_index = &index_data [0];
          
          for (size_t i=0; i<ib.Size (); i++, src_index++, dst_index++)
            *dst_index = static_cast<unsigned short> (*src_index);

          BufferPtr (test.device->CreateBuffer (desc), false).swap (ib_buffer);

          ib_buffer->SetData (0, desc.size, &index_data [0]);
        }

        index_buffers [ib.Id ()] = ib_buffer;
      }
      else ib_buffer = iter->second;

      model_mesh->index_buffer = ib_buffer;

      for (size_t i=0, primitives_count=mesh.PrimitivesCount (); i<primitives_count; i++)
      {
        const media::geometry::Primitive& src_primitive = mesh.Primitive (i);
        ModelPrimitive                    dst_primitive;

        memset (&dst_primitive, 0, sizeof dst_primitive);

        switch (src_primitive.type)
        {
          case media::geometry::PrimitiveType_LineList:
            dst_primitive.type   = PrimitiveType_LineList;
            dst_primitive.count  = src_primitive.count * 2;
            break;
          case media::geometry::PrimitiveType_LineStrip:
            dst_primitive.type  = PrimitiveType_LineStrip;
            dst_primitive.count = src_primitive.count + 1;
            break;
          case media::geometry::PrimitiveType_TriangleList:
            dst_primitive.type  = PrimitiveType_TriangleList;
            dst_primitive.count = src_primitive.count * 3;
            break;
          case media::geometry::PrimitiveType_TriangleStrip:
            dst_primitive.type = PrimitiveType_TriangleStrip;
            dst_primitive.count = src_primitive.count + 2;
            break;
          case media::geometry::PrimitiveType_TriangleFan:
            dst_primitive.type = PrimitiveType_TriangleFan;
            dst_primitive.count = src_primitive.count + 2;
            break;
          default:
            continue;
        }

        if (src_primitive.vertex_buffer >= model_mesh->vertex_buffers.size ())
          continue;

        dst_primitive.vertex_buffer = model_mesh->vertex_buffers [src_primitive.vertex_buffer];
        dst_primitive.first         = src_primitive.first;
        dst_primitive.material      = FindMaterial (common::format ("%s-fx", src_primitive.material).c_str ());
        
        if (!dst_primitive.material)
        {
          printf ("No material '%s' for mesh '%s' primitive %u\n", src_primitive.material, mesh.Name (), i);
          continue;
        }
        
        model_mesh->primitives.push_back (dst_primitive);
      }

      meshes.push_back (model_mesh);
    }
  }

  void LoadVertexBuffers ()
  {
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;

      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);

        VertexBufferPtr model_vb (new ModelVertexBuffer);

        model_vb->id = vb.Id ();

        stl::vector<VertexAttribute> vertex_attributes;

        for (size_t j=0, streams_count=vb.StreamsCount (); j<streams_count; j++)
        {
          media::geometry::VertexStream& vs = vb.Stream (j);

          BufferMap::iterator iter = vertex_streams.find (vs.Id ());
          BufferPtr vs_buffer;

          if (iter == vertex_streams.end ())
          {
            BufferDesc desc;

            memset (&desc, 0, sizeof desc);

            desc.size         = vs.Size () * vs.VertexSize ();
            desc.usage_mode   = UsageMode_Default;
            desc.bind_flags   = BindFlag_VertexBuffer;
            desc.access_flags = AccessFlag_Read | AccessFlag_Write;

            vs_buffer = BufferPtr (test.device->CreateBuffer (desc), false);

            vs_buffer->SetData (0, desc.size, vs.Data ());

            vertex_streams [vs.Id ()] = vs_buffer;
          }
          else vs_buffer = iter->second;

          const media::geometry::VertexFormat& vf = vs.Format ();

          for (size_t k=0, attr_count=vf.AttributesCount (); k<attr_count; k++)
          {
            const media::geometry::VertexAttribute& src_va = vf.Attribute (k);
            VertexAttribute                         dst_va;

            memset (&dst_va, 0, sizeof dst_va);

            switch (src_va.semantic)
            {
              case media::geometry::VertexAttributeSemantic_Position:
                dst_va.semantic = VertexAttributeSemantic_Position;
                break;
              case media::geometry::VertexAttributeSemantic_Normal:
                dst_va.semantic = VertexAttributeSemantic_Normal;
                break;
              case media::geometry::VertexAttributeSemantic_Color:
                dst_va.semantic = VertexAttributeSemantic_Color;
                break;
              case media::geometry::VertexAttributeSemantic_TexCoord0:
              case media::geometry::VertexAttributeSemantic_TexCoord1:
              case media::geometry::VertexAttributeSemantic_TexCoord2:
              case media::geometry::VertexAttributeSemantic_TexCoord3:
              case media::geometry::VertexAttributeSemantic_TexCoord4:
              case media::geometry::VertexAttributeSemantic_TexCoord5:
              case media::geometry::VertexAttributeSemantic_TexCoord6:
              case media::geometry::VertexAttributeSemantic_TexCoord7:
                dst_va.semantic = (VertexAttributeSemantic)(VertexAttributeSemantic_TexCoord0 + src_va.semantic - media::geometry::VertexAttributeSemantic_TexCoord0);
                break;
              default:
                continue;
            }

            switch (src_va.type)
            {
              case media::geometry::VertexAttributeType_Float2:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector2;
                break;
              case media::geometry::VertexAttributeType_Float3:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector3;
                break;
              case media::geometry::VertexAttributeType_Float4:
                dst_va.type   = InputDataType_Float;
                dst_va.format = InputDataFormat_Vector4;
                break;
              case media::geometry::VertexAttributeType_Short2:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector2;
                break;
              case media::geometry::VertexAttributeType_Short3:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector3;
                break;
              case media::geometry::VertexAttributeType_Short4:
                dst_va.type   = InputDataType_Short;
                dst_va.format = InputDataFormat_Vector4;
                break;
              case media::geometry::VertexAttributeType_UByte4:
                dst_va.type   = InputDataType_UByte;
                dst_va.format = InputDataFormat_Vector4;
                break;
              default:
                continue;
            }

            dst_va.stride = vs.VertexSize ();
            dst_va.slot   = model_vb->vertex_streams.size ();
            dst_va.offset = src_va.offset;

            vertex_attributes.push_back (dst_va);
          }

          model_vb->vertex_streams.push_back (vs_buffer);
        }

        InputLayoutDesc layout_desc;

        memset (&layout_desc, 0, sizeof layout_desc);

        layout_desc.vertex_attributes_count = vertex_attributes.size ();
        layout_desc.vertex_attributes       = &vertex_attributes [0];
        layout_desc.index_type              = InputDataType_UShort;
        layout_desc.index_buffer_offset     = 0;

        model_vb->input_layout = InputLayoutPtr (test.device->CreateInputLayout (layout_desc), false);

        vertex_buffers [vb.Id ()] = model_vb;
      }
    }
  }
  
  void LoadMaterials ()
  {
    media::rfx::MaterialLibrary materials (MATERIAL_LIBRARY);
    
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
  
  void LoadMaterial (const char* id, const media::rfx::Material& src_mtl)
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
    
    materials.insert_pair (id, dst_mtl);
  }
  
  ModelMaterialPtr FindMaterial (const char* name)
  {
    if (!name)
      return ModelMaterialPtr ();
      
    ModelMaterialMap::iterator iter = materials.find (name);
    
    if (iter == materials.end ())
      return ModelMaterialPtr ();
      
    return iter->second;
  }
  
  static render::low_level::PixelFormat GetPixelFormat (media::Image& image)
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
  
  TexturePtr LoadTexture (const char* name)
  {
    TextureMap::iterator iter = textures.find (name);
    
    if (iter != textures.end ())
      return iter->second;
      
    media::Image image (name);
    
    TextureDesc tex_desc;

    memset (&tex_desc, 0, sizeof (tex_desc));

    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = image.Width ();
    tex_desc.height               = image.Height ();
    tex_desc.layers               = 1;
    tex_desc.format               = GetPixelFormat (image);
    tex_desc.generate_mips_enable = true;
    tex_desc.bind_flags           = BindFlag_Texture;
    tex_desc.access_flags         = AccessFlag_ReadWrite;

    TexturePtr texture = TexturePtr (test.device->CreateTexture (tex_desc), false);

    texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, tex_desc.format, image.Bitmap ());
    
    textures.insert_pair (name, texture);
    
    return texture;
  }
  
  //создание сэмплера
  SamplerStatePtr CreateSampler ()
  {
    SamplerDesc sampler_desc;

    memset (&sampler_desc, 0, sizeof (sampler_desc));

    sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
    sampler_desc.mag_filter           = TexMagFilter_Linear;
    sampler_desc.max_anisotropy       = 1;
    sampler_desc.wrap_u               = TexcoordWrap_Clamp;
    sampler_desc.wrap_v               = TexcoordWrap_Clamp;
    sampler_desc.comparision_function = CompareMode_AlwaysPass;
    sampler_desc.min_lod              = 0;
    sampler_desc.max_lod              = FLT_MAX;

    return SamplerStatePtr (test.device->CreateSamplerState (sampler_desc), false);
  }

  void visit (scene_graph::VisualModel& model)
  {
    for (ModelMeshArray::iterator iter=meshes.begin (); iter!=meshes.end (); ++iter)
    {
      const ModelMesh& mesh = **iter;

      if (xtl::xstrcmp (model.MeshName (), mesh.name.c_str ()))
        continue;

      CommonShaderParams my_shader_parameters;

      IBuffer* common_cb = test.device->SSGetConstantBuffer (ConstantBufferSemantic_Common);

      if (!common_cb)
      {
        printf ("Null common constant buffer\n");
        return;
      }

      common_cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);
      
      math::mat4f modelview = model.WorldTM () * my_shader_parameters.view_tm;
//      math::mat4f modelview = my_shader_parameters.view_tm * model.WorldTM ();
                                          
      my_shader_parameters.object_tm          = model.WorldTM ();
      my_shader_parameters.model_view_tm      = transpose (modelview);
      my_shader_parameters.model_view_proj_tm = transpose (my_shader_parameters.proj_tm * modelview);

      common_cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
      
      test.device->ISSetIndexBuffer (mesh.index_buffer.get ());

      for (PrimitiveArray::const_iterator iter=mesh.primitives.begin (); iter!=mesh.primitives.end (); ++iter)
      {
        const ModelPrimitive& primitive = *iter;
        ModelVertexBuffer&    vb        = *primitive.vertex_buffer;
        ModelMaterial&        material  = *primitive.material;
        
        if (!&material)
          continue;
          
        for (int i=0; i<SamplerChannel_Num; i++)
          if (material.texmaps [i].texture && material.texmaps [i].sampler)
          {
            test.device->SSSetTexture (i, &*material.texmaps [i].texture);
            test.device->SSSetSampler (i, &*material.texmaps [i].sampler);
          }
          else
          {
            test.device->SSSetTexture (i, 0);
            test.device->SSSetSampler (i, 0);
          }
          
        test.device->SSSetConstantBuffer (ConstantBufferSemantic_Material, &*material.constant_buffer);
        
        test.device->ISSetInputLayout (vb.input_layout.get ());

        for (size_t i=0; i<vb.vertex_streams.size (); i++)
        {
          BufferPtr vs = vb.vertex_streams [i];

          test.device->ISSetVertexBuffer (i, vs.get ());
        }
        
        if (mesh.index_buffer)
        {
          test.device->DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
        }
        else
        {
          test.device->Draw (primitive.type, primitive.first, primitive.count);
        }
      }

      return;
    }

    printf ("Can't find mesh '%s'\n", model.MeshName ());
  }

  void Draw ()
  {
    test.scene.VisitEach (*this);
  }
};

typedef xtl::shared_ptr<Model> ModelPtr;

ModelPtr load_model (Test& test, const char* file_name)
{
  printf ("Load model '%s':\n", file_name);

  return ModelPtr (new Model (test, file_name));
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

void reload_shaders (Test& test)
{
  printf ("Load shaders\n");

  stl::string vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME),
              pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME);

  ShaderDesc shader_descs [] = {
    {VERTEX_SHADER_FILE_NAME, size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""},
    {PIXEL_SHADER_FILE_NAME, size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
  };

  ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
  
  test.device->SSSetProgram (shader.get ());

  test.shader = shader;
}

void reload (Test& test)
{
  reload_shaders (test);
}

Model* model_ptr = 0;

size_t frames_count = 0;

void redraw (Test& test)
{
  Model& model = *model_ptr;

  model.Draw ();

  frames_count++;
}

void idle (Test& test)
{
  if (test.window.IsClosed ())
    return;

  static size_t last     = 0;
  static size_t last_fps = 0;

  size_t current_time = common::milliseconds ();

  float dt = (current_time - last) / 1000.f;

  last = current_time;

  if (fabs (test.x_camera_speed) > EPS || fabs (test.y_camera_speed) > EPS)
    test.camera->Translate (math::vec3f (dt * test.x_camera_speed, dt * test.y_camera_speed, 0.f), NodeTransformSpace_Local);
  if (fabs (test.x_camera_rotation_speed) > EPS || fabs (test.y_camera_rotation_speed) > EPS)
    test.camera->Rotate (math::degree (dt * test.y_camera_rotation_speed), math::degree (dt * test.x_camera_rotation_speed), math::degree (0.f));

  if (current_time - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count) / float (current_time - last_fps) * 1000.f);

    last_fps = current_time;
    frames_count = 0;
    return;
  }

  CommonShaderParams my_shader_parameters;

  IBuffer* cb = test.device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);
  
  static float light_phi = -3.14 / 2;

  my_shader_parameters.light_pos = math::vec3f (10 * cos (light_phi), 0.f, 10 * sin (light_phi));
  my_shader_parameters.light_dir = -normalize (my_shader_parameters.light_pos);

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  test.window.Invalidate ();
}

//обновление матрицы проецирования
void update_proj_matrix (DevicePtr device, const math::mat4f& proj_matrix)
{
  CommonShaderParams my_shader_parameters;

  IBuffer* cb = device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  my_shader_parameters.proj_tm = proj_matrix;

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
}

//обновление матрицы проецирования
void update_view_matrix (DevicePtr device, const math::mat4f& view_matrix)
{
  CommonShaderParams my_shader_parameters;

  IBuffer* cb = device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  my_shader_parameters.view_tm = inverse (view_matrix);

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
}

int main ()
{
  printf ("Results of model_load_test:\n");

  try
  {
    Test test (L"OpenGL device test window (model_load)", &redraw, &reload, "*", "max_texture_size=1024 GL_ARB_texture_non_power_of_two=0");

    test.window.Show ();
    
    printf ("Set shader stage\n");
    
    reload_shaders (test);

    static ProgramParameter shader_parameters[] = {
      {"ModelViewProjectionMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, model_view_proj_tm)},
      {"ModelViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, model_view_tm)},
      {"LightPosition", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_pos)},
      {"LightDirection", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_dir)},
      {"BumpTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, bump_sampler)},
      {"DiffuseTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, diffuse_sampler)},
      {"AmbientTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, ambient_sampler)},
      {"SpecularTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, specular_sampler)},
      {"EmissionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, emission_sampler)},
      {"ReflectionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, reflection_sampler)},      
      
      {"ShaderType", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, shader_type)},
      {"Reflectivity", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, reflectivity)},
      {"Transparency", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, transparency)},   
      {"Shininess", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, shininess)},
      {"BumpAmount", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_amount)},
      {"BumpTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_texture_channel)},
      {"DiffuseTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_texture_channel)},
      {"AmbientTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_texture_channel)},
      {"SpecularTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_texture_channel)},
      {"EmissionTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_texture_channel)},
      {"DiffuseColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_color)},
      {"AmbientColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_color)},
      {"SpecularColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_color)},
      {"EmissionColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_color)},
    };
    
    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};
    
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (CommonShaderParams);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    CommonShaderParams my_shader_parameters;

    my_shader_parameters.proj_tm = test.camera->ProjectionMatrix ();
    my_shader_parameters.view_tm = inverse (test.camera->WorldTM ());
    
    my_shader_parameters.bump_sampler       = SamplerChannel_Bump;
    my_shader_parameters.diffuse_sampler    = SamplerChannel_Diffuse;
    my_shader_parameters.specular_sampler   = SamplerChannel_Specular;
    my_shader_parameters.ambient_sampler    = SamplerChannel_Ambient;
    my_shader_parameters.emission_sampler   = SamplerChannel_Emission;
    my_shader_parameters.reflection_sampler = SamplerChannel_Reflection;
    
    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (ConstantBufferSemantic_Common, cb.get ());

    printf ("Load model\n");

    ModelPtr model = load_model (test, MODEL_NAME);

    model_ptr = model.get ();

    printf ("Register callbacks\n");

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
