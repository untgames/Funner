/*
    Конструкторы
*/

template <class T>
inline stride_ptr<T>::stride_ptr ()
  : ptr (0), stride (0)
  {}

template <class T>
inline stride_ptr<T>::stride_ptr (const pointer in_ptr, difference_type in_stride)
  : ptr (reinterpret_cast<char*> (in_ptr)), stride (in_stride)
  {}

template <class T>
inline stride_ptr<T>::stride_ptr (const stride_ptr& p)
  : ptr (p.ptr), stride (p.stride)
  {}

template <class T> template <class T1>
inline stride_ptr<T>::stride_ptr (const stride_ptr<T1>& p)
  : ptr (p.ptr), stride (p.stride)
  {}

/*
    Присваивание
*/

template <class T>
inline stride_ptr<T>& stride_ptr<T>::operator = (const stride_ptr& p)
{
  ptr    = p.ptr;
  stride = p.stride;

  return *this;
}

template <class T> template <class T1>
inline stride_ptr<T>& stride_ptr<T>::operator = (const stride_ptr<T1>& p)
{
  ptr    = p.ptr;
  stride = p.stride;

  return *this;
}

/*
    Операции доступа
*/

template <class T>
inline typename stride_ptr<T>::pointer stride_ptr<T>::get () const
{
  return reinterpret_cast<pointer> (ptr);
}

template <class T>
inline typename stride_ptr<T>::reference stride_ptr<T>::operator * () const
{
  return *reinterpret_cast<pointer> (ptr);
}

template <class T>
inline typename stride_ptr<T>::pointer stride_ptr<T>::operator -> () const
{
  return reinterpret_cast<pointer> (ptr);
}

template <class T>
inline typename stride_ptr<T>::reference stride_ptr<T>::operator [] (difference_type i) const
{
  return *reinterpret_cast<pointer> (ptr + i * stride);
}

/*
    Проверка указателя
*/

template <class T>
inline stride_ptr<T>::operator unspecified_bool_type () const
{
  return ptr ? &stride_ptr::get : 0;
}

/*
    Арифметика указателей
*/

template <class T>
inline stride_ptr<T>& stride_ptr<T>::operator ++ ()
{
  ptr += stride;
  return *this;
}

template <class T>
inline stride_ptr<T>& stride_ptr<T>::operator -- ()
{
  ptr -= stride;
  return *this;
}

template <class T>
inline stride_ptr<T>& stride_ptr<T>::operator += (difference_type offset)
{
  ptr += stride * offset;
  return *this;
}

template <class T>
stride_ptr<T>& stride_ptr<T>::operator -= (difference_type offset)
{
  ptr -= stride * offset;
  return *this;
}

template <class T>
inline stride_ptr<T> stride_ptr<T>::operator ++ (int)
{
  stride_ptr tmp = *this;
  
  ++*this;
  
  return tmp;
}

template <class T>
inline stride_ptr<T> stride_ptr<T>::operator -- (int)
{
  stride_ptr tmp = *this;
  
  --*this;
  
  return tmp;
}

template <class T>
inline stride_ptr<T> stride_ptr<T>::operator + (difference_type offset) const
{
  return stride_ptr (*this) += offset;
}

template <class T>
inline stride_ptr<T> stride_ptr<T>::operator - (difference_type offset) const
{
  return stride_ptr<T> (*this) -= offset;
}

/*
    Операторы сравнения
*/

template <class T> template <class T1>
inline bool stride_ptr<T>::operator == (const stride_ptr<T1>& p) const
{
  return ptr == p.ptr;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator != (const stride_ptr<T1>& p) const
{
  return !(*this == p);
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator < (const stride_ptr<T1>& p) const
{
  return ptr < p.ptr;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator > (const stride_ptr<T1>& p) const
{
  return p < *this;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator <= (const stride_ptr<T1>& p) const
{
  return !(p < *this);
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator >= (const stride_ptr<T1>& p) const
{
  return !(*this < p);
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator == (const T1* p) const
{
  return get () == p;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator != (const T1* p) const
{
  return !(*this == p);
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator < (const T1* p) const
{
  return get () < p;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator > (const T1* p) const
{
  return p < *this;
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator <= (const T1* p) const
{
  return !(p < *this);
}

template <class T> template <class T1>
inline bool stride_ptr<T>::operator >= (const T1* p) const
{
  return !(*this < p);
}

template <class T1, class T2>
inline bool operator == (const T1* p1, const stride_ptr<T2>& p2)
{
  return p1 == p2.get ();
}

template <class T1, class T2>
inline bool operator != (const T1* p1, const stride_ptr<T2>& p2)
{
  return !(p1 == p2);
}

template <class T1, class T2>
inline bool operator < (const T1* p1, const stride_ptr<T2>& p2)
{
  return p1 < p2.get ();
}

template <class T1, class T2>
inline bool operator > (const T1* p1, const stride_ptr<T2>& p2)
{
  return p2 < p1;
}

template <class T1, class T2>
inline bool operator <= (const T1* p1, const stride_ptr<T2>& p2)
{
  return !(p2 < p1);
}

template <class T1, class T2>
inline bool operator >= (const T1* p1, const stride_ptr<T2>& p2)
{
  return !(p1 < p2);
}

/*
    Получение plain-указателя
*/

template <class T>
inline T* get_pointer (const stride_ptr<T>& p)
{
  return p.get ();
}
