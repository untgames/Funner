#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Константы (имена библиотек)
*/

const char* SCENE_STATIC_NODE_BIND_MODE_LIBRARY       = "Scene.NodeBindMode";
const char* SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY = "Scene.NodeTransformSpace";
const char* SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY   = "Scene.NodeTraverseMode";
const char* SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY     = "Scene.NodeSearchMode";
const char* SCENE_STATIC_NODE_ORT_LIBRARY             = "Scene.NodeOrt";
const char* SCENE_STATIC_NODE_EVENT_LIBRARY           = "Scene.NodeEvent";
const char* SCENE_STATIC_NODE_SUBTREE_EVENT_LIBRARY   = "Scene.NodeSubTreeEvent";
const char* SCENE_STATIC_NODE_ARRAY_LINK_MODE_LIBRARY = "Scene.NodeArrayLinkMode";
const char* SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY  = "Scene.TextLineAlignment";
const char* SCENE_STATIC_NODE_PROPERTY_TYPE_LIBRARY   = "Scene.NodePropertyType";
const char* SCENE_CONTROLLER_OWNER_MODE_LIBRARY       = "Scene.ControllerOwnerMode";
const char* SCENE_SCENE_LIBRARY                       = "Scene.Scene";
const char* SCENE_NODE_LIBRARY                        = "Scene.Node";
const char* SCENE_NODE_PROPERTIES_LIBRARY             = "Scene.NodeProperties";
const char* SCENE_NODE_ARRAY_LIBRARY                  = "Scene.NodeArray";
const char* SCENE_CONTROLLER_LIBRARY                  = "Scene.Controller";
const char* SCENE_ENTITY_LIBRARY                      = "Scene.Entity";
const char* SCENE_PERSPECTIVE_CAMERA_LIBRARY          = "Scene.PerspectiveCamera";
const char* SCENE_ORTHO_CAMERA_LIBRARY                = "Scene.OrthoCamera";
const char* SCENE_LIGHT_LIBRARY                       = "Scene.Light";
const char* SCENE_DIRECT_LIGHT_LIBRARY                = "Scene.DirectLight";
const char* SCENE_SPOT_LIGHT_LIBRARY                  = "Scene.SpotLight";
const char* SCENE_POINT_LIGHT_LIBRARY                 = "Scene.PointLight";
const char* SCENE_BOX_HELPER_LIBRARY                  = "Scene.Helpers.Box";
const char* SCENE_LISTENER_LIBRARY                    = "Scene.Listener";
const char* SCENE_SOUND_EMITTER_LIBRARY               = "Scene.SoundEmitter";
const char* SCENE_SPRITE_MODEL_LIBRARY                = "Scene.SpriteModel";
const char* SCENE_SPRITE_LIBRARY                      = "Scene.Sprite";
const char* SCENE_TEXT_LINE_LIBRARY                   = "Scene.TextLine";
const char* SCENE_VISUAL_MODEL_LIBRARY                = "Scene.VisualModel";
const char* SCENE_HEIGHT_MAP_LIBRARY                  = "Scene.HeightMap";
const char* BINDER_NAME                               = "SceneGraph";
const char* COMPONENT_NAME                            = "script.binds.SceneGraph";

}

}

namespace
{

/*
    Регистрация библиотеки работы со сценой
*/

void bind_scene_graph_library (Environment& environment)
{
  bind_scene_library                         (environment);
  bind_node_library                          (environment);
  bind_node_properties_library               (environment);
  bind_node_array_library                    (environment);
  bind_controller_owner_mode_library         (environment);
  bind_controller_library                    (environment);
  bind_entity_library                        (environment);
  bind_perspective_camera_library            (environment);
  bind_ortho_camera_library                  (environment);
  bind_light_library                         (environment);
  bind_direct_light_library                  (environment);
  bind_spot_light_library                    (environment);
  bind_point_light_library                   (environment);
  bind_box_helper_library                    (environment);
  bind_listener_library                      (environment);
  bind_sound_emitter_library                 (environment);
  bind_sprite_model_library                  (environment);
  bind_sprite_library                        (environment);
  bind_text_line_library                     (environment);
  bind_visual_model_library                  (environment);
  bind_height_map_library                    (environment);
  bind_controller_water_library              (environment);
  bind_controller_move_to_node_point_library (environment);
  bind_controller_look_to_node_point_library (environment);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_scene_graph_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SceneGraphScriptBind (COMPONENT_NAME);

}

}
