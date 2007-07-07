/*
    Конструкторы
*/

template <class T,class Sequence>
inline stack<T,Sequence>::stack () 
  : c ()
  { }

template <class T,class Sequence>
inline stack<T,Sequence>::stack (const Sequence& seq) 
  : c (seq) 
  { }

/*
    Функции общего назначения
*/  

template <class T,class Sequence>  
inline bool stack<T,Sequence>::empty () const 
{ 
  return c.empty (); 
}

template <class T,class Sequence>  
inline typename stack<T,Sequence>::size_type stack<T,Sequence>::size () const 
{ 
  return c.size (); 
}

/*
    Доступ
*/

template <class T,class Sequence>  
inline typename stack<T,Sequence>::reference stack<T,Sequence>::top () 
{ 
  return c.back (); 
}

template <class T,class Sequence>  
inline typename stack<T,Sequence>::const_reference stack<T,Sequence>::top () const 
{ 
  return c.back (); 
}

/*
    Вставка / извлечение элементов
*/

template <class T,class Sequence>  
inline void stack<T,Sequence>::push (const value_type& x) 
{ 
  c.push_back (x); 
}

template <class T,class Sequence>  
inline void stack<T,Sequence>::pop () 
{ 
  c.pop_back (); 
}

/*
    Сравнение
*/

template <class T,class Sequence>
inline bool stack<T,Sequence>::operator == (const stack& x) const
{
  return c == x.c;
}

template <class T,class Sequence>
inline bool stack<T,Sequence>::operator < (const stack& x) const
{
  return c < x.c;
}


template <class T,class Sequence>
inline bool stack<T,Sequence>::operator != (const stack& x) const
{
  return !(*this == x);
}

template <class T,class Sequence>
inline bool stack<T,Sequence>::operator > (const stack& x) const
{
  return x < *this;
}

template <class T,class Sequence>
inline bool stack<T,Sequence>::operator <= (const stack& x) const
{
  return !(x < *this);
}

template <class T,class Sequence>
inline bool stack<T,Sequence>::operator >= (const stack& x) const
{
  return !(*this < x);
}
