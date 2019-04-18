#include "shared.h"

using namespace media::geometry;

/*
    MeshObserver implementation
*/

namespace
{

//Constants
const unsigned int RESERVE_VERTEX_STREAMS_PER_VERTEX_BUFFER = 2; //typically vertex buffer contains two vertex streams

//Structure holding update data for each object
struct UpdateDataCacheEntry
{
  unsigned int    structure_update_index;     //last reported structure_update_index
  unsigned int    data_update_index;          //last reported data update index
  xtl::connection object_destroy_connection;  //object destroy connection

  UpdateDataCacheEntry (int in_structure_update_index, int in_data_update_index, const xtl::connection& in_object_destroy_connection)
    : structure_update_index (in_structure_update_index)
    , data_update_index (in_data_update_index)
    , object_destroy_connection (in_object_destroy_connection)
    {}
};

typedef stl::hash_map<object_id_t, UpdateDataCacheEntry> UpdateDataCache;
typedef stl::vector<const VertexStream*>                 VertexStreamPtrArray;
typedef stl::vector<const VertexWeightStream*>           VertexWeightStreamPtrArray;
typedef xtl::array<UpdateDataCache, ObjectType_Num>      UpdateDataCacheArray;

}

struct MeshObserver::Impl: public xtl::reference_counter
{
  UpdateDataCacheArray       update_data_cache;                  //cache holding last reported update indices
  VertexStreamPtrArray       changed_data_vertex_streams;        //array holding pointers to vertex streams whose data was changed and should be reported after structure change reports
  VertexWeightStreamPtrArray changed_data_vertex_weight_streams; //array holding pointers to vertex weight streams whose data was changed and should be reported after structure change reports

  ~Impl ()
  {
    for (unsigned int i = 0, count = update_data_cache.size (); i < count; i++)
    {
      for (UpdateDataCache::iterator iter = update_data_cache [i].begin (), end = update_data_cache [i].end (); iter != end; ++iter)
      {
        iter->second.object_destroy_connection.disconnect ();
      }
    }
  }

  void RemoveCacheEntry (unsigned int object_type, object_id_t id)
  {
    update_data_cache [object_type].erase (id);
  }

  template<class T>
  unsigned int GetObjectType (xtl::type<T>);

  unsigned int GetObjectType (xtl::type<Mesh>)               { return ObjectType_Mesh;               }
  unsigned int GetObjectType (xtl::type<MaterialMap>)        { return ObjectType_MaterialMap;        }
  unsigned int GetObjectType (xtl::type<IndexBuffer>)        { return ObjectType_IndexBuffer;        }
  unsigned int GetObjectType (xtl::type<VertexBuffer>)       { return ObjectType_VertexBuffer;       }
  unsigned int GetObjectType (xtl::type<VertexStream>)       { return ObjectType_VertexStream;       }
  unsigned int GetObjectType (xtl::type<VertexWeightStream>) { return ObjectType_VertexWeightStream; }

  template<class T>
  void CheckObjectChanged (const T& object, unsigned int structure_update_index, unsigned int data_update_index, bool& structure_changed, bool& data_changed)
  {
    unsigned int object_type = GetObjectType (xtl::type<T>());

    UpdateDataCache&          object_type_cache = update_data_cache [object_type];
    UpdateDataCache::iterator iter              = object_type_cache.find (object.SourceId ());

    if (iter == object_type_cache.end ())
    {
      structure_changed = true;
      data_changed      = false;

      object_type_cache.insert_pair (object.SourceId (), UpdateDataCacheEntry (structure_update_index, data_update_index, object.Trackable ().connect_tracker (xtl::bind (&Impl::RemoveCacheEntry, this, object_type, object.SourceId ()))));
    }
    else
    {
      if (iter->second.structure_update_index != structure_update_index)
      {
        structure_changed = true;
        data_changed      = false;

        iter->second.structure_update_index = structure_update_index;
        iter->second.data_update_index      = data_update_index;
      }
      else if (iter->second.data_update_index != data_update_index)
      {
        structure_changed = false;
        data_changed      = true;

        iter->second.data_update_index = data_update_index;
      }
      else
      {
        structure_changed = false;
        data_changed      = false;
      }
    }
  }
};

/*
    Constructor / destructor
*/

MeshObserver::MeshObserver ()
  : impl (new Impl, false)
  {}
  
MeshObserver::MeshObserver (const MeshObserver& observer)
  : impl (observer.impl)
  {}

MeshObserver::~MeshObserver ()
{
}

/*
    Assignment
*/

