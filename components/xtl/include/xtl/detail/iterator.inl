namespace detail
{

/*
    Описание интерфейса итерируемого объекта
*/

template <class T> struct iterator_interface
{
  typedef T value_type;

  virtual ~iterator_interface () {}

  virtual T&                  get     () = 0;
  virtual bool                empty   () = 0;
  virtual void                next    () = 0;
  virtual void                prev    () = 0;
  virtual bool                equal   (const iterator_interface&) = 0;
  virtual iterator_interface* clone   () = 0;
  virtual void                release () { delete this; }
};

/*
    Операции итератора
*/

template <class Iter> void iterator_next (Iter& iter, stl::input_iterator_tag)
{
  throw xtl::bad_iterator_operation ();
}

template <class Iter> void iterator_next (Iter& iter, stl::forward_iterator_tag)
{
  ++iter;
}

template <class Iter> void iterator_prev (Iter& iter, stl::input_iterator_tag)
{
  throw xtl::bad_iterator_operation ();
}

template <class Iter> void iterator_prev (Iter& iter, stl::bidirectional_iterator_tag)
{
  --iter;
}

/*
    Хранителище для итератора
*/

template <class Iter> class iterator_base: public iterator_interface<typename stl::iterator_traits<Iter>::value_type>
{
  public:
    iterator_base (const Iter& in_iter) : iter (in_iter) {}
    
    bool equal (const iterator_interface& i)
    {
      const iterator_base* impl = dynamic_cast<const iterator_base*> (&i);

      return impl && iter == impl->iter;
    }    

  protected:
    Iter iter;
};

/*
    Реализация обычного итератора
*/

template <class Iter> class iterator_impl: public iterator_base<Iter>
{
  public:
    typedef iterator_base<Iter>                                    base;  
    typedef typename stl::iterator_traits<Iter>::iterator_category iterator_category;
    typedef typename base::value_type                              value_type;
  
    iterator_impl (const Iter& in_iter) : base (in_iter) {}
    
    bool                empty () { return !iter; }
    void                next  () { detail::iterator_next (iter, iterator_category ()); }
    void                prev  () { detail::iterator_prev (iter, iterator_category ()); }
    iterator_interface* clone () { return new iterator_impl (*this); }

    value_type& get ()
    {
      if (!iter)
        throw xtl::bad_iterator_dereference ();

      return *iter;
    }
};

/*
    Реализация интервального итератора
*/

template <class Iter> class range_iterator_impl: public iterator_base<Iter>
{
  public:
    typedef iterator_base<Iter>                                    base;
    typedef typename stl::iterator_traits<Iter>::iterator_category iterator_category;
    typedef typename base::value_type                              value_type;
  
    range_iterator_impl (const Iter& in_iter, const Iter& in_first, const Iter& in_last) : base (in_iter), first (in_first), last (in_last) {}
    
    value_type& get ()
    {
      if (iter == last)
        throw xtl::bad_iterator_dereference ();

      return *iter;
    }
    
    bool empty () { return iter == last; }
    
    void next ()
    {
      if (iter != last) detail::iterator_next (iter, iterator_category ());
      else              iter = first;
    }

    void prev ()
    {
      if (iter != first) detail::iterator_prev (iter, iterator_category ());
      else               iter = last;
    }

    iterator_interface* clone () { return new range_iterator_impl (*this); }

  private:
    Iter first, last;
};

/*
    Пустой итератор
*/

template <class T> struct empty_iterator_impl: public iterator_interface<T>
{
  T&                  get     () { throw xtl::bad_iterator_dereference (); }
  bool                empty   () { return true; }
  void                next    () {}
  void                prev    () {}
  iterator_interface* clone   () { return this; }
  void                release () {}

  bool equal (const iterator_interface& iter)
  {
    return this == &iter;
  }
  
  static empty_iterator_impl* instace_ptr ()
  {
    static char buffer [sizeof (empty_iterator_impl)];
    static empty_iterator_impl* iter = new (buffer) empty_iterator_impl;

    return iter;
  }
};

}

