/*
    Описание контейнера any
*/

namespace detail
{

/*
    Фабрика преобразователей по умолчанию
*/

class default_caster_factory
{
  struct default_caster: public basic_any_caster {};   
  public:
    template <class T> static basic_any_caster& get_caster ()
    {
      static default_caster caster;

      return caster;
    }
};

/*
    Интерфейс хранилища вариантных данных 
*/

struct any_holder
{
  virtual ~any_holder () {}

  virtual const std::type_info&   type       () = 0;
  virtual const basic_any_caster& get_caster () = 0;
  virtual any_holder*             clone      () = 0;
  virtual void                    release    () = 0;
};

/*
    Содержимое вариативной переменной
*/

template <class T> struct any_content: public any_holder
{
  any_content (const T& in_value) : value (in_value) {}

  const std::type_info& type () { return typeid (T); }

  T value;
};

/*
    Реализации вариативной переменной
*/

template <class T, class CasterFactory=default_caster_factory>
struct copyable_any_impl: public any_content<T>, public CasterFactory
{
  copyable_any_impl (const T& in_value, const CasterFactory& in_factory) : any_content<T> (in_value), CasterFactory (in_factory) {}

  any_holder* clone   () { return new copyable_any_impl (*this); }
  void        release () { delete this; }

  const basic_any_caster& get_caster () { return CasterFactory::get_caster<T> (); }
};

template <class T, class CasterFactory=default_caster_factory>
class referenced_any_impl: public any_content<T>, public CasterFactory
{
  public:
    referenced_any_impl (const T& in_value, const CasterFactory& in_factory)
       : any_content<T> (in_value), CasterFactory (in_factory), ref_count (1) {}

  any_holder* clone () { return ref_count++, this; }
  
  void release ()
  {
    if (!--ref_count)
      delete this;
  }

  const basic_any_caster& get_caster () { return CasterFactory::get_caster<T> (); }

  private:
    size_t ref_count;
};

}

/*
    any
*/

/*
    Конструкторы / деструктор
*/

inline any::any ()
  : content_ptr (0)
  {}

template <class T>
inline any::any (const T& value, bool ref_counted)
  : content_ptr (create_holder (value, detail::default_caster_factory (), ref_counted))
  {}

template <class T, class CasterFactory>
inline any::any (const T& value, const CasterFactory& factory, bool ref_counted)
  : content_ptr (create_holder (value, factory, ref_counted))
  {}

inline any::any (const any& in_any)
  : content_ptr (in_any.content_ptr ? in_any.content_ptr->clone () : 0)
  {}

inline any::~any ()
{
  if (content_ptr)
    content_ptr->release ();
}

/*
    Создание хранилища
*/

template <class T, class CasterFactory>
inline detail::any_holder* any::create_holder (const T& value, const CasterFactory& factory, bool ref_counted)
{
  return ref_counted ? static_cast<detail::any_holder*> (new detail::referenced_any_impl<T, CasterFactory> (value, factory)) :
                       static_cast<detail::any_holder*> (new detail::copyable_any_impl<T, CasterFactory> (value, factory));
}

/*
    Присваивание
*/

inline any& any::operator = (const any& in_any)
{
  any (in_any).swap (*this);
  return *this;
}

template <class T>
inline any& any::operator = (const T& value)
{
  any (value).swap (*this);
  return *this;
}

/*
    Проверка на пустоту
*/

inline bool any::empty () const
{
  return content_ptr == 0;
}

/*
    Тип и данные
*/

inline const std::type_info& any::type () const
{
  return content_ptr ? content_ptr->type () : typeid (void);
}

template <class T>
inline T* any::content ()
{
  return const_cast<T*> (const_cast<const any&> (*this).content<const T> ());
}

template <class T>
inline const T* any::content () const
{
  if (!content_ptr)
    return 0;

  return &typeid (T) == &content_ptr->type () ? &static_cast<const detail::any_content<T>*> (content_ptr)->value : 0;
}

/*
    Преобразователь
*/

inline basic_any_caster& any::get_caster () const
{
  return content_ptr ? content_ptr->get_caster () : detail::default_caster_factory::get_caster<void> ();
}

/*
    Обмен
*/

inline any& any::swap (any& in_any)
{
  detail::any_holder* tmp = content_ptr;
  content_ptr             = in_any.content_ptr;
  in_any.content_ptr      = tmp;
  
  return *this;
}
                       
inline void swap (any& a1, any& a2)
{
  a1.swap (a2);
}

/*
    Создание any с подсчётом ссылок
*/

