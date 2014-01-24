#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <render/manager.h>

#include <shared/camera.h>
#include <shared/render_manager.h>
#include <shared/rendering_context.h>
#include <shared/scene_manager.h>

#include <shared/sg/collection_visitor.h>
#include <shared/sg/light.h>
#include <shared/sg/page_curl.h>
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
