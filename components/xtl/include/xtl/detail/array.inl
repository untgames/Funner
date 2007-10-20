/*
===================================================================================================
    Описание array<T, N>
===================================================================================================
*/

/*
    Присваивание c преобразованием типа
*/

template <class T, size_t N> template <class T2>
inline array<T, N>& array<T, N>::operator = (const array<T2, N>& rhs)
{
  stl::copy (rhs.begin (), rhs.end (), begin ());
  return *this;
}

/*
    Присваивание одного значения для всех элементов
*/

template <class T, size_t N>
void array<T, N>::assign (const value_type& value)
{
  stl::fill_n (begin (), size (), value);
}

/*
    Прямой доступ к данным (только чтение)
*/

template <class T, size_t N>
inline const typename array<T, N>::value_type* array<T, N>::data () const
{
  return elems;
}

template <class T, size_t N>
inline typename array<T, N>::value_type* array<T, N>::data ()
{ 
  return elems;
}

/*
    Использование как С массива (чтение/запись)
*/

template <class T, size_t N>
inline typename array<T, N>::value_type* array<T, N>::c_array ()
{ 
  return elems;
}

/*
    Поддержка итераторов
*/

template <class T, size_t N>
inline typename array<T, N>::iterator array<T, N>::begin ()
{
  return elems;
}

template <class T, size_t N>
inline typename array<T, N>::const_iterator array<T, N>::begin () const 
{
  return elems;
}

template <class T, size_t N>
inline typename array<T, N>::iterator array<T, N>::end ()
{
  return elems + N;
}

template <class T, size_t N>
inline typename array<T, N>::const_iterator array<T, N>::end () const
{
  return elems + N;
}

/*
    Информация о размере
*/

template <class T, size_t N>
inline typename array<T, N>::size_type array<T, N>::size ()
{
  return N;
}

template <class T, size_t N>
inline bool array<T, N>::empty ()
{
  return false;
}

template <class T, size_t N>
inline typename array<T, N>::size_type array<T, N>::max_size ()
{
  return N;
}

/*
    Поддержка реверсных итераторов
*/

template <class T, size_t N>
inline typename array<T, N>::reverse_iterator array<T, N>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T, size_t N>
inline typename array<T, N>::const_reverse_iterator array<T, N>::rbegin() const
{
  return const_reverse_iterator (end ());
}

template <class T, size_t N>
inline typename array<T, N>::reverse_iterator array<T, N>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T, size_t N>
inline typename array<T, N>::const_reverse_iterator array<T, N>::rend () const
{
  return const_reverse_iterator (begin ());
}
    
/*
    Доступ к элементу (operator[])
*/

template <class T, size_t N>
inline typename array<T, N>::reference array<T, N>::operator [] (size_type i) 
{ 
  return elems [i];
}

template <class T, size_t N>
inline typename array<T, N>::const_reference array<T, N>::operator [] (size_type i) const 
{     
  return elems [i]; 
}
    
/*
    Доступ к элементу с проверкой диапазона (at())
*/

template <class T, size_t N>
inline typename array<T, N>::reference array<T, N>::at (size_type i)
{
  rangecheck (i);
  return elems [i];
}

template <class T, size_t N>
inline typename array<T, N>::const_reference array<T, N>::at (size_type i) const
{
  rangecheck (i);
  return elems [i];
}

/*
    Доступ к первому (front) и последнему (back) элементам
*/

template <class T, size_t N>
inline typename array<T, N>::reference array<T, N>::front () 
{ 
  return elems [0]; 
}

template <class T, size_t N>
inline typename array<T, N>::const_reference array<T, N>::front () const 
{
  return elems [0];
}

template <class T, size_t N>
inline typename array<T, N>::reference array<T, N>::back () 
{ 
  return elems [N-1]; 
}

template <class T, size_t N>
inline typename array<T, N>::const_reference array<T, N>::back () const 
{ 
  return elems [N-1]; 
}

/*
   Обмен (за линейное время)
*/

template <class T, size_t N>
inline void array<T, N>::swap (array& y)
{
  stl::swap_ranges (begin (), end (), y.begin ());
}

/*
    Проверка диапазона
*/

template <class T, size_t N>
inline void array<T, N>::rangecheck (size_type i)
{
  if (i >= size ())
    stl::stl_raise_out_of_range (*this, i);
}

/*
===================================================================================================
    Описание array<T, 0>
===================================================================================================
*/

/*
    Присваивание c преобразованием типа
*/

template <class T> template <class T2>
array<T, 0>& array<T, 0>::operator = (const array<T2, 0>&)
{
  return *this;
}

/*
    Присваивание одного значения для всех элементов
*/

template <class T>
void array<T, 0>::assign (const value_type&)
{
}