template <class T>
inline any make_ref_any (const T& value)
{
  return any (value, true);
}

template <class T, class CasterFactory>
inline any make_ref_any (const T& value, const CasterFactory& factory)
{
  return any (value, factory, true);
}

/*
    Приведение типов
*/

template <class T>
inline T* any_cast (any* a)
{
  return a ? a->content<T> () : 0;
}

template <class T>
inline const T* any_cast (const any* a)
{
  return a ? a->content<const T> () : 0;
}

template <class T>
inline T any_cast (any& a)
{
  typedef typename remove_reference<T>::type nonref;

  nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}

template <class T>
inline const T any_cast (const any& a)
{
  typedef typename remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}

/*
    Полиморфное приведение типов
*/

template <class T>
inline T polymorphic_any_cast (any& a)
{
  typedef typename remove_reference<T>::type nonref;

  nonref* result = any_cast<nonref> (&a);

  return result ? *result : a.get_caster ().cast<T> (a);
}

template <class T>
inline const T polymorphic_any_cast (const any& a)
{
  typedef typename remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  return result ? *result : a.get_caster ().cast<T> (a);
}

/*
    Описание полиморфного приведения типов контейнера any
*/

/*
    basic_any_caster
*/

template <class T>
inline const T basic_any_caster::cast (const any& a) const
{
  const detail::any_caster<T>* caster = dynamic_cast<const detail::any_caster<T>*> (this);
  
  if (!caster)
    throw bad_any_cast ();
    
  return caster->cast (const_cast<any&> (a));
}

template <class T>
inline T basic_any_caster::cast (any& a) const
{
  return const_cast<T> (cast (const_cast<const any&> (a)));
}

/*
    Фабрика преобразователей типов
*/

namespace detail
{

/*
    Интерфейс преобразователя типов
*/

template <class DstT> class any_caster
{
  public:
    virtual DstT cast (any&) const = 0;

  protected:
    virtual ~any_caster () {}
};

/*
    Преобразователь static_cast
*/

template <class SrcT, class DstT> struct static_caster
{
  typedef DstT result_type;
  typedef SrcT argument_type;
  
  result_type operator () (const argument_type& value) const { return static_cast<result_type> (value); }
};

/*
    Преобразователь dynamic_cast
*/

template <class SrcT, class DstT> struct dynamic_caster
{
  typedef DstT result_type;
  typedef SrcT argument_type;
  
  result_type operator () (const argument_type& value) const { return dynamic_cast<result_type> (value); }
};

template <class SrcT, class DstT> struct dynamic_caster<SrcT, DstT*>
{
  typedef DstT* result_type;
  typedef SrcT  argument_type;
  
  result_type operator () (const argument_type& value) const
  {
    result_type result = dynamic_cast<result_type> (value);
    
    if (!result)
      throw bad_any_cast ();
      
    return result;
  }
};

/*
    Узел преобразователя типов
*/

template <template <class SrcT, class DstT> class Caster, class SrcT, class DstT>
struct any_caster_impl: public any_caster<DstT>
{
  DstT cast (any& a) const
  {
    SrcT* source = a.content<SrcT> ();

    if (!source)
      throw bad_any_cast ();

    return Caster<SrcT, DstT> ()(*source);
  }
};

template <template <class SrcT, class DstT> class Caster, class SrcT, class DstT, class Base>
struct any_caster_node: public Base, public any_caster_impl<Caster, SrcT, DstT> {};

template <template <class SrcT, class DstT> class Caster, class SrcT, class Head, class Tail, class Base>
struct any_caster_node<Caster, SrcT, mpl::type_node<Head, Tail>, Base>: public any_caster_node<Caster, SrcT, Tail, any_caster_node<Caster, SrcT, Head, Base> > {};

template <template <class SrcT, class DstT> class Caster, class SrcT, class Base>
struct any_caster_node<Caster, SrcT, mpl::null_type, Base>: public Base {};

}

template <template <class SrcT, class DstT> class Caster, class T1, class T2, class T3, class T4, class T5, class T6,
class T7, class T8, class T9, class T10> template <class SrcT>
inline basic_any_caster& caster_factory<Caster, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::get_caster ()
{
  using namespace mpl;

  typedef type_node<T1, type_node<T2, type_node<T3, type_node<T4, type_node<T5, type_node<T6, type_node<T7, type_node<T8, type_node<T9, type_node<T10, null_type> > > > > > > > > > type_list;

  static detail::any_caster_node<Caster, SrcT, type_list, basic_any_caster> caster;
  
  return caster;
}
