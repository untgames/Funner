/*
    Обработка посещений объектов разных типов
*/

template <class Ret> template <class T, class Fn>
inline Ret basic_visitor<Ret>::operator () (T& visited, Fn default_action)
{
  typedef visitor_node<T, Ret> visitor_impl;

  if (visitor_impl* visitor = dynamic_cast<visitor_impl*> (this))
    return visitor->visit (visited);

  return default_action (visited, *this);
}

template <class Ret> template <class T>
inline Ret basic_visitor<Ret>::operator () (T& visited)
{
  return (*this)(visited, default_visitor_action<Ret> ());
}
