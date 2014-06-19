#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации модели линий
*/

typedef xtl::signal<void (LineModel& sender, LineModelEvent event_id)> LineModelSignal;

struct LineModel::Impl: public xtl::instance_counter<LineModel>
{
  stl::string     material;                     //имя материала
  size_t          material_hash;                //хэш имени материала
  stl::string     batch;                        //имя пакета
  size_t          batch_hash;                   //хэш имени пакета
  LineUsage       usage;                        //режим использования линий
  LineModelSignal signals [LineModelEvent_Num]; //сигналы модели 

  Impl ()
    : material_hash (0xffffffff)
    , batch_hash (0xffffffff)
    , usage (LineUsage_Default)
  {
  }

    //оповещение о событии
  void Notify (LineModel& sender, LineModelEvent event_id)
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

LineModel::LineModel ()
  : impl (new Impl)
{
}

LineModel::~LineModel ()
{
}

/*
    Материал
*/

void LineModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::LineModel::SetMaterial", "material");

  size_t hash = common::strhash (in_material);

  if (hash == impl->material_hash)
    return;

  impl->material      = in_material;  
  impl->material_hash = hash;

  UpdateNotify ();
}

const char* LineModel::Material () const
{
  return impl->material.c_str ();
}

size_t LineModel::MaterialHash () const
{
  return impl->material_hash;
}

/*
    Пакет
*/

void LineModel::SetBatch (const char* in_batch)
{
  if (!in_batch)
    throw xtl::make_null_argument_exception ("scene_graph::LineModel::SetBatch", "batch");

  size_t hash = common::strhash (in_batch);

  if (hash == impl->batch_hash)
    return;

  impl->batch      = in_batch;  
  impl->batch_hash = hash;

  impl->Notify (*this, LineModelEvent_AfterCreationParamsUpdate);

  UpdateNotify ();
}

const char* LineModel::Batch () const
{
  return impl->batch.c_str ();
}

size_t LineModel::BatchHash () const
{
  return impl->batch_hash;
}

/*
    Режим использования линий
*/

void LineModel::SetUsage (LineUsage usage)
{
  switch (usage)
  {
    case LineUsage_Static:
    case LineUsage_Dynamic:
    case LineUsage_Stream:
    case LineUsage_Batching:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::LineModel::SetUsage", "usage", usage);
  }

  if (impl->usage == usage)
    return;

  impl->usage = usage;

  impl->Notify (*this, LineModelEvent_AfterCreationParamsUpdate);

  UpdateNotify ();
}

LineUsage LineModel::Usage () const
{
  return impl->usage;
}

/*
    Количество линий / размер буфера линий / получение массива линий
*/

size_t LineModel::LineDescsCount () const
{
  return const_cast<LineModel&> (*this).LineDescsCountCore ();
}

size_t LineModel::LineDescsCapacity () const
{
  return const_cast<LineModel&> (*this).LineDescsCapacityCore ();
}

const LineDesc* LineModel::LineDescs () const
{
  return const_cast<LineModel&> (*this).LineDescsCore ();
}

/*
    Динамическая диспетчеризация
*/

void LineModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}

/*
    Подписка на события модели
*/

xtl::connection LineModel::RegisterEventHandler (LineModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case LineModelEvent_AfterLineDescsUpdate:
    case LineModelEvent_AfterCreationParamsUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::LineModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    Оповещение об изменении данных линий
*/

void LineModel::UpdateLineDescsNotify ()
{
  impl->Notify (*this, LineModelEvent_AfterLineDescsUpdate);

  UpdateNotify ();
}

/*
    Связывание свойств
*/

void LineModel::BindProperties (common::PropertyBindingMap& bindings)
{
  VisualModel::BindProperties (bindings);

  bindings.AddProperty ("Material", xtl::bind (&LineModel::Material, this), xtl::bind (&LineModel::SetMaterial, this, _1));
  bindings.AddProperty ("Batch", xtl::bind (&LineModel::Batch, this), xtl::bind (&LineModel::SetBatch, this, _1));
}
