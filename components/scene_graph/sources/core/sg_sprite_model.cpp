#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации спрайтовой модели
*/

typedef xtl::signal<void (SpriteModel& sender, SpriteModelEvent event_id)> SpriteModelSignal;

struct SpriteModel::Impl: public xtl::instance_counter<SpriteModel>
{
  stl::string       material;                       //имя материала
  SpriteModelSignal signals [SpriteModelEvent_Num]; //сигналы модели 
  float             alpha_reference;                //параметр, используемый для альфа-теста

  Impl () : alpha_reference (0.f) {}

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

  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    Параметр, используемый для альфа-теста
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
    VisualModel::AcceptCore (visitor);
}

/*
    Подписка на события модели
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
    Оповещение об изменении данных спрайтов
*/

void SpriteModel::UpdateSpriteDescsNotify ()
{
  impl->Notify (*this, SpriteModelEvent_AfterSpriteDescsUpdate);

  UpdateNotify ();
}

/*
    Связывание свойств
*/

void SpriteModel::BindProperties (common::PropertyBindingMap& bindings)
{
  VisualModel::BindProperties (bindings);

  bindings.AddProperty ("Material", xtl::bind (&SpriteModel::Material, this), xtl::bind (&SpriteModel::SetMaterial, this, _1));
  bindings.AddProperty ("AlphaReference", xtl::bind (&SpriteModel::AlphaReference, this), xtl::bind (&SpriteModel::SetAlphaReference, this, _1));
}
