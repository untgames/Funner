#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации спрайтовой модели
*/

typedef xtl::signal<void (SpriteModel& sender, SpriteModelEvent event_id)> SpriteModelSignal;

struct SpriteModel::Impl
{
  stl::string       material;                       //имя материала
  SpriteModelSignal signals [SpriteModelEvent_Num]; //сигналы модели 
  
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
    Конструктор / деструктор
*/

SpriteModel::SpriteModel ()
  : impl (new Impl)
{
}

SpriteModel::~SpriteModel ()
{
}

/*
    Материал
*/

void SpriteModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetMaterial", "material");

  impl->material = in_material;
  
  impl->Notify (*this, SpriteModelEvent_AfterMaterialUpdate);

  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    Количество спрайтов / получение массива спрайтов
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
    Динамическая диспетчеризация
*/

void SpriteModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Подписка на события модели
*/

xtl::connection SpriteModel::RegisterEventHandler (SpriteModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case SpriteModelEvent_AfterMaterialUpdate:
    case SpriteModelEvent_AfterSpriteDescsUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    Оповещение об изменении данных спрайтов
*/

void SpriteModel::UpdateSpriteDescsNotify ()
{
  impl->Notify (*this, SpriteModelEvent_AfterSpriteDescsUpdate);

  UpdateNotify ();
}
