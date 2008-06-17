#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;
using namespace xtl;

const float INFINITE_BOUND_VALUE = 1e8f; //значение бесконечности для ограничивающих объёмов

/*
    Описание реализации Entity
*/

struct Entity::Impl: public SceneObject
{
  vec3f  color;                    //цвет объекта
  aaboxf local_bound_box;          //ограничивающий параллелипиппед в локальной системе координат
  aaboxf world_bound_box;          //ограничивающий параллелипиппед в мировой системе координат
  bool   need_local_bounds_update; //локальные ограничивающие объёмы требуют пересчёта
  bool   need_world_bounds_update; //мировые ограничивающие объёмы требуют пересчёта
  bool   infinite_bounds;          //являются ли ограничивающие объёмы узла бесконечными
  
  Impl (scene_graph::Entity& entity) : SceneObject (entity) {}
};

/*
    Конструктор / деструктор
*/

Entity::Entity ()
  : impl (new Impl (*this))
{
  impl->need_local_bounds_update = false;
  impl->need_world_bounds_update = false;
  impl->infinite_bounds          = true;
  impl->local_bound_box          = impl->world_bound_box = aaboxf (vec3f (-INFINITE_BOUND_VALUE), vec3f (INFINITE_BOUND_VALUE));
}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    Цвет узла
*/

void Entity::SetColor (const vec3f& color)
{
  impl->color = color;
  
  UpdateNotify ();
}

void Entity::SetColor (float red, float green, float blue)
{
  SetColor (vec3f (red, green, blue));
}

const vec3f& Entity::Color () const
{
  return impl->color;
}

/*
    Работа с ограничивающими объёмами
*/

//оповещение об обновлении локальных ограничивающих объёмов
void Entity::UpdateBoundsNotify ()
{
  impl->need_local_bounds_update = true;
}

//оповещение об обновлении мировых ограничивающих объёмов
void Entity::UpdateWorldBoundsNotify ()
{
  impl->need_world_bounds_update = true;
}

//пересчёт мировых ограничивающих объёмов
void Entity::UpdateWorldBounds () const
{
  if (impl->infinite_bounds) impl->world_bound_box = impl->local_bound_box;
  else                       impl->world_bound_box = impl->local_bound_box * WorldTM ();

  impl->need_world_bounds_update = false;
}

//установка ограничивающего объёма
void Entity::SetBoundBox (const bound_volumes::aaboxf& box)
{
  impl->infinite_bounds = false;
  impl->local_bound_box = box;
  
  UpdateWorldBoundsNotify ();
  UpdateNotify ();
}

//ограничивающий объём узла в локальной системе координат
const aaboxf& Entity::BoundBox () const
{
  if (impl->need_local_bounds_update)
  {
    const_cast <Entity&> (*this).UpdateBoundsCore ();
    impl->need_local_bounds_update = false;
  }

  return impl->local_bound_box;
}

//ограничивающий объём узла в мировой системе координат
const aaboxf& Entity::WorldBoundBox () const
{
  if (impl->need_world_bounds_update)
    UpdateWorldBounds ();

  return impl->world_bound_box;
}

//установка бесконечных ограничивающий объёмов
void Entity::SetInfiniteBounds ()
{
  if (impl->infinite_bounds) //игнорируем повторную установку бесконечных ограничивающих объёмов
    return;

  impl->infinite_bounds = true;
  impl->local_bound_box = aaboxf (vec3f (-INFINITE_BOUND_VALUE), vec3f (INFINITE_BOUND_VALUE));
  
  UpdateWorldBoundsNotify ();
  UpdateNotify ();
}

//Обновление локального ограничивающего объёма
void Entity::UpdateBoundsCore ()
{
}

//являются ли ограничивающие объёмы узла бесконечными
bool Entity::IsInfiniteBounds () const
{
  if (impl->need_local_bounds_update)
  {
    const_cast <Entity&> (*this).UpdateBoundsCore ();
    impl->need_local_bounds_update = false;
  }

  return impl->infinite_bounds;
}

//ограничивающий объём потомков в локальной системе координат узла
aaboxf Entity::ChildrenBoundBox () const
{
  return WorldChildrenBoundBox () * invert (WorldTM ());
}

//полный ограничивающий объём узла с потомками в локальной системе координат узла
aaboxf Entity::FullBoundBox () const
{
  return WorldFullBoundBox () * invert (WorldTM ());
}

//ограничивающий объём потомков в мировой системе координат
aaboxf Entity::WorldChildrenBoundBox () const
{
  struct EntityVisitor: public visitor<void, Entity>
  {
    EntityVisitor () : initialized (false) {}
    
    void visit (Entity& entity)
    {
      if (initialized)
      {
        box += entity.WorldBoundBox ();
        
        return;
      }

      initialized = true;

      box = entity.WorldBoundBox ();
    }

    aaboxf box;
    bool   initialized;
  };

  EntityVisitor visitor;

  VisitEach (visitor);
  
  return visitor.box;
}

//полный ограничивающий объём узла с потомками в мировой системе координат
aaboxf Entity::WorldFullBoundBox () const
{
  return WorldChildrenBoundBox () += WorldBoundBox ();
}

/*
    Обработка оповещения об изменении мирового положения узла
*/

void Entity::AfterUpdateWorldTransformEvent ()
{
  UpdateWorldBoundsNotify ();
}

/*
    Обработка оповещений об присоединении/отсоединении объекта к сцене
*/

void Entity::AfterSceneAttachEvent ()
{
  Scene ()->Attach (*impl);
}

void Entity::BeforeSceneDetachEvent ()
{
  impl->BindToSpace (0);
}

/*
    Метод, вызываемый при посещении объекта
*/

void Entity::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Node::AcceptCore (visitor);
}
