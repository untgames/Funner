#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Константы
*/

const size_t TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE = 1024; //резервируемое количество визуализируемых моделей

/*
    Конструктор / деструктор
*/

ShadowMapRenderer::ShadowMapRenderer (server::RenderManager& manager, const common::ParseNode& node)
try
  : BasicRenderer (manager, common::get<const char*> (node, "effect"))
  , color_map (manager.Manager ().CreateTexture (common::get<const char*> (node, "color_texture")))
  , depth_map (manager.Manager ().CreateTexture (common::get<const char*> (node, "depth_texture")))
{
  try
  {
    color_render_target_name = common::get<const char*> (node, "color_render_target");
    depth_render_target_name = common::get<const char*> (node, "depth_render_target");
    local_texture_name       = common::get<const char*> (node, "framemap");

    unsigned int layers_count = depth_map.Depth ();

    if (layers_count > 1)
      throw xtl::format_operation_exception ("", "More than one depth render target not implemented");

    color_render_targets.reserve (layers_count);
    depth_render_targets.reserve (layers_count);

    manager::Frame& frame = Frame ();

    for (unsigned int i=0; i<layers_count; i++)
    {
      manager::RenderTarget color_target = color_map.RenderTarget (i, 0),
                            depth_target = depth_map.RenderTarget (i, 0);

      color_render_targets.push_back (color_target);
      depth_render_targets.push_back (depth_target);

      frame.SetRenderTarget (color_render_target_name.c_str (), color_render_targets [i]);
      frame.SetRenderTarget (depth_render_target_name.c_str (), depth_render_targets [i]);
    }

    traverse_result.visual_models.reserve (TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE);
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

namespace
{

struct TraverseResultCache: public ITraverseResultCache
{
  TraverseResult& result;

  TraverseResultCache (TraverseResult& in_result) : result (in_result) {}

  TraverseResult& Result () { return result; }
};

}

void ShadowMapRenderer::UpdateShadowMap (RenderingContext& parent_context, Light& light)
{
  try
  {
    if (!light.SceneOwner ())
      return;

    Scene& scene = *light.SceneOwner ();

    size_t pass_count = light.CamerasCount () < depth_render_targets.size () ? light.CamerasCount () : depth_render_targets.size ();

    manager::Frame& frame = Frame ();

    frame.SetClearDepthValue (1.f);  //TODO is it needed here?

    TraverseResultCache cache (traverse_result);

    for (size_t i=0; i<pass_count; i++)
    {
      Camera& camera = const_cast<Camera&> (light.Camera (i));

      traverse_result.Clear ();

      scene.Traverse (camera.Frustum (), traverse_result, Collect_VisualModels);

      RenderingContext context (frame, RenderManager (), cache, camera, &parent_context);

      Draw (context);
    }

    parent_context.Frame ().AddFrame (frame);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ShadowMapRenderer::UpdateShadowMap");
    throw;
  }
}
