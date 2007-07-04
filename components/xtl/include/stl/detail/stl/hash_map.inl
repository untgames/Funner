/*
    Конструкторы
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline hash_map_base<Key,T,HashFn,EqualKey,Allocator>::hash_map_base 
 (size_type             table_size,
  const hasher&         hash,
  const key_equal&      eql,
  const allocator_type& allocator)
    : ht (table_size,hash,eql,allocator)
{ }

/*
    Аллокатор
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::allocator_type
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::get_allocator () const
{
  return ht.get_allocator ();
}

/*
    Функции общего назначения
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size () const
{
  return ht.size ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::max_size () const
{
  return ht.max_size ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::empty () const
{
  return ht.empty ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::hasher 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::hash_func () const
{
  return ht.hash_func ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::key_equal
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::key_eq () const
{
  return ht.key_eq ();
}


/*
    Итераторы
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::begin ()
{
  return ht.begin ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::end ()
{
  return ht.end ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::reverse_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::rbegin ()
{
  return ht.rbegin ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::reverse_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::rend ()
{
  return ht.rend ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::begin () const
{
  return ht.begin ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::end () const
{
  return ht.end ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_reverse_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::rbegin () const
{
  return ht.rbegin ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_reverse_iterator 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::rend () const
{
  return ht.rend ();
}

/*
    Информация о пакетах
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::bucket_count () const
{
  return ht.bucket_count ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::max_bucket_count () const
{
  return ht.max_bucket_count ();
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::elems_in_bucket (size_type bucket) const
{
  return ht.elems_in_bucket (bucket);
}

/*
    Изменение размера таблицы
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_map_base<Key,T,HashFn,EqualKey,Allocator>::resize (size_type size)
{
  ht.resize (size);
}

/*
    Удаление
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_map_base<Key,T,HashFn,EqualKey,Allocator>::erase (iterator position)
{
  ht.erase ((typename table_type::iterator&)position);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::erase (const key_type& x)
{
  return ht.erase (x);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_map_base<Key,T,HashFn,EqualKey,Allocator>::erase (iterator first,iterator last)
{
  ht.erase ((typename table_type::iterator&)first,(typename table_type::iterator&)last);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_map_base<Key,T,HashFn,EqualKey,Allocator>::clear ()
{
  ht.clear ();  
}

/*
    Поиск
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::iterator
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::find (const key_type& x)
{
  return ht.find (x);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_iterator
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::find (const key_type& x) const
{
  return ht.find (x);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::size_type 
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::count (const key_type& x) const
{
  return ht.count (x);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::iterator,
            typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::iterator>
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::equal_range (const key_type& x) 
{
  return ht.equal_range (x);  
}
 
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_iterator,
            typename hash_map_base<Key,T,HashFn,EqualKey,Allocator>::const_iterator>
hash_map_base<Key,T,HashFn,EqualKey,Allocator>::equal_range (const key_type& x) const
{
  return ht.equal_range (x);  
}

/*
    Операции сравнения
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator == (const hash_map_base& x) const
{
  return ht == x.ht;
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator < (const hash_map_base& x) const
{
  return ht < x.ht;
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator != (const hash_map_base& x) const
{
  return !(x == *this);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator > (const hash_map_base& x) const
{
  return x < *this;
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator <= (const hash_map_base& x) const
{
  return !(x < *this);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline bool hash_map_base<Key,T,HashFn,EqualKey,Allocator>::operator >= (const hash_map_base& x) const
{
  return !(*this < x);
}

/*
    Множество с уникальными ключами
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline hash_map<Key,T,HashFn,EqualKey,Allocator>::hash_map (const allocator_type& allocator)
    : base (DEFAULT_TABLE_SIZE,hasher (),key_equal (),allocator)  
{ }     

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline hash_map<Key,T,HashFn,EqualKey,Allocator>::hash_map
 (size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : base (table_size,hash,eql,allocator)
{ }

template <class Key,class T,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline hash_map<Key,T,HashFn,EqualKey,Allocator>::hash_map
 (Iter                  first,
  Iter                  last,
  size_type             table_size,
  const hasher&         hash,
  const key_equal&      eql,
  const allocator_type& allocator)
    : base (table_size,hash,eql,allocator)
{ 
  insert (first,last);
}

//Операции доступа
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline T& hash_map<Key,T,HashFn,EqualKey,Allocator>::operator [] (const key_type& key)
{
  return ht.find_or_insert (value_type (key,T ())).second;
}

//Вставка
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_map<Key,T,HashFn,EqualKey,Allocator>::iterator,bool> 
hash_map<Key,T,HashFn,EqualKey,Allocator>::insert (const value_type& x)
{ 
  pair<typename table_type::iterator,bool> p = ht.insert_unique (x);
  
  return pair<iterator,bool> (p.first,p.second);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_map<Key,T,HashFn,EqualKey,Allocator>::iterator,bool> 
hash_map<Key,T,HashFn,EqualKey,Allocator>::insert_pair (const key_type& key,const T& x)
{ 
  return insert (value_type (key,x));
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline void hash_map<Key,T,HashFn,EqualKey,Allocator>::insert (Iter first,Iter last)
{
  ht.insert_unique (first,last);
}

//Обмен
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_map<Key,T,HashFn,EqualKey,Allocator>::swap (hash_map& s)
{
  ht.swap (s.ht);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void swap (hash_map<Key,T,HashFn,EqualKey,Allocator>& a,hash_map<Key,T,HashFn,EqualKey,Allocator>& b)
{
  a.swap (b);
}

/*
    Множество с неуникальными ключами
*/

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline hash_multimap<Key,T,HashFn,EqualKey,Allocator>::hash_multimap (const allocator_type& allocator)
  : base (DEFAULT_TABLE_SIZE,hasher (),key_equal (),allocator)  
{ }     

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline hash_multimap<Key,T,HashFn,EqualKey,Allocator>::hash_multimap
 (size_type             table_size,
  const hasher&         hash,
  const key_equal&      eql,
  const allocator_type& allocator)
    : base (table_size,hash,eql,allocator)
{ }

template <class Key,class T,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline hash_multimap<Key,T,HashFn,EqualKey,Allocator>::hash_multimap
 (Iter                  first,
  Iter                  last,
  size_type             table_size,
  const hasher&         hash,
  const key_equal&      eql,
  const allocator_type& allocator)
    : base (table_size,hash,eql,allocator)
{ 
  insert (first,last);
}

//Вставка
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_multimap<Key,T,HashFn,EqualKey,Allocator>::iterator 
hash_multimap<Key,T,HashFn,EqualKey,Allocator>::insert (const value_type& x)
{
  return ht.insert_equal (x);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline typename hash_multimap<Key,T,HashFn,EqualKey,Allocator>::iterator 
hash_multimap<Key,T,HashFn,EqualKey,Allocator>::insert_pair (const key_type& key,const T& x)
{
  return insert (value_type (key,x));
}
  
template <class Key,class T,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline void hash_multimap<Key,T,HashFn,EqualKey,Allocator>::insert (Iter first,Iter last)
{
  ht.insert_equal (first,last);
}

//Обмен
template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void hash_multimap<Key,T,HashFn,EqualKey,Allocator>::swap (hash_multimap& s)
{
  ht.swap (s.ht);
}

template <class Key,class T,class HashFn,class EqualKey,class Allocator>
inline void swap 
 (hash_multimap<Key,T,HashFn,EqualKey,Allocator>& a,
  hash_multimap<Key,T,HashFn,EqualKey,Allocator>& b)
{
  a.swap (b);
}
