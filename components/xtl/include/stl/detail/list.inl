/*
    Базовый узел
*/

inline list_node_base::list_node_base ()
{
  prev = next = this;
}

inline void list_node_base::transfer (list_node_base* position,list_node_base* first,list_node_base* last) 
{
  if (position == last)
    return;

  // Remove [first, last) from its old position.
  last->prev->next     = position;
  first->prev->next    = last;
  position->prev->next = first; 

  // Splice [first, last) into its new position.
  list_node_base* tmp = position->prev;
  position->prev      = last->prev;
  last->prev          = first->prev; 
  first->prev         = tmp;
}

/*
    Итератор
*/

template <class T>
inline list_iterator<T>::list_iterator ()
  : node (0)
  { }

template <class T>
inline list_iterator<T>::list_iterator (Node* _node)
  : node (_node)
 { }  
 
template <class T>
inline list_iterator<T>::list_iterator (const iterator& i)
  : node (((list_iterator&)i).node)
 { }   
 
template <class T>
inline list_iterator<T>& list_iterator<T>::operator = (const iterator& i)
{ 
  node = ((list_iterator&)i).node;
  return *this;
}    
    
template <class T> 
inline typename list_iterator<T>::reference list_iterator<T>::operator * () const
{
  return ((typename list<T>::Node*)node)->data;
}

template <class T> 
inline typename list_iterator<T>::pointer list_iterator<T>::operator -> () const
{
  return &((typename list<T>::Node*)node)->data;
}

template <class T> 
inline list_iterator<T>& list_iterator<T>::operator ++ ()
{
  return node = node->next, *this;
}

template <class T> 
inline list_iterator<T>& list_iterator<T>::operator -- ()
{
  return node = node->prev, *this;
}

template <class T> 
inline list_iterator<T> list_iterator<T>::operator ++ (int)
{
  list_iterator t = *this;
  
  node = node->next;
  
  return t;   
}

template <class T> 
inline list_iterator<T> list_iterator<T>::operator -- (int)
{
  list_iterator t = *this;
  
  node = node->prev;
  
  return t;
}
    
template <class T> 
inline bool list_iterator<T>::operator == (const list_iterator& i) const
{
  return node == i.node;
}

template <class T> 
inline bool list_iterator<T>::operator != (const list_iterator& i) const
{
  return !(*this == i);
}  

/*
    Получение unqualified итератора
*/

template <class T>
inline typename list_iterator<T>::iterator list_iterator<T>::get_unqualified_iterator () const
{
  return iterator (node);
}

/*
    Конструкторы
*/

template <class T,class Allocator>
inline list<T,Allocator>::list (const allocator_type& _allocator)
  : allocator (_allocator)
{ }

template <class T,class Allocator>
inline list<T,Allocator>::list (size_type count,const allocator_type& _allocator)
  : allocator (_allocator)
{
  insert (begin (),count,T ());
}

template <class T,class Allocator>
inline list<T,Allocator>::list (size_type count,const value_type& value,const allocator_type& _allocator)
  : allocator (_allocator)
{
  insert (begin (),count,value);
}

template <class T,class Allocator>
inline list<T,Allocator>::list (const list& x)
  : allocator (x.allocator)
{
  insert (begin (),x.begin (),x.end ());  
}

template <class T,class Allocator> template <class Iter> 
inline list<T,Allocator>::list (Iter first,Iter last,const allocator_type& _allocator)
  : allocator (_allocator)
{
  insert (begin (),first,last);
}

template <class T,class Allocator>
inline list<T,Allocator>::~list ()
{
  clear ();
}

/*
    Аллокатор
*/

template <class T,class Allocator>
inline typename list<T,Allocator>::allocator_type list<T,Allocator>::get_allocator () const
{
  return allocator;
}

/*
    Функции общего назначения
*/

template <class T,class Allocator>
inline bool list<T,Allocator>::empty () const
{
  return base.next == &base;
}

template <class T,class Allocator>
inline typename list<T,Allocator>::size_type list<T,Allocator>::size () const
{
  return distance (begin (),end ());
}

template <class T,class Allocator>
inline typename list<T,Allocator>::size_type list<T,Allocator>::max_size () const
{
  return allocator.max_size ();
}

/*
    Получение итераторов
*/

template <class T,class Allocator> inline typename list<T,Allocator>::iterator list<T,Allocator>::begin ()
{
  return base.next;
}

