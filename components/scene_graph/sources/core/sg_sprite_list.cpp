#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации списка спрайтов
*/

typedef xtl::uninitialized_storage<SpriteList::Item> ItemArray;

struct SpriteList::Impl
{
  stl::string material; //имя материала
  ItemArray   items;    //массив спрайтов  
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
    Материал
*/

void SpriteList::SetMaterial (const char* material)
{
  if (!material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::SetMaterial", "material");
    
  impl->material = material;
  
  UpdateNotify ();
}

const char* SpriteList::Material () const
{
  return impl->material.c_str ();
}

/*
    Размер массива спрайтов
*/

size_t SpriteList::Size () const
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

const SpriteList::Item* SpriteList::Data () const
{
  return impl->items.data ();
}

SpriteList::Item* SpriteList::Data ()
{
  return const_cast<Item*> (const_cast<const SpriteList&> (*this).Data ());
}

/*
    Добавление спрайтов в массив
*/

void SpriteList::Insert (const Item& item)
{
  Insert (1, &item);  
}

void SpriteList::Insert (size_t items_count, const Item* items)
{
  if (items_count && !items)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteList::Insert", "items");
    
  if (!items_count)
    return;

  size_t old_size = impl->items.size ();

  impl->items.resize (impl->items.size () + items_count);

  memcpy (impl->items.data () + old_size, items, items_count * sizeof (Item));

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
    Entity::AcceptCore (visitor);
}

/*
    Оповещение об обновлении данных
*/

void SpriteList::InvalidateData ()
{
  UpdateNotify ();
}
