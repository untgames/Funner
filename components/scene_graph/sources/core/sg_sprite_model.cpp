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
  size_t            material_hash;                  //хэш имени материала
  stl::string       batch;                          //имя пакета
  size_t            batch_hash;                     //хэш пакета
  SpriteMode        mode;                           //режим спрайтов
  SpriteUsage       usage;                          //режим использования спрайтов
  math::vec3f       up;                             //вектор вверх
  SpriteModelSignal signals [SpriteModelEvent_Num]; //сигналы модели 

  Impl ()
    : material_hash (0xffffffff)
    , batch_hash (0xffffffff)
    , mode (SpriteMode_Default)
    , usage (SpriteUsage_Default)
    , up (0, 1.0f, 0)
  {
  }

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

  size_t hash = common::strhash (in_material);

  if (hash == impl->material_hash)
    return;

  impl->material      = in_material;  
  impl->material_hash = hash;

  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

size_t SpriteModel::MaterialHash () const
{
  return impl->material_hash;
}

/*
    Пакет
*/

void SpriteModel::SetBatch (const char* in_batch)
{
  if (!in_batch)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetBatch", "batch");

  size_t hash = common::strhash (in_batch);

  if (hash == impl->batch_hash)
    return;

  impl->batch      = in_batch;  
  impl->batch_hash = hash;

  impl->Notify (*this, SpriteModelEvent_AfterCreationParamsUpdate);

  UpdateNotify ();
}

const char* SpriteModel::Batch () const
{
  return impl->batch.c_str ();
}

size_t SpriteModel::BatchHash () const
{
  return impl->batch_hash;
}

/*
    Режим спрайтов
*/

void SpriteModel::SetMode (SpriteMode mode)
{
  switch (mode)
  {
    case SpriteMode_Billboard:
    case SpriteMode_Oriented:
    case SpriteMode_OrientedBillboard:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::SetMode", "mode", mode);
  }

  if (impl->mode == mode)
    return;

  impl->mode = mode;

  impl->Notify (*this, SpriteModelEvent_AfterCreationParamsUpdate);

  UpdateNotify ();
}

SpriteMode SpriteModel::Mode () const
{
  return impl->mode;
}

/*
    Режим использования спрайтов
*/

void SpriteModel::SetUsage (SpriteUsage usage)
{
  switch (usage)
  {
    case SpriteUsage_Static:
    case SpriteUsage_Dynamic:
    case SpriteUsage_Stream:
    case SpriteUsage_Batching:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::SetUsage", "usage", usage);
  }

  if (impl->usage == usage)
    return;

  impl->usage = usage;

  impl->Notify (*this, SpriteModelEvent_AfterCreationParamsUpdate);

  UpdateNotify ();
}

SpriteUsage SpriteModel::Usage () const
{
  return impl->usage;
}

/*
    Вектор "вверх"
*/

void SpriteModel::SetOrtUp (const math::vec3f& up)
{
  if (up == impl->up)
    return;

  impl->up = up;

  impl->Notify (*this, SpriteModelEvent_AfterCreationParamsUpdate);  

  UpdateNotify ();
}

const math::vec3f& SpriteModel::OrtUp () const
{
  return impl->up;
}

/*
    Количество спрайтов / размер буфера спрайтов / получение массива спрайтов
*/

size_t SpriteModel::SpriteDescsCount () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCountCore ();
}

size_t SpriteModel::SpriteDescsCapacity () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCapacityCore ();
}

const SpriteDesc* SpriteModel::SpriteDescs () const
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
    case SpriteModelEvent_AfterCreationParamsUpdate:
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
  bindings.AddProperty ("Batch", xtl::bind (&SpriteModel::Batch, this), xtl::bind (&SpriteModel::SetBatch, this, _1));
  bindings.AddProperty ("Up", xtl::bind (&SpriteModel::OrtUp, this), xtl::bind (&SpriteModel::SetOrtUp, this, _1));
}