MeshObserver& MeshObserver::operator = (const MeshObserver& observer)
{
  impl = observer.impl;

  return *this;
}

/*
   Check meshes for changes and notifies listener. Calls structure changes first, after all structure changes calls data changes
*/

void MeshObserver::NotifyUpdates (const Mesh& mesh, IMeshUpdatesListener& listener) const
{
  bool continue_processing = true;

  bool structure_changed, data_changed;

  impl->CheckObjectChanged (mesh, mesh.CurrentStructureUpdateIndex (), mesh.CurrentPrimitivesDataUpdateIndex (), structure_changed, data_changed);

  if (structure_changed)
    listener.OnMeshStructureChanged (mesh, continue_processing);

  bool should_report_mesh_data_changed = data_changed;

  const IndexBuffer& ib = mesh.IndexBuffer ();

  impl->CheckObjectChanged (ib, ib.CurrentStructureUpdateIndex (), ib.CurrentDataUpdateIndex (), structure_changed, data_changed);

  if (structure_changed && continue_processing)
    listener.OnIndexBufferStructureChanged (ib, continue_processing);

  bool should_report_ib_data_changed = data_changed;

  unsigned int vertex_buffers_count = mesh.VertexBuffersCount ();

  impl->changed_data_vertex_streams.clear ();
  impl->changed_data_vertex_streams.reserve (vertex_buffers_count * RESERVE_VERTEX_STREAMS_PER_VERTEX_BUFFER);
  impl->changed_data_vertex_weight_streams.clear ();
  impl->changed_data_vertex_weight_streams.reserve (vertex_buffers_count);

  for (unsigned int i = 0; i < vertex_buffers_count; i++)
  {
    const VertexBuffer& vb = mesh.VertexBuffer (i);

    impl->CheckObjectChanged (vb, vb.CurrentStructureUpdateIndex (), 0, structure_changed, data_changed);

    if (structure_changed && continue_processing)
      listener.OnVertexBufferStructureChanged (vb, continue_processing);

    const VertexWeightStream& vws = vb.Weights ();

    impl->CheckObjectChanged (vws, vws.CurrentStructureUpdateIndex (), vws.CurrentDataUpdateIndex (), structure_changed, data_changed);

    if (structure_changed && continue_processing)
      listener.OnVertexWeightStreamStructureChanged (vws, continue_processing);

    if (data_changed && continue_processing)
      impl->changed_data_vertex_weight_streams.push_back (&vws);

    for (unsigned int j = 0, vs_count = vb.StreamsCount (); j < vs_count; j++)
    {
      const VertexStream& vs = vb.Stream (j);

      impl->CheckObjectChanged (vs, vs.CurrentStructureUpdateIndex (), vs.CurrentDataUpdateIndex (), structure_changed, data_changed);

      if (structure_changed && continue_processing)
        listener.OnVertexStreamStructureChanged (vs, continue_processing);

      if (data_changed && continue_processing)
        impl->changed_data_vertex_streams.push_back (&vs);
    }
  }

  if (should_report_mesh_data_changed && continue_processing)
    listener.OnMeshPrimitivesChanged (mesh, continue_processing);

  if (should_report_ib_data_changed && continue_processing)
    listener.OnIndexBufferDataChanged (ib, continue_processing);

  for (VertexStreamPtrArray::iterator iter = impl->changed_data_vertex_streams.begin (), end = impl->changed_data_vertex_streams.end (); iter != end; ++iter)
  {
    if (continue_processing)
      listener.OnVertexStreamDataChanged (**iter, continue_processing);
  }

  for (VertexWeightStreamPtrArray::iterator iter = impl->changed_data_vertex_weight_streams.begin (), end = impl->changed_data_vertex_weight_streams.end (); iter != end; ++iter)
  {
    if (continue_processing)
      listener.OnVertexWeightStreamDataChanged (**iter, continue_processing);
  }

  const MaterialMap& material_map = mesh.MaterialMap ();

  //use data index as structure index, because structure is checked first
  impl->CheckObjectChanged (material_map, material_map.CurrentDataUpdateIndex (), 0, structure_changed, data_changed);

  if (structure_changed && continue_processing)
    listener.OnMaterialMapDataChanged (material_map, continue_processing);
}

/*
    Swap
*/

void MeshObserver::Swap (MeshObserver& observer)
{
  swap (observer.impl, impl);
}

namespace media
{

namespace geometry
{

void swap (MeshObserver& observer1, MeshObserver& observer2)
{
  observer1.Swap (observer2);
}

}

}
