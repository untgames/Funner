#include "shared.h"

using namespace store;

namespace
{

const size_t DEFAULT_ITEMS_RESERVE = 8;

}

///Реализация списка продуктов
struct ProductList::Impl : public xtl::reference_counter
{
  stl::vector<Product> items;

  Impl ()
  {
    items.reserve (DEFAULT_ITEMS_RESERVE);
  }
};

/*
   Конструктор / деструктор / копирование
*/

ProductList::ProductList ()
  : impl (new Impl)
  {}

ProductList::ProductList (const ProductList& source)
  : impl (source.impl)
{
  addref (impl);
}

ProductList::~ProductList ()
{
  release (impl);
}

ProductList& ProductList::operator = (const ProductList& source)
{
  ProductList (source).Swap (*this);

  return *this;
}

/*
   Количество элементов / проверка на пустоту
*/

size_t ProductList::Size () const
{
  return impl->items.size ();
}

bool ProductList::IsEmpty () const
{
  return impl->items.empty ();
}

/*
   Доступ к элементу по индексу
*/

const Product& ProductList::operator [] (size_t index) const
{
  return const_cast<ProductList&> (*this) [index];
}

Product& ProductList::operator [] (size_t index)
{
  if (index >= impl->items.size ())
    throw xtl::make_range_exception ("store::ProductList::operator []", "index", index, impl->items.size ());

  return impl->items [index];
}

/*
   Резервирование элементов
*/

size_t ProductList::Capacity () const
{
  return impl->items.capacity ();
}

void ProductList::Reserve (size_t count)
{
  impl->items.reserve (count);
}

/*
   Добавление элементов в коллекцию
*/

size_t ProductList::Add (const Product& item)
{
  impl->items.push_back (item);

  return impl->items.size () - 1;
}

/*
   Удаление элементов из коллекции
*/

void ProductList::Remove (size_t index)
{
  if (index >= impl->items.size ())
    return;

  impl->items.erase (impl->items.begin () + index);
}

/*
   Очистка коллекции
*/

void ProductList::Clear ()
{
  impl->items.clear ();
}

/*
   Обмен
*/

void ProductList::Swap (ProductList& source)
{
  stl::swap (impl, source.impl);
}

namespace store
{

/*
   Обмен
*/

void swap (ProductList& product1, ProductList& product2)
{
  product1.Swap (product2);
}

}
