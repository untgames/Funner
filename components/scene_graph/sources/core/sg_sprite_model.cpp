#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    ќписание реализации спрайтовой модели
*/

typedef xtl::signal<void (SpriteModel& sender, SpriteModelEvent event_id)> SpriteModelSignal;

struct SpriteModel::Impl
{
  stl::string       material;                       //им€ материала
  SpriteModelSignal signals [SpriteModelEvent_Num]; //сигналы модели 
  vec3f             pivot_position;                 //положение центра в локальной системе координат
  float             pivot_rotation;                 //поворот центра в локальной системе координат
  float             alpha_reference;                //параметр, используемый дл€ альфа-теста
  bool              need_recalc_world_tm;           //необходим пересчЄт мировой матрицы преобразований
  mat4f             world_tm;                       //мирова€ матрица преобразований

  Impl () : need_recalc_world_tm (true), pivot_rotation (0.0f), alpha_reference (0.f) {}

    //оповещение о событии
  void Notify (SpriteModel& sender, SpriteModelEvent event_id)
  {
    try
    {
      if (!signals [event_id])
        return;

      signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
     онструктор / деструктор
*/

SpriteModel::SpriteModel ()
  : impl (new Impl)
{
}

SpriteModel::~SpriteModel ()
{
}

/*
    ћатериал
*/

void SpriteModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetMaterial", "material");

  impl->material = in_material;  

  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    ѕараметр, используемый дл€ альфа-теста
*/

void SpriteModel::SetAlphaReference (float value)
{
  impl->alpha_reference = value;
  
  UpdateNotify ();
}

float SpriteModel::AlphaReference () const
{
  return impl->alpha_reference;
}

/*
    ѕоложение и ориентиаци€ центра модели
*/

void SpriteModel::SetPivotPosition (const vec3f& position)
{
  SetPivot (position, impl->pivot_rotation);
}

void SpriteModel::SetPivotPosition (float x, float y, float z)
{
  SetPivotPosition (vec3f (x, y, z));
}

//поворот центра относительно Oz
void SpriteModel::SetPivotRotation (float angle_in_degrees)
{
  SetPivot (impl->pivot_position, angle_in_degrees);
}

void SpriteModel::SetPivot (const vec3f& position, float angle_in_degrees)
{
  impl->pivot_position       = position;
  impl->pivot_rotation       = angle_in_degrees;
  impl->need_recalc_world_tm = true;    

    //оповещение об обновлении

  UpdateNotify ();
}

const vec3f& SpriteModel::PivotPosition () const
{
  return impl->pivot_position;
}

float SpriteModel::PivotRotation () const
{
  return impl->pivot_rotation;
}

/*
    ѕолучение матриц преобразовани€ после применени€ pivot-преобразовани€
*/

namespace
{

//расчЄт матрицы центра
void eval_pivot_tm (const math::vec3f& position, float angle, math::mat4f& pivot_tm)
{
  pivot_tm = translate (-position) * fromAxisAngle (deg2rad (angle), vec3f (0, 0, 1));
}

}

void SpriteModel::EvalTransformationAfterPivot (NodeTransformSpace space, math::mat4f& result) const
{
  switch (space)
  {
    case NodeTransformSpace_Local:
    {
      eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, result);
      break;
    }
    case NodeTransformSpace_Parent:
    {
      math::mat4f pivot_tm;

      eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, pivot_tm);

      result = LocalTM () * pivot_tm;

      break;
    }
    case NodeTransformSpace_World:
      if (impl->need_recalc_world_tm)
      {        
        math::mat4f pivot_tm;

        eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, pivot_tm);

        impl->world_tm = WorldTM () * pivot_tm;

        impl->need_recalc_world_tm = false;
      }

      result = impl->world_tm;

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::EvalTransformationAfterPivot", "space", space);
  }
}

/*
     оличество спрайтов / получение массива спрайтов
*/

size_t SpriteModel::SpriteDescsCount () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCountCore ();
}

const SpriteModel::SpriteDesc* SpriteModel::SpriteDescs () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCore ();
}

/*
    ƒинамическа€ диспетчеризаци€
*/

void SpriteModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    ѕодписка на событи€ модели
*/

xtl::connection SpriteModel::RegisterEventHandler (SpriteModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case SpriteModelEvent_AfterSpriteDescsUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    ќповещение об изменении мировой матрицы преобразований
*/

void SpriteModel::AfterUpdateWorldTransformEvent ()
{
  Entity::AfterUpdateWorldTransformEvent ();

  impl->need_recalc_world_tm = true;
}

/*
    ќповещение об изменении данных спрайтов
*/

void SpriteModel::UpdateSpriteDescsNotify ()
{
  impl->Notify (*this, SpriteModelEvent_AfterSpriteDescsUpdate);

  UpdateNotify ();
}
