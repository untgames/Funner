#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <common/parser.h>
#include <common/property_map.h>
#include <common/property_binding_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <shared/camera.h>
#include <shared/render_manager.h>
#include <shared/render_target_map.h>
#include <shared/resource_manager.h>
#include <shared/scene_manager.h>
#include <shared/screen_manager.h>
#include <shared/server.h>
#include <shared/technique_manager.h>
#include <shared/viewport_manager.h>
#include <shared/window_manager.h>

#include <render/manager.h>

namespace render
{

namespace scene
{

namespace server
{

///Преобразование узла конфигурации в карту свойств
common::PropertyMap to_properties (const common::ParseNode& node);

}

}

}
