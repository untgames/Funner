#include "shared.h"

//TODO: draw only visual models affected by light

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

namespace render
{

namespace scene
{

namespace server
{

namespace
{

/*
    Константы
*/

const size_t LIGHTS_RESERVE_SIZE = 64;   //резервируемое количество источников света

}

///Техника рендеринга: отрисовка изображения из каждого источника и сложение полученных изображений
class ForwardShading: public Technique
{
  public:
///Конструктор / деструктор
    ForwardShading (RenderManager& in_manager, const common::ParseNode& node)
      : manager (in_manager)
      , shadow_map_renderer (in_manager, node.First ("shadow_map"))
    {
        //чтение конфигурации

      lighting_effect                = common::get<const char*> (node, "lighting_effect");
      root_effect                    = common::get<const char*> (node, "root_effect");
      spot_light_shader_light_type   = common::get<const char*> (node, "spot_light_shader_type", "");
      direct_light_shader_light_type = common::get<const char*> (node, "direct_light_shader_type", "");
      point_light_shader_light_type  = common::get<const char*> (node, "point_light_shader_type", "");
    }    

/// Имена для регистрации
    static const char* ClassName     () { return "forward_shading"; }
    static const char* ComponentName () { return "render.scene.server.techniques.forward_shading"; }

  protected:
///Обновление свойств
    void UpdatePropertiesCore () {}

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    struct LightContext: public xtl::reference_counter
    {
      BasicRendererPtr    renderer;
      common::PropertyMap shader_defines;
      manager::Frame      shadow_frame;

      LightContext (ForwardShading& technique)
        : renderer (new BasicRenderer (technique.manager, technique.lighting_effect.c_str ()), false)
        , shadow_frame (technique.manager.Manager ().CreateFrame ())
      {
        renderer->Frame ().SetShaderOptions (shader_defines);
      }
    };

    typedef xtl::intrusive_ptr<LightContext> LightContextPtr;
    typedef stl::vector<LightContextPtr>     LightContextArray;

    struct PrivateData
    {
      LightContextArray light_frames;
      manager::Frame   frame;

      PrivateData (ForwardShading& technique)
        : frame (technique.manager.Manager ().CreateFrame ())
      {
        frame.SetEffect (technique.root_effect.c_str ());
        frame.SetLocalTexture (technique.shadow_map_renderer.LocalTextureName (), technique.shadow_map_renderer.ShadowMap ());

        light_frames.reserve (LIGHTS_RESERVE_SIZE);
      }
    };
  
  private:
///Обновление кадра
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //получение приватных данных техники

        PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

          //определение списка источников и визуализируемых объектов, попадающих в камеру
        
        TraverseResult& result = parent_context.TraverseResult ();

          //обход источников

        RenderingContext context (parent_context, private_data.frame);

        size_t light_index = 0;
          
        for (TraverseResult::LightArray::iterator iter=result.lights.begin (), end=result.lights.end (); iter!=end; ++iter, ++light_index)
          UpdateFromLight (context, **iter, result, light_index, private_data);

          //добавление дочернего кадра

        parent_context.Frame ().AddFrame (private_data.frame);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
        throw;
      }
    }

///Обновление кадра из источника света
    void UpdateFromLight (RenderingContext& context, Light& light, TraverseResult& result, size_t light_index, PrivateData& private_data)
    {
        //получение данных отрисовки

      LightContext& light_context = AllocateLightContext (private_data, light_index);

        //обновление тени

      RenderingContext shadow_context (context, light_context.renderer->Frame ());   //TODO this for debug

      shadow_map_renderer.UpdateShadowMap (shadow_context, light);   //TODO this for debug too 
//      UpdateShadowMap (context, light);   //TODO this commented for debug

        //установка параметров источника

      manager::Frame&                 frame  = light_context.renderer->Frame ();
      const interchange::LightParams& params = light.Params ();

      common::PropertyMap& frame_properties = light_context.renderer->FrameProperties ();

      frame_properties.SetProperty ("LightWorldPosition", light.WorldMatrix () * math::vec3f (0.0f));
      frame_properties.SetProperty ("LightWorldDirection", light.WorldMatrix() * math::vec4f (0.0f, 0.0f, 1.0f, 0.0f));
      frame_properties.SetProperty ("LightColor", params.color * params.intensity);
      frame_properties.SetProperty ("LightAttenuation", params.attenuation);
      frame_properties.SetProperty ("LightRange", params.range);
      frame_properties.SetProperty ("LightExponent", params.exponent);
      frame_properties.SetProperty ("LightAngle", math::radian (params.angle));
      frame_properties.SetProperty ("LightRadius", params.radius);
//      frame_properties.SetProperty ("LightShadowMatrix", light.Camera (0).ViewProjectionMatrix ());

      math::mat4f light_shadow_tm    = math::translate (math::vec3f (0.5f)) * math::scale (math::vec3f (0.5f)) * light.Camera (0).ViewProjectionMatrix ();

      frame_properties.SetProperty ("LightShadowMatrix", light_shadow_tm);

      common::PropertyMap& frame_defines     = light_context.shader_defines;
      const char*          shader_light_type = "";

      switch (light.Type ())
      {
        case interchange::NodeType_PointLight:
          shader_light_type = point_light_shader_light_type.c_str ();
          break;
        case interchange::NodeType_DirectLight:
          shader_light_type = direct_light_shader_light_type.c_str ();
          break;
        case interchange::NodeType_SpotLight:
          shader_light_type = spot_light_shader_light_type.c_str ();
          break;
        default:
          return;
      }

      frame_defines.SetProperty ("LightType", shader_light_type);
     
        //обновление визуализируемых объектов

//TODO: draw only visual models affected by light

      light_context.renderer->Draw (context);
    }

///Отрисовка теневой карты
    void UpdateShadowMap (RenderingContext& context, Light& light)
    {
      try
      {
        shadow_map_renderer.UpdateShadowMap (context, light);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::ForwardShading::UpdateShadowMap");
        throw;
      }
    }

///Резервирование контекста источника
    LightContext& AllocateLightContext (PrivateData& private_data, size_t light_index)
    {
      if (light_index < private_data.light_frames.size ())
        return *private_data.light_frames [light_index];
      
      LightContextPtr light_context (new LightContext (*this), false);      
        
      private_data.light_frames.push_back (light_context);
        
      return *light_context;
    }

  private:
    RenderManager     manager;                        //менеджер рендеринга
    stl::string       lighting_effect;                //эффект отрисовки с источником света
    stl::string       root_effect;                    //корневой эффект отрисовки
    stl::string       spot_light_shader_light_type;   //настройки шейдера для конического источника света
    stl::string       direct_light_shader_light_type; //настройки шейдера для цилиндрического источника света
    stl::string       point_light_shader_light_type;  //настройки шейдера для точечного источника света
    ShadowMapRenderer shadow_map_renderer;            //рендер карты теней
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::ForwardShading> ForwardShading;

}
