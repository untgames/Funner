#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

/*
    Описание реализации сущности
*/

struct VisualModel::Impl
{
  scene_graph::Scissor* scissor; //сохраненная область отсечения

  Impl () : scissor () {}
};

/*
    Конструктор / деструктор
*/

VisualModel::VisualModel (scene_graph::VisualModel& entity, SceneManager& manager, interchange::NodeType node_type)
  : Entity (entity, manager, node_type)
  , impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    Реализация синхронизации
*/

void VisualModel::UpdateCore (client::Context& context)
{
  try
  {
    Entity::UpdateCore (context);

    scene_graph::VisualModel& model = SourceNode ();

      //синхронизация области отсечения

    scene_graph::Scissor* scissor = model.Scissor ();

    if (scissor != impl->scissor)
    {
      if (scissor)
      {
        NodePtr scissor_node = Scenes ().FindNode (*scissor);

        context.SetVisualModelScissor (Id (), scissor_node ? scissor_node->Id () : 0u);
      }
      else context.SetVisualModelScissor (Id (), 0u);

      impl->scissor = scissor;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::VisualModel::UpdateCore");
    throw;
  }
}
