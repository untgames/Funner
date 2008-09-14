#include <stl/hash_map>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <media/mesh.h>

#include "shared.h"

using namespace render::low_level;

#pragma pack (1)

typedef xtl::com_ptr<IBuffer>      BufferPtr;
typedef xtl::com_ptr<IInputLayout> InputLayoutPtr;
typedef stl::vector<BufferPtr>     BufferArray;

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
};

typedef xtl::shared_ptr<ModelMesh>             ModelMeshPtr;
typedef stl::vector<ModelMeshPtr>              ModelMeshArray;

struct RenderableModel::Impl: public xtl::trackable
{
  IDevice*                     device;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;
  ModelMeshArray               meshes;

///Конструктор
  Impl (IDevice& in_device, const char* file_name)
    : device (&in_device), library (file_name)
  {    
    LoadVertexBuffers ();
    LoadMeshes ();
  }

  void LoadMeshes ()
  {
//    printf ("Load meshes\n");    
    
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)    
    {
      media::geometry::Mesh& mesh = *iter;      
      
      if (!check (mesh, xtl::bind (&Impl::LogMessage, this, _1)))
        continue;
      
//      printf ("Load mesh '%s'\n", mesh.Name ());
      
      ModelMeshPtr model_mesh (new ModelMesh);
      
//      printf ("Add vertex buffers\n");
      
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
      
//      printf ("Add index buffer\n");      
      
      const media::geometry::IndexBuffer& ib = mesh.IndexBuffer ();
      
      BufferMap::iterator iter = index_buffers.find (ib.Id ());
      BufferPtr ib_buffer;
      
      if (iter == index_buffers.end ())
      {
        if (ib.Size ())
        {        
//          printf ("Create new hw index-buffer (%u indices)\n", ib.Size ());
          
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
      
//      printf ("Add primitives\n");
      
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

//        printf ("primitive#%u: type=%s vb-slot=%u vb-id=VB#%u vb-ptr=%p first=%u count=%u (vb-slots=%u)\n", i, get_name (dst_primitive.type), src_primitive.vertex_buffer,
//                dst_primitive.vertex_buffer->id, dst_primitive.vertex_buffer.get (), dst_primitive.first, dst_primitive.count, model_mesh->vertex_buffers.size ());

        model_mesh->primitives.push_back (dst_primitive);
      }

      meshes.push_back (model_mesh);
    }
  }

  void LoadVertexBuffers ()
  {    
//    printf ("Load vertex buffers:\n");
    
    for (media::geometry::MeshLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::geometry::Mesh& mesh = *iter;
      
      for (size_t i=0, vb_count=mesh.VertexBuffersCount (); i<vb_count; i++)
      {
        media::geometry::VertexBuffer& vb = mesh.VertexBuffer (i);
        
//        printf ("Load VB#%u\n", vb.Id ());
        
        VertexBufferPtr model_vb (new ModelVertexBuffer);
        
        model_vb->id = vb.Id ();
        
        stl::vector<VertexAttribute> vertex_attributes;
        
        for (size_t j=0, streams_count=vb.StreamsCount (); j<streams_count; j++)
        {
          media::geometry::VertexStream& vs = vb.Stream (j);
          
//          printf ("Load VS#%u\n", vs.Id ());          
          
          BufferMap::iterator iter = vertex_streams.find (vs.Id ());
          BufferPtr vs_buffer;
          
          if (iter == vertex_streams.end ())
          {            
//            printf ("Create new hw vertex-buffer\n");
            
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
            
//            printf ("va #%u: semantic=%s type=%s format=%s stride=%u slot=%u offset=%u\n",
//                    vertex_attributes.size (), get_name (dst_va.semantic), get_name (dst_va.type), get_name (dst_va.format),
//                    dst_va.stride, dst_va.slot, dst_va.offset);
            
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
  
  void Draw (IDevice& in_device)
  {
    if (device != &in_device)
    {
      printf ("Invalid device\n");
      return;
    }

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
          printf ("draw %u vertices from vertex %u\n", primitive.count, primitive.first);
          device->DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
        }
        else
        {
          device->Draw (primitive.type, primitive.first, primitive.count);
        }        
      }
    }
  }
  
  private:
    void LogMessage (const char* message)
    {
      printf ("%s\n", message);
    }
};

RenderableModel::RenderableModel (IDevice& device, const char* file_name)
  : impl (new Impl (device, file_name))
  {}

RenderableModel::~RenderableModel ()
{
}

void RenderableModel::Draw (IDevice& device)
{
  impl->Draw (device);
}

