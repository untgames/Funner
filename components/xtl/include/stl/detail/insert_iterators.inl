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
