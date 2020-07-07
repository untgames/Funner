#ifndef RENDER_SCENE_SERVER_DYNAMIC_MESH_SHARED_HEADER
#define RENDER_SCENE_SERVER_DYNAMIC_MESH_SHARED_HEADER

#include <shared/sg/visual_model.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Dynaic mesh
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicMesh: public VisualModel
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicMesh  (RenderManager& render_manager);
    ~DynamicMesh ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Updating mesh data
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateMesh (const media::geometry::Mesh& mesh);
    void UpdateIndexBufferData (media::geometry::object_id_t source_id, const void* buffer, size_t buffer_size);
    void UpdateVertexStreamData (media::geometry::object_id_t source_id, const void* buffer, size_t buffer_size);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif