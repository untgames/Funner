#include "shared.h"

#include <gl/glew.h> ///????

const char* PIXEL_SHADER_FILE_NAME  = "data/glsl/wood.frag";
const char* VERTEX_SHADER_FILE_NAME = "data/glsl/wood.vert";
const char* MODEL_NAME              = "data/teapot1.xmesh";

#pragma pack (1)

typedef stl::vector<BufferPtr> BufferArray;

struct ModelVertexBuffer
{
  BufferArray    vertex_streams;
  InputLayoutPtr input_layout;
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
};

typedef xtl::shared_ptr<ModelMesh> ModelMeshPtr;
typedef stl::vector<ModelMeshPtr>  ModelMeshArray;

struct Model
{
  DevicePtr                    device;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;
  ModelMeshArray               meshes;
    
  Model (const DevicePtr& in_device, const char* name) : device (in_device), library (name)
  {
    LoadVertexBuffers ();
    LoadMeshes ();
    
    printf ("Model has loaded successful!\n");    
  }
  
  void LoadMeshes ()
  {
    printf ("Load meshes\n");    
    
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)    
    {
      media::geometry::Mesh& mesh = *iter;      
      
      printf ("Load mesh '%s'\n", mesh.Name ());
      
      ModelMeshPtr model_mesh (new ModelMesh);
      
      printf ("Add vertex buffers\n");
      
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
      
      printf ("Add index buffer\n");      
      
      const media::geometry::IndexBuffer& ib = mesh.IndexBuffer ();
      
      BufferMap::iterator iter = index_buffers.find (ib.Id ());
      BufferPtr ib_buffer;
      
      if (iter == index_buffers.end ())
      {
        if (ib.Size ())
        {        
          printf ("Create new hw index-buffer\n");                
          
          BufferDesc desc;
          
          memset (&desc, 0, sizeof desc);
          
          desc.size         = ib.Size () * sizeof size_t;
          desc.usage_mode   = UsageMode_Default;
          desc.bind_flags   = BindFlag_IndexBuffer;
          desc.access_flags = AccessFlag_ReadWrite;
          
          BufferPtr (device->CreateBuffer (desc), false).swap (ib_buffer);
          
          ib_buffer->SetData (0, desc.size, ib.Data ());
        }

        index_buffers [ib.Id ()] = ib_buffer;
      }
      else ib_buffer = iter->second;
      
      model_mesh->index_buffer = ib_buffer;
      
      printf ("Add primitives\n");
      
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
            dst_primitive.count = src_primitive.count - 1;
            break;
          case media::geometry::PrimitiveType_TriangleList:
            dst_primitive.type  = PrimitiveType_TriangleList;
            dst_primitive.count = src_primitive.count * 3;
            break;
          case media::geometry::PrimitiveType_TriangleStrip:
            dst_primitive.type = PrimitiveType_TriangleStrip;
            dst_primitive.count = src_primitive.count - 2;
            break;          
          case media::geometry::PrimitiveType_TriangleFan:
            dst_primitive.type = PrimitiveType_TriangleFan;
            dst_primitive.count = src_primitive.count - 2;
            break;            
          default:
            continue;
        }
        
        if (src_primitive.vertex_buffer >= model_mesh->vertex_buffers.size ())
          continue;          
        
        dst_primitive.vertex_buffer = model_mesh->vertex_buffers [src_primitive.vertex_buffer];
        dst_primitive.first         = src_primitive.first;

        printf ("primitive#%u: type=%s vb-slot=%u first=%u count=%u\n", i, get_name (dst_primitive.type), src_primitive.vertex_buffer,
                dst_primitive.first, dst_primitive.count);

