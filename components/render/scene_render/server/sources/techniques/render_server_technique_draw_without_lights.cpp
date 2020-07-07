#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

namespace render
{

namespace scene
{

namespace server
{

///Rendering technique: draw without lights
class DrawWithoutLights: public Technique
{
  public:
///Constructor
    DrawWithoutLights (RenderManager& in_manager, const common::ParseNode& node)
      : Technique (in_manager, node)
      , manager (in_manager)
    {
        //read configuration

      effect_name = common::get<const char*> (node, "effect");
    }

///Registration names
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  protected:
///Render data for each frame
    struct PrivateData
    {
      BasicRendererPtr renderer;

      PrivateData (DrawWithoutLights& technique) : renderer (new BasicRenderer (technique.manager, technique.effect_name.c_str ()), false) {}
    };

///Update frame
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //get technique private data

        PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

          //update frame

        private_data.renderer->Draw (parent_context);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///Update properties
    void UpdatePropertiesCore () {}

///Binding technique properties with methods
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager manager;     //render manager
    stl::string   effect_name; //effect name
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
