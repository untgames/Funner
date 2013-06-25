/*
    Приведение типов
*/

template <class From, class To>
struct static_caster
{
   To operator () (From& value) const { return static_cast<To> (value); }
};

template <class From, class To>
struct reinterpret_caster
{
   To operator () (From& value) const { return reinterpret_cast<To> (value); }
};

template <class From, class To>
struct dynamic_caster
{
  To& operator () (From& value) const { return dynamic_cast<To&> (value); }

  To* operator () (From* value) const
  {
    To* result = dynamic_cast<To*> (value); 

    if (!result)
      throw xtl::bad_any_cast (typeid (From), typeid (To*));

    return result;
  }
};

template <class From, class To>
struct dynamic_caster<From, To*>
{
  To* operator () (From& value) const
  {
    To* result = dynamic_cast<To*> (value); 

    if (!result)
      throw xtl::bad_any_cast (typeid (From), typeid (To*));

    return result;
  }
};

/*
    type_converter
*/

namespace detail
{

template <class T> struct converter_type                    { typedef T                                                type; };
template <class T> struct converter_type<T&>                { typedef typename converter_type<T*>::type                type; };
template <class T> struct converter_type<const T&>          { typedef typename converter_type<const T*>::type          type; };
template <class T> struct converter_type<volatile T&>       { typedef typename converter_type<volatile T*>::type       type; };
template <class T> struct converter_type<const volatile T&> { typedef typename converter_type<const volatile T*>::type type; };

template <class T> struct converter_remove_cv                    { typedef typename type_traits::remove_cv<T>::type type; };
template <class T> struct converter_remove_cv<const T*>          { typedef typename converter_remove_cv<T*>::type   type; };
template <class T> struct converter_remove_cv<volatile T*>       { typedef typename converter_remove_cv<T*>::type   type; };
template <class T> struct converter_remove_cv<const volatile T*> { typedef typename converter_remove_cv<T*>::type   type; };

template <class T> struct converter_get_cv                    { enum { is_const = type_traits::is_const<T>::value, is_volatile = type_traits::is_volatile<T>::value }; };
template <class T> struct converter_get_cv<const T*>          { enum { is_const = true, is_volatile = false }; };
template <class T> struct converter_get_cv<volatile T*>       { enum { is_const = false, is_volatile = true }; };
template <class T> struct converter_get_cv<const volatile T*> { enum { is_const = true, is_volatile = true }; };

struct type_converter_item
{
  const std::type_info* source_type;
  const std::type_info* target_type;
  void*                 handler;
  bool                  is_reinterpret_casted;
  type_converter_item*  next;

  type_converter_item () : source_type (), target_type (), handler (), is_reinterpret_casted (), next () {}
};

class type_converter
{
  public:
    type_converter ()
    {
      memset (items, 0, sizeof (items));
    }

    void add (const std::type_info& source_type, const std::type_info& result_type, type_converter_item* new_item)
    {
      size_t bucket_id = get_bucket_id (source_type, result_type);

      type_converter_item* item = items [bucket_id], *prev_item = 0;

      for (; item && !(item->source_type == &source_type && item->target_type == &result_type); prev_item=item, item=item->next);

      if (item)
        return; //double cast handlers

      if (prev_item)
      {
        new_item->next  = prev_item->next;
        prev_item->next = new_item;
      }
      else
      {
        new_item->next    = items [bucket_id];
        items [bucket_id] = new_item;
      }
    }
  
    const type_converter_item* find (const std::type_info& source_type, const std::type_info& result_type) const
    {
      size_t bucket_id = get_bucket_id (source_type, result_type);

      for (const type_converter_item* item = items [bucket_id]; item; item=item->next)
      {

        if (item->source_type == &source_type && item->target_type == &result_type)
          return item;
      }

      return 0;
    }

    static type_converter& instance () { return singleton_default<type_converter, false>::instance (); }

