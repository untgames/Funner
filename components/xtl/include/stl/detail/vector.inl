/*
    Конструкторы
*/

template <class T,class Allocator>
inline vector<T,Allocator>::vector (const allocator_type& _allocator)
  : allocator (_allocator)
{
  start = finish = end_of_storage = NULL;
}

template <class T,class Allocator>
inline vector<T,Allocator>::vector (size_type count,const allocator_type& _allocator)
  : allocator (_allocator)
{
  start = finish = end_of_storage = NULL;
  resize (count);
}

template <class T,class Allocator>
inline vector<T,Allocator>::vector (size_type count,const value_type& value,const allocator_type& _allocator)
  : allocator (_allocator)
{
  start = finish = end_of_storage = NULL;
  resize (count,value);
}

template <class T,class Allocator>
inline vector<T,Allocator>::vector (const vector& x)
  : allocator (x.allocator)
{
  start = finish = end_of_storage = NULL;

  if (x.size ())
    realloc (x.size (),x.begin (),x.end ());
}

template <class T,class Allocator> template <class Iter> 
inline vector<T,Allocator>::vector (Iter first,Iter last,const allocator_type& _allocator)
  : allocator (_allocator)
{
  start = finish = end_of_storage = NULL;  
  insert (finish,first,last);
}

template <class T,class Allocator>
inline vector<T,Allocator>::~vector ()
{
  destroy (start,finish);
  allocator.deallocate (start,end_of_storage-start);
}               

/*
    Аллокатор
*/

template <class T,class Allocator>
inline typename vector<T,Allocator>::allocator_type vector<T,Allocator>::get_allocator () const
{
  return allocator;
}

/*
    Операции общего назначения
*/
    
