#include "shared.h"

const char* SHADER_FILE_NAME  = "data/ffp_shader.wxf";
const char* MODEL_NAME        = "data/meshes.xmesh";
const char* SCENE_NAME        = "data/scene.xscene";

#pragma pack (1)

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
  PrimitiveType   type;          //тип примитива
  VertexBufferPtr vertex_buffer; //вершинный буфер
  size_t          first;         //индекс первой вершины/индекса
  size_t          count;         //количество примитивов
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

#pragma pack (1)

struct MyShaderParameters
{
  math::mat4f object_tm;
  math::mat4f view_tm;
  math::mat4f proj_tm;
  math::vec3f light_pos;
  math::vec3f light_dir;
};

typedef xtl::shared_ptr<ModelMesh> ModelMeshPtr;
typedef stl::vector<ModelMeshPtr>  ModelMeshArray;

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

struct Model : public xtl::visitor<void, scene_graph::VisualModel>
{
  DevicePtr                    device;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;
  ModelMeshArray               meshes;
  scene_graph::Scene           scene;

  Model (const DevicePtr& in_device, const char* name) : device (in_device), library (name)
  {
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

    new_node->Translate (transform.column (3));

    new_node->BindToParent (*parent, scene_graph::NodeBindMode_AddRef);

    for_each_child (*node, "mesh", xtl::bind (&Model::ReadMeshInfo, this, _1, new_node));
    for_each_child (*node, "node", xtl::bind (&Model::ReadNodeInfo, this, _1, new_node));
  }

  void LoadScene ()
  {
    common::Parser   p (SCENE_NAME);
    common::ParseLog log = p.Log ();

    for_each_child (p.Root (), "scene.node", xtl::bind (&Model::ReadNodeInfo, this, _1, scene_graph::Node::Pointer (&scene.Root ())));

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

      printf ("Loaded mesh '%s'\n", mesh.Name ());

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

          BufferPtr (device->CreateBuffer (desc), false).swap (ib_buffer);

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

            vs_buffer = BufferPtr (device->CreateBuffer (desc), false);

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

        model_vb->input_layout = InputLayoutPtr (device->CreateInputLayout (layout_desc), false);

        vertex_buffers [vb.Id ()] = model_vb;
      }
    }
  }

  void visit (scene_graph::VisualModel& model)
  {
    for (ModelMeshArray::iterator iter=meshes.begin (); iter!=meshes.end (); ++iter)
    {
      const ModelMesh& mesh = **iter;

      if (xtl::xstrcmp (model.MeshName (), mesh.name.c_str ()))
        continue;

      MyShaderParameters my_shader_parameters;

      IBuffer* cb = device->SSGetConstantBuffer (0);

      if (!cb)
      {
        printf ("Null constant buffer #0\n");
        return;
      }

      cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

      my_shader_parameters.object_tm = model.WorldTM ();

      cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

      device->ISSetIndexBuffer (mesh.index_buffer.get ());

      for (PrimitiveArray::const_iterator iter=mesh.primitives.begin (); iter!=mesh.primitives.end (); ++iter)
      {
        const ModelPrimitive& primitive = *iter;
        ModelVertexBuffer&    vb        = *primitive.vertex_buffer;

        device->ISSetInputLayout (vb.input_layout.get ());

        for (size_t i=0; i<vb.vertex_streams.size (); i++)
        {
          BufferPtr vs = vb.vertex_streams [i];

          device->ISSetVertexBuffer (i, vs.get ());
        }

        if (mesh.index_buffer)
        {
          device->DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
        }
        else
        {
          device->Draw (primitive.type, primitive.first, primitive.count);
        }
      }

      return;
    }

    printf ("Can't find mesh '%s'\n", model.MeshName ());
  }

  void Draw ()
  {
    scene.VisitEach (*this);
  }
};

typedef xtl::shared_ptr<Model> ModelPtr;

ModelPtr load_model (const DevicePtr& device, const char* file_name)
{
  printf ("Load model '%s':\n", file_name);

  return ModelPtr (new Model (device, file_name));
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
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

  static size_t last = 0;
  static float angle;

  static size_t last_fps = 0;

  float dt = float (common::milliseconds () - last) / 1000.f;

  if (common::milliseconds () - last > 25)
  {
    last = common::milliseconds ();
    return;
  }

  if (common::milliseconds () - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);

    last_fps = common::milliseconds ();
    frames_count = 0;
    return;
  }

  MyShaderParameters my_shader_parameters;

  IBuffer* cb = test.device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }

  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  angle += 0.5f*dt;

  my_shader_parameters.view_tm = inverse (math::lookat (math::vec3f (sin (angle) * 400, cos (angle) * 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1)));

  my_shader_parameters.light_pos = math::vec3f (40 * cos (angle), 40 * sin (angle), 0.0f);
  my_shader_parameters.light_dir = -normalize (my_shader_parameters.light_pos);

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

  test.window.Invalidate ();
}

//получение ортографической матрицы проекции
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar)
{
  math::mat4f proj_matrix;

  float width  = right - left,
        height = top - bottom,
        depth  = zfar - znear;

    //выбрана матрица проецирования, используемая gluOrtho2D

  proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = math::vec4f (0, 0, -2.0f / depth, - (znear + zfar) / depth);
  proj_matrix [3] = math::vec4f (0, 0, 0, 1);

  return proj_matrix;
}

int main ()
{
  printf ("Results of model_load_test:\n");

  try
  {
    Test test (L"OpenGL device test window (model_load)", &redraw);

    test.window.Show ();

    ModelPtr model = load_model (test.device, MODEL_NAME);

    model_ptr = model.get ();

    printf ("Set shader stage\n");

    stl::string shader_source  = read_shader (SHADER_FILE_NAME);

    ShaderDesc shader_descs [] = {
      {"ffp_shader", size_t (-1), shader_source.c_str (), "ffp", ""},
    };

    static ProgramParameter shader_parameters[] = {
      {"myProjMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, proj_tm)},
      {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, view_tm)},
      {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, object_tm)},
      {"lightPos", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_pos)},
      {"lightDir", ProgramParameterType_Float3, 0, 1, TEST_OFFSETOF (MyShaderParameters, light_dir)},
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters;

    my_shader_parameters.proj_tm   = get_ortho_proj (-10, 10, -10, 10, -1000, 1000);
    my_shader_parameters.view_tm = inverse (math::lookat (math::vec3f (0, 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1)));

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());

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
