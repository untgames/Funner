#include "shared.h"

using namespace media::geometry;
using namespace render::scene;
using namespace render::scene::client;

///Dynamic mesh
struct DynamicMeshHelper::Impl : public IMeshUpdatesListener
{
  const Node&           node;
  media::geometry::Mesh mesh;
  MeshObserver          observer;

///Constructor
  Impl (const Node& in_node, const media::geometry::Mesh& in_mesh)
    : node (in_node)
    , mesh (in_mesh)
    {}

///IMeshUpdateListener methods
  void OnMeshStructureChanged (const Mesh& mesh, bool& continue_processing)
  {
//      printf ("OnMeshStructureChanged, new update index = %u\n", mesh.CurrentStructureUpdateIndex ());

    UpdateMesh ();

    continue_processing = false;
  }

  void OnMeshPrimitivesChanged (const Mesh& mesh, bool& continue_processing)
  {
/*      printf ("OnMeshPrimitivesChanged, new update index = %u\n", mesh.CurrentPrimitivesDataUpdateIndex ());
          printf ("SOURCE ID = %llu, primitives count = %u\n", mesh.SourceId (), mesh.PrimitivesCount ());

    for (uint32_t i=0; i<mesh.PrimitivesCount (); i++)
    {
      const Primitive& p = mesh.Primitive (i);

      printf ("  primitive #%u: type='%s' vertex_buffer=%u first=%u count=%u material=%u ('%s')\n", i, get_type_name (p.type),
              p.vertex_buffer, p.first, p.count, p.material_id, mesh.MaterialMap ().MaterialName (p.material_id));
    }*/

    UpdateMesh ();

    continue_processing = false;
  }

  void OnIndexBufferStructureChanged (const IndexBuffer& ib, bool& continue_processing)
  {
//      printf ("OnIndexBufferStructureChanged, new update index = %u\n", ib.CurrentStructureUpdateIndex ());

    UpdateMesh ();

    continue_processing = false;
  }

  void OnIndexBufferDataChanged (const IndexBuffer& ib, bool& continue_processing)
  {
    render::scene::interchange::OutputStream& s = node.Scenes ().Context ().UpdateIndexBufferData ();

    s.Write (node.Id ());
    s.Write (ib.SourceId ());

    uint32_t data_size = ib.SerializationDataSize ();

    s.Write (data_size);

    ib.WriteData (s.WriteData (data_size), data_size);

    s.EndCommand ();
  }

  void OnVertexBufferStructureChanged (const VertexBuffer& vb, bool& continue_processing)
  {
//      printf ("OnVertexBufferStructureChanged, new update index = %u\n", vb.CurrentStructureUpdateIndex ());

    UpdateMesh ();

    continue_processing = false;
  }

  void OnVertexStreamStructureChanged (const VertexStream& vs, bool& continue_processing)
  {
//      printf ("OnVertexStreamStructureChanged, new update index = %u\n", vs.CurrentStructureUpdateIndex ());

    UpdateMesh ();

    continue_processing = false;
  }

  void OnVertexStreamDataChanged (const VertexStream& vs, bool& continue_processing)
  {
    render::scene::interchange::OutputStream& s = node.Scenes ().Context ().UpdateVertexStreamData ();

    s.Write (node.Id ());
    s.Write (vs.SourceId ());

    uint32_t data_size = vs.SerializationDataSize ();

    s.Write (data_size);

    vs.WriteData (s.WriteData (data_size), data_size);

    s.EndCommand ();
  }

  void OnMaterialMapDataChanged (const MaterialMap& material_map, bool& continue_processing)
  {
//      printf ("OnMaterialMapDataChanged, new update index = %u\n", material_map.CurrentDataUpdateIndex ());

    UpdateMesh ();

    continue_processing = false;
  }

  void UpdateCore (client::Context& context, const media::geometry::Mesh& new_mesh)
  {
    try
    {
      if (new_mesh.SourceId () != mesh.SourceId ())
      {
        MeshObserver ().Swap (observer);

        mesh = new_mesh;
      }

      observer.NotifyUpdates (mesh, *this);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::DynamicMeshHelper::UpdateCore");
      throw;
    }
  }

///Update whole mesh
  void UpdateMesh ()
  {
    node.Scenes ().Context ().UpdateDynamicMesh (node.Id (), mesh.Clone ());
  }
};

/*
   Constructor / Destructor
*/

DynamicMeshHelper::DynamicMeshHelper (const Node& node, const media::geometry::Mesh& mesh)
  : impl (new Impl (node, mesh))
{
}

DynamicMeshHelper::~DynamicMeshHelper ()
{
}

/*
   Synchronize
*/

void DynamicMeshHelper::UpdateCore (client::Context& context, const media::geometry::Mesh& mesh)
{
  impl->UpdateCore (context, mesh);
}
