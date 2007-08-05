/*
    Обработка посещений объектов разных типов
*/

template <class Ret, class DefaultVisitorAction> template <class T>
inline Ret basic_visitor<Ret, DefaultVisitorAction>::operator () (T& visited)
{
  typedef visitor_node<T, Ret> visitor_impl;

  if (visitor_impl* visitor = dynamic_cast<visitor_impl*> (this))
    return visitor->visit (visited);

  return static_cast<DefaultVisitorAction&> (*this)(visited, *this);
}

template <class Ret, class DefaultVisitorAction> template <class T>
inline Ret basic_visitor<Ret, DefaultVisitorAction>::operator () (T& visited) const
{
  typedef visitor_node<T, Ret> visitor_impl;

  if (const visitor_impl* visitor = dynamic_cast<const visitor_impl*> (this))
    return visitor->visit (visited);

  return static_cast<const DefaultVisitorAction&> (*this)(visited, *this);
}
