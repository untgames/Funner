/*
    Очередь (FIFO)
*/

//constructors
template <class T,class Sequence>
inline queue<T,Sequence>::queue () 
  : c() 
  { }
  
template <class T,class Sequence>
inline queue<T,Sequence>::queue (const Sequence& _c) 
  : c (_c) 
  { }

//функции общего назначения  
template <class T,class Sequence>
inline bool queue<T,Sequence>::empty () const 
{ 
  return c.empty (); 
}

template <class T,class Sequence>
inline typename queue<T,Sequence>::size_type queue<T,Sequence>::size () const 
{ 
  return c.size (); 
}

//доступ
template <class T,class Sequence>
inline typename queue<T,Sequence>::reference queue<T,Sequence>::front () 
{ 
  return c.front (); 
}

template <class T,class Sequence>
inline typename queue<T,Sequence>::const_reference queue<T,Sequence>::front () const 
{ 
  return c.front (); 
}

template <class T,class Sequence>
inline typename queue<T,Sequence>::reference queue<T,Sequence>::back () 
{ 
  return c.back (); 
}

template <class T,class Sequence>
inline typename queue<T,Sequence>::const_reference queue<T,Sequence>::back () const 
{ 
  return c.back (); 
}

//вставка в очередь
template <class T,class Sequence>
inline void queue<T,Sequence>::push (const value_type& x) 
{ 
  c.push_back (x); 
}

//извлечение из очереди
template <class T,class Sequence>
inline void queue<T,Sequence>::pop () 
{ 
  c.pop_front (); 
}

//сравнение
template <class T,class Sequence>
inline bool queue<T,Sequence>::operator == (const queue& x) const
{
  return c == x.c;
}

template <class T,class Sequence>
inline bool queue<T,Sequence>::operator < (const queue& x) const
{
  return c < x.c;
}

template <class T,class Sequence>
inline bool queue<T,Sequence>::operator != (const queue& x) const
{
  return !(x == *this);
}

template <class T,class Sequence>
inline bool queue<T,Sequence>::operator > (const queue& x) const
{
  return x < *this;
}

template <class T,class Sequence>
inline bool queue<T,Sequence>::operator <= (const queue& x) const
{
  return !(x < *this);
}

template <class T,class Sequence>
inline bool queue<T,Sequence>::operator >= (const queue& x) const
{
  return !(*this < x);
}

/*
    Очередь с приоритетами
*/

//constructors
template <class T,class Sequence,class Compare>
inline priority_queue<T,Sequence,Compare>::priority_queue () 
  : c () 
  { }
  
template <class T,class Sequence,class Compare>
inline priority_queue<T,Sequence,Compare>::priority_queue (const Compare& less) 
  : c (), comp (less) 
  { }

template <class T,class Sequence,class Compare>
inline priority_queue<T,Sequence,Compare>::priority_queue (const Compare& less,const Sequence& seq) 
    : c (seq), comp (less) 
{ 
  make_heap (c.begin (),c.end (),comp); 
}

template <class T,class Sequence,class Compare> template <class Iter>
inline priority_queue<T,Sequence,Compare>::priority_queue (Iter first,Iter last) 
   : c (first,last) 
{ 
  make_heap (c.begin (),c.end (),comp);
}

template <class T,class Sequence,class Compare> template <class Iter>
inline priority_queue<T,Sequence,Compare>::priority_queue (Iter first,Iter last,const Compare& less)
   : c (first,last), comp (less) 
{ 
  make_heap (c.begin (),c.end (),comp); 
}

template <class T,class Sequence,class Compare> template <class Iter>
inline priority_queue<T,Sequence,Compare>::priority_queue (Iter first,Iter last,const Compare& less,const Sequence& seq)
  : c (seq), comp (less)
{ 
  c.insert  (c.end (),first,last);
  make_heap (c.begin (),c.end (),comp);
}

//функции общего назначения
template <class T,class Sequence,class Compare>  
inline bool priority_queue<T,Sequence,Compare>::empty () const 
{ 
  return c.empty (); 
}

template <class T,class Sequence,class Compare>  
inline typename priority_queue<T,Sequence,Compare>::size_type priority_queue<T,Sequence,Compare>::size () const 
{ 
  return c.size (); 
}
  
//доступ
template <class T,class Sequence,class Compare>  
inline typename priority_queue<T,Sequence,Compare>::const_reference priority_queue<T,Sequence,Compare>::top () const 
{ 
  return c.front (); 
}

//вставка в очередь
template <class T,class Sequence,class Compare>  
void priority_queue<T,Sequence,Compare>::push (const value_type& x) 
{
  try 
  {
    c.push_back (x); 
    push_heap   (c.begin (),c.end (),comp);
  }
  catch (...)
  {
    c.clear ();
    throw;
  }
}

//извлечение из очереди
template <class T,class Sequence,class Compare>  
void priority_queue<T,Sequence,Compare>::pop () 
{
  try 
  {
    pop_heap   (c.begin (),c.end (),comp);
    c.pop_back ();
  }
  catch (...)
  {
    c.clear ();
    throw;
  }
}
