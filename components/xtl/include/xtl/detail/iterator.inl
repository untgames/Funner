namespace detail
{

/*
    Описание интерфейса итерируемого объекта
*/

template <class T> struct iterator_interface
{
  virtual ~iterator_interface () {}

  virtual const std::type_info&  target_type () = 0;
  virtual void*                  target      () = 0;
  virtual T&                     get         () = 0;
  virtual bool                   empty       () = 0;
  virtual void                   next        () = 0;
  virtual void                   prev        () = 0;
  virtual bool                   equal       (const iterator_interface&) = 0;
  virtual iterator_interface*    clone       () = 0;
  virtual size_t                 use_count   () = 0;
  virtual void                   addref      () = 0;
  virtual void                   release     () = 0;
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

template <class T, class Iter, class Fn> class iterator_base: public iterator_interface<T>
{
  public:
    iterator_base (const Iter& in_iter, const Fn& in_selector) : iter (in_iter), selector (in_selector), ref_count (1) {}
    iterator_base (const iterator_base& i) : iter (i.iter), selector (i.selector), ref_count (1) {}
    
    bool equal (const iterator_interface& i)
    {
      const iterator_base* impl = dynamic_cast<const iterator_base*> (&i);

      return impl && iter == impl->iter;
    }
    
    T& get () { return *const_cast<T*> (static_cast<const T*> (&selector (*iter))); }
    
    size_t use_count () { return ref_count; }
    
    void addref  () { ++ref_count; }
    void release ()
    {
      if (!--ref_count)
        delete this;
    }

    const std::type_info& target_type () { return typeid (Iter); }
    void*                 target      () { return &iter; }

  private:
    iterator_base& operator = (const iterator_base&); //no impl

  protected:
    Iter   iter;
    size_t ref_count;
    Fn     selector;
};

/*
    Реализация обычного итератора
*/

template <class T, class Iter, class Fn> class iterator_impl: public iterator_base<T, Iter, Fn>
{
  public:
    typedef iterator_base<T, Iter, Fn>                             base;  
    typedef typename stl::iterator_traits<Iter>::iterator_category iterator_category;
  
    iterator_impl (const Iter& in_iter, const Fn& in_selector) : base (in_iter, in_selector) {}
    
    bool                empty () { return !iter; }
    void                next  () { detail::iterator_next (iter, iterator_category ()); }
    void                prev  () { detail::iterator_prev (iter, iterator_category ()); }
    iterator_interface* clone () { return new iterator_impl (*this); }

    T& get ()
    {
      if (!iter)
        throw xtl::bad_iterator_dereference ();

      return base::get ();
    }    
};

/*
    Реализация интервального итератора
*/

template <class T, class Iter, class Fn> class range_iterator_impl: public iterator_base<T, Iter, Fn>
{
  public:
    typedef iterator_base<T, Iter, Fn>                             base;
    typedef typename stl::iterator_traits<Iter>::iterator_category iterator_category;
  
    range_iterator_impl (const Iter& in_iter, const Iter& in_first, const Iter& in_last, const Fn& in_selector) 
      : base (in_iter, in_selector), first (in_first), last (in_last) {}
    
    T& get ()
    {
      if (iter == last)
        throw xtl::bad_iterator_dereference ();

      return base::get ();
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
  const std::type_info& target_type () { return typeid (empty_iterator_impl); }
  void*                 target      () { return this; }
  T&                    get         () { throw xtl::bad_iterator_dereference (); }
  bool                  empty       () { return true; }
  void                  next        () {}
  void                  prev        () {}
  iterator_interface*   clone       () { return this; }
  size_t                use_count   () { return 1; }
  void                  addref      () {}
  void                  release     () {}

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

/*
    Селектор по умолчанию
*/

template <class T>
struct default_iterator_selector
{
                      T& operator () (T& value) const                { return value; }
  template <class T1> T& operator () (stl::pair<T1, T>& value) const { return value.second; }
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
  : impl (detail::empty_iterator_impl<value_type>::instace_ptr ())
  {}

template <class T>
inline iterator<T>::iterator (const iterator& i)
  : impl (i.impl)
{
  impl->addref ();
}

template <class T> template <class Iter>
inline iterator<T>::iterator (Iter iter)
  : impl (create_dispatch (iter, static_cast<Iter*> (0)))
  {}

template <class T> template <class Iter, class Fn>
inline iterator<T>::iterator (Iter iter, Fn selector)
  : impl (new detail::iterator_impl<value_type, Iter, Fn> (iter, selector))
  {}  

template <class T> template <class Iter>
inline iterator<T>::iterator (Iter iter, Iter first, Iter last)
  : impl (new detail::range_iterator_impl<value_type, Iter, detail::default_iterator_selector<value_type> > (iter, first, last, detail::default_iterator_selector<value_type> ()))
  {}

template <class T> template <class Iter, class Fn>
inline iterator<T>::iterator (Iter iter, Iter first, Iter last, Fn selector)
  : impl (new detail::range_iterator_impl<value_type, Iter, Fn> (iter, first, last, selector))
  {}  

template <class T>
inline iterator<T>::~iterator ()
{
  impl->release ();
}  

/*
    Диспетчер создания итератора    
*/

template <class T> template <class Iter>
inline typename iterator<T>::iterator_interface* iterator<T>::create_dispatch (const Iter& i, Iter*)
{
  using namespace detail;

  return new iterator_impl<value_type, Iter, default_iterator_selector<value_type> > (i, default_iterator_selector<value_type> ());
}

template <class T>
inline typename iterator<T>::iterator_interface* iterator<T>::create_dispatch (const iterator<value_type>& i, iterator<value_type>*)
{
  i.impl->addref ();
  
  return i.impl;
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
  if (impl->use_count () > 1)
    impl = impl->clone ();

  impl->next ();

  return *this;
}

template <class T>
inline iterator<T>& iterator<T>::operator -- ()
{
  if (impl->use_count () > 1)
    impl = impl->clone ();

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
    Определение типа хранимого итератора и возвращение указателя на него
*/

template <class T>
inline const std::type_info& iterator<T>::target_type () const
{
  return impl->target_type ();
}

template <class T> template <class Iter>
inline Iter* iterator<T>::target ()
{
  return &typeid (Iter) == &impl->target_type () ? static_cast<Iter*> (impl->target ()) : 0;
}

template <class T> template <class Iter>
inline const Iter* iterator<T>::target () const
{
  return &typeid (Iter) == &impl->target_type () ? static_cast<const Iter*> (impl->target ()) : 0;
}

/*
    Проверка хранится ли итератор эквивалентный заданному
*/

template <class T> template <class Iter>
inline bool iterator<T>::contains (const Iter& i) const
{
  if (const Iter* this_iter = target<Iter> ())
  {
    return *this_iter == i;
  }
  else if (const Iter* this_iter = target<const Iter> ())
  {
    return *this_iter == i;
  }
  else return false;
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
  return !(*this == i);
}

template <class Iter, class T>
inline bool operator == (const iterator<T>& i1, const Iter& i2)
{
  return i1.contains (i2);
}

template <class Iter, class T>
inline bool operator != (const iterator<T>& i1, const Iter& i2)
{
  return !(i1 == i2);
}

template <class Iter, class T>
inline bool operator == (const Iter& i1, const iterator<T>& i2)
{
  return i2.contains (i1);
}

template <class Iter, class T>
inline bool operator != (const Iter& i1, const iterator<T>& i2)
{
  return !(i1 == i2);
}

template <class T>
inline bool operator == (const iterator<T>& i1, const iterator<const T>& i2)
{
  return i1 == iterator<T> (i2);
}

template <class T>
inline bool operator == (const iterator<const T>& i1, const iterator<T>& i2)
{
  return iterator<T> (i1) == i2;
}

template <class T>
inline bool operator != (const iterator<T>& i1, const iterator<const T>& i2)
{
  return !(i1 == i2);
}

template <class T>
inline bool operator != (const iterator<const T>& i1, const iterator<T>& i2)
{
  return !(i1 == i2);
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

/*
    Создание итератора
*/

template <class T, class Iter>
inline iterator<T> make_iterator (Iter i)
{
  return iterator<T> (i);
}

template <class Iter>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter i)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (i);
}

template <class T, class Iter, class Fn>
inline iterator<T> make_iterator (Iter i, Fn selector)
{
  return iterator<T> (i, selector);
}

template <class Iter, class Fn>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter i, Fn selector)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (i, selector);
}

template <class T, class Iter>
inline iterator<T> make_iterator (Iter iter, Iter first, Iter last)
{
  return iterator<T> (iter, first, last);
}

template <class Iter>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (iter, first, last);
}

template <class T, class Iter, class Fn>
inline iterator<T> make_iterator (Iter iter, Iter first, Iter last, Fn selector)
{
  return iterator<T> (iter, first, last, selector);
}

template <class Iter, class Fn>
inline iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last, Fn selector)
{
  return iterator<typename stl::iterator_traits<Iter>::value_type> (iter, first, last, selector);
}
