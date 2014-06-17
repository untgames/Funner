#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации списка линий
*/

typedef xtl::uninitialized_storage<LineDesc> LineDescArray;

struct LineList::Impl: public xtl::instance_counter<LineList>
{
  LineDescArray items;    //массив линий
};

/*
    Конструктор / деструктор
*/

LineList::LineList ()
  : impl (new Impl)
{
}

LineList::~LineList ()
{
}

/*
    Создание списка линий
*/

LineList::Pointer LineList::Create ()
{
  return Pointer (new LineList, false);
}

/*
    Размер массива линий
*/

size_t LineList::LinesCount () const
{
  return impl->items.size ();
}

size_t LineList::LineDescsCountCore ()
{
  return impl->items.size ();
}

size_t LineList::LineDescsCapacityCore ()
{
  return impl->items.capacity ();
}

/*
    Изменение размера списка линий / резервирование места для хранения линий
*/

void LineList::Resize (size_t count)
{
  impl->items.resize (count);
  
  UpdateNotify ();
}

void LineList::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
    Массив линий
*/

const LineDesc* LineList::Lines () const
{
  return impl->items.data ();
}

const LineDesc* LineList::LineDescsCore ()
{
  return impl->items.data ();
}

LineDesc* LineList::Lines ()
{
  return const_cast<LineDesc*> (const_cast<const LineList&> (*this).Lines ());
}

/*
    Добавление линий в массив
*/

void LineList::Insert (const LineDesc& desc)
{
  Insert (1, &desc);
}

void LineList::Insert (size_t items_count, const LineDesc* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::LineList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (LineDesc));

  UpdateNotify ();
}

/*
    Очистка списка линий
*/

void LineList::RemoveAll ()
{
  impl->items.resize (0);
}

/*
    Динамическая диспетчеризация
*/

void LineList::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    LineModel::AcceptCore (visitor);
}

/*
    Оповещение об обновлении данных
*/

void LineList::Invalidate ()
{
  UpdateLineDescsNotify ();
}
