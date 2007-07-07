/*
    Advance & distance
*/

template <class Iter,class Distance>
inline void advance (Iter& i,Distance n) 
{
  if (n >= 0) while (n--) ++i;
  else        while (n++) --i;
}

template <class T,class Distance>
inline void advance (T*& i,Distance n)
{
  i += n;
}

template <class Iter>
inline typename iterator_traits<Iter>::difference_type distance (Iter first,Iter last) 
{
  typename iterator_traits<Iter>::difference_type n;
  
  for (n=0;first!=last;++first,++n);
  
  return n;
}

template <class T>
inline ptrdiff_t distance (T* first,T* last)
{
  return last-first;  
}

template <class Iter,class Distance> 
inline void distance (Iter first,Iter last,Distance& n)
{
  n = distance (first,last);
}

/*
    reverse_iterator
*/

//конструкторы
template <class Iter>
inline reverse_iterator<Iter>::reverse_iterator (Iter i)
 : current (i)
 { }

template <class Iter>
inline reverse_iterator<Iter>::reverse_iterator (const reverse_iterator& x)
  : current (x.current)
{ }

template <class Iter> template <class Iter1> 
reverse_iterator<Iter>::reverse_iterator (const reverse_iterator<Iter1>& x)
  : current (x.base ())
{ }

//операции доступа и индексирования
template <class Iter>
inline typename reverse_iterator<Iter>::reference reverse_iterator<Iter>::operator * () const
{
  Iter tmp (current);
  return *--tmp;
}

template <class Iter>
inline typename reverse_iterator<Iter>::reference reverse_iterator<Iter>::operator [] (difference_type i) const
{
  return *(*this + i);
}

template <class Iter>
inline typename reverse_iterator<Iter>::pointer reverse_iterator<Iter>::operator -> () const
{
  return &(operator*());
}

//арифметические операции
template <class Iter>
inline reverse_iterator<Iter>& reverse_iterator<Iter>::operator ++ ()
{
  return --current, *this;
}

template <class Iter>
inline reverse_iterator<Iter>& reverse_iterator<Iter>::operator -- ()
{
  return ++current, *this;
}

template <class Iter>
inline reverse_iterator<Iter>& reverse_iterator<Iter>::operator += (difference_type n)
{
  return current -= n, *this;
}

template <class Iter>
inline reverse_iterator<Iter>& reverse_iterator<Iter>::operator -= (difference_type n)
{
  return current += n, *this;
}

template <class Iter>
inline const reverse_iterator<Iter> reverse_iterator<Iter>::operator ++ (int)
{
  reverse_iterator tmp = *this;
  --current;
  return tmp;
}

template <class Iter>
inline const reverse_iterator<Iter> reverse_iterator<Iter>::operator -- (int)
{
  reverse_iterator tmp = *this;
  ++current;
  return tmp;
}

template <class Iter>
inline const reverse_iterator<Iter> reverse_iterator<Iter>::operator + (difference_type n) const
{
  return reverse_iterator (current - n);
}

template <class Iter>
inline const reverse_iterator<Iter> reverse_iterator<Iter>::operator - (difference_type n) const
{
  return reverse_iterator (current + n);
}

template <class Iter>
inline typename reverse_iterator<Iter>::difference_type reverse_iterator<Iter>::operator - (const reverse_iterator& x) const
{
  return x.base() - base();
}

//сравнение двух итераторов обратного перебора
template <class Iter>
inline bool reverse_iterator<Iter>::operator == (const reverse_iterator& x) const
{
  return base () == x.base ();
}

template <class Iter>
inline bool reverse_iterator<Iter>::operator != (const reverse_iterator& x) const
{
  return !(*this == x);
}

template <class Iter>
inline bool reverse_iterator<Iter>::operator < (const reverse_iterator& x) const
{
  return x.base () < base ();
}

template <class Iter>
inline bool reverse_iterator<Iter>::operator > (const reverse_iterator& x) const
{
  return x < *this;
}

template <class Iter>
inline bool reverse_iterator<Iter>::operator <= (const reverse_iterator& x) const
{
  return !(x < *this);
}

template <class Iter>
inline bool reverse_iterator<Iter>::operator >= (const reverse_iterator& x) const
{
  return !(*this < x);
}

/*
    back_insert_iterator
*/

template <class Container>
inline back_insert_iterator<Container>::back_insert_iterator (container_type& _container)
  : container (&_container)
  { }

template <class Container>
inline back_insert_iterator<Container>& back_insert_iterator<Container>::operator = 
 (const typename container_type::value_type& x)
{
  container->push_back (x);
  
  return *this;  
}

template <class Container> 
inline back_insert_iterator<Container> back_inserter (Container& x)
{
  return back_insert_iterator<Container> (x);
}

/*
    front_insert_iterator
*/

template <class Container>
inline front_insert_iterator<Container>::front_insert_iterator (container_type& _container)
  : container (&_container)
  { }

template <class Container>
inline front_insert_iterator<Container>& front_insert_iterator<Container>::operator = 
 (const typename container_type::value_type& x)
{
  container->push_front (x);
  
  return *this;  
}

template <class Container> 
inline front_insert_iterator<Container> front_inserter (Container& x)
{
  return front_insert_iterator<Container> (x);
}

/*
    insert_iterator
*/

template <class Container>
inline insert_iterator<Container>::insert_iterator (container_type& _container,typename container_type::iterator i)
  : container (&_container), pos (i)
  { }

template <class Container>
inline insert_iterator<Container>& insert_iterator<Container>::operator = 
 (const typename container_type::value_type& x)
{
  pos = container->insert (pos,x);
  
  ++pos;  
  
  return *this;
}

template <class Container,class Iter> 
inline insert_iterator<Container> inserter (Container& x,Iter i)
{
  return insert_iterator<Container> (x,typename Container::iterator (i));
}

/*
    Категория итератора
*/

template <class Iter> 
inline typename iterator_traits<Iter>::iterator_category iterator_category ()
{
  return typename iterator_traits<Iter>::iterator_category ();
}
