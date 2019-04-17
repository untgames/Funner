#include "shared.h"

class MeshUpdatesListener : public IMeshUpdatesListener
{
  public:
    bool stop_processing_on_ib_structrure_change;

    MeshUpdatesListener ()
      : stop_processing_on_ib_structrure_change (false)
      {}

    void OnMeshStructureChanged (const Mesh& mesh, bool& continue_processing)
    {
      printf ("OnMeshStructureChanged, new update index = %u\n", mesh.CurrentStructureUpdateIndex ());
    }

    void OnMeshPrimitivesChanged (const Mesh& mesh, bool& continue_processing)
    {
      printf ("OnMeshPrimitivesChanged, new update index = %u\n", mesh.CurrentPrimitivesDataUpdateIndex ());
    }

    void OnIndexBufferStructureChanged (const IndexBuffer& ib, bool& continue_processing)
    {
      printf ("OnIndexBufferStructureChanged, new update index = %u\n", ib.CurrentStructureUpdateIndex ());

      if (stop_processing_on_ib_structrure_change)
        continue_processing = false;
    }

    void OnIndexBufferDataChanged (const IndexBuffer& ib, bool& continue_processing)
    {
      printf ("OnIndexBufferDataChanged, new update index = %u\n", ib.CurrentDataUpdateIndex ());
    }

    void OnVertexBufferStructureChanged (const VertexBuffer& vb, bool& continue_processing)
    {
      printf ("OnVertexBufferStructureChanged, new update index = %u\n", vb.CurrentStructureUpdateIndex ());
    }

    void OnVertexStreamStructureChanged (const VertexStream& vs, bool& continue_processing)
    {
      printf ("OnVertexStreamStructureChanged, new update index = %u\n", vs.CurrentStructureUpdateIndex ());
    }

    void OnVertexStreamDataChanged (const VertexStream& vs, bool& continue_processing)
    {
      printf ("OnVertexStreamDataChanged, new update index = %u\n", vs.CurrentDataUpdateIndex ());
    }

    void OnVertexWeightStreamStructureChanged (const VertexWeightStream& vws, bool& continue_processing)
    {
      printf ("OnVertexWeightStreamStructureChanged, new update index = %u\n", vws.CurrentStructureUpdateIndex ());
    }

    void OnVertexWeightStreamDataChanged (const VertexWeightStream& vws, bool& continue_processing)
    {
      printf ("OnVertexWeightStreamDataChanged, new update index = %u\n", vws.CurrentDataUpdateIndex ());
    }

    void OnMaterialMapDataChanged (const MaterialMap& material_map, bool& continue_processing)
    {
      printf ("OnMaterialMapDataChanged, new update index = %u\n", material_map.CurrentDataUpdateIndex ());
    }
};


int main ()
{
  MeshUpdatesListener listener;
  Mesh                mesh1;
  MeshObserver        observer;
  Mesh                mesh2;
  
  printf ("Initial notify for mesh1:\n");

  observer.NotifyUpdates (mesh1, listener);

  printf ("Second initial notify for mesh1:\n");

  observer.NotifyUpdates (mesh1, listener);

  listener.stop_processing_on_ib_structrure_change = true;

  printf ("Initial notify for mesh2:\n");

  observer.NotifyUpdates (mesh2, listener);

  listener.stop_processing_on_ib_structrure_change = false;

  mesh2 = mesh1.Clone ();

  printf ("Initial notify for cloned mesh2:\n");

  observer.NotifyUpdates (mesh2, listener);

  mesh2.DetachAllBuffers ();

  printf ("Notify after detaching all buffers:\n");

  observer.NotifyUpdates (mesh2, listener);

  mesh2.IndexBuffer ().Resize (5);

  VertexBuffer vb;

  mesh2.Attach (vb);

  mesh2.MaterialMap ().SetMaterial ("material1");

  VertexFormat position_vertex_format;

  position_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Position, media::geometry::VertexAttributeType_Float2, 0);

  VertexStream vs (position_vertex_format);

  vb.Attach (vs);

  printf ("Notify after attaching buffers:\n");

  observer.NotifyUpdates (mesh2, listener);

  printf ("Second notify after attaching buffers:\n");

  observer.NotifyUpdates (mesh2, listener);

  mesh2.IndexBuffer ().InvalidateData ();
  vs.InvalidateData ();
  vb.Weights ().InvalidateData ();
  mesh2.MaterialMap ().RemoveMaterial ("material1");
  mesh2.AddPrimitive (PrimitiveType_TriangleList, 0, 0, 3, (uint32_t)0);

  printf ("Notify after updating buffers:\n");

  observer.NotifyUpdates (mesh2, listener);

  VertexBuffer vb2;

  vb2.Attach (vs);

  vb.Weights ().Resize (10);

  vs.Resize (10);

  mesh2.DetachIndexBuffer ();
  mesh2.Attach (vb2);
  mesh2.IndexBuffer ().InvalidateData ();
  vs.InvalidateData ();
  vb.Weights ().InvalidateData ();

  printf ("Notify after changing structure and data:\n");

  observer.NotifyUpdates (mesh2, listener);

  listener.stop_processing_on_ib_structrure_change = true;

  mesh2.DetachIndexBuffer ();
  vs.InvalidateData ();
  vb.Weights ().InvalidateData ();

  printf ("Notify after changing structure and data and stopping on ib structure change:\n");

  observer.NotifyUpdates (mesh2, listener);

  listener.stop_processing_on_ib_structrure_change = false;

  mesh2.DetachIndexBuffer ();
  vs.InvalidateData ();
  vb.Weights ().InvalidateData ();

  printf ("Second notify after changing structure and data and without stopping on ib structure change:\n");

  observer.NotifyUpdates (mesh2, listener);

  return 0;
}
