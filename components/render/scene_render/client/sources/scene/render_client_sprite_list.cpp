#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Список спрайтов
class SpriteList: public VisualModel
{
  public:
///Конструктор
    SpriteList (scene_graph::SpriteModel& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_SpriteList)
      , on_update_descs_connection (model.RegisterEventHandler (scene_graph::SpriteModelEvent_AfterSpriteDescsUpdate, xtl::bind (&SpriteList::UpdateDescsNotify, this)))
      , on_update_params_connection (model.RegisterEventHandler (scene_graph::SpriteModelEvent_AfterModeUsageUpUpdate, xtl::bind (&SpriteList::UpdateParamsNotify, this)))
      , need_update_descs (true)
      , need_update_params (true)
      , cached_descs_count ()
      , cached_descs_capacity ()
      , material_name_hash ()
    {
    }

///Исходный узел
    scene_graph::SpriteModel& SourceNode () const { return static_cast<scene_graph::SpriteModel&> (Node::SourceNode ()); }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::SpriteModel& model = SourceNode ();

        if (need_update_params)
        {
          interchange::SpriteMode mode;

          switch (model.Mode ())
          {
            default:
            case scene_graph::SpriteMode_Billboard:         mode = interchange::SpriteMode_Billboard; break;
            case scene_graph::SpriteMode_Oriented:          mode = interchange::SpriteMode_Oriented; break;
            case scene_graph::SpriteMode_OrientedBillboard: mode = interchange::SpriteMode_OrientedBillboard; break;
          }

          interchange::PrimitiveUsage usage;

          switch (model.Usage ())
          {
            case scene_graph::SpriteUsage_Static:   usage = interchange::PrimitiveUsage_Static; break;
            case scene_graph::SpriteUsage_Dynamic:  usage = interchange::PrimitiveUsage_Dynamic; break;
            case scene_graph::SpriteUsage_Stream:   usage = interchange::PrimitiveUsage_Stream; break;
            default:
            case scene_graph::SpriteUsage_Batching: usage = interchange::PrimitiveUsage_Batching; break;
          }

          context.SetSpriteListParams (Id (), mode, usage, model.OrtUp ());

          need_update_params = false;
        }

        size_t model_material_name_hash = model.MaterialHash ();

        if (material_name_hash != model_material_name_hash)
        {
          context.SetSpriteListMaterial (Id (), model.Material ());

          material_name_hash = model_material_name_hash;
        }

        if (need_update_descs)
        {
          size_t count = model.SpriteDescsCount (), capacity = model.SpriteDescsCapacity ();

          if (count != cached_descs_count || capacity != cached_descs_capacity)
          {
            context.SetSpriteListBuffer (Id (), uint32 (count), uint32 (capacity));

            cached_descs_count    = count;
            cached_descs_capacity = capacity;
          }

          static const int ct_check = xtl::compile_time_assert<sizeof (scene_graph::SpriteDesc) == sizeof (interchange::SpriteDesc)>::value;

          context.SetSpriteListDescs (Id (), 0, interchange::RawArray<interchange::SpriteDesc> (reinterpret_cast<const interchange::SpriteDesc*> (model.SpriteDescs ()), uint32 (count)));
                                                                                                                                                                                 
          need_update_descs = false;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::SpriteList::UpdateCore");
        throw;
      }
    }

  private:
    void UpdateDescsNotify () { need_update_descs = true; }
    void UpdateParamsNotify () { need_update_params = true; }

  private:
    xtl::auto_connection on_update_descs_connection;
    xtl::auto_connection on_update_params_connection;
    bool                 need_update_descs;
    bool                 need_update_params;
    size_t               cached_descs_count;
    size_t               cached_descs_capacity;
    size_t               material_name_hash;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::SpriteModel& model, SceneManager& scene_manager)
{
  return new SpriteList (model, scene_manager);
}

}}}
