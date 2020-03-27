#include "shared.h"

using namespace media::geometry;
using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Dynamic mesh
class DynamicMesh: public VisualModel, public IMeshUpdatesListener
{
  public:
///Constructor
    DynamicMesh (scene_graph::DynamicMesh& mesh, SceneManager& manager)
      : VisualModel (mesh, manager, interchange::NodeType_DynamicMesh)
      , prev_mesh_source_id (mesh.Mesh ().SourceId ())
      {}

///Source node
    scene_graph::DynamicMesh& SourceNode () const { return static_cast<scene_graph::DynamicMesh&> (Node::SourceNode ()); }

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
      render::scene::interchange::OutputStream& s = Scenes ().Context ().UpdateIndexBufferData ();

      s.Write (Id ());
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
      render::scene::interchange::OutputStream& s = Scenes ().Context ().UpdateVertexStreamData ();

      s.Write (Id ());
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

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::DynamicMesh& mesh = SourceNode ();

        const media::geometry::Mesh& media_mesh = mesh.Mesh ();

        if (media_mesh.SourceId () != prev_mesh_source_id)
        {
          prev_mesh_source_id = media_mesh.SourceId ();

          MeshObserver ().Swap (observer);
        }

        observer.NotifyUpdates (media_mesh, *this);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::DynamicMesh::UpdateCore");
        throw;
      }
    }

  private:
///Update whole mesh
    void UpdateMesh ()
    {
      scene_graph::DynamicMesh& mesh = SourceNode ();

      Scenes ().Context ().UpdateDynamicMesh (Id (), mesh.Mesh ().Clone ());
    }

  private:
    media::geometry::object_id_t prev_mesh_source_id;
    MeshObserver                 observer;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::DynamicMesh& mesh, SceneManager& scene_manager)
{
  return new DynamicMesh (mesh, scene_manager);
}

}}}