  private:
    static size_t get_hash (const std::type_info* t1, const std::type_info* t2)
    {
      return size_t (t1) ^ size_t (t2);
    }

    size_t get_bucket_id (const std::type_info& source_type, const std::type_info& result_type) const
    {
      return get_hash (&source_type, &result_type) % table_size;
    }

  private:
    enum { table_size = 24593 }; //простое число для размера хэш-таблицы

    type_converter_item* items [table_size];
};

template <class T> struct identity_converter_impl: public type_converter_item
{
  identity_converter_impl ()
  {
    source_type           = &typeid (T);
    target_type           = &typeid (T);
    is_reinterpret_casted = true;
    handler               = (void*)&cast;
  }

  static T& cast (T& value) { throw bad_any_cast (typeid (T), typeid (T)); } //do not remove reference at return type
};

template <class T> struct pointer_address_converter_impl: public type_converter_item
{
  pointer_address_converter_impl ()
  {
    source_type = &typeid (T);
    target_type = &typeid (T*);
    handler     = (void*)&cast;
  }

  static T* cast (T& value) { return &value; }
};

template <class T> struct type_converter_base_impl
{
  public:
    type_converter_base_impl ()
      : converter (type_converter::instance ())
      , identity_converter (singleton_default<identity_converter_impl<T>, false>::instance ())
      , pointer_address_converter (singleton_default<pointer_address_converter_impl<T>, false>::instance ())
    {
      add (typeid (T), &identity_converter);
      add (typeid (T*), &pointer_address_converter);
    }

    void add (const std::type_info& type, type_converter_item* new_item)
    {
      static const std::type_info& source_type = typeid (T);

      converter.add (source_type, type, new_item);
    }
  
    const type_converter_item* find (const std::type_info& type) const
    {
      static const std::type_info& source_type = typeid (T);

      return converter.find (source_type, type);
    }

  private:
    type_converter&                    converter;
    identity_converter_impl<T>&        identity_converter;
    pointer_address_converter_impl<T>& pointer_address_converter;
};

template <class T> struct type_converter_impl: public type_converter_base_impl<T> {};

struct custom_ref_caster_type_info
{
  const std::type_info* source_type;
  bool                  is_const;
  bool                  is_volatile;
};

template <class T> struct custom_ref_caster_type_info_impl: public custom_ref_caster_type_info
{
  typedef typename converter_remove_cv<typename converter_type<T>::type>::type from_type;

  custom_ref_caster_type_info_impl ()
  {
    source_type = &typeid (from_type);
    is_const    = converter_get_cv<T>::is_const;
    is_volatile = converter_get_cv<T>::is_volatile;

    singleton_default<declcast<T>, false>::instance ();
  }
};

template <class Ret>
inline const type_converter_item* find_converter (const custom_ref_caster_type_info& source_type, bool throwOnCV = false)
{
  if (source_type.is_const && !converter_get_cv<Ret>::is_const)
  {
    if (throwOnCV)
      throw xtl::bad_cv_any_cast (*source_type.source_type, typeid (Ret));

    return 0;
  }
    
  if (source_type.is_volatile && !converter_get_cv<Ret>::is_volatile)
  {
    if (throwOnCV)
      throw xtl::bad_cv_any_cast (*source_type.source_type, typeid (Ret));

    return 0;
  }

  typedef typename converter_remove_cv<Ret>::type To;

  if (&typeid (To) == source_type.source_type)
    return &singleton_default<identity_converter_impl<To>, false>::instance ();

  return type_converter::instance ().find (*source_type.source_type, typeid (To));
}

}

/*
    Преобразователь типов
*/

namespace detail
{

template <class To> struct identity_ref_caster
{
  To& operator () (void* source) const { return *reinterpret_cast<To*> (source); }
};

template <class To> struct identity_ref_caster<To&>
{
  To& operator () (void* source) const { return **reinterpret_cast<To**> (source); }
};

}

