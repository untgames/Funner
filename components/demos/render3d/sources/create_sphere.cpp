#include "shared.h"

namespace
{

#pragma pack (1)

struct Vertex
{
  math::vec3f pos;
  math::vec3f n;
};

typedef stl::vector<Vertex> VertexArray;
typedef stl::vector<int>    IndexArray;

struct Build
{
  VertexArray verts;
  IndexArray  indices;

  inline int push_vertex (const math::vec3f& pos,const math::vec3f& n)
  {
    Vertex v;
    
    v.pos = pos;
    v.n   = n;
    
    verts.push_back (v);

    return verts.size () - 1;
  }

  inline void push_face (int v1,int v2,int v3)
  {
    indices.push_back (v1);
    indices.push_back (v2);
    indices.push_back (v3);
  }
};

void build_volume (Build& build,size_t parallels,size_t meridians)
{
  float delta_parallel_angle = 3.14f / parallels,
        delta_meridian_angle = 6.28f / meridians;
        
  int vertex_index = 0;

  for (size_t parallel=0; parallel<=parallels; parallel++)
  {
    float r0 = sin (parallel * delta_parallel_angle),
          y0 = cos (parallel * delta_parallel_angle);
          
    for (size_t meridian=0; meridian<=meridians; meridian++)
    {
      float x0 = r0 * sin (meridian * delta_meridian_angle),
            z0 = r0 * cos (meridian * delta_meridian_angle);
            
      math::vec3f v (x0, y0, z0);
           
      build.push_vertex (v, v);
      
      if (meridian != meridians)
      {
        build.push_face (vertex_index + meridians + 1, vertex_index, vertex_index + meridians);
        build.push_face (vertex_index + meridians + 1, vertex_index + 1, vertex_index);
        
        vertex_index++;
      }
    }
  }
}

}

//создание сферы
ModelMeshPtr create_sphere (const char* name, IDevice& device, size_t parallels, size_t meridians, const ModelMaterialPtr& material)
{
  parallels = stl::max ((size_t)1, parallels);
  meridians = stl::max ((size_t)2, meridians);
  
  Build build;
  
  build.verts.reserve (parallels*meridians+2);
  build.indices.reserve (2 * (parallels+1) * meridians * 3);  

  build_volume (build, parallels, meridians);
  
  BufferDesc vs_desc;

  memset (&vs_desc, 0, sizeof vs_desc);

  vs_desc.size         = build.verts.size () * sizeof (Vertex);
  vs_desc.usage_mode   = UsageMode_Default;
  vs_desc.bind_flags   = BindFlag_VertexBuffer;
  vs_desc.access_flags = AccessFlag_Read | AccessFlag_Write;    

  BufferPtr vs (device.CreateBuffer (vs_desc), false);
  
  vs->SetData (0, vs_desc.size, &build.verts [0]);
  
  static const VertexAttribute vertex_attributes [] = {
    {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, 0, sizeof (Vertex)},
    {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, sizeof (math::vec3f), sizeof (Vertex)},    
  };
  
  static const size_t vertex_attributes_count = sizeof (vertex_attributes) / sizeof (*vertex_attributes);
  
  InputLayoutDesc layout_desc;
  
  memset (&layout_desc, 0, sizeof (layout_desc));
  
  layout_desc.vertex_attributes_count = vertex_attributes_count;
  layout_desc.vertex_attributes       = vertex_attributes;
  layout_desc.index_type              = InputDataType_UInt;
  layout_desc.index_buffer_offset     = 0;
  
  InputLayoutPtr layout (device.CreateInputLayout (layout_desc), false);
  
  BufferDesc ib_desc;

  memset (&ib_desc, 0, sizeof ib_desc);

  ib_desc.size         = build.indices.size () * sizeof (int);
  ib_desc.usage_mode   = UsageMode_Default;
  ib_desc.bind_flags   = BindFlag_IndexBuffer;
  ib_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

  BufferPtr ib (device.CreateBuffer (ib_desc), false);
  
  ib->SetData (0, ib_desc.size, &build.indices [0]);
  
  VertexBufferPtr vb (new ModelVertexBuffer);
  
  vb->vertex_streams.push_back (vs);
  
  vb->id           = 0;
  vb->input_layout = layout;
    
  ModelMeshPtr mesh (new ModelMesh (name));
  
  mesh->vertex_buffers.push_back (vb);
  
  mesh->index_buffer = ib;
  
  ModelPrimitive primitive;
  
  primitive.type          = PrimitiveType_TriangleList;
  primitive.material      = material;
  primitive.vertex_buffer = vb;
  primitive.first         = 0;
  primitive.count         = build.indices.size ();
  
  mesh->primitives.push_back (primitive);
  
  return mesh;
}
