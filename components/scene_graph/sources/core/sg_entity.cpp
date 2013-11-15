#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;
using namespace xtl;

namespace
{

/*
    Константы
*/

const float  INFINITE_BOUND_VALUE       = 1e8f; //значение бесконечности для ограничивающих объёмов
const size_t INTERSECTIONS_RESERVE_SIZE = 16;   //резервируемое число пересечений

//получение хэша от ограничивающего параллелипиппеда
inline size_t hash (const bound_volumes::aaboxf& box)
{
  return common::crc32 (&box, sizeof (box));
}

}

/*
    Описание реализации Entity
*/

struct Entity::Impl: public SceneObject, public xtl::instance_counter<Entity>
{
  vec3f  wire_color;               //цвет проволочного представления объекта
  aaboxf local_bound_box;          //ограничивающий параллелипиппед в локальной системе координат
  aaboxf world_bound_box;          //ограничивающий параллелипиппед в мировой системе координат
  size_t local_bounds_hash;        //хэш локального ограничивающего параллелипиппеда
  size_t world_bounds_hash;        //хэш мирового ограничивающего параллелипиппеда
  bool   need_local_bounds_update; //локальные ограничивающие объёмы требуют пересчёта
  bool   need_world_bounds_update; //мировые ограничивающие объёмы требуют пересчёта
  bool   infinite_bounds;          //являются ли ограничивающие объёмы узла бесконечными
  bool   visible;                  //являются ли объект видимым
  
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
  impl->visible                  = true;
  impl->local_bounds_hash        = hash (impl->local_bound_box);
  impl->world_bounds_hash        = hash (impl->world_bound_box);
}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    Цвет проволочного представления объекта
*/

void Entity::SetWireColor (const vec3f& color)
{
  impl->wire_color = color;
  
  UpdateNotify ();
}

void Entity::SetWireColor (float red, float green, float blue)
{
  SetWireColor (vec3f (red, green, blue));
}

const vec3f& Entity::WireColor () const
{
  return impl->wire_color;
}

/*
    Видимость объекта
*/

void Entity::SetVisible (bool state)
{
  impl->visible = state;
  
  UpdateNotify ();
}

bool Entity::IsVisible () const
{
  return impl->visible;
}

/*
    Работа с ограничивающими объёмами
*/

//оповещение об обновлении локальных ограничивающих объёмов
void Entity::UpdateBoundsNotify ()
{
  impl->need_local_bounds_update = true;

  UpdateNotify ();
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

  impl->world_bounds_hash        = hash (impl->world_bound_box);
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
    
    impl->local_bounds_hash        = hash (impl->local_bound_box);
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

size_t Entity::BoundBoxHash () const
{
  if (!impl->need_local_bounds_update)
    return impl->local_bounds_hash;

  BoundBox ();

  return impl->local_bounds_hash;
}

size_t Entity::WorldBoundBoxHash () const
{
  if (!impl->need_world_bounds_update)
    return impl->world_bounds_hash;

  WorldBoundBox ();

  return impl->world_bounds_hash;
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
  return WorldChildrenBoundBox () * inverse (WorldTM ());
}

//полный ограничивающий объём узла с потомками в локальной системе координат узла
aaboxf Entity::FullBoundBox () const
{
  return WorldFullBoundBox () * inverse (WorldTM ());
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
    Получение пересечений
*/

namespace
{

//враппер для добавления объекта в массив
struct ArrayInserter: public INodeTraverser
{
  ArrayInserter (const Entity& in_self, NodeArray& in_array) : self (in_self), array (in_array) {}

  void operator () (Node& entity)
  {
    if (&entity == &self) //защита от самопересечений
      return;

    array.Add (entity);
  }

  const Entity& self;
  NodeArray&    array;
};

}

void Entity::GetIntersections (NodeArray& intersections) const
{
  intersections.Clear ();

  const scene_graph::Scene* scene = Scene ();
  
  if (!scene)
    return;

  ArrayInserter inserter (*this, intersections);

  scene->Traverse (WorldBoundBox (), inserter);
}

NodeArray Entity::GetIntersections () const
{
  NodeArray intersections;

  intersections.Reserve (INTERSECTIONS_RESERVE_SIZE);

  GetIntersections (intersections);

  return intersections;
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

/*
    Связывание свойств
*/

void Entity::BindProperties (common::PropertyBindingMap& bindings)
{
  Node::BindProperties (bindings);

  bindings.AddProperty ("InfiniteBounds", xtl::bind (&Entity::IsInfiniteBounds, this));
  bindings.AddProperty ("WireColor", xtl::bind (&Entity::WireColor, this), xtl::bind (xtl::implicit_cast<void (Entity::*)(const math::vec3f&)> (&Entity::SetWireColor), this, _1));
  bindings.AddProperty ("Visible", xtl::bind (&Entity::IsVisible, this), xtl::bind (&Entity::SetVisible, this, _1));  
}