/*
    iterator
*/

/*
    Конструкторы / деструктор
*/

template <class T>
inline iterator<T>::iterator ()
  : impl (detail::empty_iterator_impl<T>::instace_ptr ())
  {}

template <class T>
inline iterator<T>::iterator (const iterator& i)
  : impl (i.impl->clone ())
  {}
  
template <class T> template <class T1>
inline iterator<T>::iterator (const iterator<T1>& i)
  : impl (i.impl->clone ())
  {}

template <class T> template <class Iter>
inline iterator<T>::iterator (Iter iter)
  : impl (new detail::iterator_impl<Iter> (iter))
  {}

template <class T> template <class Iter>
inline iterator<T>::iterator (Iter iter, Iter first, Iter last)
  : impl (new detail::range_iterator_impl<Iter> (iter, first, last))
  {}

template <class T>
inline iterator<T>::~iterator ()
{
  impl->release ();
}  

/*
    Присваивание
*/

template <class T>
inline iterator<T>& iterator<T>::operator = (const iterator& i)
{
  iterator (i).swap (*this);
  return *this;
}

template <class T> template <class T1>
inline iterator<T>& iterator<T>::operator = (const iterator<T1>& i)
{
  iterator (i).swap (*this);
  return *this;
}

template <class T> template <class Iter>
inline iterator<T>& iterator<T>::operator = (Iter i)
{
  iterator (i).swap (*this);
  return *this;
}

/*
    Очистка
*/

template <class T>
inline void iterator<T>::clear ()
{
  iterator ().swap (*this);
}

/*
    Проверка на пустоту
*/

template <class T>
inline bool iterator<T>::empty () const
{
  return impl->empty ();
}

template <class T>
inline iterator<T>::operator safe_bool () const
{
  return empty () ? 0 : &iterator::empty;
}

/*
    Селекторы
*/

template <class T>
inline typename iterator<T>::reference iterator<T>::operator * () const
{
  return impl->get ();
}

template <class T>
inline typename iterator<T>::pointer iterator<T>::operator -> () const
{
  return &impl->get ();
}

/*
    Инкремент / декремент
*/

template <class T>
inline iterator<T>& iterator<T>::operator ++ ()
{
  impl->next ();
  return *this;
}

template <class T>
inline iterator<T>& iterator<T>::operator -- ()
{
  impl->prev ();
  return *this;
}

template <class T>
inline iterator<T> iterator<T>::operator ++ (int)
{
  iterator tmp = *this;
  
  ++*this;
  
  return tmp;
}

template <class T>
inline iterator<T> iterator<T>::operator -- (int)
{
  iterator tmp = *this;
  
  --*this;
  
  return tmp;
}

/*
    Сравнение
*/

template <class T>
inline bool iterator<T>::operator == (const iterator& i) const
{
  return impl == i.impl || impl->equal (*i.impl) || !*this && !i;
}

template <class T>
inline bool iterator<T>::operator != (const iterator& i) const
{
  return !impl->equal (*i.impl) && (*this || i);
}

/*
    Обмен
*/

template <class T>
inline void iterator<T>::swap (iterator& i)
{
  iterator_interface* tmp = impl;
  impl                    = i.impl;
  i.impl                  = tmp;
}

template <class T>
inline void swap (iterator<T>& i1, iterator<T>& i2)
{
  i1.swap (i2);
}

template <class Iter>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter i)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (i);
}

template <class Iter>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (iter, first, last);
}

template <class Container>
inline iterator<typename Container::value_type> make_iterator (Container& c, size_t offset)
{
  typename Container::iterator iter = c.begin ();
  
  stl::advance (iter, offset);
  
  return make_iterator (iter, c.begin (), c.end ());
}

template <class Container>
inline iterator<const typename Container::value_type> make_iterator (const Container& c, size_t offset)
{
  typename Container::const_iterator iter = c.begin ();
  
  stl::advance (iter, offset);
  
  return make_iterator (iter, c.begin (), c.end ());
}
