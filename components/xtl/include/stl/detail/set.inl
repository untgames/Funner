/*
    Конструкторы
*/

template <class Key,class Compare,class Allocator>
inline set_base<Key,Compare,Allocator>::set_base (const allocator_type& _allocator)
  : tree (_allocator)
{ }

template <class Key,class Compare,class Allocator>
inline set_base<Key,Compare,Allocator>::set_base (const key_compare& cmp,const allocator_type& _allocator)
  : tree (cmp,_allocator)
{ }

/*
    Аллокатор
*/

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::allocator_type set_base<Key,Compare,Allocator>::get_allocator () const
{
  return tree.get_allocator ();
}

/*
    Функции общего назначения
*/

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::size_type set_base<Key,Compare,Allocator>::size () const
{
  return tree.size ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::size_type set_base<Key,Compare,Allocator>::max_size () const
{
  return tree.max_size ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::key_compare set_base<Key,Compare,Allocator>::key_comp () const
{
  return tree.key_comp ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::value_compare set_base<Key,Compare,Allocator>::value_comp () const
{
  return tree.key_comp ();
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::empty () const
{
  return tree.empty ();
}

/*
    Итераторы
*/

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::iterator set_base<Key,Compare,Allocator>::begin () const
{
  return tree.begin ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::iterator set_base<Key,Compare,Allocator>::end () const
{
  return tree.end ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::reverse_iterator set_base<Key,Compare,Allocator>::rbegin () const
{
  return tree.rbegin ();
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::reverse_iterator set_base<Key,Compare,Allocator>::rend () const
{
  return tree.rend ();
}

/*
    Удаление
*/

template <class Key,class Compare,class Allocator>
inline void set_base<Key,Compare,Allocator>::erase (iterator position)
{
  tree.erase (position.get_unqualified_iterator ());
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::size_type set_base<Key,Compare,Allocator>::erase (const key_type& x)
{
  return tree.erase (x);
}

template <class Key,class Compare,class Allocator>
inline void set_base<Key,Compare,Allocator>::erase (iterator first,iterator last)
{
  tree.erase (first.get_unqualified_iterator (),last.get_unqualified_iterator ());
}

template <class Key,class Compare,class Allocator>
inline void set_base<Key,Compare,Allocator>::clear ()
{
  tree.clear ();  
}

/*
    Поиск
*/

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::iterator set_base<Key,Compare,Allocator>::find (const key_type& x) const
{
  return tree.find (x);
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::size_type set_base<Key,Compare,Allocator>::count (const key_type& x) const
{
  return tree.count (x);
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::iterator set_base<Key,Compare,Allocator>::lower_bound (const key_type& x) const
{
  return tree.lower_bound (x);
}

template <class Key,class Compare,class Allocator>
inline typename set_base<Key,Compare,Allocator>::iterator set_base<Key,Compare,Allocator>::upper_bound (const key_type& x) const
{
  return tree.upper_bound (x);
}
  
template <class Key,class Compare,class Allocator>
inline pair<typename set_base<Key,Compare,Allocator>::iterator,typename set_base<Key,Compare,Allocator>::iterator> 
set_base<Key,Compare,Allocator>::equal_range (const key_type& x) const
{
  return tree.equal_range (x);  
}

/*
    Операции сравнения
*/

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator == (const set_base& x) const
{
  return tree == x.tree;
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator < (const set_base& x) const
{
  return tree < x.tree;
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator != (const set_base& x) const
{
  return !(x == *this);
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator > (const set_base& x) const
{
  return x < *this;
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator <= (const set_base& x) const
{
  return !(x < *this);
}

template <class Key,class Compare,class Allocator>
inline bool set_base<Key,Compare,Allocator>::operator >= (const set_base& x) const
{
  return !(*this < x);
}

/*
    Множество с уникальными ключами
*/

template <class Key,class Compare,class Allocator>
inline set<Key,Compare,Allocator>::set (const allocator_type& _allocator)
  : base (_allocator)
{ }

template <class Key,class Compare,class Allocator>
inline set<Key,Compare,Allocator>::set (const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)
{ }

template <class Key,class Compare,class Allocator> template <class Iter>
inline set<Key,Compare,Allocator>::set (Iter first,Iter last,const allocator_type& _allocator)
  : base (_allocator)
{ 
  tree.insert_unique (first,last);
}

template <class Key,class Compare,class Allocator> template <class Iter>
inline set<Key,Compare,Allocator>::set (Iter first,Iter last,const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)
{ 
  tree.insert_unique (first,last);
}

template <class Key,class Compare,class Allocator>
inline pair<typename set<Key,Compare,Allocator>::iterator,bool> set<Key,Compare,Allocator>::insert (const value_type& x)
{
  pair<typename tree_type::iterator,bool> p = tree.insert_unique (x);
  
  return pair<iterator,bool> (p.first.get_unqualified_iterator (),p.second);
}

template <class Key,class Compare,class Allocator>
inline typename set<Key,Compare,Allocator>::iterator set<Key,Compare,Allocator>::insert (iterator position,const value_type& x)
{
  return tree.insert_unique (position.get_unqualified_iterator (),x);
}
  
template <class Key,class Compare,class Allocator> template <class Iter> 
inline void set<Key,Compare,Allocator>::insert (Iter first,Iter last)
{
  tree.insert_unique (first,last);
}

//Обмен содердимого двух множеств
template <class Key,class Compare,class Allocator>
inline void set<Key,Compare,Allocator>::swap (set& s)
{
  tree.swap (s.tree);  
}

template <class Key,class Compare,class Allocator>
inline void swap (set<Key,Compare,Allocator>& a,set<Key,Compare,Allocator>& b)
{
  a.swap (b);
}

/*
    Множество с неуникальными ключами
*/

template <class Key,class Compare,class Allocator>
inline multiset<Key,Compare,Allocator>::multiset (const allocator_type& _allocator)
  : base (_allocator)
{ }

template <class Key,class Compare,class Allocator>
inline multiset<Key,Compare,Allocator>::multiset (const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)
{ }

template <class Key,class Compare,class Allocator> template <class Iter>
inline multiset<Key,Compare,Allocator>::multiset (Iter first,Iter last,const allocator_type& _allocator)
  : base (_allocator)
{ 
  tree.insert_equal (first,last);
}

template <class Key,class Compare,class Allocator> template <class Iter>
inline multiset<Key,Compare,Allocator>::multiset (Iter first,Iter last,const key_compare& cmp,const allocator_type& _allocator)
  : base (cmp,_allocator)  
{ 
  tree.insert_equal (first,last);
}

template <class Key,class Compare,class Allocator>
inline typename multiset<Key,Compare,Allocator>::iterator multiset<Key,Compare,Allocator>::insert (const value_type& x)
{
  return tree.insert_equal (x);
}

template <class Key,class Compare,class Allocator>
inline typename multiset<Key,Compare,Allocator>::iterator multiset<Key,Compare,Allocator>::insert (iterator position,const value_type& x)
{
  return tree.insert_equal (position.get_unqualified_iterator (),x);
}
  
template <class Key,class Compare,class Allocator> template <class Iter> 
inline void multiset<Key,Compare,Allocator>::insert (Iter first,Iter last)
{
  tree.insert_equal (first,last);
}

//Обмен содердимого двух множеств
template <class Key,class Compare,class Allocator>
inline void multiset<Key,Compare,Allocator>::swap (multiset& s)
{
  tree.swap (s.tree);
}

template <class Key,class Compare,class Allocator>
inline void swap (multiset<Key,Compare,Allocator>& a,multiset<Key,Compare,Allocator>& b)
{
  a.swap (b);
}
