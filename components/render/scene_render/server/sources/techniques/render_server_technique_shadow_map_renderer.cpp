#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Конструктор / деструктор
*/

ShadowMapRenderer::ShadowMapRenderer (server::RenderManager& manager, const common::ParseNode& node)
try
  : BasicRenderer (manager, common::get<const char*> (node, "effect"))
  , shadow_map (manager.Manager ().CreateTexture (common::get<const char*> (node, "texture")))
{
  try
  {
    render_target_name = common::get<const char*> (node, "render_target");

    size_t layers_count = shadow_map.Depth ();

    render_targets.reserve (layers_count);

    for (size_t i=0; i<layers_count; i++)
    {
      manager::RenderTarget target = shadow_map.RenderTarget (i, 0);

      render_targets.push_back (target);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ShadowMapRenderer::ShadowMapRenderer");
    throw;
  }
}
catch (xtl::exception& e)
{
  e.touch ("render::scene::server::ShadowMapRenderer::ShadowMapRenderer");
  throw;
}

ShadowMapRenderer::~ShadowMapRenderer ()
{
}

/*
    Обновление карты
*/

void ShadowMapRenderer::UpdateShadowMap (RenderingContext& parent_context, Light& light, ITraverseResultCache& traverse_result_cache)
{
  try
  {
    size_t pass_count = light.CamerasCount () < render_targets.size () ? light.CamerasCount () : render_targets.size ();

    manager::Frame& frame = Frame ();;

    for (size_t i=0; i<pass_count; i++)
    {
      Camera& camera = const_cast<Camera&> (light.Camera (i));

      frame.SetRenderTarget (render_target_name.c_str (), render_targets [i]);

      RenderingContext context (frame, RenderManager (), traverse_result_cache, camera, &parent_context);

      Draw (context); 
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ShadowMapRenderer::UpdateShadowMap");
    throw;
  }
}
