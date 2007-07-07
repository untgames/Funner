/*
    Инициализация / разрушение
*/

template <class T>
inline void destroy_aux (T* x,false_type)
{
  x->~T ();  
}

template <class T>
inline void destroy_aux (T*,true_type)
{ }

template <class Iter> 
inline void destroy_aux (Iter first,Iter last,false_type)
{
  while (first != last) destroy (&*first++);
}

template <class Iter> 
inline void destroy_aux (Iter,Iter,true_type)
{  }

template <class T>
inline void destroy (T* x)
{
  destroy_aux (x,typename type_traits<T>::has_trivial_destructor ());
}

template <class Iter> 
inline void destroy (Iter first,Iter last)
{  
  destroy_aux (first,last,typename type_traits<typename iterator_traits<Iter>::value_type>::has_trivial_destructor ());
}

template <class T1,class T2>
inline void construct (T1* p,const T2& x)
{
  new (p) T1 (x);
}

template <class T>
inline void construct_aux (T* p,false_type)
{
  new (p) T ();
}

template <class T>
inline void construct_aux (T*,true_type)
{ }

template <class T>
inline void construct (T* p)
{
  construct_aux (p,typename type_traits<T>::has_trivial_default_constructor ());
}

/*
    Упрощение вызова копирующего конструктора
*/

template <class T> 
inline T copy (const T& x)
{
  return x;
}
