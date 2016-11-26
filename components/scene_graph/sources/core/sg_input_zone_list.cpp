#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации списка спрайтов
*/

typedef xtl::uninitialized_storage<InputZoneModel::ZoneDesc> ZoneDescArray;

struct InputZoneList::Impl: public xtl::instance_counter<InputZoneList>
{
  ZoneDescArray items;    //массив спрайтов  
};

/*
    Конструктор / деструктор
*/

InputZoneList::InputZoneList ()
  : impl (new Impl)
{
}

InputZoneList::~InputZoneList ()
{
}

/*
    Создание списка спрайтов
*/

InputZoneList::Pointer InputZoneList::Create ()
{
  return Pointer (new InputZoneList, false);
}

/*
    Размер массива спрайтов
*/

size_t InputZoneList::ZonesCount () const
{
  return impl->items.size ();
}

size_t InputZoneList::ZoneDescsCountCore ()
{
  return impl->items.size ();
}

/*
    Изменение размера списка спрайтов / резервирование места для хранения спрайтов
*/

void InputZoneList::Resize (size_t count)
{
  impl->items.resize (count);
  
  UpdateNotify ();
}

void InputZoneList::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
    Массив спрайтов
*/

const InputZoneModel::ZoneDesc* InputZoneList::Zones () const
{
  return impl->items.data ();
}

const InputZoneModel::ZoneDesc* InputZoneList::ZoneDescsCore ()
{
  return impl->items.data ();
}

InputZoneModel::ZoneDesc* InputZoneList::Zones ()
{
  return const_cast<ZoneDesc*> (const_cast<const InputZoneList&> (*this).Zones ());
}

/*
    Добавление спрайтов в массив
*/

void InputZoneList::Insert (const ZoneDesc& ZoneDesc)
{
  Insert (1, &ZoneDesc);  
}

void InputZoneList::Insert (size_t items_count, const ZoneDesc* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::InputZoneList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (ZoneDesc));

  UpdateNotify ();
}

/*
    Очистка списка спрайтов
*/

void InputZoneList::RemoveAll ()
{
  impl->items.resize (0);
}

/*
    Динамическая диспетчеризация
*/

void InputZoneList::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    InputZoneModel::AcceptCore (visitor);
}

/*
    Оповещение об обновлении данных
*/

void InputZoneList::Invalidate ()
{
  UpdateZoneDescsNotify ();
}