template <class To, class From>
inline To custom_cast (From& value)
{
  const detail::type_converter_item* item = detail::find_converter<typename detail::converter_type<To>::type> (singleton_default<detail::custom_ref_caster_type_info_impl<From>, false>::instance (), true);

  if (!item)
    throw bad_any_cast (typeid (From), typeid (To));

  if (item->is_reinterpret_casted)
    return detail::identity_ref_caster<To> ()(const_cast<void*> (&value));

  if (item->source_type == item->target_type)
    return const_cast<typename detail::converter_remove_cv<From>::type> (value);

  typedef To (*ConverterFn)(From&);
    
  return reinterpret_cast<ConverterFn> (item->handler)(value);
}

template <class From, class To>
inline bool has_custom_cast ()
{
  return detail::find_converter<typename detail::converter_type<To>::type> (singleton_default<detail::custom_ref_caster_type_info_impl<From>, false>::instance ()) != 0;
}

/*
    custom_ref_caster
*/

inline custom_ref_caster::custom_ref_caster ()
  : source ()
  , source_type (&singleton_default<detail::custom_ref_caster_type_info_impl<void*>, false>::instance ())
{
}

template <class From>
inline custom_ref_caster::custom_ref_caster (From& value)
  : source ((void*)&value)
  , source_type (&singleton_default<detail::custom_ref_caster_type_info_impl<From>, false>::instance ())
{
}

template <class From>
inline custom_ref_caster::custom_ref_caster (From*& value)
  : source (value ? (void*)&value : (void*)0)
  , source_type (&singleton_default<detail::custom_ref_caster_type_info_impl<From*>, false>::instance ())
{
}

template <class To>
inline To custom_ref_caster::cast () const
{
  const detail::type_converter_item* item = detail::find_converter<typename detail::converter_type<To>::type> (*source_type, true);

  if (!item)
    throw bad_any_cast (*source_type->source_type, typeid (To));

  if (item->is_reinterpret_casted)
    return detail::identity_ref_caster<To> ()(const_cast<void*> (source));

  if (item->source_type == item->target_type)
    return detail::identity_ref_caster<To> ()(const_cast<void*> (source));

  typedef To (*ConverterFn)(void*);
    
  return reinterpret_cast<ConverterFn> (item->handler)(source);  
}

template <class To>
inline bool custom_ref_caster::has_cast () const
{
  return detail::find_converter<typename detail::converter_type<To>::type> (*source_type) != 0;
}

inline bool custom_ref_caster::empty () const
{
  return source == 0;
}

inline const std::type_info& custom_ref_caster::type () const
{
  return *source_type->source_type;
}

template <class From>
inline custom_ref_caster make_custom_ref_caster (From& value)
{
  return custom_ref_caster (value);
}

/*
    declcast
*/

namespace detail
{

template <class From, class To, class CastTag>
struct declcast_impl_base: public type_converter_item
{
  declcast_impl_base ()
  {
    source_type = &typeid (From);
    target_type = &typeid (To);
    handler     = (void*)&cast;

    type_converter_impl<From>& converter_impl = singleton_default<type_converter_impl<From>, false>::instance ();

    converter_impl.add (*target_type, this);
  }

  static To cast (From& value) { return CastTag ()(value); }
};

template <class From, class To, class CastTag>
struct declcast_impl: public declcast_impl_base<From, To, CastTag>
{
};

template <class From, class To>
struct declcast_impl<From, To, reinterpret_caster<From, To> >: public declcast_impl_base<From, To, reinterpret_caster<From, To> >
{
  typedef declcast_impl_base<From, To, reinterpret_caster<From, To> > base;