template <class T,class Allocator>
inline typename vector<T,Allocator>::size_type vector<T,Allocator>::size () const
{
  return size_type (end () - begin ());
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::size_type vector<T,Allocator>::max_size () const
{
  return allocator.max_size (); 
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::size_type vector<T,Allocator>::capacity () const
{
  return size_type (end_of_storage - begin ());
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::empty () const
{
  return begin () == end ();
}

//следующий допустимый размер для массива не меньший inc_size
template <class T,class Allocator>
inline typename vector<T,Allocator>::size_type vector<T,Allocator>::next_size (size_type inc_size) const
{
  return size () + (inc_size < size () && max_size () - size () >= size () ? size () : inc_size);  
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::is_grow_possible (size_type inc_size) const
{
  return max_size () - size () >= inc_size;
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::is_valid (iterator i) const
{
  return i >= start && i <= finish; //i in [begin ();end ()]
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::is_valid (iterator first,iterator last) const
{
  return first <= last && first >= start && last <= finish;
}

/*
    Получение итераторов
*/

template <class T,class Allocator> inline typename vector<T,Allocator>::iterator vector<T,Allocator>::begin ()
{
  return start;
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::iterator vector<T,Allocator>::end ()
{
  return finish;
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_iterator vector<T,Allocator>::begin () const
{
  return start;
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_iterator vector<T,Allocator>::end () const
{
  return finish;
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::reverse_iterator vector<T,Allocator>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::rbegin () const
{
  return const_reverse_iterator (end ());
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::reverse_iterator vector<T,Allocator>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::rend () const
{
  return const_reverse_iterator (begin ());
}

/*
    Операции доступа
*/   

template <class T,class Allocator>
inline typename vector<T,Allocator>::reference vector<T,Allocator>::operator [] (size_type i)
{
  return start [i];
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_reference vector<T,Allocator>::operator [] (size_type i) const
{
  return start [i];
}

template <class T,class Allocator>
typename vector<T,Allocator>::reference vector<T,Allocator>::at (size_type i)
{
  if (i >= size ())
    stl_raise_out_of_range (*this,i);

  return start [i];
}

template <class T,class Allocator>
typename vector<T,Allocator>::const_reference vector<T,Allocator>::at (size_type i) const
{
  if (i >= size ())
    stl_raise_out_of_range (*this,i);

  return start [i];
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::reference vector<T,Allocator>::front ()
{
  return *begin ();
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_reference vector<T,Allocator>::front () const
{
  return *begin ();
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::reference vector<T,Allocator>::back ()
{
  return *(end () - 1);
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::const_reference vector<T,Allocator>::back () const
{
  return *(end () - 1);
}

/*
    Присваивание
*/

template <class T,class Allocator> template <class Iter>
void vector<T,Allocator>::realloc (size_type count,Iter first,Iter last)
{
  iterator tmp = allocator.allocate (count,start), new_finish;
  
  try
  {
    new_finish = uninitialized_copy (first,last,tmp);
  }
  catch (...)
  {
    deallocate (tmp,count);
    throw;
  }
  
  destroy (start,finish);
  allocator.deallocate (start,end_of_storage-start);
  
  start          = tmp;
  finish         = new_finish;
  end_of_storage = tmp+count;
}

template <class T,class Allocator>
vector<T,Allocator>& vector<T,Allocator>::operator = (const vector& x)
{
  if (&x == this)
    return *this;
       
  const size_type xlen = x.size();    
  
  if (xlen > capacity ()) 
  {
    try
    {
      if (max_size () < xlen)
        stl_raise_length_error (*this,xlen);
    
      realloc (xlen,x.begin (),x.end ());    
    }
    catch (...)
    {
      clear ();
      throw;
    }
  }
  else if (size () >= xlen) 
  {
    destroy (copy (x.begin (),x.end (),start),finish);
  }
  else 
  {
    copy               (x.begin (),x.begin ()+size (),start);
    uninitialized_copy (x.begin ()+size (),x.end (),finish);
  }
  
  finish = start + xlen;
  
  return *this;
}

template <class T,class Allocator> template <class Iter> 
inline void vector<T,Allocator>::_assign (Iter count,Iter value,int_iterator_tag)
{
  assign ((size_type)count,(value_type)value);
}
        
template <class T,class Allocator> template <class Iter> 
void vector<T,Allocator>::_assign (Iter first,Iter last,input_iterator_tag)
{
  iterator cur = start;
    
  try
  {
    for (;first!=last && cur!=end ();++cur,++first) 
      *cur = *first; 
      
    if (first == last) erase  (cur,end ()); 
    else               insert (end (),first,last);   
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T,class Allocator> template <class Iter> 
void vector<T,Allocator>::_assign (Iter first,Iter last,forward_iterator_tag)
{
  try
  {
    size_type len = distance (first,last);
    
    if (max_size () < len)
      stl_raise_length_error (*this,len);

    if (len > capacity ()) 
    {
      realloc (len,first,last);
    }
    else if (size () >= len) 
    {
      destroy (copy (first,last,start),finish);
    }
    else 
    {
      Iter mid = first;
      
      advance (mid,size ());
      copy    (first,mid,start);

      finish = uninitialized_copy (mid,last,finish);    
    }
    
    finish = start+len;  
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T,class Allocator> template <class Iter> 
inline void vector<T,Allocator>::assign (Iter first,Iter last)
{
  _assign (first,last,iterator_category<Iter> ());
}

template <class T,class Allocator>
void vector<T,Allocator>::assign (size_type count,const value_type& val)
{
  try
  {
    if (max_size () < count)
      stl_raise_length_error (*this,count);

    if (count > capacity()) 
    {
      vector (count,val,get_allocator ()).swap (*this);
    }
    else if (count > size()) 
    {
      fill (begin (),end(),val);    
      finish = uninitialized_fill_n (finish,count-size (),val);    
    }
    else erase (fill_n (begin (),count,val),end ());  
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

/*
    Добавление / удаление элементов
*/                          

template <class T,class Allocator>
inline void vector<T,Allocator>::push_back (const value_type& x)
{
  if (finish != end_of_storage)  construct (finish++,x);
  else                           _insert (end (),x);
}

template <class T,class Allocator>
inline void vector<T,Allocator>::push_back ()
{
  if (finish != end_of_storage)  construct (finish++);
  else                           _insert (end (),T ());
}

template <class T,class Allocator>
inline void vector<T,Allocator>::pop_back ()
{
  destroy (--finish);
}

/*
    Вставка элементов
*/       

template <class T,class Allocator>
inline typename vector<T,Allocator>::iterator vector<T,Allocator>::insert (iterator position)
{
  return insert (position,T ());
}

template <class T,class Allocator>
void vector<T,Allocator>::_insert (iterator position,const value_type& value)
{
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);

  if (finish != end_of_storage) 
  {
    construct (finish,finish [-1]);

    ++finish;
    
    T tmp = value;
    
    copy_backward (position,finish-2,finish-1);
    
    *position = tmp;
  }
  else 
  {
    if (!is_grow_possible (1))
      stl_raise_length_error (*this,size ()+1);
  
    const volatile size_type len        = next_size (1);
    volatile iterator        new_start  = allocator.allocate (len,start), //обход предупреждения на gcc: 'new_start' might be used uninitialized in this function
                             new_finish = new_start;
             
    try 
    {
      new_finish = uninitialized_copy (start,position,new_start);
      
      construct (new_finish++,value);
            
      new_finish = uninitialized_copy (position,finish,new_finish);
    }
    catch (...)
    {
      destroy (new_start,new_finish);
      allocator.deallocate (new_start,len);
      throw;
    }
    
    destroy (begin (),end ());
    allocator.deallocate (start,end_of_storage-start);
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start+len;
  }
}

template <class T,class Allocator>
inline typename vector<T,Allocator>::iterator vector<T,Allocator>::insert 
 (iterator           position,
  const value_type&  value)
{       
  size_type count = position - begin ();
  
  if (finish != end_of_storage && position == end ()) construct (finish++,value);
  else                                                _insert   (position,value);
  
  return begin ()+count;
}

template <class T,class Allocator>
void vector<T,Allocator>::insert (iterator position,size_type count,const value_type& value)
{
  if (!count)
    return;
    
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);
    
  if (!is_grow_possible (count))
    stl_raise_length_error (*this,count+size ());

  if (size_type (end_of_storage-finish) >= count) 
  {
    T tmp = value;
    
    const size_type elems_after = finish-position;    
    iterator        old_finish  = finish;
    
    if (elems_after > count) 
    {
      uninitialized_copy (finish-count,finish,finish);
      
      finish += count;
      
      copy_backward (position,old_finish-count,old_finish);
      fill          (position,position+count,tmp);
    }
    else 
    {
      uninitialized_fill_n (finish,count-elems_after,tmp);
      
      finish += count-elems_after;
      
      uninitialized_copy (position,old_finish,finish);
      
      finish += elems_after;
      
      fill (position,old_finish,tmp);
    }
  }
  else 
  {
    const volatile size_type len        = next_size (count);
    volatile iterator        new_start  = allocator.allocate (len,start), new_finish = new_start;

    try
    {
      new_finish = uninitialized_copy   (start,position,new_start);
      new_finish = uninitialized_fill_n (new_finish,count,value);
      new_finish = uninitialized_copy   (position,finish,new_finish);
    }
    catch (...)
    {
      destroy (new_start,new_finish);
      allocator.deallocate (new_start,len);
      throw;
    }

    destroy (start,finish);
    allocator.deallocate (start,end_of_storage-start);
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start+len;
  }
}

template <class T,class Allocator> template <class Iter>
void vector<T,Allocator>::_insert_range (iterator pos,Iter first,Iter last,input_iterator_tag)
{
  for (;first!=last;++first,++pos) pos = insert (pos,*first);
}

template <class T,class Allocator> template <class Iter>
void vector<T,Allocator>::_insert_range (iterator position,Iter first,Iter last,forward_iterator_tag)
{
  if (first == last)
    return;    
    
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);

  size_type count = distance (first,last);
  
  if (!is_grow_possible (count))
    stl_raise_length_error (*this,size ()+count);

  if (size_type (end_of_storage-finish) >= count) 
  {
    const size_type elems_after = finish-position;
    iterator        old_finish  = finish;
    
    if (elems_after > count) 
    {
      uninitialized_copy (finish-count,finish,finish);
      
      finish += count;
      
      copy_backward (position,old_finish-count,old_finish);
      copy          (first,last,position);
    }
    else 
    {
      Iter mid = first;
      
      advance            (mid,elems_after);     
      uninitialized_copy (mid,last,finish);
      
      finish += count - elems_after;
      
      uninitialized_copy (position,old_finish,finish);
      
      finish += elems_after;
      
      copy (first,mid,position);
    }
  }
  else 
  {
    const size_type len        = next_size (count);
    iterator        new_start  = allocator.allocate (len,start),
                    new_finish = new_start;
                    
    try
    {
      new_finish = uninitialized_copy (start,position,new_start);
      new_finish = uninitialized_copy (first,last,new_finish);
      new_finish = uninitialized_copy (position,finish,new_finish);
    }
    catch (...)
    {
      destroy (new_start,new_finish);
      allocator.deallocate (new_start,len);
      throw;
    }

    destroy (start,finish);
    allocator.deallocate (start,end_of_storage-start);
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start+len;
  }
}

template <class T,class Allocator> template <class Iter>
inline void vector<T,Allocator>::_insert_range (iterator pos,Iter count,Iter value,int_iterator_tag)
{
  insert (pos,(size_type)count,(value_type)value);
}

template <class T,class Allocator> template <class Iter>
inline void vector<T,Allocator>::insert (iterator position,Iter first,Iter last)
{
  _insert_range (position,first,last,iterator_category<Iter>()); 
}

/*
    Удаление элементов
*/   

template <class T,class Allocator>
inline typename vector<T,Allocator>::iterator vector<T,Allocator>::erase (iterator i)
{
  if (!is_valid (i) || i == end ())
    stl_raise_out_of_range (*this,i);
   
  if (i + 1 != end ())
    copy (i + 1,finish,i);
    
  destroy (--finish);
  
  return i;
}

template <class T,class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::erase (iterator first,iterator last)
{
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);

  iterator i = copy (last,finish,first);
  
  destroy (i,finish);
  
  finish = finish - (last - first);
  
  return first;
}

template <class T,class Allocator>
inline void vector<T,Allocator>::clear ()
{
  erase (begin (),end ());
}

/*
    Изменение размера массива
*/ 

template <class T,class Allocator>
inline void vector<T,Allocator>::resize (size_type new_size,const value_type& x)
{
  if (new_size < size ()) erase  (begin ()+new_size,end ());
  else                    insert (end (),new_size-size (),x);
}

template <class T,class Allocator>
inline void vector<T,Allocator>::resize (size_type new_size)
{
  resize (new_size,T ());
}

template <class T,class Allocator>
void vector<T,Allocator>::reserve (size_type count)
{
  if (capacity () >= count)
    return;
    
  if (max_size () < count)
    stl_raise_length_error (*this,count);  
        
  realloc (count,start,finish);
}

/*
    Обмен содержимого двух массивов
*/

template <class T,class Allocator>
inline void vector<T,Allocator>::swap (vector& x)
{
  if (allocator == x.allocator)
  {
    stl::swap (start,x.start);
    stl::swap (finish,x.finish);  
    stl::swap (end_of_storage,x.end_of_storage);
  }
  else
  {
    vector tmp = *this;
    *this      = x;
    x          = tmp;
  }
}
    
template <class T,class Allocator> 
inline void swap (vector<T,Allocator>& a,vector<T,Allocator>& b)
{
  a.swap (b);
}

/*
    Операции сравнения
*/    

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator == (const vector& x) const
{
  return size() == x.size() && equal (begin(),end(),x.begin());
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator < (const vector& x) const
{
  return lexicographical_compare (begin (),end (),x.begin (),x.end ());
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator != (const vector& x) const
{
  return !(x == *this);
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator > (const vector& x) const
{
  return x < *this;
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator <= (const vector& x) const
{
  return !(x < *this);
}

template <class T,class Allocator>
inline bool vector<T,Allocator>::operator >= (const vector& x) const
{
  return !(*this < x);
}
