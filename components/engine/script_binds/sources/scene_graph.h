#include "shared.h"

#include <math/angle.h>
#include <math/matrix.h>
#include <math/quat.h>

#include <sg/camera.h>
#include <sg/height_map.h>
#include <sg/helper.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/node_array.h>
#include <sg/scene.h>
#include <sg/sound_emitter.h>
#include <sg/sprite.h>
#include <sg/text_line.h>
#include <sg/visual_model.h>

#include <sg/controllers/water.h>

namespace scene_graph
{

inline void push_argument (script::IStack& stack, Node& node)
{
  stack.Push (xtl::any (&node, true));
}

}

/*
    �������������� math::angle �� ������ ����������
*/

namespace script
{

namespace detail
{

template <>
struct argument_selector<math::angle<float> >
{
  typedef float dump_type;

  static math::angle<float> get (IStack& stack, size_t index) { return math::degree (stack.GetFloat (index)); }
  
  static float make_invoker_argument (const math::angle<float>& angle) { return math::degree (angle); }
};

template <>
struct argument_invoker<math::angle<float> >
{
  typedef float type;
  
  static float make (const math::angle<float>& angle) { return math::degree (angle); }  
};

}

}

namespace engine
{

namespace scene_graph_script_binds
{

/*
    ��������� (����� ���������)
*/

extern const char* SCENE_STATIC_NODE_BIND_MODE_LIBRARY;
extern const char* SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY;
extern const char* SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY;
extern const char* SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY;
extern const char* SCENE_STATIC_NODE_ORT_LIBRARY;
extern const char* SCENE_STATIC_NODE_EVENT_LIBRARY;
extern const char* SCENE_STATIC_NODE_SUBTREE_EVENT_LIBRARY;
extern const char* SCENE_STATIC_NODE_ARRAY_LINK_MODE_LIBRARY;
extern const char* SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY;
extern const char* SCENE_STATIC_NODE_PROPERTY_TYPE_LIBRARY;
extern const char* SCENE_CONTROLLER_OWNER_MODE_LIBRARY;
extern const char* SCENE_SCENE_LIBRARY;
extern const char* SCENE_NODE_LIBRARY;
extern const char* SCENE_NODE_PROPERTIES_LIBRARY;
extern const char* SCENE_NODE_ARRAY_LIBRARY;
extern const char* SCENE_CONTROLLER_LIBRARY;
extern const char* SCENE_ENTITY_LIBRARY;
extern const char* SCENE_PERSPECTIVE_CAMERA_LIBRARY;
extern const char* SCENE_ORTHO_CAMERA_LIBRARY;
extern const char* SCENE_LIGHT_LIBRARY;
extern const char* SCENE_DIRECT_LIGHT_LIBRARY;
extern const char* SCENE_SPOT_LIGHT_LIBRARY;
extern const char* SCENE_POINT_LIGHT_LIBRARY;
extern const char* SCENE_BOX_HELPER_LIBRARY;
extern const char* SCENE_LISTENER_LIBRARY;
extern const char* SCENE_SOUND_EMITTER_LIBRARY;
extern const char* SCENE_SPRITE_MODEL_LIBRARY;
extern const char* SCENE_SPRITE_LIBRARY;
extern const char* SCENE_TEXT_LINE_LIBRARY;
extern const char* SCENE_VISUAL_MODEL_LIBRARY;
extern const char* SCENE_HEIGHT_MAP_LIBRARY;
extern const char* SCENE_CONTROLLER_WATER_LIBRARY;

/*
    ������� �������� ���������
*/

void bind_scene_library                 (script::Environment&);
void bind_node_library                  (script::Environment&);
void bind_node_properties_library       (script::Environment&);
void bind_node_array_library            (script::Environment&);
void bind_controller_owner_mode_library (script::Environment&);  
void bind_controller_library            (script::Environment&);
void bind_entity_library                (script::Environment&);
void bind_perspective_camera_library    (script::Environment&);
void bind_ortho_camera_library          (script::Environment&);
void bind_light_library                 (script::Environment&);
void bind_direct_light_library          (script::Environment&);
void bind_spot_light_library            (script::Environment&);
void bind_point_light_library           (script::Environment&);
void bind_box_helper_library            (script::Environment&);
void bind_listener_library              (script::Environment&);
void bind_sound_emitter_library         (script::Environment&);
void bind_sprite_model_library          (script::Environment&);
void bind_sprite_library                (script::Environment&);
void bind_text_line_library             (script::Environment&);
void bind_visual_model_library          (script::Environment&);
void bind_height_map_library            (script::Environment&);
void bind_controller_water_library      (script::Environment&);


}

}

using namespace engine::scene_graph_script_binds;
using namespace script;
using namespace scene_graph;
using namespace scene_graph::controllers;
using namespace math;
using namespace xtl;