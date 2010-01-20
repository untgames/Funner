#include "shared.h"

namespace
{

struct MeshLoader
{
  Test&                        test;
  media::geometry::MeshLibrary library;
  BufferMap                    vertex_streams;
  VertexBufferMap              vertex_buffers;
  BufferMap                    index_buffers;  
  ModelMeshArray&              meshes;

  MeshLoader (Test& in_test, ModelMeshArray& in_meshes, const char* file_name)
    : test (in_test)
    , library (file_name)
    , meshes (in_meshes)
  {
    LoadVertexBuffers ();
    LoadMeshes ();
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
        dst_primitive.material      = test.material_manager.FindMaterial (common::format ("%s-fx", src_primitive.material).c_str ());
        
        if (!dst_primitive.material)
        {
          dst_primitive.material = test.material_manager.FindMaterial (common::format ("%s", src_primitive.material).c_str ());

          if (!dst_primitive.material)
          {
            char* material_suffix_begin = strstr ((char*)src_primitive.material, "-material");

            if (material_suffix_begin)
            {
              dst_primitive.material = test.material_manager.FindMaterial (stl::string (src_primitive.material, material_suffix_begin - src_primitive.material).c_str ());

              if (!dst_primitive.material)
              {
                printf ("No material '%s' for mesh '%s' primitive %u\n", src_primitive.material, mesh.Name (), i);
                continue;
              }
            }
          }
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
};

}

MeshManager::MeshManager (Test& in_test)
  : test (in_test)
{
}

///Загрузка мешей
void MeshManager::LoadMeshes (const char* file_name)
{
  MeshLoader (test, meshes, file_name);
}

///Регистрация меша
void MeshManager::RegisterMesh (const ModelMeshPtr& mesh)
{
  meshes.push_back (mesh);
}

///Поиск меша
ModelMeshPtr MeshManager::FindMesh (const char* name)
{
  if (!name)
    return ModelMeshPtr ();    

  for (ModelMeshArray::iterator iter=meshes.begin (); iter!=meshes.end (); ++iter)
  {
    const ModelMesh& mesh = **iter;        

    if (!xtl::xstrcmp (name, mesh.name.c_str ()))
      return *iter;
  }    
  
  return ModelMeshPtr ();
}
