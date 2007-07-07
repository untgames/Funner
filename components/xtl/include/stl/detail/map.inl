/*
    map_base::value_compare
*/

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::value_compare::operator () (const value_type& a,const value_type& b) const
{
  return comp (a.first,b.first);
}

template <class Key,class T,class Compare,class Allocator>            
inline map_base<Key,T,Compare,Allocator>::value_compare::value_compare (Compare cmp)
  : comp (cmp)
{  }  

/*
    Конструкторы
*/

template <class Key,class T,class Compare,class Allocator>
inline map_base<Key,T,Compare,Allocator>::map_base (const allocator_type& _allocator)
  : tree (_allocator)
{ }

template <class Key,class T,class Compare,class Allocator>
inline map_base<Key,T,Compare,Allocator>::map_base (const key_compare& cmp,const allocator_type& _allocator)
  : tree (cmp,_allocator)
{ }

/*
    Аллокатор
*/

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::allocator_type map_base<Key,T,Compare,Allocator>::get_allocator () const
{
  return tree.get_allocator ();
}

/*
    Функции общего назначения
*/

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::size_type map_base<Key,T,Compare,Allocator>::size () const
{
  return tree.size ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::size_type map_base<Key,T,Compare,Allocator>::max_size () const
{
  return tree.max_size ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::key_compare map_base<Key,T,Compare,Allocator>::key_comp () const
{
  return tree.key_comp ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::value_compare map_base<Key,T,Compare,Allocator>::value_comp () const
{
  return value_compare (key_comp ());
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::empty () const
{
  return tree.empty ();
}

/*
    Итераторы
*/

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::iterator map_base<Key,T,Compare,Allocator>::begin ()
{
  return tree.begin ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_iterator map_base<Key,T,Compare,Allocator>::begin () const
{
  return tree.begin ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::iterator map_base<Key,T,Compare,Allocator>::end ()
{
  return tree.end ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_iterator map_base<Key,T,Compare,Allocator>::end () const
{
  return tree.end ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::reverse_iterator map_base<Key,T,Compare,Allocator>::rbegin ()
{
  return tree.rbegin ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_reverse_iterator map_base<Key,T,Compare,Allocator>::rbegin () const
{
  return tree.rbegin ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::reverse_iterator map_base<Key,T,Compare,Allocator>::rend ()
{
  return tree.rend ();
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_reverse_iterator map_base<Key,T,Compare,Allocator>::rend () const
{
  return tree.rend ();
}

/*
    Удаление
*/

template <class Key,class T,class Compare,class Allocator>
inline void map_base<Key,T,Compare,Allocator>::erase (iterator position)
{
  tree.erase (position);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::size_type map_base<Key,T,Compare,Allocator>::erase (const key_type& x)
{
  return tree.erase (x);
}

template <class Key,class T,class Compare,class Allocator>
inline void map_base<Key,T,Compare,Allocator>::erase (iterator first,iterator last)
{
  tree.erase (first,last);
}

template <class Key,class T,class Compare,class Allocator>
inline void map_base<Key,T,Compare,Allocator>::clear ()
{
  tree.clear ();  
}

/*
    Поиск
*/

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::iterator 
map_base<Key,T,Compare,Allocator>::find (const key_type& x)
{
  return tree.find (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_iterator 
map_base<Key,T,Compare,Allocator>::find (const key_type& x) const
{
  return tree.find (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::size_type 
map_base<Key,T,Compare,Allocator>::count (const key_type& x) const
{
  return tree.count (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::iterator 
map_base<Key,T,Compare,Allocator>::lower_bound (const key_type& x)
{
  return tree.lower_bound (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_iterator 
map_base<Key,T,Compare,Allocator>::lower_bound (const key_type& x) const
{
  return tree.lower_bound (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::iterator 
map_base<Key,T,Compare,Allocator>::upper_bound (const key_type& x)
{
  return tree.upper_bound (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map_base<Key,T,Compare,Allocator>::const_iterator 
map_base<Key,T,Compare,Allocator>::upper_bound (const key_type& x) const
{
  return tree.upper_bound (x);
}
  
template <class Key,class T,class Compare,class Allocator>
inline pair<typename map_base<Key,T,Compare,Allocator>::iterator,
            typename map_base<Key,T,Compare,Allocator>::iterator>
map_base<Key,T,Compare,Allocator>::equal_range (const key_type& x)
{
  return tree.equal_range (x);  
}

template <class Key,class T,class Compare,class Allocator>
inline pair<typename map_base<Key,T,Compare,Allocator>::const_iterator,
            typename map_base<Key,T,Compare,Allocator>::const_iterator> 
map_base<Key,T,Compare,Allocator>::equal_range (const key_type& x) const
{
  return tree.equal_range (x);  
}

/*
    Операции сравнения
*/

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator == (const map_base& x) const
{
  return tree == x.tree;
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator < (const map_base& x) const
{
  return tree < x.tree;
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator != (const map_base& x) const
{
  return !(x == *this);
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator > (const map_base& x) const
{
  return x < *this;
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator <= (const map_base& x) const
{
  return !(x < *this);
}

template <class Key,class T,class Compare,class Allocator>
inline bool map_base<Key,T,Compare,Allocator>::operator >= (const map_base& x) const
{
  return !(*this < x);
}

/*
    Множество с уникальными ключами
*/

template <class Key,class T,class Compare,class Allocator>
inline map<Key,T,Compare,Allocator>::map (const allocator_type& _allocator)
  : base (_allocator)
{ }

template <class Key,class T,class Compare,class Allocator>
inline map<Key,T,Compare,Allocator>::map (const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)
{ }

template <class Key,class T,class Compare,class Allocator> template <class Iter>
inline map<Key,T,Compare,Allocator>::map (Iter first,Iter last,const allocator_type& _allocator)
  : base (_allocator)
{ 
  tree.insert_unique (first,last);
}

template <class Key,class T,class Compare,class Allocator> template <class Iter>
inline map<Key,T,Compare,Allocator>::map (Iter first,Iter last,const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)  
{ 
  tree.insert_unique (first,last);
}

//Операции доступа
template <class Key,class T,class Compare,class Allocator>
inline T& map<Key,T,Compare,Allocator>::operator [] (const key_type& k) 
{
  iterator i = lower_bound (k); //i->first is greater than or equivalent to k
    
  if (i == tree.end () || key_comp () (k,i->first))
    i = insert (i,value_type (k,T ()));
    
  return i->second;
}

//Вставка
template <class Key,class T,class Compare,class Allocator>
inline pair<typename map<Key,T,Compare,Allocator>::iterator,bool> 
map<Key,T,Compare,Allocator>::insert (const value_type& x)
{
  pair<typename tree_type::iterator,bool> p = tree.insert_unique (x);
  
  return pair<iterator,bool> (p.first,p.second);
}

template <class Key,class T,class Compare,class Allocator>
inline pair<typename map<Key,T,Compare,Allocator>::iterator,bool> 
map<Key,T,Compare,Allocator>::insert_pair (const key_type& key,const T& x)
{
  return insert (value_type (key,x));
}

template <class Key,class T,class Compare,class Allocator>
inline typename map<Key,T,Compare,Allocator>::iterator 
map<Key,T,Compare,Allocator>::insert (iterator position,const value_type& x)
{
  return tree.insert_unique (position,x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename map<Key,T,Compare,Allocator>::iterator map<Key,T,Compare,Allocator>::insert_pair 
 (iterator        position,
  const key_type& key,
  const T&        x)
{
  return insert (position,value_type (key,x));  
}  
  
template <class Key,class T,class Compare,class Allocator> template <class Iter> 
inline void map<Key,T,Compare,Allocator>::insert (Iter first,Iter last)
{
  tree.insert_unique (first,last);
}

//Обмен
template <class Key,class T,class Compare,class Allocator>
inline void map<Key,T,Compare,Allocator>::swap (map& s)
{
  tree.swap (s.tree);  
}

template <class Key,class T,class Compare,class Allocator>
inline void swap (map<Key,T,Compare,Allocator>& a,map<Key,T,Compare,Allocator>& b)
{
  a.swap (b);
}

/*
    Множество с неуникальными ключами
*/

template <class Key,class T,class Compare,class Allocator>
inline multimap<Key,T,Compare,Allocator>::multimap (const allocator_type& _allocator)
  : base (_allocator)
{ }

template <class Key,class T,class Compare,class Allocator>
inline multimap<Key,T,Compare,Allocator>::multimap (const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)
{ }

template <class Key,class T,class Compare,class Allocator> template <class Iter>
inline multimap<Key,T,Compare,Allocator>::multimap (Iter first,Iter last,const allocator_type& _allocator)
  : base (_allocator)
{ 
  tree.insert_equal (first,last);
}

template <class Key,class T,class Compare,class Allocator> template <class Iter>
inline multimap<Key,T,Compare,Allocator>::multimap (Iter first,Iter last,const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)  
{ 
  tree.insert_equal (first,last);
}

template <class Key,class T,class Compare,class Allocator>
inline typename multimap<Key,T,Compare,Allocator>::iterator 
multimap<Key,T,Compare,Allocator>::insert (const value_type& x)
{
  return tree.insert_equal (x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename multimap<Key,T,Compare,Allocator>::iterator 
multimap<Key,T,Compare,Allocator>::insert_pair (const key_type& key,const T& x)
{
  return insert (value_type (key,x));
}

template <class Key,class T,class Compare,class Allocator>
inline typename multimap<Key,T,Compare,Allocator>::iterator 
multimap<Key,T,Compare,Allocator>::insert (iterator position,const value_type& x)
{
  return tree.insert_equal (position,x);
}

template <class Key,class T,class Compare,class Allocator>
inline typename multimap<Key,T,Compare,Allocator>::iterator multimap<Key,T,Compare,Allocator>::insert_pair
 (iterator        position,
  const key_type& key,
  const T&        x)
{
  return insert (position,value_type (key,x));  
}  
  
template <class Key,class T,class Compare,class Allocator> template <class Iter> 
inline void multimap<Key,T,Compare,Allocator>::insert (Iter first,Iter last)
{
  tree.insert_equal (first,last);
}

//Обмен
template <class Key,class T,class Compare,class Allocator>
inline void multimap<Key,T,Compare,Allocator>::swap (multimap& s)
{
  tree.swap (s.tree);
}

template <class Key,class T,class Compare,class Allocator>
inline void swap (multimap<Key,T,Compare,Allocator>& a,multimap<Key,T,Compare,Allocator>& b)
{
  a.swap (b);
}
