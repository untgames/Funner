/*
    Итератор (base)
*/

template <class T,class Allocator>
inline deque_iterator_base<T,Allocator>::deque_iterator_base ()
  : owner (NULL), offset (0)
  { }
  
template <class T,class Allocator>  
inline deque_iterator_base<T,Allocator>::deque_iterator_base (deque<T,Allocator>* _owner,size_t _offset)
  : owner (_owner), offset (_offset)
  { }
  
template <class T,class Allocator>
inline ptrdiff_t deque_iterator_base<T,Allocator>::operator - (const deque_iterator_base& i) const
{
  return i.offset <= offset ? offset - i.offset : -(ptrdiff_t)(i.offset-offset);
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator == (const deque_iterator_base& i) const
{
  return owner == i.owner && i.offset == offset;  
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator != (const deque_iterator_base& i) const
{
  return !(*this == i);
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator < (const deque_iterator_base& i) const
{
  return owner == i.owner ? offset < i.offset : false;
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator > (const deque_iterator_base& i) const
{
  return i < *this;
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator <= (const deque_iterator_base& i) const
{
  return !(i < *this);
}

template <class T,class Allocator>
inline bool deque_iterator_base<T,Allocator>::operator >= (const deque_iterator_base& i) const
{
  return !(*this < i);
}   

/*
    Итератор
*/
  
template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>::deque_iterator (deque<T,Allocator>* owner,size_t offset)
  : base (owner,offset)
  { }

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>::deque_iterator (const base& i)    
  : base (i)
  { }

template <class T,class Ref,class Ptr,class Allocator>
inline typename deque_iterator<T,Ref,Ptr,Allocator>::reference 
deque_iterator<T,Ref,Ptr,Allocator>::operator * () const
{
  return owner->get (offset);
}

template <class T,class Ref,class Ptr,class Allocator>
inline typename deque_iterator<T,Ref,Ptr,Allocator>::pointer 
deque_iterator<T,Ref,Ptr,Allocator>::operator -> () const
{
  return &owner->get (offset);
}

template <class T,class Ref,class Ptr,class Allocator>
inline typename deque_iterator<T,Ref,Ptr,Allocator>::reference 
deque_iterator<T,Ref,Ptr,Allocator>::operator [] (difference_type i) const
{
  return owner->get (offset+i);
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>& deque_iterator<T,Ref,Ptr,Allocator>::operator ++ ()
{
  offset++;
  return *this;
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>& deque_iterator<T,Ref,Ptr,Allocator>::operator -- ()
{
  offset--;
  return *this;
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator> deque_iterator<T,Ref,Ptr,Allocator>::operator ++ (int)
{
  return deque_iterator (owner,offset++);
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator> deque_iterator<T,Ref,Ptr,Allocator>::operator -- (int)
{
  return deque_iterator (owner,offset--);
}
   
template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>& deque_iterator<T,Ref,Ptr,Allocator>::operator += (difference_type n)
{
  offset += n;
  return *this;
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator>& deque_iterator<T,Ref,Ptr,Allocator>::operator -= (difference_type n)
{
  offset -= n;
  return *this;
}
    
template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator> deque_iterator<T,Ref,Ptr,Allocator>::operator + (difference_type n)
{
  return deque_iterator (owner,offset+n);
}

template <class T,class Ref,class Ptr,class Allocator>
inline deque_iterator<T,Ref,Ptr,Allocator> deque_iterator<T,Ref,Ptr,Allocator>::operator - (difference_type n)
{
  return deque_iterator (owner,offset-n);
}

template <class T,class Ref,class Ptr,class Allocator>
inline typename deque_iterator<T,Ref,Ptr,Allocator>::difference_type 
deque_iterator<T,Ref,Ptr,Allocator>::operator - (const base& i) const
{
  return base::operator - (i);
}

/*
    Конструкторы
*/

template <class T,class Allocator>
inline deque<T,Allocator>::deque (const allocator_type& _allocator)
  : allocator_type (_allocator)
  , map (0)
  , map_size (0)
  , seq_offset (0)
  , seq_size (0)
  , map_allocator (_allocator)
  {  }

template <class T,class Allocator>
deque<T,Allocator>::deque (size_type count,const value_type& value,const allocator_type& _allocator)
  : allocator_type (_allocator)
  , map (0)
  , map_size (0)
  , seq_offset (0)
  , seq_size (0)
  , map_allocator (_allocator)
{
  try
  {
    insert (begin (),count,value);
  }
  catch (...)
  {
    clear ();
    throw;
  }    
}

template <class T,class Allocator> template <class Iter> 
deque<T,Allocator>::deque (Iter first,Iter last,const allocator_type& _allocator)
  : allocator_type (_allocator)
  , map_allocator (_allocator)
{
  map        = 0;
  map_size   = 0; 
  seq_offset = 0;
  seq_size   = 0;
  
  try
  {
    insert (begin (),first,last);
  }
  catch (...)
  {
    clear ();
    throw;
  }    
}

template <class T,class Allocator>
deque<T,Allocator>::deque (const deque& x)
  : allocator_type (static_cast<const allocator_type&> (x))
  , map (0)
  , map_size (0)
  , seq_offset (0)
  , seq_size (0)
  , map_allocator (x.map_allocator)  
{
  try
  {
    insert (begin (),x.begin (),x.end ());  
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T,class Allocator>
inline deque<T,Allocator>::~deque ()
{
  clear ();
}

/*
    Аллокатор
*/           

template <class T,class Allocator>
inline typename deque<T,Allocator>::allocator_type deque<T,Allocator>::get_allocator () const
{
  return static_cast<const allocator_type&> (*this);
}

/*
    Возбуждение исключений
*/

template <class T,class Allocator>
inline void deque<T,Allocator>::raise_out_of_range (const char* func_name,iterator pos) const
{
  stl_raise_out_of_range (*this,pos.owner==this?pos-begin():-1);
}

template <class T,class Allocator>
inline void deque<T,Allocator>::raise_out_of_range (const char* func_name,iterator first,iterator last) const
{
  if (first.owner == this && last.owner == this) 
    stl_raise_out_of_range (*this,first,last);
  else
    stl_raise_out_of_range (*this,-1,-1);
}

/*
    Функции общего назначения
*/

template <class T,class Allocator>
inline bool deque<T,Allocator>::is_valid (iterator i) const
{
  return i.owner == this && i <= end ();
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::is_valid (iterator first,iterator last) const
{
  return first.owner == this && last.owner == this && first <= last && is_valid (last);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::size_type deque<T,Allocator>::size () const
{
  return seq_size;
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::size_type deque<T,Allocator>::max_size () const
{
  return allocator_type::max_size ();
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::empty () const
{
  return seq_size == 0;
}

/*
    Получение итераторов
*/

template <class T,class Allocator>
inline typename deque<T,Allocator>::iterator deque<T,Allocator>::begin ()
{
  return iterator (this,seq_offset);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_iterator deque<T,Allocator>::begin () const
{
  return const_iterator (const_cast<deque<T,Allocator>*>(this),seq_offset);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::iterator deque<T,Allocator>::end ()
{
  return iterator (this,seq_offset+seq_size);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_iterator deque<T,Allocator>::end () const
{
  return const_iterator (const_cast<deque<T,Allocator>*>(this),seq_offset+seq_size);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reverse_iterator deque<T,Allocator>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reverse_iterator deque<T,Allocator>::rbegin () const
{
  return reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reverse_iterator deque<T,Allocator>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reverse_iterator deque<T,Allocator>::rend () const
{
  return reverse_iterator (begin ());
}

/*
    Доступ
*/

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reference deque<T,Allocator>::get (size_type index) const
{
  size_type block = index/BLOCK_SIZE, offset = index%BLOCK_SIZE;

  if (block >= map_size)            
    block -= map_size;     
    
  return map [block][offset];
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reference deque<T,Allocator>::get (size_type index)
{
  size_type block = index/BLOCK_SIZE, offset = index%BLOCK_SIZE;
  
  if (block >= map_size)
    block -= map_size;        

  return map [block][offset];
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reference deque<T,Allocator>::at (size_type index)
{
  if (index >= size ())
    stl_raise_out_of_range (*this,index);
    
  return get (index);    
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reference deque<T,Allocator>::at (size_type index) const
{
  if (index >= size ())
    stl_raise_out_of_range (*this,index);  
    
  return get (index);    
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reference deque<T,Allocator>::operator [] (size_type i)
{
  return get (i+seq_offset);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reference deque<T,Allocator>::operator [] (size_type i) const
{
  return get (i+seq_offset);
}
   
template <class T,class Allocator>
inline typename deque<T,Allocator>::reference deque<T,Allocator>::front ()
{
  return map [seq_offset/BLOCK_SIZE][seq_offset%BLOCK_SIZE];
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reference deque<T,Allocator>::front () const
{
  return map [seq_offset/BLOCK_SIZE][seq_offset%BLOCK_SIZE];
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::reference deque<T,Allocator>::back ()
{
  return get (seq_offset+seq_size-1);
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::const_reference deque<T,Allocator>::back () const
{
  return get (seq_offset+seq_size-1);
}

/*
    Изменение размера дека    
*/

template <class T,class Allocator>
inline typename deque<T,Allocator>::size_type deque<T,Allocator>::next_size () const
{        
  size_type map_max_size = min (map_allocator.max_size (),max_size () / BLOCK_SIZE), //макс. размер карты
            new_map_size = map_size + 1; 
  
  if (!(map_max_size-map_size)) // возможен ли рост карты?
    return 0;    
    
  if (map_max_size - map_size >= map_size) //возможно ли двухкратное увеличение размера карты?
    new_map_size = max (2*map_size,new_map_size);
    
  if (new_map_size < MIN_MAP_SIZE && MIN_MAP_SIZE < map_max_size) //карта слишком мала?
    new_map_size = MIN_MAP_SIZE;
  
  return min (new_map_size,max_size ()/BLOCK_SIZE);
}

//увеличивает потенциальный размер дека на inc_size элементов
template <class T,class Allocator>
void deque<T,Allocator>::grow ()
{   
  size_type new_map_size = next_size (),
            map_inc_size = new_map_size - map_size;

  if (!new_map_size)
    stl_raise_length_error (*this,size ()+1);
    
  size_type first_block = seq_offset / BLOCK_SIZE;  
     
  pointer *new_map   = map_allocator.allocate (new_map_size,map),
          *new_first = new_map + first_block;
          
  /*
    Поскольку дек является закольцованным, возможны лишь 2 ситуации:
      1) ......12345... - данные расположены в одном диапозоне       
      2) 45.........123 - данные расположены в двух диапозонах                 

    Следующий код производит перемещение указателей на блоки в новую карту с корректировкой расположения
  */

  new_first = uninitialized_copy (map+first_block,map+map_size,new_first); 
    
  if (first_block <= map_inc_size)
  { 
    new_first = uninitialized_copy (map,map+first_block,new_first);
    
    uninitialized_fill_n (new_first,map_inc_size-first_block,(pointer)0);
    uninitialized_fill_n (new_map,first_block,(pointer)0);
  }
  else
  { 
    uninitialized_copy   (map,map+map_inc_size,new_first);
    new_first = uninitialized_copy (map+map_inc_size,map+first_block,new_map);
    uninitialized_fill_n (new_first,map_inc_size,(pointer)0);
  }
  
  if (map)
    map_allocator.deallocate (map,map_size);

  map       = new_map;
  map_size += map_inc_size;
}

template <class T,class Allocator>
inline void deque<T,Allocator>::resize (size_type new_size,const value_type& val)
{
  if      (seq_size < new_size) insert (end (),new_size-seq_size,val);
  else if (new_size < seq_size) erase  (begin ()+new_size,end ());
}

template <class T,class Allocator>
inline void deque<T,Allocator>::resize (size_type new_size)
{
  resize (new_size,T ());
}

/*
    Добавление / удаление элементов
*/

template <class T,class Allocator>
inline void deque<T,Allocator>::push_back (const value_type& val)
{
  if (!((seq_offset+seq_size) % BLOCK_SIZE) && map_size <= (seq_offset+seq_size+BLOCK_SIZE) / BLOCK_SIZE)
    grow ();
    
  size_type new_offset = seq_offset+seq_size, 
            block      = new_offset/BLOCK_SIZE;
  
  if (map_size <= block)
    block -= map_size;
    
  if (!map [block])
    map [block] = allocator_type::allocate (BLOCK_SIZE);
    
  stl::construct (map [block]+new_offset%BLOCK_SIZE,val);
  
  ++seq_size;    
}

template <class T,class Allocator>
inline void deque<T,Allocator>::push_front (const value_type& val)
{
  if (!(seq_offset % BLOCK_SIZE) && map_size <= (seq_size+BLOCK_SIZE) / BLOCK_SIZE)
    grow ();
      
  size_type new_offset = seq_offset ? seq_offset : map_size * BLOCK_SIZE, 
            block      = --new_offset / BLOCK_SIZE;
              
  if (!map [block])
    map [block] = allocator_type::allocate (BLOCK_SIZE);    
    
  stl::construct (map [block]+new_offset%BLOCK_SIZE,val);  

  seq_offset = new_offset;

  ++seq_size;  
}

template <class T,class Allocator>
inline void deque<T,Allocator>::push_back ()
{
  push_back (T ());
}

template <class T,class Allocator>
inline void deque<T,Allocator>::push_front ()
{
  push_front (T ());
}

template <class T,class Allocator>
void deque<T,Allocator>::pop_back ()
{
  if (empty ())
    return ;

  size_type new_offset = seq_size+seq_offset-1, 
            block      = new_offset / BLOCK_SIZE;

  if (map_size <= block)
    block -= map_size;

  stl::destroy (map [block]+new_offset%BLOCK_SIZE);   

  if (!--seq_size)
    seq_offset = 0;
}

template <class T,class Allocator>
void deque<T,Allocator>::pop_front ()
{
  if (empty ())
    return ;

  size_type block = seq_offset / BLOCK_SIZE;
  
  stl::destroy (map [block]+seq_offset%BLOCK_SIZE);

  if (map_size * BLOCK_SIZE <= ++seq_offset) seq_offset = 0;
  if (!--seq_size) seq_offset = 0;
}

/*
    Присваивание
*/

template <class T,class Allocator>
inline void deque<T,Allocator>::assign (size_type count,const value_type& val)
{
  T tmp = val;

  erase  (begin (),end ());
  insert (begin (),count,tmp);
}

template <class T,class Allocator> template <class Iter>
inline void deque<T,Allocator>::assign (Iter first,Iter last)
{
  _assign (first,last,iterator_category<Iter> ());  
}

template <class T,class Allocator> template <class Iter>
inline void deque<T,Allocator>::_assign (Iter count,Iter value,int_iterator_tag)
{
  assign ((size_type)count,(value_type)value);
}

template <class T,class Allocator> template <class Iter>
inline void deque<T,Allocator>::_assign (Iter first,Iter last,input_iterator_tag)
{
  erase  (begin (),end ());
  insert (begin (),first,last);
}

template <class T,class Allocator>
deque<T,Allocator>& deque<T,Allocator>::operator = (const deque& x)
{
  if (&x == this)
    return *this;   

  if (!x.size ())
  {
    clear ();
    return *this;
  }

  if (x.size () > size ())
  {
    const_iterator mid = x.begin () + size ();
    
    copy   (x.begin (),mid,begin ());
    insert (end (),mid,x.end ());  
  }
  else erase (copy (x.begin (),x.end (),begin ()),end ());    

  return *this;
}

/*
    Вставка элементов
*/

template <class T,class Allocator>
typename deque<T,Allocator>::iterator deque<T,Allocator>::insert (iterator position,const value_type& x)
{
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);
    
  if (position == begin ())
  {
    push_front (x);
    return begin ();
  }
  
  if (position == end ())
  {
    push_back (x);
    return end () - 1;
  }
    
  iterator mid;
  size_type offset = position - begin ();
  
  T t = x; //если x уже в последовательности

  if (offset < size () / 2) //ближе к началу
  { 
    push_front (front ());
    mid = begin () + offset;
    copy (begin ()+2,mid+1,begin ()+1);
  }
  else //ближе к концу
  {
    push_back (back ());
    mid = begin () + offset;
    copy_backward (mid,end ()-2,end ()-1);
  }

  *mid = t;
    
  return mid;
}

template <class T,class Allocator>
inline typename deque<T,Allocator>::iterator deque<T,Allocator>::insert (iterator position)
{
  return insert (position,T ());
}

template <class T,class Allocator>
void deque<T,Allocator>::insert (iterator position,size_type count,const value_type& x)
{
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);

  iterator mid;
  size_type num, offset = position - begin (), rem = size () - offset, old_size = size ();
  
  if (offset < rem) //ближе к началу
  { 
    try
    {    
      if (offset < count) //insert longer than prefix
      {      
        for (num=count-offset;num;--num) push_front (x); // push excess values 
        for (num=offset;num;--num)       push_front (begin()[count-1]);  // push prefix
                
        mid = begin() + count;
        fill (mid,mid+offset,x);  // fill in rest of values
      }
      else // insert not longer than prefix
      { 
        for (num=count;num;--num) push_front (begin()[count-1]);  // push part of prefix

        mid = begin () + count;
        
        T t = x;  // in case x is in sequence
        
        copy (mid+count,mid+offset,mid); // copy rest of prefix
        fill (begin ()+offset,mid+offset,t);  // fill in values
      }      
    }
    catch (...)
    {
      for (;old_size<size ();)  pop_front ();  // restore old size, at least
      throw;
    }
  }
  else //ближе к концу
  {
    try
    {
      if (rem < count) // insert longer than suffix
      { 
        for (num=count-rem;num;--num) push_back (x);  // push excess values
        for (num=0;num<rem;++num)     push_back (begin()[offset+num]);  // push suffix

        mid = begin () + offset;
        
        fill (mid,mid+rem,x);  // fill in rest of values
      }
      else // insert not longer than prefix
      { 
        for (num=0;num<count;++num) push_back (begin()[offset+rem-count+num]);  // push part of prefix

        mid = begin () + offset;
        T t = x;  // in case x is in sequence
        
        copy_backward (mid,mid+rem-count,mid+rem); // copy rest of prefix
        fill (mid,mid+count,t);  // fill in values
      }
    }
    catch (...)
    {
      while (old_size<seq_size) pop_back(); // restore old size, at least
      throw;
    }
  }
}

template <class T,class Allocator> template <class Iter> 
inline void deque<T,Allocator>::insert (iterator pos,Iter first,Iter last)
{
  _insert (pos,first,last,iterator_category<Iter> ());
}

template <class T,class Allocator> template <class Iter> 
inline void deque<T,Allocator>::_insert (iterator pos,Iter count,Iter value,int_iterator_tag)
{
  insert (pos,(size_type)count,(value_type)value);
}

template <class T,class Allocator> template <class Iter> 
inline void deque<T,Allocator>::_insert (iterator pos,Iter first,Iter last,input_iterator_tag)
{
  size_type offset = pos-begin ();
  
  for (;first!=last;++first,++offset)
    insert (begin ()+offset,*first);
}

template <class T,class Allocator> template <class Iter> 
void deque<T,Allocator>::_insert (iterator pos,Iter first,Iter last,bidirectional_iterator_tag)
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);

  size_type offset   = pos - begin (),
            rem      = seq_size - offset,
            old_size = seq_size,
            count    = distance (first,last);

  if (offset < rem) // closer to front
  { 
    try
    {
      if (offset < count) // insert longer than prefix
      { 
        Iter mid = first;
        
        advance (mid,count-offset);

        for (Iter next=mid;first!=next;)     push_front (*--next); // push head of insert        
        for (size_type num=offset;num;--num) push_front (begin ()[count-1]);  // push prefix
          
        copy (mid,last,begin ()+count);  // copy rest of insert
      }
      else // insert not longer than prefix
      { 
        for (size_type num=count;num;--num) push_front (begin () [count-1]);  // push part of prefix

        iterator mid = begin ()+count;
        
        copy (mid+count,mid+offset,mid);  // copy rest of prefix
        copy (first,last,begin ()+offset);  // copy in insert
      }
    }    
    catch (...)
    {
      while (old_size<seq_size) pop_front ();
      throw;
    }
  }
  else // closer to back
  { 
    try
    {
      if (rem < count) // insert longer than suffix
      { 
        Iter mid = first;
        
        advance (mid,rem);

        for (Iter next=mid;next!=last;++next) push_back (*next);  // push tail of insert
        for (size_type num=0;num<rem;++num)   push_back (begin ()[offset+num]);  // push suffix

        copy (first,mid,begin ()+offset);  // copy rest of insert
      }
      else // insert not longer than suffix
      { 
        for (size_type num=0;num<count;++num) push_back (begin ()[offset+rem-count+num]);  // push part of suffix

        iterator mid = begin ()+offset;
        
        copy_backward (mid,mid+rem-count,mid+rem); // copy rest of prefix
        copy          (first,last,mid); // copy in values
      }
    }  
    catch (...)
    {
      while (old_size<seq_size) pop_back ();
      throw;
    }
  }
}

/*
    Удаление
*/

template <class T,class Allocator>
inline typename deque<T,Allocator>::iterator deque<T,Allocator>::erase (iterator pos)
{
  return erase (pos,pos+1);  
}

template <class T,class Allocator>
typename deque<T,Allocator>::iterator deque<T,Allocator>::erase (iterator first,iterator last)
{   
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);

  size_type offset = first-begin (), count = last-first;

  if (offset < (size_type)(end () - last)) // closer to front
  { 
    copy_backward (begin (),first,last);  // copy over hole
    
    for (;count;--count) pop_front ();  // pop copied elements
  }
  else // closer to back
  { 
    copy (last,end (),first);  // copy over hole
    
    for (;count;--count) pop_back (); // pop copied elements
  }

  return begin ()+offset;
}

template <class T,class Allocator>
void deque<T,Allocator>::clear ()
{
  while (!empty ()) pop_back ();
  
  for (size_type cnt=map_size;cnt;)
    if (map [--cnt])
      allocator_type::deallocate (map [cnt],BLOCK_SIZE);

  if (map)
    map_allocator.deallocate (map,map_size);
    
  map_size = 0;
  map = 0;
}

/*
    Обмен содержимого двух деков
*/

template <class T,class Allocator>
inline void deque<T,Allocator>::swap (deque& x)
{
  if (static_cast<allocator_type&> (*this) == static_cast<allocator_type&> (x) && map_allocator == x.map_allocator)
  {
    stl::swap (map,x.map);
    stl::swap (map_size,x.map_size);
    stl::swap (seq_offset,x.seq_offset);
    stl::swap (seq_size,x.seq_size);  
  }
  else
  {
    deque tmp = *this;
    *this     = x;
    x         = tmp;
  }
}

template <class T,class Allocator> 
inline void swap (deque<T,Allocator>& a,deque<T,Allocator>& b)
{
  a.swap (b);
}

/*
    Сравнение
*/

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator == (const deque& x) const
{
  return size () == x.size () && equal (begin (),end (),x.begin ());
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator < (const deque& x) const
{
  return lexicographical_compare (begin (),end (),x.begin (),x.end ());
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator != (const deque& x) const
{
  return !(x == *this);
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator > (const deque& x) const
{
  return x < *this;
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator <= (const deque& x) const
{
  return !(x < *this);
}

template <class T,class Allocator>
inline bool deque<T,Allocator>::operator >= (const deque& x) const
{
  return !(*this < x);
}
