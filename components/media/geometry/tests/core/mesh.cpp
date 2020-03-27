#include "shared.h"

void dump (const Mesh& mesh)
{
  printf ("Mesh id=%llu, source_id=%llu, name='%s' (%u vertex_buffers, %u indices, %u primitives)\n",
          mesh.Id (), mesh.SourceId (), mesh.Name (), mesh.VertexBuffersCount (),
          mesh.IndexBuffer ().Size (), mesh.PrimitivesCount ());
          
  for (uint32_t i=0; i<mesh.VertexBuffersCount (); i++)
  {
    const VertexBuffer& vb = mesh.VertexBuffer (i);
    
    printf ("  vb#%u: vertices_count=%u, weights_count=%u\n", i, vb.VerticesCount (), vb.Weights ().Size ());
  }

  const MaterialMap& material_map = mesh.MaterialMap ();

  for (uint32_t i=0; i<mesh.PrimitivesCount (); i++)
  {
    const Primitive& p = mesh.Primitive (i);
    
    printf ("  primitive #%u: type='%s' vertex_buffer=%u first=%u count=%u material=%u ('%s')\n", i, get_type_name (p.type),
            p.vertex_buffer, p.first, p.count, p.material_id, material_map.MaterialName (p.material_id));
  }
}

int main ()
{
  Mesh mesh1;
  
  mesh1.Rename ("mesh1");
  
  VertexStream vs (get_vertex_format<CustomVertex> ());
  VertexBuffer vb;
  
  vb.Attach (vs);  
  
  mesh1.Attach (vb);
    
  IndexBuffer ib (12);
  
  mesh1.Attach (ib);
  
  vs.Resize (5);
  
  printf ("dump mesh\n");
  
  dump (mesh1);
  
  printf ("add primitives\n");
  
  mesh1.AddPrimitive (PrimitiveType_TriangleList, 0, 12, 5, "my_material");
  mesh1.AddPrimitive (PrimitiveType_TriangleStrip, 0, 14, 8, "my_material1");
  
  dump (mesh1);
  
  printf ("resize vertex stream\n");
  
  vs.Resize (10);
  
  dump (mesh1);
  
  Mesh mesh2;
  
  printf ("copy mesh\n");
  
  mesh2 = mesh1.Clone ();
  
  mesh2.Rename ("mesh2");
  
  dump (mesh2);

  if (mesh2.SourceId () == mesh1.Id ())
    printf ("mesh source id is correct after clone\n");

  if (mesh2.IndexBuffer ().SourceId () == mesh1.IndexBuffer ().Id ())
    printf ("ib source id is correct after clone\n");

  if (mesh2.MaterialMap ().SourceId () == mesh1.MaterialMap ().Id ())
    printf ("material map source id is correct after clone\n");

  for (size_t i = 0, count = mesh2.VertexBuffersCount (); i < count; i++)
  {
    if (mesh2.VertexBuffer (i).SourceId () == mesh1.VertexBuffer (i).Id ())
      printf ("vb source id is correct after clone\n");
  }

  printf ("mesh1 serialization size is %u\n", mesh1.SerializationSize ());
  printf ("mesh1 serialization data size is %u\n", mesh1.SerializationPrimitivesDataSize ());

  xtl::uninitialized_storage<char> serialization_buffer (mesh1.SerializationSize ());

  size_t written_bytes = mesh1.Write (serialization_buffer.data (), serialization_buffer.size ());

  printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

  size_t bytes_read;

  Mesh mesh3 = Mesh::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), bytes_read);

  printf ("Bytes read during deserialization = %u\n", bytes_read);

  printf ("deserialized mesh:\n");

  dump (mesh3);

  mesh3.RemoveAllPrimitives ();

  written_bytes = mesh1.WritePrimitivesData (serialization_buffer.data (), serialization_buffer.size ());

  printf ("Bytes written during data serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

  printf ("Bytes read during data deserialization = %u\n", mesh3.ReadPrimitivesData (serialization_buffer.data (), serialization_buffer.size ()));

  printf ("deserialized mesh data:\n");

  dump (mesh3);
  
  printf ("clear mesh\n");
  
  mesh1.Clear ();
  
  dump (mesh1);
  
  printf ("detach mesh buffers\n");
  
  mesh2.DetachAllBuffers ();
  
  dump (mesh2);
  
  printf ("remove primitives\n");
  
  mesh2.RemoveAllPrimitives ();
  
  dump (mesh2);
  
  return 0;
}
