/*
    Таблица размеров хэш-таблицы (from SGI STL)
*/

template <bool dummy> struct hashtable_prime_list
{
  static const unsigned long table [28];
};

template <bool dummy> const unsigned long hashtable_prime_list<dummy>::table [28] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
}; 

/*
    Управление ростом хэш-таблицы
*/

inline unsigned long stl_next_prime (unsigned long size)
{
  const unsigned long *first = hashtable_prime_list<true>::table, 
                      *last  = first + sizeof (hashtable_prime_list<true>::table)/sizeof (*hashtable_prime_list<true>::table),
                      *pos   = lower_bound (first,last,size);
                      
  return pos == last ? last [-1] : *pos;
}

/*
    hashtable::Bucket
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::Bucket::Bucket (iterator _first,size_type _count)
{
  first = _first;
  count = _count;
}

/*
    Конструкторы
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::hashtable 
(size_type             _elements_count,
 const hasher&         _hash_fn,
 const key_equal&      _equals,
 const allocator_type& _allocator)
  : list (_allocator), table (_elements_count,Bucket (end (),0),_allocator), hash (_hash_fn), 
    equals (_equals), num_elements (0)
{ }

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::hashtable (const hashtable& ht)
  : list (ht.get_allocator ()), table (ht.get_allocator ()), hash (ht.hash), 
    equals (ht.equals), num_elements (0)
{
  _copy (ht);
}

/*
    Аллокатор
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::allocator_type
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::get_allocator () const
{
  return list.get_allocator ();
}

/*
    Функции общего назначения
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size () const
{
  return num_elements;
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::max_size () const
{
  return list.max_size ();
} 

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::empty () const
{
  return num_elements == 0;
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::hasher 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::hash_func () const
{
  return hash;
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::key_equal 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::key_eq () const
{
  return equals;
}

/*
    Получение итераторов
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator> 
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::begin ()
{
  return list.begin ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::end ()
{
  return list.end ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::begin () const
{
  return list.begin ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::end () const
{
  return list.end ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::reverse_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::rbegin ()
{
  return list.rbegin ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_reverse_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::rbegin () const
{
  return list.rbegin ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::reverse_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::rend ()
{
  return list.rend ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_reverse_iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::rend () const
{
  return list.rend ();
}

/*
    Информация о пакетах
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::bucket_count () const
{
  return table.size ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::max_bucket_count () const
{
  return table.max_size ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::elems_in_bucket (size_type bucket) const
{
  if (bucket >= table.size ())
    stl_raise_out_of_range (*this,bucket);

  return table [bucket].count;
}

/*
    Internal
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline const typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::key_type& 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::keyof (const value_type& obj)
{
  return KeyOf () (obj);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::bkt_num_key (const key_type& key) const
{
  return hash (key) % table.size ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::bkt_num (const value_type& obj) const
{
  return bkt_num_key (keyof (obj));
}   

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::next_size (size_type size) const
{
  return stl_next_prime (size);  
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::_copy (const hashtable& ht)
{     
  table.assign (ht.table.size (),Bucket (end (),0));
  list.clear ();
    
  try
  {
    for (const_reverse_iterator i=ht.rbegin ();i!=ht.rend ();++i)
    {       
      Bucket& bucket = table [bkt_num (*i)];
          
      bucket.first = list.insert (bucket.first,*i);
      bucket.count++;       
    }

    num_elements = ht.num_elements;
  }
  catch (...)
  {
    clear ();
    throw;
  }  
}

/*
    Присваивание
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>& 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator = (const hashtable& ht)
{
  if (&ht == this)
    return *this;

  clear ();
  
  hash    = ht.hash;
  equals  = ht.equals;
  
  _copy (ht);

  return *this;
}

/*
    Вставка
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
pair<typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator,bool> 
inline hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_unique (const value_type& obj)
{
  resize (num_elements+1);
  return insert_unique_noresize (obj);  
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator 
inline hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_equal (const value_type& obj)
{
  resize (num_elements+1);
  return insert_equal_noresize (obj);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
pair<typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator,bool> 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_unique_noresize (const value_type& obj)
{
  Bucket& bucket = table [bkt_num (obj)];

  iterator i = bucket.first;

  if (bucket.count)
  {  
    for (size_type count=bucket.count;count--;++i)
      if (equals (keyof (*i),keyof (obj)))
        return pair<iterator,bool> (i,false); //объект уже присутствует
        
    i = list.insert (i,obj); //вставка нового значения
  }
  else  bucket.first = i = list.insert (end (),obj);

  bucket.count++;      
  num_elements++;
  
  return pair<iterator,bool> (i,true);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_equal_noresize (const value_type& obj)
{
  Bucket& bucket = table [bkt_num (obj)];

  iterator i = bucket.first;
  
  if (bucket.count)
  {
    size_type count = bucket.count;
    
    for (;count && !equals (keyof (*i),keyof (obj));++i,count--);
    
    if (count)
      for (++i;--count && equals (keyof (*i),keyof (obj));++i);
    
    i = list.insert (i,obj); //вставка нового значения  
  }
  else bucket.first = i = list.insert (end (),obj);
  
  bucket.count++;
  num_elements++;  
  
  return i;
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator> template <class Iter> 
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_unique (Iter first,Iter last)
{
  size_type count = distance (first,last);
  
  resize (num_elements + count);
  
  for (;count--;++first) insert_unique_noresize (*first);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator> template <class Iter> 
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::insert_equal (Iter first,Iter last)
{
  size_type count = distance (first,last);
  
  resize (num_elements + count);
  
  for (;count--;++first) insert_equal_noresize (*first);
}

/*
    Удаление
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::erase (iterator position)
{
  if (position == end ())
    stl_raise_out_of_range (*this,position);

  Bucket& bucket = table [bkt_num (*position)]; 
  
  bucket.count--;
  
  if (bucket.first == position)
    bucket.first++;
    
  num_elements--;
    
  list.erase (position);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::erase (iterator first,iterator last)
{
  if (first == last)
    return;

  if (first == begin () && last == end ())
  { 
    clear ();
    return ;
  }

  iterator next = first;

  for (++next;first!=last;first=next,++next) erase (first);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type 
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::erase (const key_type& key)
{
  pair<iterator,iterator> range = equal_range (key);

  size_type count = distance (range.first,range.second);  

  erase (range.first,range.second);  

  return count;  
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::erase (const key_type* first,const key_type* last)
{
  for (;first!=last;++first) erase (*first);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::clear ()
{
  list.clear ();
  table.assign (size (),Bucket (end (),0));

  num_elements = 0;
}

/*
    Изменение размера
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::resize (size_type new_size)
{
  size_type old_size = table.size ();
  
  if (new_size <= old_size)
    return;
  
  new_size = next_size (new_size);
  
  if (new_size <= old_size)
    return;
            
  list_type   new_list  (list.get_allocator ());
  vector_type new_table (new_size,Bucket (new_list.end (),0),table.get_allocator ());
    
  try
  {   
    while (!list.empty ())
    {   
      iterator i = --list.end ();
    
      Bucket& bucket = new_table [hash (keyof (*i)) % new_size];
                       
      new_list.splice (bucket.first,list,i);
            
      bucket.first = i;
      bucket.count++;
    }
    
      //list.end () возвращает итератор на узел жёстко связанный со списком
      //при выходе из функции данный итератор будет не действителен
    for (typename vector_type::iterator i=new_table.begin ();i!=new_table.end ();++i)
      if (i->first == new_list.end ())
        i->first = end ();

    stl::swap (new_table,table);
    stl::swap (new_list,list);    
  }
  catch (...)
  {
    clear ();
    throw;
  }  
}

/*
    Поиск
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::find (const key_type& key)
{
  if (!num_elements)
    return end ();

  const Bucket& bucket = table [bkt_num_key (key)];
  iterator      i      = bucket.first;
  size_type     count  = bucket.count;

  for (;count && !equals (keyof (*i),key);++i,--count);
  
  return count ? i : end ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_iterator
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::find (const key_type& key) const
{
  if (!num_elements)
    return end ();

  const Bucket&  bucket = table [bkt_num_key (key)];
  const_iterator i      = bucket.first;
  size_type      count  = bucket.count;

  for (;count && !equals (keyof (*i),key);++i,--count);
  
  return count ? i : end ();
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::size_type
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::count (const key_type& key) const
{
  if (!num_elements)
    return 0;

  stl::pair<const_iterator, const_iterator> range = equal_range (key);

  return distance (range.first, range.second);
}
    
template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline pair<typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator,
            typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::iterator>
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::equal_range (const key_type& key)
{
  if (!num_elements)
    return pair<iterator,iterator> (end (),end ());

  const Bucket& bucket = table [bkt_num_key (key)];
  iterator  i          = bucket.first, first;
  size_type count      = bucket.count;

  for (;count && !equals (keyof (*i),key);++i,--count);

  if (count) for (first=i++,count--;count && equals (keyof (*i),key);++i,--count);
  else       first = i;

  return pair<iterator,iterator> (first,i);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline pair<typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_iterator,
            typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::const_iterator>
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::equal_range (const key_type& key) const
{
  if (!num_elements)
    return pair<const_iterator,const_iterator> (end (),end ());

  const Bucket& bucket = table [bkt_num_key (key)];
  iterator  i          = bucket.first, first;
  size_type count      = bucket.count;

  for (;count && !equals (keyof (*i),key);++i,--count);

  if (count) for (first=i++,count--;count && equals (keyof (*i),key);++i,--count);
  else       first = i;

  return pair<const_iterator,const_iterator> (first,i);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
typename hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::reference
hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::find_or_insert (const value_type& obj)
{
  if (!num_elements)
    return *insert_unique (obj).first;

  Bucket*  bucket = &table [bkt_num (obj)];
  iterator pos    = bucket->first;
      
  for (size_type count=bucket->count;count--;++pos)
    if (equals (keyof (*pos),keyof (obj)))
      return *pos; 
    
  size_type old_table_size = table.size ();
  
  resize (num_elements+1);

  if (old_table_size != table.size ()) //если хэш-таблица была перестроена
    return *insert_unique_noresize (obj).first;
    
  pos = list.insert (pos,obj); //вставка нового значения

  if (!bucket->count)
    bucket->first = pos;

  bucket->count++;
  num_elements++;

  return *pos;
}

/*
    Обмен содержимого двух таблиц
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
void hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::swap (hashtable& ht)
{
  stl::swap (hash,ht.hash);
  stl::swap (equals,ht.equals);
  stl::swap (table,ht.table);
  stl::swap (list,ht.list);
  stl::swap (num_elements,ht.num_elements);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline void swap 
 (hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>& a,
  hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>& b)
{
  a.swap (b);
}

/*
    Сравнение (в лексикографическом порядке)
*/

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator == (const hashtable& ht) const
{
  return size () == ht.size () && equal (begin (),end (),ht.begin ());
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator < (const hashtable& ht) const
{
  return lexicographical_compare (begin (),end (),ht.begin (),ht.end ());
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator != (const hashtable& ht) const
{
  return !(*this == ht);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator > (const hashtable& x) const
{
  return x < *this;
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator <= (const hashtable& x) const
{
  return !(x < *this);
}

template <class Val,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>    
inline bool hashtable<Val,Key,HashFn,KeyOf,EqualKey,Allocator>::operator >= (const hashtable& x) const
{
  return !(*this < x);
}
