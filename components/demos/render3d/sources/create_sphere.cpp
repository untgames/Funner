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
  float angle = 2.0f * 3.14f;
  float a1    = 0.0f,
        da1   = angle / float (meridians-1),
        da2   = (float)3.14f / float (parallels+2);        
        
  build.push_vertex (math::vec3f (0.0f,1.0f,0.0f),math::vec3f (0.0f,1.0f,0.0f));
  build.push_vertex (math::vec3f (0.0f,-1.0f,0.0f),math::vec3f (0.0f,-1.0f,0.0f));
        
  int vbase = build.verts.size () - 1;
        
  for (size_t i=0;i<meridians;i++,a1+=da1)
  {
    float a2 = da2;
    
    float x = cos (a1),
          z = sin (a1);
    
    for (size_t j=0;j<parallels;j++,a2+=da2)
    {
      float r = sin (a2);
      math::vec3f v (r*x,cos (a2),r*z);
      
      build.push_vertex (v,v);
    }        
  }
  
  for (size_t i=0;i<meridians;i++) 
  {
    int v1 = vbase + i * parallels,
        v2 = vbase + (i+1) % meridians * parallels;
        
    build.push_face (0,v1,v2);
    build.push_face (1,v2+parallels-1,v1+parallels-1);
    
    for (size_t j=0;j<parallels-1;j++)
    {            
      build.push_face (v1+j,v1+j+1,v2+j+1);
      build.push_face (v1+j,v2+j+1,v2+j);
    }
  }  
}

}

//создание сферы
ModelMeshPtr create_sphere (IDevice& device, size_t parallels, size_t meridians, ModelMaterialPtr& material)
{
  parallels = stl::max (1u, parallels);
  meridians = stl::max (2u, meridians);
  
  Build build;
  
  build.verts.reserve (parallels*meridians+2);
  build.verts.reserve (2 * (parallels+1) * meridians * 3);  

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
    {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, sizeof (Vertex)},
    {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, sizeof (Vertex)},    
  };
  
  static const size_t vertex_attributes_count = sizeof (vertex_attributes) / sizeof (*vertex_attributes);
  
  InputLayoutDesc layout_desc;
  
  memset (&layout_desc, 0, sizeof (layout_desc));
  
  layout_desc.vertex_attributes_count = vertex_attributes_count;
  layout_desc.vertex_attributes       = vertex_attributes;
  layout_desc.index_type              = InputDataType_Int;
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
    
  ModelMeshPtr mesh (new ModelMesh (""));
  
  mesh->vertex_buffers.push_back (vb);
  
  mesh->index_buffer = ib;
  
  ModelPrimitive primitive;
  
  primitive.type          = PrimitiveType_TriangleList;
  primitive.material      = material;
  primitive.vertex_buffer = vb;
  primitive.first         = 0;
  primitive.count         = build.indices.size () / 3;
  
  mesh->primitives.push_back (primitive);
  
  return mesh;
}
