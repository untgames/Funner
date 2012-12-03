/*
===================================================================================================
    Array
===================================================================================================
*/

/*
    Конструктор
*/

template <class T, size_t N>
inline Array<T, N>::Array ()
  : count ()
{
}

template <class T, size_t N>
inline Array<T, N>::Array (size_t in_count, const T* in_items)
  : count ()
{
  Assign (in_count, in_items);
}

template <class T, size_t N>
inline Array<T, N>::Array (IArray<T>& source)
  : count ()
{
  source.CopyTo (*this);
}

/*
    Присваивание
*/

template <class T, size_t N>
inline void Array<T, N>::Assign (size_t in_count, const T* in_items)
{
  if (!in_items)
    in_count = 0;

  Resize (in_count);

  const T* src = in_items;
  T*       dst = items;

  for (size_t i=0; i<count; i++, src++, dst++)
    *dst = *src;
}

/*
    Количество элементов / проверка на пустоту
*/

template <class T, size_t N>
inline size_t Array<T, N>::Size () const
{
  return count;
}

template <class T, size_t N>
inline void Array<T, N>::Resize (size_t in_count)
{
  count = in_count;

  if (count > N)
    count = N;
}

template <class T, size_t N>
inline bool Array<T, N>::IsEmpty () const
{
  return count == 0;
}

/*
    Получение элемента
*/

template <class T, size_t N>
inline T& Array<T, N>::operator [] (size_t index)
{
  return items [index];
}

template <class T, size_t N>
inline const T& Array<T, N>::operator [] (size_t index) const
{
  return items [index];
}

/*
    Сравнение
*/

template <class T, size_t N>
inline bool Array<T, N>::operator == (const Array& a) const
{
  for (size_t i=0, cmp_count = count<a.count ? count : a.count; i<cmp_count; i++)
    if (items [i] != a.items [i])
      return false;

  return true;
}

template <class T, size_t N>
inline bool Array<T, N>::operator != (const Array& a) const
{
  return !(*this == a);
}

/*
===================================================================================================
    IArray
===================================================================================================
*/

/*
    Количество элементов / проверка на пустоту
*/

template <class T> 
inline size_t IArray<T>::Size () const
{
  return SizeCore (Selector ());
}

template <class T> 
bool IArray<T>::IsEmpty () const
{
  return Size () == 0;
}

/*
    Получение элемента
*/

template <class T> 
inline T IArray<T>::operator [] (size_t index) const
{
  return ItemCore (index, Selector ());
}

/*
    Копирование в массив
*/

template <class T> template <size_t N>
inline void IArray<T>::CopyTo (Array<T, N>& result) const
{
  result.Resize (0);

  size_t count = CopyToCore (N, &result [0], Selector ());

  result.Resize (count);
}
