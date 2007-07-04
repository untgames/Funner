/*
    Конструкторы
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline hash_set_base<Value,HashFn,EqualKey,Allocator>::hash_set_base 
 (size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : ht (table_size,hash,eql,allocator)
{ }

/*
    Аллокатор
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::allocator_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::get_allocator () const
{
  return ht.get_allocator ();
}

/*
    Функции общего назначения
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::size () const
{
  return ht.size ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::max_size () const
{
  return ht.max_size ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::empty () const
{
  return ht.empty ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::hasher 
hash_set_base<Value,HashFn,EqualKey,Allocator>::hash_func () const
{
  return ht.hash_func ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::key_equal
hash_set_base<Value,HashFn,EqualKey,Allocator>::key_eq () const
{
  return ht.key_eq ();
}


/*
    Итераторы
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::iterator 
hash_set_base<Value,HashFn,EqualKey,Allocator>::begin () const
{
  return ht.begin ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::iterator 
hash_set_base<Value,HashFn,EqualKey,Allocator>::end () const
{
  return ht.end ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::reverse_iterator 
hash_set_base<Value,HashFn,EqualKey,Allocator>::rbegin () const
{
  return ht.rbegin ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::reverse_iterator 
hash_set_base<Value,HashFn,EqualKey,Allocator>::rend () const
{
  return ht.rend ();
}

/*
    Информация о пакетах
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::bucket_count () const
{
  return ht.bucket_count ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::max_bucket_count () const
{
  return ht.max_bucket_count ();
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::elems_in_bucket (size_type bucket) const
{
  return ht.elems_in_bucket (bucket);
}

/*
    Изменение размера таблицы
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_set_base<Value,HashFn,EqualKey,Allocator>::resize (size_type size)
{
  ht.resize (size);
}

/*
    Удаление
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_set_base<Value,HashFn,EqualKey,Allocator>::erase (iterator position)
{
  ht.erase ((typename table_type::iterator&)position);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::erase (const key_type& x)
{
  return ht.erase (x);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_set_base<Value,HashFn,EqualKey,Allocator>::erase (iterator first,iterator last)
{
  ht.erase ((typename table_type::iterator&)first,(typename table_type::iterator&)last);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_set_base<Value,HashFn,EqualKey,Allocator>::clear ()
{
  ht.clear ();  
}

/*
    Поиск
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::iterator
hash_set_base<Value,HashFn,EqualKey,Allocator>::find (const key_type& x) const
{
  return ht.find (x);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_set_base<Value,HashFn,EqualKey,Allocator>::size_type 
hash_set_base<Value,HashFn,EqualKey,Allocator>::count (const key_type& x) const
{
  return ht.count (x);
}
 
template <class Value,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_set_base<Value,HashFn,EqualKey,Allocator>::iterator,
            typename hash_set_base<Value,HashFn,EqualKey,Allocator>::iterator>
hash_set_base<Value,HashFn,EqualKey,Allocator>::equal_range (const key_type& x) const
{
  return ht.equal_range (x);  
}

/*
    Операции сравнения
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator == (const hash_set_base& x) const
{
  return ht == x.ht;
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator < (const hash_set_base& x) const
{
  return ht < x.ht;
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator != (const hash_set_base& x) const
{
  return !(x == *this);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator > (const hash_set_base& x) const
{
  return x < *this;
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator <= (const hash_set_base& x) const
{
  return !(x < *this);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline bool hash_set_base<Value,HashFn,EqualKey,Allocator>::operator >= (const hash_set_base& x) const
{
  return !(*this < x);
}

/*
    Множество с уникальными ключами
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline hash_set<Value,HashFn,EqualKey,Allocator>::hash_set (const allocator_type& allocator)
    : base (DEFAULT_TABLE_SIZE,hasher (),key_equal (),allocator)
{ }     

template <class Value,class HashFn,class EqualKey,class Allocator>
inline hash_set<Value,HashFn,EqualKey,Allocator>::hash_set
 (size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : base (table_size,hash,eql,allocator)
{ }

template <class Value,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline hash_set<Value,HashFn,EqualKey,Allocator>::hash_set
 (Iter                   first,
  Iter                   last,
  size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : base (table_size,hash,eql,allocator)
{ 
  insert (first,last);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline pair<typename hash_set<Value,HashFn,EqualKey,Allocator>::iterator,bool> 
hash_set<Value,HashFn,EqualKey,Allocator>::insert (const value_type& x)
{ 
  pair<typename table_type::iterator,bool> p = ht.insert_unique (x);
  
  return pair<iterator,bool> (p.first,p.second);
}

template <class Value,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline void hash_set<Value,HashFn,EqualKey,Allocator>::insert (Iter first,Iter last)
{
  ht.insert_unique (first,last);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_set<Value,HashFn,EqualKey,Allocator>::swap (hash_set& s)
{
  ht.swap (s.ht);  
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void swap (hash_set<Value,HashFn,EqualKey,Allocator>& a,hash_set<Value,HashFn,EqualKey,Allocator>& b)
{
  a.swap (b);
}

/*
    Множество с неуникальными ключами
*/

template <class Value,class HashFn,class EqualKey,class Allocator>
inline hash_multiset<Value,HashFn,EqualKey,Allocator>::hash_multiset (const allocator_type& allocator)
  : base (DEFAULT_TABLE_SIZE,hasher (),key_equal (),allocator)
{ }     

template <class Value,class HashFn,class EqualKey,class Allocator>
inline hash_multiset<Value,HashFn,EqualKey,Allocator>::hash_multiset
 (size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : base (table_size,hash,eql,allocator)
{ }

template <class Value,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline hash_multiset<Value,HashFn,EqualKey,Allocator>::hash_multiset
 (Iter                   first,
  Iter                   last,
  size_type              table_size,
  const hasher&          hash,
  const key_equal&       eql,
  const allocator_type&  allocator)
    : base (table_size,hash,eql,allocator)
{ 
  insert (first,last);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline typename hash_multiset<Value,HashFn,EqualKey,Allocator>::iterator 
hash_multiset<Value,HashFn,EqualKey,Allocator>::insert (const value_type& x)
{
  return ht.insert_equal (x);
}
  
template <class Value,class HashFn,class EqualKey,class Allocator> template <class Iter> 
inline void hash_multiset<Value,HashFn,EqualKey,Allocator>::insert (Iter first,Iter last)
{
  ht.insert_equal (first,last);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void hash_multiset<Value,HashFn,EqualKey,Allocator>::swap (hash_multiset& s)
{
  ht.swap (s.ht);
}

template <class Value,class HashFn,class EqualKey,class Allocator>
inline void swap 
 (hash_multiset<Value,HashFn,EqualKey,Allocator>& a,
  hash_multiset<Value,HashFn,EqualKey,Allocator>& b)
{
  a.swap (b);
}