/*
    Прямой доступ к данным (только чтение)
*/

template <class T>
inline const typename array<T, 0>::value_type* array<T, 0>::data () const
{
  return 0;
}

template <class T>
inline typename array<T, 0>::value_type* array<T, 0>::data ()
{ 
  return 0;
}

/*
    Использование как С массива (чтение/запись)
*/

template <class T>
inline typename array<T, 0>::value_type* array<T, 0>::c_array ()
{ 
  return 0;
}

/*
    Информация о размере
*/

template <class T>
inline typename array<T, 0>::size_type array<T, 0>::size ()
{
  return 0;
}

template <class T>
inline bool array<T, 0>::empty ()
{
  return true;
}

template <class T>
inline typename array<T, 0>::size_type array<T, 0>::max_size ()
{
  return 0;
}

/*
    Поддержка итераторов
*/

template <class T>
inline typename array<T, 0>::iterator array<T, 0>::begin ()
{
  return iterator (reinterpret_cast<T*> (this));
}

template <class T>
inline typename array<T, 0>::const_iterator array<T, 0>::begin () const 
{
  return const_iterator (reinterpret_cast<const T*> (this));
}

template <class T>
inline typename array<T, 0>::iterator array<T, 0>::end ()
{
  return begin ();
}

template <class T>
inline typename array<T, 0>::const_iterator array<T, 0>::end () const
{
  return begin ();
}

/*
    Поддержка реверсных итераторов
*/
template <class T>
inline typename array<T, 0>::reverse_iterator array<T, 0>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T>
inline typename array<T, 0>::const_reverse_iterator array<T, 0>::rbegin() const
{
  return const_reverse_iterator (end ());
}

template <class T>
inline typename array<T, 0>::reverse_iterator array<T, 0>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T>
inline typename array<T, 0>::const_reverse_iterator array<T, 0>::rend () const
{
  return const_reverse_iterator (begin ());
}
    
/*
    Доступ к элементу (operator[])
*/

template <class T>
inline typename array<T, 0>::reference array<T, 0>::operator [] (size_type) 
{ 
  return failed_rangecheck ();
}

template <class T>
inline typename array<T, 0>::const_reference array<T, 0>::operator [] (size_type) const 
{
  return failed_rangecheck ();
}
    
/*
    Доступ к элементу с проверкой диапазона (at())
*/

template <class T>
inline typename array<T, 0>::reference array<T, 0>::at (size_type)
{
  return failed_rangecheck ();
}

template <class T>
inline typename array<T, 0>::const_reference array<T, 0>::at (size_type) const
{
  return failed_rangecheck ();
}

/*
    Доступ к первому (front) и последнему (back) элементам
*/

template <class T>
inline typename array<T, 0>::reference array<T, 0>::front () 
{ 
  return failed_rangecheck ();
}

template <class T>
inline typename array<T, 0>::const_reference array<T, 0>::front () const 
{
  return failed_rangecheck ();
}

template <class T>
inline typename array<T, 0>::reference array<T, 0>::back () 
{ 
  return failed_rangecheck ();
}

template <class T>
inline typename array<T, 0>::const_reference array<T, 0>::back () const 
{ 
  return failed_rangecheck ();
}

/*
   Обмен (за линейное время)
*/

template <class T>
inline void array<T, 0>::swap (array<T, 0>&)
{
}

/*
    Проверка диапазона
*/

template <class T>
inline typename array<T, 0>::reference array<T, 0>::failed_rangecheck ()
{
  throw stl::range_error ("attempt to access element of an empty array");
}

/*
===================================================================================================
    Сравнение
===================================================================================================
*/

template <class T, size_t N>
inline bool operator == (const array<T, N>& x, const array<T, N>& y)
{
  return stl::equal (x.begin (), x.end (), y.begin ());
}

template <class T, size_t N>
inline bool operator < (const array<T, N>& x, const array<T, N>& y)
{
  return stl::lexicographical_compare (x.begin (), x.end (), y.begin (), y.end ());
}

template <class T, size_t N>
inline bool operator != (const array<T, N>& x, const array<T, N>& y)
{
  return !(x == y);
}

template <class T, size_t N>
inline bool operator > (const array<T, N>& x, const array<T, N>& y)
{
  return y < x;
}

template <class T, size_t N>
inline bool operator <= (const array<T, N>& x, const array<T, N>& y)
{
  return !(y < x);
}

template <class T, size_t N>
inline bool operator >= (const array<T, N>& x, const array<T, N>& y)
{
  return !(x < y);
}

/*
===================================================================================================
    Обмен
===================================================================================================
*/

template <class T, size_t N>
inline void swap (array<T, N>& x, array<T, N>& y)
{
  x.swap (y);
}
