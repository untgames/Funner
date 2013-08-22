///////////////////////////////////////////////////////////////////////////////////////////////////
///Кэшированное отображение (стратегия кэширования - LRU)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key, class Value>
class CacheMap: public Cache
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    CacheMap (const CacheManagerPtr&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    ();
    bool   IsEmpty ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента
///////////////////////////////////////////////////////////////////////////////////////////////////
    Value* Find (const Key&);
    Value& Get  (const Key&);
    Value& operator [] (const Key& key) { return Get (key); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка и удаление элемента
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add    (const Key& key, const Value& value);
    void Remove (const Key& key);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> void ForEach (Fn fn);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс закэшированных элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushCache ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка
///////////////////////////////////////////////////////////////////////////////////////////////////
    Value& AddCore (const Key& key, const Value& value);
  
  private:
    struct Item;
  
    typedef stl::hash_map<Key, Item>     ItemMap;    
    typedef stl::list<typename ItemMap::iterator> ItemList;
    
    struct Item: public Value
    {
      size_t                      last_use_frame; //кадр последнего использования элемента
      size_t                      last_use_time;  //время последнего использования элемента
      typename ItemList::iterator position;       //положение в списке элементов      
    };        

  private:
    ItemMap  item_map;
    ItemList item_list;
};

/*
    Реализация
*/

/*
    Конструктор
*/

template <class Key, class Value>
CacheMap<Key, Value>::CacheMap (const CacheManagerPtr& manager)
  : Cache (manager)
{
}

/*
    Количество элементов / проверка на пустоту
*/

template <class Key, class Value>
size_t CacheMap<Key, Value>::Size ()
{
  return item_map.size ();
}

template <class Key, class Value>
bool CacheMap<Key, Value>::IsEmpty ()
{
  return item_map.empty ();
}

/*
    Поиск элемента
*/

template <class Key, class Value>
Value* CacheMap<Key, Value>::Find (const Key& key)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter == item_map.end ())
    return 0;
    
  iter->second.last_use_time  = CurrentTime ();
  iter->second.last_use_frame = CurrentFrame ();
  
  item_list.splice (item_list.begin (), item_list, iter->second.position);
  
  return &iter->second;
}

template <class Key, class Value>
Value& CacheMap<Key, Value>::Get (const Key& key)
{
  if (Value* result = Find (key))
    return *result;
    
  return AddCore (key, Value ());
}

/*
    Вставка и удаление элемента
*/

template <class Key, class Value>
Value& CacheMap<Key, Value>::AddCore (const Key& key, const Value& value)
{
  Item item;
  
  item.last_use_time  = CurrentTime ();
  item.last_use_frame = CurrentFrame ();
  
  static_cast<Value&> (item) = value;
  
  stl::pair<typename ItemMap::iterator, bool> result = item_map.insert_pair (key, item);
  
  if (!result.second)
    throw xtl::make_argument_exception ("render::manager::CacheMap<Key, Value>::Add", "Internal error: item with specified key/value has been already inserted");    
    
  try
  {
    item_list.push_front (result.first);
  
    result.first->second.position = item_list.begin ();
    
    return result.first->second;
  }
  catch (...)
  {
    item_map.erase (result.first);
    throw;
  }
}

template <class Key, class Value>
void CacheMap<Key, Value>::Add (const Key& key, const Value& value)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter != item_map.end ())
    throw xtl::make_argument_exception ("render::manager::CacheMap<Key, Value>::Add", "Item with specified key/value has been already inserted");
    
  AddCore (key, value);
}

template <class Key, class Value>
void CacheMap<Key, Value>::Remove (const Key& key)
{
  typename ItemMap::iterator iter = item_map.find (key);
  
  if (iter == item_map.end ())
    return;
  
  item_list.erase (iter->second.position);
  item_map.erase (iter);
}

/*
    Очистка
*/

template <class Key, class Value>
void CacheMap<Key, Value>::Clear ()
{
  item_map.clear ();
  item_list.clear ();
}

/*
    Итераторы
*/

template <class Key, class Value> template <class Fn>
void CacheMap<Key, Value>::ForEach (Fn fn)
{
  for (typename ItemMap::iterator iter=item_map.begin (); iter!=item_map.end (); ++iter)
    fn (iter->first, iter->second);
}

/*
    Сброс закэшированных элементов которые не использовались с кадра min_frame или времени min_time
*/

template <class Key, class Value>
void CacheMap<Key, Value>::FlushCache ()
{
  size_t current_time  = CurrentTime (),
         current_frame = CurrentFrame (),
         time_delay    = TimeDelay (),
         frame_delay   = FrameDelay ();

  while (!item_list.empty ())
  {
    Item& item = item_list.back ()->second;
    
    if (current_time - item.last_use_time <= time_delay && current_frame - item.last_use_frame <= frame_delay)
      return;      
      
    item_map.erase (item_list.back ());
    item_list.pop_back ();
  }
}