        model_mesh->primitives.push_back (dst_primitive);
      }

      meshes.push_back (model_mesh);
    }
  }

  void LoadVertexBuffers ()
  {    
    printf ("Load vertex buffers:\n");
    
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;
      
      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);
        
        printf ("Load VB#%u\n", vb.Id ());
        
        VertexBufferPtr model_vb (new ModelVertexBuffer);
        
        stl::vector<VertexAttribute> vertex_attributes;
        
        for (size_t j=0, streams_count=vb.StreamsCount (); j<streams_count; j++)
        {
          media::geometry::VertexStream& vs = vb.Stream (j);
          
          printf ("Load VS#%u\n", vs.Id ());          
          
          BufferMap::iterator iter = vertex_streams.find (vs.Id ());
          BufferPtr vs_buffer;
          
          if (iter == vertex_streams.end ())
          {            
            printf ("Create new hw vertex-buffer\n");
            
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
            
            printf ("va #%u: semantic=%s type=%s format=%s stride=%u slot=%u offset=%u\n",
                    vertex_attributes.size (), get_name (dst_va.semantic), get_name (dst_va.type), get_name (dst_va.format),
                    dst_va.stride, dst_va.slot, dst_va.offset);
            
            vertex_attributes.push_back (dst_va);
          }

          model_vb->vertex_streams.push_back (vs_buffer);
        }
        
        InputLayoutDesc layout_desc;
        
        memset (&layout_desc, 0, sizeof layout_desc);

        layout_desc.vertex_attributes_count = vertex_attributes.size ();
        layout_desc.vertex_attributes       = &vertex_attributes [0];
        layout_desc.index_type              = InputDataType_UInt;
        layout_desc.index_buffer_offset     = 0;          
        
        model_vb->input_layout = InputLayoutPtr (device->CreateInputLayout (layout_desc), false);

        vertex_buffers [vb.Id ()] = model_vb;
      }
    }
  }
  
  void Draw ()
  {
    for (ModelMeshArray::iterator iter=meshes.begin (); iter!=meshes.end (); ++iter)
    {
      const ModelMesh& mesh = **iter;
      
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
    }
  }
};

typedef xtl::shared_ptr<Model> ModelPtr;

#pragma pack (1)

struct MyShaderParameters
{
  float        grain_size_recip;
  math::vec3f  dark_color;
  math::vec3f  color_spread;
  math::vec3f  light_position;
  float        scale;
  math::mat4f  transform;  
};

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

void redraw (Test& test)
{   
  Model& model = *model_ptr;

  model.Draw ();
}

int main ()
{
  printf ("Results of model_load_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (model_load)", &redraw, "check_gl_errors=0");
    
    test.window.Show ();    
    
    ModelPtr model = load_model (test.device, MODEL_NAME);
    
    model_ptr = model.get ();
    
    printf ("Set shader stage\n");
    
    stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);
    
    ShaderDesc shader_descs [] = {
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""}
    };

    static ProgramParameter shader_parameters[] = {
      {"GrainSizeRecip", ProgramParameterType_Float, 0, 1, offsetof (MyShaderParameters, grain_size_recip)},
      {"DarkColor", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, dark_color)},
      {"colorSpread", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, color_spread)},
      {"LightPosition", ProgramParameterType_Float3, 0, 1, offsetof (MyShaderParameters, light_position)},
      {"Scale", ProgramParameterType_Float, 0, 1, offsetof (MyShaderParameters, scale)},
      {"Transform", ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, transform)}
    };
    
    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print));
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc));

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters = {
      1.0f,
      math::vec3f (0.6f, 0.3f, 0.1f),
      math::vec3f (0.15f, 0.15f / 2.0f, 0),
      math::vec3f (0.0f, 40.0f, 40.0f),
      1.f
    };

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());
    
    glMatrixMode (GL_PROJECTION);
    glOrtho (-100, 100, -100, 100, -1000, 1000);
    glMatrixMode (GL_MODELVIEW);
    glRotatef    (-90, 1, 0, 0);
    glRotatef    (180, 0, 0, 1);
//    glTranslatef (0, 0, 400);
    glTranslatef (0, -400, 0);   

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