template <class T,class Allocator>
inline typename list<T,Allocator>::iterator list<T,Allocator>::end ()
{
  return &base;
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_iterator list<T,Allocator>::begin () const
{
  return base.next;
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_iterator list<T,Allocator>::end () const
{
  return (Node*)&base;
}

template <class T,class Allocator>
inline typename list<T,Allocator>::reverse_iterator list<T,Allocator>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_reverse_iterator list<T,Allocator>::rbegin () const
{
  return const_reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename list<T,Allocator>::reverse_iterator list<T,Allocator>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_reverse_iterator list<T,Allocator>::rend () const
{
  return const_reverse_iterator (begin ());
}

/*
    Операции доступа
*/

template <class T,class Allocator>
inline typename list<T,Allocator>::reference list<T,Allocator>::front ()
{
  return *begin ();
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_reference list<T,Allocator>::front () const
{
  return *begin ();
}

template <class T,class Allocator>
inline typename list<T,Allocator>::reference list<T,Allocator>::back ()
{
  return *(--end());
}

template <class T,class Allocator>
inline typename list<T,Allocator>::const_reference list<T,Allocator>::back () const
{
  return *(--end());
}

/*
    Создание / удаление узлов
*/

template <class T,class Allocator> 
typename list<T,Allocator>::Node* list<T,Allocator>::create_node (const value_type& x)
{
  Node* p = allocator.allocate (1);
  
  try
  {
    construct (&p->data,x);
  }
  catch (...)
  {
    allocator.deallocate (p,1);
    throw;
  }

  return p;  
}

template <class T,class Allocator> 
typename list<T,Allocator>::Node* list<T,Allocator>::create_node ()
{
  Node* p = allocator.allocate (1);
  
  try
  {
    construct (&p->data);
  }
  catch (...)
  {
    allocator.deallocate (p,1);
    throw;
  }

  return p;    
}

template <class T,class Allocator> 
inline void list<T,Allocator>::delete_node (Node* p)
{
  destroy (&p->data);
  allocator.deallocate (p,1);
}

/*
    Присваивание
*/

template <class T,class Allocator>
inline list<T,Allocator>& list<T,Allocator>::operator = (const list& x)
{
  if (this != &x) 
    assign (x.begin (),x.end ());

  return *this;
}

template <class T,class Allocator>
void list<T,Allocator>::assign (size_type count,const value_type& val)
{
  iterator i = begin ();
  
  for (;i!=end () && count;++i,count--) *i = val;
    
  if (count) insert (end (),count,val);
  else       erase  (i,end ());  
}

template <class T,class Allocator> template <class Iter> 
inline void list<T,Allocator>::_assign (Iter count,Iter value,int_iterator_tag)
{
  assign ((size_type)count,(value_type)value);
}
        
template <class T,class Allocator> template <class Iter> 
void list<T,Allocator>::_assign (Iter first2,Iter last2,input_iterator_tag)
{
  iterator first1 = begin (), last1 = end ();

  for (;first1 != last1 && first2 != last2;++first1,++first2) *first1 = *first2;
  
  if (first2 == last2) erase  (first1,last1);
  else                 insert (last1,first2,last2);
}

template <class T,class Allocator> template <class Iter> 
inline void list<T,Allocator>::assign (Iter first,Iter last)
{
  _assign (first,last,iterator_category<Iter> ());
}

/*
    Вставка
*/    

template <class T,class Allocator> 
inline typename list<T,Allocator>::iterator list<T,Allocator>::insert (iterator position,const value_type& x)
{
  Node* node = create_node (x);
  
  node->next = position.node;
  node->prev = position.node->prev;
  
  return position.node->prev = position.node->prev->next = node;
}

template <class T,class Allocator> 
inline typename list<T,Allocator>::iterator list<T,Allocator>::insert (iterator position)
{
  return insert (position,T ());
}

template <class T,class Allocator> 
void list<T,Allocator>::insert (iterator position,size_type count,const value_type& x)
{
  while (count--) insert (position,x);
}

template <class T,class Allocator> template <class Iter> 
inline void list<T,Allocator>::_insert (iterator position,Iter count,Iter value,int_iterator_tag)
{
  insert (position,(size_type)count,(value_type)value);
}

template <class T,class Allocator> template <class Iter> 
void list<T,Allocator>::_insert (iterator position,Iter first,Iter last,input_iterator_tag)
{
  for (;first!=last;++first) insert (position,*first);
}

template <class T,class Allocator> template <class Iter> 
inline void list<T,Allocator>::insert (iterator position,Iter first,Iter last)
{
  _insert (position,first,last,iterator_category<Iter> ());
}

/*
    Удаление
*/

template <class T,class Allocator> 
inline typename list<T,Allocator>::iterator list<T,Allocator>::erase (iterator position)
{
  Node * volatile next_node = (Node*)position.node->next, * volatile prev_node = (Node*)position.node->prev;
  
  prev_node->next = next_node;
  next_node->prev = prev_node;
  
  delete_node ((Node*)position.node);
  
  return next_node; 
}

template <class T,class Allocator> 
typename list<T,Allocator>::iterator list<T,Allocator>::erase (iterator first,iterator last)
{
  while (first!=last) first=erase (first);
  return last;
}

template <class T,class Allocator> 
void list<T,Allocator>::clear ()
{
  for (Node* i=(Node*)base.next;i!=&base;) 
  {
    Node* tmp = i;
    
    i = (Node*)i->next;
    
    delete_node (tmp);
  }
  
  base.prev = base.next = &base;
}

/*
    Добавление / удаление элемента
*/

template <class T,class Allocator> 
inline void list<T,Allocator>::push_front (const value_type& x)
{
  insert (begin (),x);
}

template <class T,class Allocator> 
inline void list<T,Allocator>::push_front ()
{
  insert (begin ());
}

template <class T,class Allocator> 
inline void list<T,Allocator>::push_back (const value_type& x)
{
  insert (end (),x);
}

template <class T,class Allocator> 
inline void list<T,Allocator>::push_back ()
{
  insert (end ());
}

template <class T,class Allocator> 
inline void list<T,Allocator>::pop_front ()
{
  erase (begin ());
}

template <class T,class Allocator> 
inline void list<T,Allocator>::pop_back ()
{
  iterator tmp = end ();
  
  erase (--tmp);
}

/*
    Изменение размера списка
*/

template <class T,class Allocator>
void list<T,Allocator>::resize (size_type new_size,const value_type& x)
{
  iterator  i   = begin ();  
  size_type len = 0;
  
  for (;i != end () && len < new_size;++i,++len);
  
  if (len == new_size) erase  (i,end ());
  else                 insert (end (),new_size-len,x);  
}

template <class T,class Allocator>
inline void list<T,Allocator>::resize (size_type new_size)
{
  resize (new_size,T ());
}

/*
    Обмен содержимого двух списков
*/

template <class T,class Allocator>
inline void list<T,Allocator>::swap (list<T,Allocator>& x)
{ 
  if (allocator == x.allocator)
  {
    list_node_base *prev = base.prev, *x_prev = x.base.prev;

    prev->next   = base.next->prev   = &x.base;
    x_prev->next = x.base.next->prev = &base;
    
    stl::swap (base.prev,x.base.prev);
    stl::swap (base.next,x.base.next);  
  }
  else
  {
    iterator pos = begin ();
    
    splice   (pos,x);
    x.splice (x.begin (),*this,pos,end ());    
  }
}

template <class T,class Allocator>
inline void swap (list<T,Allocator>& a,list<T,Allocator>& b) 
{
  a.swap (b);
}

template <class T,class Allocator>
inline void list<T,Allocator>::transfer (iterator position,iterator first,iterator last)
{
  list_node_base::transfer (position.node,first.node,last.node);
}

template <class T,class Allocator>
inline void list<T,Allocator>::splice (iterator dst_pos,list& x)
{  
  if (x.empty ())
    return;
    
  if (x.allocator == allocator)
  {
    transfer (dst_pos,x.begin (),x.end ());    
  }  
  else
  {
    insert  (dst_pos,x.begin (),x.end ());
    x.clear ();
  }
}

template <class T,class Allocator>
inline void list<T,Allocator>::splice (iterator dst_pos,list& src_list,iterator src_pos)
{
  iterator j = src_pos;
  ++j;
  
  if (dst_pos == src_pos || dst_pos == j) 
    return;

  if (allocator == src_list.allocator)
  {
    transfer (dst_pos,src_pos,j);  
  }    
  else
  {
    insert (dst_pos,*src_pos);
    src_list.erase (src_pos);
  }  
}

template <class T,class Allocator>
inline void list<T,Allocator>::splice (iterator dst_pos,list& src_list,iterator src_first,iterator src_last)
{
  if (src_first == src_last) 
    return;
    
  if (allocator == src_list.allocator)
  {
    transfer (dst_pos,src_first,src_last);
  }
  else
  {
    insert (dst_pos,src_first,src_last);
    src_list.erase (src_first,src_last);
  }  
}

/*
    Алгоритмы
*/

template <class T,class Allocator>
void list<T,Allocator>::reverse ()
{
  if (base.next->next == &base) //nothing to reverse (0 or 1 element)
    return; 

  iterator first = begin ();
  
  for (++first;first!=end ();) 
  {
    iterator old = first;
    ++first;
    transfer (begin (),old,first);
  }
}

template <class T,class Allocator> template <class Predicate> 
void list<T,Allocator>::remove_if (Predicate pred)
{
  for (iterator first=begin (),last=end ();first!=last;) 
  {
    iterator next = first;
    ++next;
    
    if (pred (*first)) 
      erase (first);
      
    first = next;
  }
}

template <class T,class Allocator>
void list<T,Allocator>::remove (const value_type& x)
{
  for (iterator first=begin (),last=end ();first!=last;) 
  {
    iterator next = first;
    ++next;
    
    if (*first == x) 
      erase (first);
      
    first = next;
  }
}

template <class T,class Allocator> template <class Predicate> 
void list<T,Allocator>::unique (Predicate pred)
{
  if (empty ())
    return;
    
  for (iterator first=begin (),last=end (),next=first;++next!=last;next=first)
    if (pred (*first,*next)) erase (next);
    else                     first = next;
}

template <class T,class Allocator>
inline void list<T,Allocator>::unique ()
{
  return unique (equal_to<T> ());
}

template <class T,class Allocator> template <class Compare>
void list<T,Allocator>::merge (list& x,Compare less)
{
  iterator first1 = begin (), last1 = end (), first2 = x.begin (), last2 = x.end (); 
      
  if (allocator == x.allocator)
  {
    while (first1!=last1 && first2!=last2)
    { 
      if (less (*first2,*first1))
      {
        iterator next = first2;

        transfer (first1,first2,++next);
        
        first2 = next;
      }
      else ++first1;    
    }  
        
    if (first2 != last2)
      transfer (last1,first2,last2);  
  }
  else
  {
    while (first1!=last1 && first2!=last2)
    { 
      if (less (*first2,*first1))
      {
        iterator next = first2;
        ++next;

        insert (first1,*first2);
        x.erase (first2);
        
        first2 = next;
      }
      else ++first1;    
    }  
        
    if (first2 != last2)
    {
      insert  (last1,first2,last2);
      x.erase (first2,last2);
    }
  }
}

template <class T,class Allocator>
inline void list<T,Allocator>::merge (list& x)
{
  merge (x,less<T> ());
}
       
template <class T,class Allocator> template <class Compare>
void list<T,Allocator>::sort (Compare less)
{
  if (base.next->next == &base) //nothing to sort (0 or 1 element)
    return; 

  list<T,Allocator> carry, counter [64];
  
  int fill = 0;
  
  while (!empty ()) 
  {
    carry.splice (carry.begin (),*this,begin ());
            
    int i = 0;
    
    while (i<fill && !counter [i].empty ()) 
    {
      counter [i].merge (carry,less);
      carry.swap (counter [i++]);      
    }    
    
    carry.swap (counter [i]);    
    
    if (i == fill) 
      ++fill;
  }   

  for (int i=1;i<fill;i++) counter [i].merge (counter [i-1],less);
  
  swap (counter [fill-1]);
}

template <class T,class Allocator>
inline void list<T,Allocator>::sort ()
{
  sort (less<T> ());
}

/*
    Сравнение
*/

template <class T,class Allocator>
bool list<T,Allocator>::operator == (const list& x) const
{
  const_iterator end1 = end (), end2 = x.end (), i1 = begin (), i2 = x.begin ();
                 
  for (;i1!=end1 && i2!=end2 && *i1==*i2;++i1,++i2);
  
  return i1==end1 && i2==end2;
}

template <class T,class Allocator>
inline bool list<T,Allocator>::operator < (const list& x) const
{
  return lexicographical_compare (begin (),end (),x.begin (),x.end ());  
}

template <class T,class Allocator>
inline bool list<T,Allocator>::operator != (const list& x) const
{
  return !(x == *this);
}

template <class T,class Allocator>
inline bool list<T,Allocator>::operator > (const list& x) const
{
  return x < *this;
}

template <class T,class Allocator>
inline bool list<T,Allocator>::operator <= (const list& x) const
{
  return !(x < *this);
}

template <class T,class Allocator>
inline bool list<T,Allocator>::operator >= (const list& x) const
{
  return !(*this < x);
}
