/*
    Итератор (base)
*/

template <class WordT>
inline bit_iterator_base<WordT>::bit_iterator_base ()
  : pos (0), offset(0) 
  { }

template <class WordT>
inline bit_iterator_base<WordT>::bit_iterator_base (word_type* _pos,word_type _offset)
  : pos (_pos), offset (_offset)
  { }

template <class WordT>
inline typename bit_iterator_base<WordT>::difference_type 
bit_iterator_base<WordT>::operator - (const bit_iterator_base& x) const
{
  return BITS_PER_WORD * (pos-x.pos) + offset-x.offset;
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator == (const bit_iterator_base& x) const
{
  return pos == x.pos && offset == x.offset;
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator < (const bit_iterator_base& x) const
{
  return pos < x.pos || (pos == x.pos && offset < x.offset);
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator != (const bit_iterator_base& x) const
{
  return !(*this == x);
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator > (const bit_iterator_base& x) const
{
  return x < *this;
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator <= (const bit_iterator_base& x) const
{
  return !(x < *this);
}

template <class WordT>
inline bool bit_iterator_base<WordT>::operator >= (const bit_iterator_base& x) const
{
  return !(*this < x);
}

/*
    Итератор
*/
  
template <class WordT,class Ref>  
inline bit_iterator<WordT,Ref>::bit_iterator (word_type* _pos,word_type _offset) 
  : base (_pos,_offset)
  { }
  
template <class WordT,class Ref>  
inline bit_iterator<WordT,Ref>::bit_iterator (const base& i) 
  : base (i)
  { } 

template <class WordT,class Ref>
inline typename bit_iterator<WordT,Ref>::reference bit_iterator<WordT,Ref>::operator * () const
{ 
  return bit_reference<WordT> (pos,word_type (1) << offset);
}

template <class WordT,class Ref>
inline typename bit_iterator<WordT,Ref>::reference bit_iterator<WordT,Ref>::operator [] (difference_type i) const
{ 
  return *(*this + i);
}

template <class WordT,class Ref>
inline void bit_iterator<WordT,Ref>::next () 
{
  if (offset++ == BITS_PER_WORD-1) 
  {
    offset = 0;
    ++pos;
  }
}

template <class WordT,class Ref>
inline void bit_iterator<WordT,Ref>::prev () 
{
  if (!offset--)
  {
    offset = BITS_PER_WORD-1;
    --pos;
  }
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref>& bit_iterator<WordT,Ref>::operator ++ () 
{
  next ();
  return *this;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref> bit_iterator<WordT,Ref>::operator ++ (int) 
{
  iterator tmp = *this;
  
  next ();
  
  return tmp;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref>& bit_iterator<WordT,Ref>::operator -- () 
{
  prev ();
  return *this;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref> bit_iterator<WordT,Ref>::operator -- (int) 
{
  iterator tmp = *this;
  
  prev ();
  
  return tmp;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref>& bit_iterator<WordT,Ref>::operator += (difference_type i) 
{
  i   += offset;
  pos += i / BITS_PER_WORD;
  i   %= BITS_PER_WORD;
  
  if (i < 0) 
  {
    offset = word_type (i) + BITS_PER_WORD;
    --pos;
  } 
  else offset = word_type (i);
  
  return *this;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref>& bit_iterator<WordT,Ref>::operator -= (difference_type i) 
{
  return *this += -i;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref> bit_iterator<WordT,Ref>::operator + (difference_type i) const
{
  return iterator (*this) += i;
}

template <class WordT,class Ref>
inline bit_iterator<WordT,Ref> bit_iterator<WordT,Ref>::operator - (difference_type i) const
{
  return iterator (*this) -= i;
}

template <class WordT,class Ref>
inline typename bit_iterator<WordT,Ref>::difference_type bit_iterator<WordT,Ref>::operator - (const base& x) const
{
  return base::operator - (x);
}

/*
    Конструкторы
*/

template <class Allocator>
inline bit_vector<Allocator>::bit_vector (const allocator_type& allocator)
  : seq_size (0), vec (allocator)
  { }

template <class Allocator>
inline bit_vector<Allocator>::bit_vector (size_type count,bool value,const allocator_type& allocator)
  : seq_size (0), vec (words_count (count),(word_type)(value ? -1 : 0),allocator)  
{ 
  trim (count);
}

template <class Allocator> template <class InIter> 
inline bit_vector<Allocator>::bit_vector (InIter first,InIter last,const allocator_type& allocator)
  : seq_size (0), vec (allocator)
{  
  _init (first,last,iterator_category<InIter> ());
}

template <class Allocator> template <class Iter> 
inline void bit_vector<Allocator>::_init (Iter count,Iter value,int_iterator_tag)
{
  size_type num = (size_type)count;
    
  vec.assign (num,(value_type)value ? -1 : 0);
  trim       (num);
}

template <class Allocator> template <class Iter> 
inline void bit_vector<Allocator>::_init (Iter first,Iter last,input_iterator_tag)
{
  insert (begin (),first,last);  
}

/*
    Отсечение 
*/

template <class Allocator>
void bit_vector<Allocator>::trim (size_type new_size)
{   
  size_type words = words_count (new_size);

  if (words < vec.size ())
    vec.erase (vec.begin ()+words,vec.end ());
    
  seq_size = new_size;
  
  new_size %= BITS_PER_WORD;
  
  if (new_size > 0)
    vec [words-1] &= (word_type (1) << new_size)-1;
}

/*
    Аллокатор
*/

template <class Allocator>
inline typename bit_vector<Allocator>::allocator_type bit_vector<Allocator>::get_allocator () const
{ 
  return vec.get_allocator (); 
}

/*
    Получение итераторов
*/

template <class Allocator>
inline typename bit_vector<Allocator>::iterator bit_vector<Allocator>::begin ()
{
  return iterator (vec.begin (),0);
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_iterator bit_vector<Allocator>::begin () const
{
  return const_iterator (const_cast<word_type*> (vec.begin ()),0);
}

template <class Allocator>
inline typename bit_vector<Allocator>::iterator bit_vector<Allocator>::end ()
{
  return iterator (vec.begin (),seq_size);
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_iterator bit_vector<Allocator>::end () const
{
  return const_iterator (const_cast<word_type*> (vec.begin ()),seq_size);
}

template <class Allocator>
inline typename bit_vector<Allocator>::reverse_iterator bit_vector<Allocator>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reverse_iterator bit_vector<Allocator>::rbegin () const
{
  return reverse_iterator (end ());
}

template <class Allocator>
inline typename bit_vector<Allocator>::reverse_iterator bit_vector<Allocator>::rend ()
{
  return reverse_iterator (begin ());
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reverse_iterator bit_vector<Allocator>::rend () const
{
  return reverse_iterator (begin ());
}

/*
    Операции доступа
*/

template <class Allocator>
inline typename bit_vector<Allocator>::reference bit_vector<Allocator>::operator [] (size_type n)
{
  return *(begin ()+n);
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reference bit_vector<Allocator>::operator [] (size_type n) const
{
  return *(begin ()+n);
}

template <class Allocator>
inline typename bit_vector<Allocator>::reference bit_vector<Allocator>::at (size_type i)
{
  if (i >= size ())
    stl_raise_out_of_range (*this,i);

  return (*this)[i];
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reference bit_vector<Allocator>::at (size_type i) const
{
  if (i >= size ())
    stl_raise_out_of_range (*this,i);

  return (*this)[i];
}

template <class Allocator>
inline typename bit_vector<Allocator>::reference bit_vector<Allocator>::front ()
{ 
  return *begin ();
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reference bit_vector<Allocator>::front () const
{ 
  return *begin ();
}

template <class Allocator>
inline typename bit_vector<Allocator>::reference bit_vector<Allocator>::back ()
{ 
  return end ()[-1];
}

template <class Allocator>
inline typename bit_vector<Allocator>::const_reference bit_vector<Allocator>::back () const
{ 
  return end ()[-1];
}

/*
    Функции общего назначения    
*/

template <class Allocator>
inline typename bit_vector<Allocator>::size_type bit_vector<Allocator>::words_count (size_type count)
{
  return (count+BITS_PER_WORD-1) / BITS_PER_WORD;
}

template <class Allocator>
inline typename bit_vector<Allocator>::size_type bit_vector<Allocator>::size () const
{ 
  return seq_size;
}

template <class Allocator>
inline typename bit_vector<Allocator>::size_type bit_vector<Allocator>::max_size () const
{ 
  const size_type max_size = vec.max_size ();  
  return max_size < size_type (-1) / BITS_PER_WORD ? max_size * BITS_PER_WORD : size_type (-1);
}

template <class Allocator>
inline typename bit_vector<Allocator>::size_type bit_vector<Allocator>::capacity () const 
{
  return vec.capacity () * BITS_PER_WORD;
}

template <class Allocator>
inline bool bit_vector<Allocator>::empty () const
{ 
  return seq_size == 0;
}

template <class Allocator>
inline bool bit_vector<Allocator>::is_valid (iterator i) const
{
  return static_cast<size_type> (i-begin ()) <= seq_size; //i in [begin ();end ()]
}

template <class Allocator>
inline bool bit_vector<Allocator>::is_valid (iterator first,iterator last) const
{
  return is_valid (first) && is_valid (last) && first <= last;
}

/*
    Изменение размера
*/

template <class Allocator>
inline void bit_vector<Allocator>::resize (size_type new_size,bool value)
{
  if      (size ()  < new_size) insert (end (),new_size-size (),value);      
  else if (new_size < size ())  erase  (begin ()+new_size,end ());
}

template <class Allocator>
inline void bit_vector<Allocator>::reserve (size_type count)
{
  if (max_size () < count)
    stl_raise_length_error (*this,count);

  vec.reserve (words_count (count));
}

/*
    Присваивание
*/

template <class Allocator>
inline void bit_vector<Allocator>::assign (size_type count,bool value)
{
  clear ();

  if (max_size () < count)
    stl_raise_length_error (*this,count);

  insert (begin (),count,value);
}

template <class Allocator> template <class Iter> 
inline void bit_vector<Allocator>::_assign (Iter count,Iter value,int_iterator_tag)
{
  assign ((size_type)count,(value_type)value);  
}

template <class Allocator> template <class Iter> 
inline void bit_vector<Allocator>::_assign (Iter first,Iter last,input_iterator_tag)
{
  clear  ();
  insert (begin (),first,last);
}

template <class Allocator> template <class Iter> 
void bit_vector<Allocator>::_assign (Iter first,Iter last,forward_iterator_tag)
{
  try
  {
    size_type len = distance (first,last);
    
    if (max_size () < len)  
      stl_raise_length_error (*this,len);
      
    vec.resize (words_count (len),0);
    stl::copy  (first,last,begin ());
    
    seq_size = len;
  }
  catch (...)
  {
    clear ();  
    throw;
  }
}

template <class Allocator> template <class Iter> 
inline void bit_vector<Allocator>::assign (Iter first,Iter last)
{
  _assign (first,last,iterator_category<Iter> ());
}

/*
    Добавление / извлечение элементов
*/

template <class Allocator>
inline void bit_vector<Allocator>::push_back (bool value)
{
  insert (end (),value);
}

template <class Allocator>
inline void bit_vector<Allocator>::pop_back ()
{
  if (!empty ())
    erase (end ()-1);
}  

/*
    Вставка элементов
*/

template <class Allocator>
typename bit_vector<Allocator>::iterator
bit_vector<Allocator>::prepare_room (iterator position,size_type count)
{
  if (!is_valid (position))
    stl_raise_out_of_range (*this,position);

  if (!count)
    return position;    
        
  if (max_size () - size () < count)
    stl_raise_length_error (*this,size ()+count);

  size_type offset = position - begin ();  
  
  vec.resize (words_count (size ()+count),0);

  if (size ())
  {
    iterator old_end = end ();
    seq_size += count;
    
    copy_backward (begin ()+offset,old_end,end ());
  }
  else seq_size += count;  
  
  return begin () + offset;
}

template <class Allocator>
inline void bit_vector<Allocator>::insert (iterator position,size_type count,bool value)
{
  position = prepare_room (position,count);
      
  fill (position,position+count,value);
}

template <class Allocator>
inline typename bit_vector<Allocator>::iterator bit_vector<Allocator>::insert (iterator position,bool value)
{
  size_type offset = position - begin ();  
  
  insert (position,size_type (1),value);
  
  return begin () + offset; 
}

template <class Allocator> template <class Iter>
inline void bit_vector<Allocator>::_insert (iterator position,Iter count,Iter value,int_iterator_tag)
{
  insert (position,(size_type)count,(value_type)value);
}

template <class Allocator> template <class InIter>
void bit_vector<Allocator>::_insert (iterator position,InIter first,InIter last,input_iterator_tag)
{
  size_type start_offset = position - begin (), offset = start_offset; 

  try
  {      
    for (;first!=last;++first,++offset) insert (begin ()+offset,*first);
  }
  catch (...)
  {
    while (offset--!=start_offset) erase (begin ()+offset);
    throw;
  }
}

template <class Allocator> template <class Iter>
inline void bit_vector<Allocator>::_insert (iterator position,Iter first,Iter last,forward_iterator_tag)
{
  stl::copy (first,last,prepare_room (position,distance (first,last)));  
}
  
template <class Allocator> template <class InIter>
inline void bit_vector<Allocator>::insert (iterator position,InIter first,InIter last)
{
  _insert (position,first,last,iterator_category<InIter> ());
}

/*
    Удаление
*/

template <class Allocator>
inline typename bit_vector<Allocator>::iterator bit_vector<Allocator>::erase (iterator position)
{
  if (!is_valid (position) || position == end ())
    stl_raise_out_of_range (*this,position);

  size_type offset = position-begin ();

  stl::copy (position+1,end (),position);
    
  trim (seq_size-1);
  
  return begin () + offset;
}

template <class Allocator>
inline typename bit_vector<Allocator>::iterator bit_vector<Allocator>::erase (iterator first,iterator last)
{
  if (!is_valid (first,last))
    stl_raise_out_of_range (*this,first,last);

  size_type offset = first-begin ();

  iterator next = stl::copy (last,end (),first);
  
  trim (next-begin ());

  return begin () + offset;
}

template <class Allocator>
inline void bit_vector<Allocator>::clear ()
{
  erase (begin (),end ());
}

/*
    Инверсия битов
*/

template <class Allocator>
inline void bit_vector<Allocator>::flip ()
{
  for (typename storage_vector::iterator next=vec.begin ();next!=vec.end ();++next)
    *next = (word_type)~*next;
    
  trim (seq_size);  
}

/*
    Обмен
*/

template <class Allocator>
inline void bit_vector<Allocator>::swap (bit_vector& x)
{
  stl::swap (seq_size,x.seq_size);
  vec.swap  (x.vec);
}

template <class Allocator>
inline void swap (bit_vector<Allocator>& x,bit_vector<Allocator>& y)
{
  x.swap (y);
}

/*
    Операции сравнения
*/

template <class Allocator>
inline bool bit_vector<Allocator>::operator == (const bit_vector& x) const
{
  return size () == x.size () && equal (begin (),end (),x.begin ());
}

template <class Allocator>
inline bool bit_vector<Allocator>::operator < (const bit_vector& x) const
{
  return lexicographical_compare (begin (),end (),x.begin (),x.end ());
}

template <class Allocator>
inline bool bit_vector<Allocator>::operator != (const bit_vector& x) const
{
  return !(x == *this);
}

template <class Allocator>
inline bool bit_vector<Allocator>::operator > (const bit_vector& x) const
{
  return x < *this;
}

template <class Allocator>
inline bool bit_vector<Allocator>::operator <= (const bit_vector& x) const
{
  return !(x < *this);
}

template <class Allocator>
inline bool bit_vector<Allocator>::operator >= (const bit_vector& x) const
{
  return !(*this < x);
}

/*
    Специализация для vector<bool>
*/

template <class Allocator>
inline vector<bool,Allocator>::vector (const allocator_type& allocator)
  : base (allocator)
  { }

template <class Allocator>
inline vector<bool,Allocator>::vector (size_type count,value_type value,const allocator_type& allocator)
  : base (count,value,allocator)
  { }

template <class Allocator> template <class InIter> 
inline vector<bool,Allocator>::vector (InIter first,InIter last,const allocator_type& allocator)
  : base (first,last,allocator)
  { }

template <class Allocator>
inline void swap (vector<bool,Allocator>& x,vector<bool,Allocator>& y)
{
  x.swap (y);
}  
