#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

/// Источник света
class Light: public Entity
{
  public:
///Конструктор
    Light (scene_graph::PointLight& light, SceneManager& manager)
      : Entity (light, manager, interchange::NodeType_PointLight)
      , type (Type ())
    {
    }

    Light (scene_graph::SpotLight& light, SceneManager& manager)
      : Entity (light, manager, interchange::NodeType_PointLight)
      , type (Type ())
    {
    }

    Light (scene_graph::DirectLight& light, SceneManager& manager)
      : Entity (light, manager, interchange::NodeType_DirectLight)
      , type (Type ())
    {
    }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
        Entity::UpdateCore (context);

        const scene_graph::Light& light = static_cast<scene_graph::Light&> (Node::SourceNode ());

        interchange::LightParams params;

        params.color       = light.LightColor ();
        params.intensity   = light.Intensity ();
        params.attenuation = light.Attenuation ();
        params.range       = light.Range ();
        
        switch (type)
        {
          case interchange::NodeType_PointLight:
            params.radius   = 0.0f;
            params.angle    = math::anglef ();
            params.exponent = 0.0f;
            break;
          case interchange::NodeType_DirectLight:
            params.radius   = static_cast<const scene_graph::DirectLight&> (light).Radius ();
            params.angle    = math::anglef ();
            params.exponent = 0.0f;
            break;
          case interchange::NodeType_SpotLight:
          {
            const scene_graph::SpotLight& spot_light = static_cast<const scene_graph::SpotLight&> (light);

            params.angle    = spot_light.Angle ();
            params.exponent = spot_light.Exponent ();

            break;
          }
          default:
            break;
        }

        context.SetLightParams (Id (), params);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::Light::UpdateCore");
        throw;
      }
    }

  private:
    interchange::NodeType type; //тип источника света
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::PointLight& light, SceneManager& scene_manager)
{
  return new Light (light, scene_manager);
}

Node* create_node (scene_graph::SpotLight& light, SceneManager& scene_manager)
{
  return new Light (light, scene_manager);
}

Node* create_node (scene_graph::DirectLight& light, SceneManager& scene_manager)
{
  return new Light (light, scene_manager);
}

}}}
