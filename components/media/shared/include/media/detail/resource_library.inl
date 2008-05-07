namespace detail
{

/*
    Селектор ресурса
*/

template <class T>
struct resource_selector
{
  template <class T1> T& operator () (T1& value) const { return value.second; }
};

}

/*
    Конструкторы / деструктор / присваивание
*/

template <class T>
inline ResourceLibrary<T>::ResourceLibrary ()
{
}

template <class T>
inline ResourceLibrary<T>::ResourceLibrary (const ResourceLibrary& l)
  : items (l.items)
{
}

template <class T>
inline ResourceLibrary<T>::~ResourceLibrary ()
{
}

template <class T>
inline ResourceLibrary<T>& ResourceLibrary<T>::operator = (const ResourceLibrary& l)
{
  ResourceLibrary (l).Swap (*this);
  return *this;
}

/*
    Количество элементов / проверка на пустоту
*/

template <class T>
inline size_t ResourceLibrary<T>::Size () const
{
  return items.size ();
}

template <class T>
inline bool ResourceLibrary<T>::IsEmpty () const
{
  return items.empty ();
}

/*
    Получение итератора
*/

template <class T>
inline typename ResourceLibrary<T>::Iterator ResourceLibrary<T>::CreateIterator ()
{
  return Iterator (items.begin (), items.begin (), items.end (), detail::resource_selector<T> ());
}

template <class T>
inline typename ResourceLibrary<T>::ConstIterator ResourceLibrary<T>::CreateConstIterator () const
{
  return ConstIterator (items.begin (), items.begin (), items.end (), detail::resource_selector<const T> ());
}

/*
    Получение имени ресурса по итератору
*/

template <class T>
inline const char* ResourceLibrary<T>::ItemId (const ConstIterator& i)
{
  const ItemMap::const_iterator* iter = i.target<ItemMap::const_iterator> ();    
  
  if (!iter)
  {
    const ItemMap::iterator* iter = i.target<ItemMap::iterator> ();

    if (!iter)
      common::RaiseInvalidArgument ("media::ResourceLibrary::ItemId", "iterator", "wrong-type");

    return (*iter)->first.c_str ();
  }

  return (*iter)->first.c_str ();
}

/*
    Поиск элемента в библиотеке
*/

template <class T>
inline const T* ResourceLibrary<T>::Find (const char* name) const
{
  if (!name)
    return 0;
    
  ItemMap::const_iterator iter = items.find (name);
  
  return iter != items.end () ? &iter->second : 0;
}

template <class T>
inline T* ResourceLibrary<T>::Find (const char* name)
{
  return const_cast<T*> (const_cast<const ResourceLibrary&> (*this).Find (name));
}

/*
    Добавление/удаление элементов из библиотеки
*/

template <class T>
inline void ResourceLibrary<T>::Insert (const char* name, Item& item)
{
  if (!name)
    common::RaiseNullArgument ("media::ResourceLibrary::Insert", "name");
  
  ItemMap::iterator iter = items.find (name);
  
  if (iter != items.end ())
  {
    iter->second = item;    
  }
  else
  {
    items.insert_pair (name, item);
  }
}

template <class T>
inline void ResourceLibrary<T>::Remove (const char* name)
{
  if (!name)
    return;
    
  items.erase (name);
}

template <class T>
inline void ResourceLibrary<T>::Remove (Item& item)
{
  for (ItemMap::iterator i=items.begin (), end=items.end (); i!=end;)
    if (i->second.Id () == item.Id ())
    {
      ItemMap::iterator next = i;
      
      ++next;
      
      items.erase (i);
      
      i = next;
    }
    else ++i;
}

/*
    Очистка библиотеки
*/

template <class T>
inline void ResourceLibrary<T>::Clear ()
{
  items.clear ();
}

/*
    Обмен
*/

template <class T>
inline void ResourceLibrary<T>::Swap (ResourceLibrary& l)
{
  items.swap (l.items);
}
    
template <class T>
void swap (ResourceLibrary<T>& lib1, ResourceLibrary<T>& lib2)
{
  lib1.Swap (lib2);
}