  declcast_impl ()
  {
    base::is_reinterpret_casted = true;
  }
};

template <class From, class To, template <class, class> class CastTag>
struct cast_to_pointer
{
  To* operator () (From& value) const { From* value_ptr = &value; return CastTag<From*, To*> ()(value_ptr); }
  To& operator () (From* value) const { return *CastTag<From*, To*> ()(value); }
};

template <bool value> struct converter_selector {};

template <class From, class To, template <class, class> class CastTag, bool Value>
inline void register_converter (converter_selector<false> is_class, converter_selector<Value>)
{
  singleton_default<detail::declcast_impl<From, To, CastTag<From, To> >, false>::instance ();
}

template <class From, class To, template <class, class> class CastTag>
inline void register_converter (converter_selector<true> is_class, converter_selector<false> is_result_abstract)
{
  singleton_default<detail::declcast_impl<From, To, CastTag<From, To> >, false>::instance ();
  singleton_default<detail::declcast_impl<From*, To*, CastTag<From*, To*> >, false>::instance ();
  singleton_default<detail::declcast_impl<From, To*, cast_to_pointer<From, To, CastTag> >, false>::instance ();
  singleton_default<detail::declcast_impl<From*, To, cast_to_pointer<From, To, CastTag> >, false>::instance ();
}

template <class From, class To, template <class, class> class CastTag>
inline void register_converter (converter_selector<true> is_class, converter_selector<true> is_result_abstract)
{
  singleton_default<detail::declcast_impl<From*, To*, CastTag<From*, To*> >, false>::instance ();
  singleton_default<detail::declcast_impl<From, To*, cast_to_pointer<From, To, CastTag> >, false>::instance ();
}

template <class From>
inline void register_converter (converter_selector<false> is_abstract)
{
  singleton_default<detail::type_converter_impl<From>, false>::instance ();
}

template <class From>
inline void register_converter (converter_selector<true> is_abstract)
{
  singleton_default<detail::type_converter_impl<From*>, false>::instance ();
}

}

template <class FromT, class ToT, template <class, class> class CastTag>
class declcast
{
  public:
    declcast ()
    {
      typedef typename detail::converter_remove_cv<typename detail::converter_type<ToT>::type>::type   To;
      typedef typename detail::converter_remove_cv<typename detail::converter_type<FromT>::type>::type From;

      detail::register_converter<From, To, CastTag> (detail::converter_selector<type_traits::is_class<From>::value && type_traits::is_class<To>::value> (),
        detail::converter_selector<type_traits::is_abstract<To>::value> ());
    }
};

template <class FromT, class ToT, template <class, class> class CastTag> 
class declcast<FromT*, ToT*, CastTag>
{
  public:
    declcast ()
    {
      typedef typename detail::converter_remove_cv<typename detail::converter_type<ToT>::type>::type   To;
      typedef typename detail::converter_remove_cv<typename detail::converter_type<FromT>::type>::type From;      

      detail::register_converter<From, To, CastTag> (detail::converter_selector<type_traits::is_class<From>::value && type_traits::is_class<To>::value> (),
        detail::converter_selector<true> ());
    }
};

template <class T, template <class, class> class CastTag>
class declcast<T, T, CastTag>
{
  public:
    declcast ()
    {
      typedef typename detail::converter_remove_cv<typename detail::converter_type<T>::type>::type From;

      detail::register_converter<From> (detail::converter_selector<type_traits::is_abstract<From>::value> ());

      singleton_default<detail::declcast_impl<From, From*, detail::cast_to_pointer<From, From, reinterpret_caster> >, false>::instance ();
    }
};

template <class T, template <class, class> class CastTag>
class declcast<T*, T*, CastTag>
{
  public:
    declcast ()
    {
      typedef typename detail::converter_remove_cv<typename detail::converter_type<T>::type>::type* From;

      detail::register_converter<From> (detail::converter_selector<type_traits::is_abstract<From>::value> ());

      singleton_default<detail::declcast_impl<From, From*, detail::cast_to_pointer<From, From, reinterpret_caster> >, false>::instance ();
    }
};
