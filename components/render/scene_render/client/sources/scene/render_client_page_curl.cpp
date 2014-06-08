#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

/// Page curl
class PageCurl: public Entity
{
  public:
///Constructor
    PageCurl (scene_graph::PageCurl& curl, SceneManager& manager)
      : Entity (curl, manager, interchange::NodeType_PageCurl)
    {}

  protected:
///Synchronization
    void UpdateCore (client::Context& context)
    {
      try
      {
        Entity::UpdateCore (context);

        const scene_graph::PageCurl& curl = static_cast<scene_graph::PageCurl&> (Node::SourceNode ());

        interchange::PageCurlParams params;

        params.page_color                    = curl.PageColor ();
        params.size                          = curl.Size ();
        params.curl_point                    = curl.CurlPoint ();
        params.curl_point_position           = curl.CurlPointPosition ();
        params.grid_size                     = curl.GridSize ();
        params.find_best_curl_steps          = curl.FindBestCurlSteps ();
        params.mode                          = curl.Mode ();
        params.curl_radius                   = curl.CurlRadius ();
        params.minimum_curl_radius           = curl.MinimumCurlRadius ();
        params.corner_shadow_offset          = curl.CornerShadowOffset ();
        params.shadow_width                  = curl.ShadowWidth ();
        params.shadow_log_base               = curl.ShadowLogBase ();
        params.shadow_min_log_value          = curl.ShadowMinLogValue ();
        params.binding_mismatch_weight       = curl.BindingMismatchWeight ();
        params.rigid_page_perspective_factor = curl.RigidPagePerspectiveFactor ();
        params.is_rigid_page                 = curl.IsRigidPage ();
        
        context.SetPageCurlParams (Id (), params, curl.PageMaterial (scene_graph::PageCurlPageType_FrontLeft),
                                  curl.PageMaterial (scene_graph::PageCurlPageType_FrontRight),
                                  curl.PageMaterial (scene_graph::PageCurlPageType_BackLeft),
                                  curl.PageMaterial (scene_graph::PageCurlPageType_BackRight));
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::PageCurl::UpdateCore");
        throw;
      }
    }
};

}

namespace render
{

namespace scene
{

namespace client
{

Node* create_node (scene_graph::PageCurl& curl, SceneManager& scene_manager)
{
  return new PageCurl (curl, scene_manager);
}

}

}

}
