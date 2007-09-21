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
