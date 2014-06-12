#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации списка спрайтов
*/

typedef xtl::uninitialized_storage<SpriteDesc> SpriteDescArray;

struct SpriteList::Impl: public xtl::instance_counter<SpriteList>
{
  SpriteDescArray items;    //массив спрайтов  
};

/*
    Конструктор / деструктор
*/

SpriteList::SpriteList ()
  : impl (new Impl)
{
}

SpriteList::~SpriteList ()
{
}

/*
    Создание списка спрайтов
*/

SpriteList::Pointer SpriteList::Create ()
{
  return Pointer (new SpriteList, false);
}

/*
    Размер массива спрайтов
*/

size_t SpriteList::SpritesCount () const
{
  return impl->items.size ();
}

size_t SpriteList::SpriteDescsCountCore ()
{
  return impl->items.size ();
}

/*
    Изменение размера списка спрайтов / резервирование места для хранения спрайтов
*/

void SpriteList::Resize (size_t count)
{
  impl->items.resize (count);
  
  UpdateNotify ();
}

void SpriteList::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
    Массив спрайтов
*/

const SpriteDesc* SpriteList::Sprites () const
{
  return impl->items.data ();
}

const SpriteDesc* SpriteList::SpriteDescsCore ()
{
  return impl->items.data ();
}

SpriteDesc* SpriteList::Sprites ()
{
  return const_cast<SpriteDesc*> (const_cast<const SpriteList&> (*this).Sprites ());
}

/*
    Добавление спрайтов в массив
*/

void SpriteList::Insert (const SpriteDesc& desc)
{
  Insert (1, &desc);
}

void SpriteList::Insert (size_t items_count, const SpriteDesc* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (SpriteDesc));

  UpdateNotify ();
}

/*
    Очистка списка спрайтов
*/

void SpriteList::RemoveAll ()
{
  impl->items.resize (0);
}

/*
    Динамическая диспетчеризация
*/

void SpriteList::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    SpriteModel::AcceptCore (visitor);
}

/*
    Оповещение об обновлении данных
*/

void SpriteList::Invalidate ()
{
  UpdateSpriteDescsNotify ();
}
