#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации отображаемой модели
*/

struct VisualModel::Impl
{
  NodePtr         scissor; //область отсечения
  manager::Entity entity;  //сущность

/// Конструктор
  Impl (manager::Entity& in_entity) : entity (in_entity) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

VisualModel::VisualModel (RenderManager& render_manager)
{
  try
  {
    manager::Entity entity = render_manager.Manager ().CreateEntity ();

    impl.reset (new Impl (entity));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::VisualModel::VisualModel");
    throw;
  }
}

VisualModel::VisualModel (manager::Entity& entity)
  : impl (new Impl (entity))
{
  entity.SetUserData (this);
}

VisualModel::~VisualModel ()
{
}

/*
    Область отсечения
*/

void VisualModel::SetScissor (Node* node)
{
  impl->scissor = node;
}

Node* VisualModel::Scissor () const
{
  return impl->scissor.get ();
}

/*
    Отображаемая сущность
*/

manager::Entity& VisualModel::Entity ()
{
  return impl->entity;
}

const manager::Entity& VisualModel::Entity () const
{
  return impl->entity;
}

/*
    Обход
*/

void VisualModel::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    Отрисовка
*/

void VisualModel::Draw (RenderingContext& context, void* user_data)
{
  try
  {    
    DrawCore (context, user_data);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::VisualModel::Draw(node='%s')", Name ());
    throw;
  }
}

void VisualModel::DrawCore (RenderingContext& context, void* user_data)
{
  //TODO: set scissor

  context.Frame ().AddEntity (impl->entity, user_data);
}
