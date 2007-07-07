/*
    Конструкторы
*/

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string (const allocator_type& _allocator)
  : allocator (_allocator)
{ 
  allocate_block (BUF_SIZE);
  terminate_string ();
}

template <class T,class Traits,class Alloc>
basic_string<T,Traits,Alloc>::basic_string (_reserve,size_type count,const allocator_type& _allocator)
  : allocator (_allocator)
{
  allocate_block (count+1);

  try
  {
    terminate_string ();
  }
  catch (...)
  {
    deallocate_block ();
    throw;
  }
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string (size_type count,value_type c,const allocator_type& _allocator)
  : allocator (_allocator)
{
  _init (count,c);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string (const basic_string& s)
{
  _init (s.begin (),s.end ());
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string 
 (const basic_string&    s,
  size_type              pos,
  size_type              count,
  const allocator_type&  _allocator)
    : allocator (_allocator)
{
  if (!s.is_valid (pos))
    stl_raise_out_of_range (s,pos);
    
  _init (s.begin ()+pos,s.begin()+pos+min (count,s.size ()-pos));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string (const value_type* s,size_type count,const allocator_type& _allocator)
  : allocator (_allocator)
{
  _init (s,s+count);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::basic_string (const value_type* s,const allocator_type& _allocator)
  : allocator (_allocator)
{
  _init (s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline basic_string<T,Traits,Alloc>::basic_string (Iter first,Iter last,const allocator_type& _allocator)
  : allocator (_allocator)
{
  _init (first,last);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>::~basic_string ()
{  
  destroy (start,finish+1);
  deallocate_block ();
}

template <class T,class Traits,class Alloc> 
void basic_string<T,Traits,Alloc>::_init (size_type count,value_type value)
{
  if (count > max_size ())
    stl_raise_length_error (*this,count);

  allocate_block (count+1);  
    
  try
  {
    finish = uninitialized_fill_n (start,count,value);
    terminate_string();
  }
  catch (...)
  {
    destroy (start,finish);
    deallocate_block ();
    throw;
  }
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline void basic_string<T,Traits,Alloc>::_init (Iter count,Iter value,int_iterator_tag)
{
  _init ((size_type)count,(value_type)value);
}

template <class T,class Traits,class Alloc> template <class Iter> 
void basic_string<T,Traits,Alloc>::_init (Iter first,Iter last,input_iterator_tag)
{
  allocate_block (BUF_SIZE);
  construct_null (finish);
  
  try 
  {
    append (first,last);
  }
  catch (...)
  {
    destroy (finish);
    deallocate_block ();
    throw;
  }  
}

template <class T,class Traits,class Alloc> template <class Iter> 
void basic_string<T,Traits,Alloc>::_init (Iter first,Iter last,forward_iterator_tag)
{ 
  size_type count = static_cast<size_type> (distance (first,last));
  
  if (count > max_size ())
    stl_raise_length_error (*this,count);

  allocate_block (count+1);
  
  try
  {
    finish = uninitialized_copy (first,last,start);
    terminate_string ();
  }
  catch (...)
  {
    deallocate_block ();    
    throw;
  }
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline void basic_string<T,Traits,Alloc>::_init (Iter first,Iter last)
{
  _init (first,last,iterator_category<Iter> ());
}

/*
    Выделение памяти
*/

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::allocator_type basic_string<T,Traits,Alloc>::get_allocator () const
{
  return allocator;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_static () const
{
  return start == buffer;
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::allocate_block (size_type count)
{
  start = finish = allocate (count,count);  
  end_of_storage = start + count;
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::deallocate_block ()
{
  deallocate (start,end_of_storage-start);
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::pointer basic_string<T,Traits,Alloc>::allocate 
 (size_type  count,
  size_type& allocated_size)
{
  if (count <= BUF_SIZE)
  {
    allocated_size = BUF_SIZE;
    return buffer;
  }
  else return allocator.allocate (allocated_size=count,buffer!=start?buffer:NULL);  
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::deallocate (pointer p,size_type count)
{
  if (p == buffer)
    return;
    
  allocator.deallocate (p,count);
}

/*
    Методы реализации    
*/

//инициализация завершающим нулём
template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::construct_null (value_type* p)
{
  construct (p,traits_type::null ());
}

template <class T,class Traits,class Alloc>
void basic_string<T,Traits,Alloc>::terminate_string ()
{
  try 
  {
    construct_null (finish);
  }
  catch (...)
  {
    destroy (start,finish);
    throw;
  }
}

//внутреннее копирование
template <class T,class Traits,class Alloc> template <class Iter> 
inline void basic_string<T,Traits,Alloc>::_copy (Iter first,Iter last,iterator result)
{
  for (;first!=last;++first,++result)
    traits_type::assign (*result,*first);
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::_copy (const value_type* first,const value_type* last,value_type* result)
{
  traits_type::copy (result,first,last-first);
}

//сравнение
template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::_compare 
 (const value_type* f1,
  const value_type* l1,
  const value_type* f2,
  const value_type* l2)
{
  const ptrdiff_t n1  = l1-f1, n2 = l2-f2; 
  const int       cmp = traits_type::compare (f1,f2,min (n1,n2));
  
  return cmp ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0));
}

//следующий размер буфера для строки (не считая завершающий 0)
template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::next_size (size_type inc_size) const
{
  return size () + (inc_size < size () && max_size () - size () >= size () ? size () : inc_size);
}

//предикат возможности роста строки
template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_grow_possible (size_type inc_size) const
{
  return max_size () - size () >= inc_size;
}

//корректность итератора
template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_valid (iterator pos) const
{
  return pos >= start && pos <= finish;
}

//корректность диапозона
template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_valid (iterator first,iterator last) const
{
  return first <= last && first >= start && last <= finish;
}

//корректность позиции
template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_valid (size_type pos) const
{
  return pos <= size ();
}

//корректность диапозона
template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::is_valid (size_type pos,size_type count) const
{
  return is_valid (pos) && count <= size () && size () - count >= pos;
}

/*
    Размер строки / макс. возможный размер / объём / проверка на пустоту
*/

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::size () const
{
  return finish-start;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::max_size () const
{
  return allocator.max_size () - 1;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::capacity () const
{
  size_type s = end_of_storage - start;
  return s ? s-1 : 0;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::empty () const
{
  return start == finish;
}

/*
    Операции доступа
*/
   
template <class T,class Traits,class Alloc>
inline const typename basic_string<T,Traits,Alloc>::value_type* basic_string<T,Traits,Alloc>::c_str () const
{
  return start;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::reference basic_string<T,Traits,Alloc>::at (size_type pos)
{
  if (pos >= size ())
    stl_raise_out_of_range (*this,pos);
    
  return start [pos];  
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_reference basic_string<T,Traits,Alloc>::at (size_type pos) const
{
  if (pos >= size ())
    stl_raise_out_of_range (*this,pos);
    
  return start [pos];  
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::reference basic_string<T,Traits,Alloc>::operator [] (size_type pos)
{   
  return start [pos];
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_reference basic_string<T,Traits,Alloc>::operator [] (size_type pos) const
{
  return start [pos];
}

/*
    Получение итераторов
*/

template <class T,class Traits,class Alloc> 
inline typename basic_string<T,Traits,Alloc>::iterator basic_string<T,Traits,Alloc>::begin ()
{
  return start;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::iterator basic_string<T,Traits,Alloc>::end ()
{
  return finish;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_iterator basic_string<T,Traits,Alloc>::begin () const
{
  return start;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_iterator basic_string<T,Traits,Alloc>::end () const
{
  return finish;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::reverse_iterator basic_string<T,Traits,Alloc>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_reverse_iterator basic_string<T,Traits,Alloc>::rbegin () const
{
  return const_reverse_iterator (end ());
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::reverse_iterator basic_string<T,Traits,Alloc>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::const_reverse_iterator basic_string<T,Traits,Alloc>::rend () const
{
  return const_reverse_iterator (begin ());
}

/*
    Изменение размера
*/

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::resize (size_type new_size,value_type c)
{
  if (new_size <= size ()) erase  (begin () + new_size,end ());
  else                     append (new_size - size (),c);
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::resize (size_type new_size)
{
  resize (new_size,traits_type::null ());
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::fast_resize (size_type new_size)
{
  if (new_size <= size ()) erase (begin () + new_size,end ());
  else
  {
    reserve (new_size);
    
    size_t count = new_size - size ();
          
    traits_type::fast_init (finish+1,count-1);
              
    try
    {
      construct_null (finish+count);
    }
    catch (...)
    {
      destroy (finish+1,finish+count);
      throw;
    }

    finish += count;
  }
}

template <class T,class Traits,class Alloc>
void basic_string<T,Traits,Alloc>::reserve (size_type new_size)
{
  if (capacity () >= new_size)
    return;
    
  new_size++;

  if (new_size > max_size ())
    stl_raise_length_error (*this,new_size);
         
  pointer new_start  = allocate (new_size,new_size), 
          new_finish = new_start;

  try 
  {
    new_finish = uninitialized_copy (start,finish,new_start);
    construct_null (new_finish);
  }
  catch (...)
  {
    destroy    (new_start,new_finish);
    deallocate (new_start,new_size);
    throw;
  }

  destroy (start,finish+1);
  deallocate_block ();
  
  start  = new_start;
  finish = new_finish;
  end_of_storage = new_start + new_size;
}

/*
    Получение подстроки
*/

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> basic_string<T,Traits,Alloc>::substr (size_type pos,size_type count) const
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);
    
  return basic_string (start+pos,start+pos+min (count,size ()-pos));
}

/*
    Присваивание
*/

template <class T,class Traits,class Alloc>
basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign 
 (const value_type* first,
  const value_type* last)
{
  try
  {
    const size_type count = static_cast<size_type> (last-first);
    
    if (count > max_size ())
      stl_raise_length_error (*this,count);
    
    if (count <= size ()) 
    {
      traits_type::copy (start,first,count);
      erase (start+count,finish);
    }
    else 
    {   
      traits_type::copy (start,first,size ());    
      append (first+size (),last);    
    }
    
    return *this;  
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T,class Traits,class Alloc>
basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign (size_type count,value_type c)
{
  try
  {
    if (count > max_size ())
      stl_raise_length_error (*this,count);

    if (count <= size()) 
    {
      traits_type::assign (start,count,c);
      erase (start+count,finish);
    }
    else 
    {
      traits_type::assign (start,size (),c);
      append (count-size (),c);    
    }
    
    return *this;  
  }
  catch (...)
  {
    clear ();  
    throw;
  }
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline void basic_string<T,Traits,Alloc>::_assign (Iter count,Iter value,int_iterator_tag)
{
  assign ((size_type)count,(value_type)value);
}

template <class T,class Traits,class Alloc> template <class Iter> 
void basic_string<T,Traits,Alloc>::_assign (Iter first,Iter last,input_iterator_tag)
{
  pointer cur = start;
  
  try
  {
    for (;first!=last && cur!=finish;++cur,++first)
      traits_type::assign (*cur,*first);
      
    if (first == last) erase  (cur,finish);
    else               append (first,last);  
  }
  catch (...)
  {
    clear ();
    throw;
  }
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign (Iter first,Iter last)
{
  _assign (first,last,iterator_category<Iter> ());  
  return *this;
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign (const basic_string& s)
{
  return assign (s.begin (),s.end ());
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign 
 (const basic_string&  s,
  size_type            pos,
  size_type            count)
{
  if (!s.is_valid (pos))
    stl_raise_out_of_range (s,pos);
    
  return assign (s.begin ()+pos,s.begin ()+pos+min (count,s.size()-pos));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign(const value_type* s,size_type count)
{ 
  return assign (s,s+count); 
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::assign (const value_type* s)
{ 
  return assign (s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator = (const basic_string& s)
{
  if (&s != this)
    assign (s.begin (),s.end ());
    
  return *this;
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator = (const value_type* s)
{
  return assign (s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator = (T c)
{
  return assign (static_cast<size_type>(1),c);
}

/*
    Конкатенация  
*/

template <class T,class Traits,class Alloc> 
basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append (size_type count,value_type c) 
{
  if (!is_grow_possible (count))
    stl_raise_length_error (*this,size ()+count);
    
  if (size () + count > capacity ())
    reserve (next_size (count));
    
  if (count > 0)
  {
    uninitialized_fill_n (finish+1,count-1,c);
    
    try
    {
      construct_null (finish+count);
    }
    catch (...)
    {
      destroy (finish+1,finish+count);
      throw;
    }

    traits_type::assign (*finish,c);
    
    finish += count;
  }
  
  return *this;
}

template <class T,class Traits,class Alloc> template <class Iter>
inline void basic_string<T,Traits,Alloc>::_append (Iter count,Iter value,int_iterator_tag)
{
  append ((size_type)count,(value_type)value);
}

template <class T,class Traits,class Alloc> template <class Iter>
void basic_string<T,Traits,Alloc>::_append (Iter first,Iter last,input_iterator_tag)
{
  for (;first!=last;++first) push_back (*first);
}

template <class T,class Traits,class Alloc> template <class Iter>
void basic_string<T,Traits,Alloc>::_append (Iter first,Iter last,forward_iterator_tag)
{
  if (first == last)
    return ;

  const size_type old_size = size ();  
  size_type       count    = static_cast<size_type> (distance (first,last));
   
  if (!is_grow_possible (count))
    stl_raise_length_error (*this,size ()+count);
    
  if (old_size+count > capacity()) 
  {
    size_type len = next_size (count) + 1;
    
    pointer new_start  = allocate (len,len), 
            new_finish = new_start;

    try
    {
      new_finish = uninitialized_copy (start,finish,new_start);
      new_finish = uninitialized_copy (first,last,new_finish);
      construct_null (new_finish);
    }
    catch (...)
    {
      destroy    (new_start,new_finish);
      deallocate (new_start,len);
      throw;
    }

    destroy (start,finish+1);
    deallocate_block ();
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start + len; 
  }
  else 
  {
    Iter f1 = first;
       
    uninitialized_copy (++f1,last,finish+1);
    
    try 
    {
      construct_null (finish+count);
    }
    catch (...)
    {
      destroy (finish+1,finish+count);
      throw;
    }

    traits_type::assign (*finish,*first);
    
    finish += count;
  }
}

template <class T,class Traits,class Alloc> template <class Iter>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append (Iter first,Iter last)
{
  _append (first,last,iterator_category<Iter> ());
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append (const basic_string& s)
{ 
  return append (s.begin (),s.end ()); 
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append(const value_type* s,size_type count) 
{ 
  return append (s,s+count); 
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append(const value_type* s) 
{ 
  return append (s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::append 
 (const      basic_string& s,
  size_type  pos,
  size_type  count)
{
  if (!s.is_valid (pos))
    stl_raise_out_of_range (s,pos);
    
  return append (s.begin ()+pos,s.begin ()+pos+min (count,s.size ()-pos));
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::push_back (value_type c)
{
  if (!is_grow_possible (1))
    stl_raise_length_error (*this,size ()+1);

  if (finish+1 == end_of_storage)
    reserve (next_size (1));

  construct_null (finish+1);  
  traits_type::assign (*finish,c);
  finish++;
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::pop_back ()
{
  traits_type::assign (finish [-1],traits_type::null ());
  destroy (finish);
  --finish;
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator += (const basic_string& s) 
{ 
  return append (s);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator += (const value_type* s) 
{ 
  return append (s); 
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::operator += (value_type c) 
{ 
  push_back (c); 
  return *this; 
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> basic_string<T,Traits,Alloc>::operator + (const basic_string& s) const
{                                                                                        
  return basic_string (_reserve (),size ()+s.size ()).append (*this).append (s);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> basic_string<T,Traits,Alloc>::operator + (const value_type* s) const
{                                                                                        
  return basic_string (_reserve (),size ()+traits_type::length (s)).append (*this).append (s);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> basic_string<T,Traits,Alloc>::operator + (const value_type c) const
{ 
  _reserve __reserve;

  basic_string res (__reserve,size ()+1);
  
  res.append (*this); 
  res.push_back (c);  
  
  return res;
//  return basic_string (size ()+1).append (*this).push_back (c);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> operator + 
 (const typename basic_string<T,Traits,Alloc>::value_type* a,
  const basic_string<T,Traits,Alloc>& b)
{
  typename basic_string<T,Traits,Alloc>::_reserve __reserve;
  return basic_string<T,Traits,Alloc> (__reserve,b.size ()+basic_string<T,Traits,Alloc>::traits_type::length (a)).append (a).append (b);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc> operator + 
 (const typename basic_string<T,Traits,Alloc>::value_type a,
  const basic_string<T,Traits,Alloc>& b)
{
  typename basic_string<T,Traits,Alloc>::_reserve __reserve;
  basic_string<T,Traits,Alloc> res (__reserve,b.size ()+1);
  
  res.push_back (a);
  res.append (b); 
  
  return res;
}

/*
    Удаление
*/

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::erase (size_type pos,size_type count)
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);
     
  erase (start+pos,start+pos+min (count,size ()-pos));
  
  return *this;  
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::iterator basic_string<T,Traits,Alloc>::erase (iterator position)
{
  if (!is_valid (position) || position == end ())
    stl_raise_out_of_range (*this,position);

  traits_type::move (position,position+1,finish-position);
  destroy           (finish);
  
  --finish;
  
  return position;  
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::iterator basic_string<T,Traits,Alloc>::erase 
 (iterator first,
  iterator last)
{
  if (first == last)
    return first;
    
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);  
        
  const iterator new_finish = finish-(last-first);
    
  traits_type::move (first,last,(finish-last)+1);
  destroy           (new_finish+1,finish+1);
  
  finish = new_finish;
  
  return first;
}

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::clear ()
{
  if (empty ())
    return;
    
  traits_type::assign (*start,traits_type::null ());
  destroy (start+1,finish+1);
  
  finish = start;
}

/*
    Вставка
*/

template <class T,class Traits,class Alloc> template <class Iter>
inline void basic_string<T,Traits,Alloc>::_insert (iterator position,Iter count,Iter value,int_iterator_tag)
{
  insert (position,(size_type)count,(value_type)value);  
}

template <class T,class Traits,class Alloc> template <class Iter>
void basic_string<T,Traits,Alloc>::_insert (iterator position,Iter first,Iter last,input_iterator_tag)
{
  for (;first!=last;++first,++position) position = insert (position,*first);
}

template <class T,class Traits,class Alloc> template <class Iter>
void basic_string<T,Traits,Alloc>::_insert (iterator position,Iter first,Iter last,forward_iterator_tag)
{
  if (first == last)
    return;   

  size_type count = static_cast<size_type> (distance (first,last));
  
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);
    
  if (!is_grow_possible (count)) 
    stl_raise_length_error (*this,size ()+count);
  
  if (static_cast<size_type> (end_of_storage-finish) >= count + 1)
  { 
    const size_type elems_after = static_cast<size_type> (finish-position);
        
    if (elems_after >= count)
    {   
      uninitialized_copy ((finish-count)+1,finish+1,finish+1);           
      traits_type::move  (position+count,position,(elems_after-count)+1);
      _copy              (first,last,position);
    }
    else 
    {   
      Iter mid = first;
            
      advance            (mid,elems_after+1);     
      uninitialized_copy (mid,last,finish+1);
          
      try 
      {
        uninitialized_copy (position,finish+1,finish+count-elems_after);
      }
      catch (...)
      {
        destroy (finish+1,finish+count-elems_after);
        throw;
      }
      
      _copy (first,mid,position);
    }
    
    finish += count;
  }
  else 
  {
    size_type len        = next_size (count) + 1;
    pointer   new_start  = allocate (len,len), 
              new_finish = new_start;
    
    try 
    {
      new_finish = uninitialized_copy (start,position,new_start);
      new_finish = uninitialized_copy (first,last,new_finish);
      new_finish = uninitialized_copy (position,finish,new_finish);
      
      construct_null (new_finish);
    }
    catch (...)
    {
      destroy    (new_start,new_finish);
      deallocate (new_start,len);
      throw;
    }

    destroy (start,finish+1);
    deallocate_block ();
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start + len; 
  }
}

template <class T,class Traits,class Alloc> template <class Iter>
inline void basic_string<T,Traits,Alloc>::insert (iterator position,Iter first,Iter last)
{
  _insert (position,first,last,iterator_category<Iter> ());
}

template <class T,class Traits,class Alloc>
void basic_string<T,Traits,Alloc>::insert (iterator position,size_type count,value_type c)
{
  if (!count)
    return;
      
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);
      
  if (!is_grow_possible (count)) 
    stl_raise_length_error (*this,size ()+count);

  if (size_type (end_of_storage-finish) >= count + 1) 
  { 
    const size_type elems_after = finish - position;    
    
    if (elems_after >= count) 
    {
      uninitialized_copy  ((finish-count)+1,finish+1,finish+1);
      traits_type::move   (position+count,position,(elems_after-count)+1);
      traits_type::assign (position,count,c);
    }
    else 
    {              
      uninitialized_fill_n (finish+1,count-elems_after-1,c);     
           
      try
      {
        uninitialized_copy (position,finish+1,finish+count-elems_after);
      }
      catch (...)
      {
        destroy (finish+1,finish+count-elems_after);
        throw;
      }
                          
      traits_type::assign (position,elems_after+1,c);
    }
    
    finish += count;
  }
  else 
  {
    size_type len       = next_size (count) + 1;
    iterator  new_start = allocate (len,len), new_finish = new_start;

    try 
    {
      new_finish = uninitialized_copy   (start,position,new_start);
      new_finish = uninitialized_fill_n (new_finish,count,c); 
      new_finish = uninitialized_copy   (position,finish,new_finish); 
      
      construct_null (new_finish);
    }
    catch (...)
    {
      destroy    (new_start,new_finish);
      deallocate (new_start,len);
      throw;
    }

    destroy (start, finish + 1);
    deallocate_block ();
    
    start  = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;    
  }
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::insert (size_type pos,const basic_string& s)
{   
  insert (start+pos,s.begin (),s.end ());
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::insert 
 (size_type           pos,
  const basic_string& s,
  size_type           offs, 
  size_type           count)
{    
  insert (start+pos,s.begin ()+offs,s.begin ()+offs+min (count,s.size()-offs));  
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::insert 
 (size_type          pos,
  const value_type*  s,
  size_type          count)
{   
  insert (start+pos,s,s+count);  
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::insert (size_type pos,const value_type* s)
{
  insert (start+pos,s,s+traits_type::length (s));
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::insert (size_type pos,size_type count,value_type c)
{
  insert (start+pos,count,c);  
  return *this;
}

template <class T,class Traits,class Alloc>
typename basic_string<T,Traits,Alloc>::iterator basic_string<T,Traits,Alloc>::insert (iterator pos,value_type c)
{
  if (pos == finish) 
  {
    push_back (c);
    return finish - 1;
  }
  else 
  {
    iterator new_pos = pos;
    
    if (finish + 1 < end_of_storage) 
    {
      construct_null      (finish+1);
      traits_type::move   (pos+1,pos,finish-pos);
      traits_type::assign (*pos,c);
      
      ++finish;
    }
    else 
    {
      size_type len        = next_size (1) + 1;
      iterator  new_start  = allocate (len,len), 
                new_finish = new_start;

      try
      {
        new_pos = uninitialized_copy (start,pos,new_start);
        construct (new_pos,c);
        new_finish = uninitialized_copy (pos,finish,new_finish=new_pos+1);
        construct_null (new_finish);
      }
      catch (...)
      {
        destroy    (new_start,new_finish);
        deallocate (new_start,len);
        throw;
      }

      destroy (start,finish+1);
      deallocate_block ();
      
      start  = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
    
    return new_pos;
  }
}

/*
    Замена
*/    

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (iterator    first,
  iterator    last,
  size_type   count,
  value_type  c)
{
  const size_type len = static_cast<size_type> (last-first);
  
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);    
  
  if (len >= count) 
  {
    traits_type::assign (first,count,c);
    erase               (first+count,last);
  }
  else 
  {
    if (!is_grow_possible (count-len))
      stl_raise_length_error (*this,size ()-len+count);
  
    traits_type::assign (first,len,c);
    insert              (last,count-len,c);
  }
  
  return *this;
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline void basic_string<T,Traits,Alloc>::_replace (iterator first,iterator last,Iter count,Iter value,int_iterator_tag)
{
  replace (first,last,(size_type)count,(value_type)value);
}

template <class T,class Traits,class Alloc> template <class Iter> 
void basic_string<T,Traits,Alloc>::_replace (iterator first,iterator last,Iter f,Iter l,input_iterator_tag)
{
  for (;first!=last && f!=l;++first,++f) traits_type::assign (*first,*f);

  if (f == l) erase  (first,last);
  else        insert (last,f,l);
}

template <class T,class Traits,class Alloc> template <class Iter> 
void basic_string<T,Traits,Alloc>::_replace (iterator first,iterator last,Iter f,Iter l,forward_iterator_tag)
{
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);

  const size_type count = static_cast<size_type> (distance (f,l)), len = last-first;
        
  if (len >= count)
  {  
    _copy (f,l,first);
    erase (first+count,last);
  }
  else 
  {
    if (!is_grow_possible (count-len))
      stl_raise_length_error (*this,size ()+static_cast<size_type> (count-len));
  
    Iter m = f;
    
    advance (m,len);    
    _copy   (f,m,first);
    insert  (last,m,l);
  }
}

template <class T,class Traits,class Alloc> template <class Iter> 
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (iterator  first,
  iterator  last,
  Iter      f,
  Iter      l)
{
  _replace (first,last,f,l,iterator_category<Iter> ());
  return *this;  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (size_type            pos,
  size_type            len,
  const basic_string&  s)
{
  return replace (start+pos,start+pos+len,s.begin (),s.end ());  
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (size_type            pos1,
  size_type            n1,
  const basic_string&  s,
  size_type            pos2,
  size_type            n2)
{
  return replace (start+pos1,start+pos1+min (n1,size ()-pos1),s.start+pos2,s.start+pos2+min (n2,s.size ()-pos2));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (size_type          pos,
  size_type          n1,
  const value_type*  s,
  size_type          n2)
{
  return replace (start+pos,start+pos+min (n1,size ()-pos),s,s+n2);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (size_type          pos,
  size_type          n1,
  const value_type*  s)
{
  return replace (start+pos,start+pos+min (n1,size ()-pos),s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (size_type   pos,
  size_type   n1,
  size_type   n2,
  value_type  c)
{
  return replace (start+pos,start+pos+min (n1,size ()-pos),n2,c);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (iterator             first,
  iterator             last,
  const basic_string&  s)
{
  return replace (first,last,s.begin (),s.end());
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (iterator           first,
  iterator           last,
  const value_type*  s,
  size_type          count)
{
  return replace (first,last,s,s+count);
}

template <class T,class Traits,class Alloc>
inline basic_string<T,Traits,Alloc>& basic_string<T,Traits,Alloc>::replace 
 (iterator           first,
  iterator           last,
  const value_type*  s)
{
  return replace (first,last,s,s+traits_type::length (s));
}

/*
    Обмен и копирование строки в буфер
*/    

template <class T,class Traits,class Alloc>
inline void basic_string<T,Traits,Alloc>::swap (basic_string& s)
{
  if (is_static () || s.is_static () || allocator != s.allocator)
  {
    basic_string t = *this;
    *this          = s;
    s              = t;
  }
  else
  {
    stl::swap (start,s.start);
    stl::swap (finish,s.finish);
    stl::swap (end_of_storage,s.end_of_storage);
  }
}

template <class T,class Traits,class Alloc>
inline void swap (basic_string<T,Traits,Alloc>& a,basic_string<T,Traits,Alloc>& b)
{
  a.swap (b);
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::copy 
 (value_type* dst,
  size_type   buf_size,
  size_type   pos) const
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);
    
  const size_type len = min (buf_size,size ()-pos);
  
  traits_type::copy (dst,start+pos,len);
  
  return len;
}  

/*
    Лексикографическое сравнение
*/

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare (const basic_string& s) const
{
  return _compare (start,finish,s.start,s.finish);
}

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare (size_type pos,size_type count,const basic_string& s) const
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);

  return _compare (start+pos,start+pos+min (count,size ()-pos),s.start,s.finish);
}

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare 
 (size_type            pos1,
  size_type            n1,
  const basic_string&  s,
  size_type            pos2,
  size_type            n2) const
{
  if (!is_valid (pos1))   stl_raise_out_of_range (*this,pos1);  
  if (!s.is_valid (pos2)) stl_raise_out_of_range (s,pos2);

  return _compare (start+pos1,start+pos1+min (n1,size ()-pos1),s.start+pos2,s.start+pos2+min (n2,size ()-pos2));
}

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare (const value_type* s) const
{
  return _compare (start,finish,s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare (size_type pos,size_type count,const value_type* s) const
{
  if (!is_valid (pos))
    stl_raise_out_of_range (*this,pos);

  return _compare (start+pos,start+pos+min (count,size ()-pos),s,s+traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline int basic_string<T,Traits,Alloc>::compare (size_type pos1,size_type n1,const value_type* s,size_type n2) const
{
  if (!is_valid (pos1))
    stl_raise_out_of_range (*this,pos1);

  return _compare (start+pos1,start+pos1+min (n1,size ()-pos1),s,s+n2);
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator == (const basic_string& s) const
{
  return size () == s.size() && !traits_type::compare (data (),s.data (),size());
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator < (const basic_string& s) const
{
  return _compare (start,finish,s.start,s.finish) < 0;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator != (const basic_string& s) const
{
  return !(*this == s);
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator > (const basic_string& s) const
{
  return s < *this;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator <= (const basic_string& s) const
{
  return !(s < *this);  
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator >= (const basic_string& s) const
{
  return !(*this < s);  
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator == (const value_type* s) const
{
  size_type count = traits_type::length (s);
  return size() == count && !traits_type::compare (data (),s,count);
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator < (const value_type* s) const
{
  size_type count = traits_type::length (s);
  return _compare (start,finish,s,s+count) < 0;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator != (const value_type* s) const
{
  return !(*this == s);
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator > (const value_type* s) const
{
  return s < *this;
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator <= (const value_type* s) const
{
  return !(s < *this);  
}

template <class T,class Traits,class Alloc>
inline bool basic_string<T,Traits,Alloc>::operator >= (const value_type* s) const
{
  return !(*this < s);  
}

template <class T,class Traits,class Alloc>
inline bool operator == 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  size_t count = basic_string<T,Traits,Alloc>::traits_type::length (x);
  return y.size() == count && !basic_string<T,Traits,Alloc>::traits_type::compare (y.data (),x,count);
}

template <class T,class Traits,class Alloc>
inline bool operator < 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  size_t count = basic_string<T,Traits,Alloc>::traits_type::length (x);
  return basic_string<T,Traits,Alloc>::_compare (y.start,y.finish,x,x+count) < 0;
}

template <class T,class Traits,class Alloc>
inline bool operator != 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  return !(x == y);
}

template <class T,class Traits,class Alloc>
inline bool operator > 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  return y < x;
}

template <class T,class Traits,class Alloc>
inline bool operator <= 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  return !(y < x);
}

template <class T,class Traits,class Alloc>
inline bool operator >= 
 (const typename basic_string<T,Traits,Alloc>::value_type* x,
  const basic_string<T,Traits,Alloc>&                      y)
{
  return !(x < y);
}

/*
    Поиск
*/

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find 
 (const basic_string& s,
  size_type           pos) const
{
  return find (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find 
 (const value_type* s,
  size_type         pos) const
{
  return find (s,pos,traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{
  if (pos >= size ())
    return npos;

  const const_iterator result = search (start+pos,finish,s,s+count,eq_traits ());
  
  return result != finish ? result - begin () : npos;  
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find 
 (value_type c,
  size_type  pos) const
{
  if (pos >= size ())
    return npos;

  const const_iterator result = find_if (start+pos,finish,bind2nd (eq_traits (),c));
  
  return result != finish ? result - begin () : npos;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::rfind 
 (const basic_string& s,
  size_type           pos) const
{
  return rfind (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::rfind 
 (const value_type* s,
  size_type         pos) const
{
  return rfind (s,pos,traits_type::length (s));
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::rfind 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{
  const size_type len = size ();

  if (count > len) 
    return npos;
    
  if (!count)
    return min (len,pos);
    
  const const_iterator last   = begin() + min (len-count,pos)+count,
                       result = find_end (begin (),last,s,s+count,eq_traits ());
                         
  return result != last ? result - begin () : npos;
}

template <class T,class Traits,class Alloc>
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::rfind 
 (value_type c,
  size_type  from_pos) const
{
  const size_type len = size ();

  if (len < 1)
    return npos;

  const const_iterator   last   = begin () + min (len-1,from_pos) + 1;    
  const_reverse_iterator result = find_if (const_reverse_iterator (last),rend (),bind2nd(eq_traits (),c));
    
  return result != rend() ? (result.base () - 1) - begin () : npos;
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_of 
 (const basic_string& s,
  size_type           pos) const
{  
  return find_first_of (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_of 
 (const value_type* s,
  size_type         pos) const
{  
  return find_first_of (s,pos,traits_type::length (s));
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_of 
 (value_type c,
  size_type  pos) const
{  
  return find (c,pos);
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_of 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{  
  if (pos >= size ())
    return npos;

  const_iterator result = stl::find_first_of (begin ()+pos,end (),s,s+count,eq_traits ());
  
  return result != finish ? result - begin () : npos;
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_of 
 (const basic_string& s,
  size_type           pos) const
{
  return find_last_of (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_of 
 (const value_type* s,
  size_type         pos) const
{
  return find_last_of (s,pos,traits_type::length (s));
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_of 
 (value_type c,
  size_type  pos) const
{
  return rfind (c,pos);
}  

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_of 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{
  const size_type len = size ();

  if (len < 1)
    return npos;

  const const_iterator         last   = start + min (len-1,pos) + 1;
  const const_reverse_iterator result = stl::find_first_of (const_reverse_iterator (last),rend (),s,s+count,eq_traits ());
    
  return result != rend () ? (result.base ()-1) - start : npos;
}  
  
template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_not_of 
 (const basic_string& s,
  size_type           pos) const
{
  return find_first_not_of (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_not_of 
 (const value_type* s,
  size_type         pos) const
{
  return find_first_not_of (s,pos,traits_type::length (s));
}
 
template <class T,class Traits,class Alloc>    
typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_not_of 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{ 
  if (pos < size ())
  { 
    const value_type* const v = finish;
    
    for (const value_type* u=start+pos;u<v;++u)
      if (!traits_type::find (s,count,*u))
        return u-start;
  }
  
  return npos;  
}  

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_first_not_of 
 (value_type c,
  size_type  pos) const
{
  if (pos > size ())
    return npos;

  const_iterator result = find_if (begin ()+pos,end (),not1 (bind2nd (eq_traits (),c)));
  
  return result != finish ? result - begin () : npos;
}        

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_not_of 
 (const basic_string& s,
  size_type           pos) const
{
  return find_last_not_of (s.begin (),pos,s.size ());
}

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_not_of 
 (const value_type* s,
  size_type         pos) const
{
  return find_last_not_of (s,pos,traits_type::length (s));
}  

template <class T,class Traits,class Alloc>    
typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_not_of 
 (const value_type* s,
  size_type         pos,
  size_type         count) const
{ 
  const size_type len = size ();  
  
  if (len)
  {
    for (const value_type* u=start+(pos<len?pos:len-1);;--u)
      if      (!traits_type::find (s,count,*u)) return u-start;
      else if (u == start)                      break;
  }

  return npos;
}  

template <class T,class Traits,class Alloc>    
inline typename basic_string<T,Traits,Alloc>::size_type basic_string<T,Traits,Alloc>::find_last_not_of 
 (value_type c,
  size_type  pos) const
{
  const size_type len = size ();

  if (len < 1)
    return npos;

  const const_iterator   last   = begin ()+min (len-1,pos)+1;
  const_reverse_iterator result = find_if (const_reverse_iterator (last),rend(),not1 (bind2nd (eq_traits (),c)));
  
  return result != rend () ? (result.base ()-1) - begin () : npos;
}
