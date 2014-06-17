#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Список спрайтов
class LineList: public VisualModel
{
  public:
///Конструктор
    LineList (scene_graph::LineModel& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_LineList)
      , on_update_descs_connection (model.RegisterEventHandler (scene_graph::LineModelEvent_AfterLineDescsUpdate, xtl::bind (&LineList::UpdateDescsNotify, this)))
      , on_update_params_connection (model.RegisterEventHandler (scene_graph::LineModelEvent_AfterModeUsageUpUpdate, xtl::bind (&LineList::UpdateParamsNotify, this)))
      , need_update_descs (true)
      , need_update_params (true)
      , cached_descs_count ()
      , cached_descs_capacity ()
      , material_name_hash ()
    {
    }

///Исходный узел
    scene_graph::LineModel& SourceNode () const { return static_cast<scene_graph::LineModel&> (Node::SourceNode ()); }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::LineModel& model = SourceNode ();

        if (need_update_params)
        {
          interchange::PrimitiveUsage usage;

          switch (model.Usage ())
          {
            case scene_graph::LineUsage_Static:   usage = interchange::PrimitiveUsage_Static; break;
            case scene_graph::LineUsage_Dynamic:  usage = interchange::PrimitiveUsage_Dynamic; break;
            case scene_graph::LineUsage_Stream:   usage = interchange::PrimitiveUsage_Stream; break;
            default:
            case scene_graph::LineUsage_Batching: usage = interchange::PrimitiveUsage_Batching; break;
          }

          context.SetLineListParams (Id (), usage);

          need_update_params = false;
        }

        size_t model_material_name_hash = model.MaterialHash ();

        if (material_name_hash != model_material_name_hash)
        {
          context.SetLineListMaterial (Id (), model.Material ());

          material_name_hash = model_material_name_hash;
        }

        if (need_update_descs)
        {
          size_t count = model.LineDescsCount (), capacity = model.LineDescsCapacity ();

          if (count != cached_descs_count || capacity != cached_descs_capacity)
          {
            context.SetLineListBuffer (Id (), uint32 (count), uint32 (capacity));

            cached_descs_count    = count;
            cached_descs_capacity = capacity;
          }

          static const int ct_check = xtl::compile_time_assert<sizeof (scene_graph::LineDesc) == sizeof (interchange::LineDesc)>::value;

          context.SetLineListDescs (Id (), 0, interchange::RawArray<interchange::LineDesc> (reinterpret_cast<const interchange::LineDesc*> (model.LineDescs ()), uint32 (count)));
                                                                                                                                                                                 
          need_update_descs = false;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::LineList::UpdateCore");
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

Node* create_node (scene_graph::LineModel& model, SceneManager& scene_manager)
{
  return new LineList (model, scene_manager);
}

}}}
