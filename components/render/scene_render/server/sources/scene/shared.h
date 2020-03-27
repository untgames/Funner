#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <math/utility.h>

#include <media/geometry/mesh.h>
#include <media/particles/particle_system.h>

#include <render/manager.h>

#include <shared/camera.h>
#include <shared/particle_system_manager.h>
#include <shared/render_manager.h>
#include <shared/rendering_context.h>
#include <shared/scene_manager.h>

#include <shared/sg/collection_visitor.h>
#include <shared/sg/dynamic_mesh.h>
#include <shared/sg/light.h>
#include <shared/sg/line_list.h>
#include <shared/sg/page_curl.h>
#include <shared/sg/particle_emitter.h>
#include <shared/sg/scissor.h>
#include <shared/sg/sprite_list.h>
#include <shared/sg/static_mesh.h>
#include <shared/sg/traverse_result.h>
#include <shared/sg/visual_model.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фабрика узлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class NodeFactory
{
  public:
    static Node* CreateNode (RenderManager& render_manager, interchange::NodeType type);
};

}

}

}
