namespace
{

const size_t DEFAULT_ITEMS_RESERVE = 8;

}

/*
   Конструкторы / присваивание
*/

template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>::CollectionImpl ()
{
  items.reserve (DEFAULT_ITEMS_RESERVE);
}
    
template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>::CollectionImpl (const CollectionImpl& collection)
  : items (collection.items)
  {}
    
template <class Item, class BaseClass>
CollectionImpl<Item, BaseClass>& CollectionImpl<Item, BaseClass>::operator = (const CollectionImpl& collection)
{
  CollectionImpl (collection).items.swap (items);
  return *this;
}

/*
   Количество элементов / проверка на пустоту
*/

template <class Item, class BaseClass>
size_t CollectionImpl<Item, BaseClass>::Size () const
{
  return items.size ();
}

template <class Item, class BaseClass>
bool CollectionImpl<Item, BaseClass>::IsEmpty () const
{
  return items.empty ();
}

/*
   Доступ к элементу по индексу
*/

template <class Item, class BaseClass>
const Item& CollectionImpl<Item, BaseClass>::operator [] (size_t index) const
{
  if (index >= items.size ())
    throw xtl::make_range_exception ("media::collada::ICollection::operator []", "index", index, items.size ());

  return items [index];
}
    
template <class Item, class BaseClass>
Item& CollectionImpl<Item, BaseClass>::operator [] (size_t index)
{
  return const_cast<Item&> (const_cast<const CollectionImpl&> (*this) [index]);
}

/*
   Получение итератора
*/

template <class Item, class BaseClass>
typename CollectionImpl<Item, BaseClass>::Iterator CollectionImpl<Item, BaseClass>::CreateIterator ()
{
  return Iterator (items.begin (), items.begin (), items.end ());
}
    
template <class Item, class BaseClass>
typename CollectionImpl<Item, BaseClass>::ConstIterator CollectionImpl<Item, BaseClass>::CreateIterator () const
{
  return ConstIterator (items.begin (), items.begin (), items.end ());
}

/*
   Добавление элемента в коллекцию
*/

template <class Item, class BaseClass>
size_t CollectionImpl<Item, BaseClass>::Insert (Item& item)
{
  items.push_back (item);

  return items.size () - 1;
}

/*
   Удаление элемента из коллекции
*/

template <class Item, class BaseClass>
void CollectionImpl<Item, BaseClass>::Remove (size_t index)
{
  if (index >= items.size ())
    return;

  items.erase (items.begin () + index);
}

/*
   Очистка коллекции
*/

template <class Item, class BaseClass>
void CollectionImpl<Item, BaseClass>::Clear ()
{
  items.clear ();
}
