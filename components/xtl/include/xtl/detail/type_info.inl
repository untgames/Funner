namespace detail
{

/*
    Реализация преобразователя типов
*/

struct type_converter_item
{
  const std::type_info* type;
  void*                 handler;
  
  type_converter_item () : type (0), handler (0) {}
};

template <class Ret, class Arg>
Ret cast_value (Arg a)
{
  return static_cast<Ret> (a);
}

inline size_t get_type_hash (const std::type_info& type)
{
  return reinterpret_cast<size_t> (&type) / 57; //магическое простое число
}

template <class Ret, class Arg, bool Enabled = type_traits::is_convertible<Arg, Ret>::value && !(type_traits::is_reference<Ret>::value && !type_traits::is_reference<Arg>::value)>
struct type_converter_registrator
{
  enum { inc_count = 1 };

  static void register_converter (unsigned int count, type_converter_item* table)
  {
    size_t i;
  
    for (i=get_type_hash (typeid (Ret)) % count; table [i].type; i = i + 1 < count ? i + 1 : 0);
    
    table [i].type    = &typeid (Ret);
    table [i].handler = (void*)((Ret (*)(Arg))&xtl::detail::cast_value<Ret, Arg>);
  }
};

template <class Ret, class Arg> struct type_converter_registrator<Ret, Arg, false>
{
  enum { inc_count = 0 };
  
  static void register_converter (unsigned int, type_converter_item*)
  {
  }  
};

template <class Base, class T> struct type_converter_unit: public Base, private type_converter_registrator<T, typename Base::result_type>
{
  typedef type_converter_registrator<T, typename Base::result_type> registrator;

  enum { converters_count = Base::converters_count + registrator::inc_count };

  static void register_converter (unsigned int count, type_converter_item* table)
  {
    Base::register_converter (count, table);
    registrator::register_converter (count, table);
  }
};

template <class T> struct type_converter_root
{
  typedef T result_type;
  
  enum { converters_count = 0 };
  
  static void register_converter (unsigned int count, type_converter_item* table) { }
};

template <class T, class TList> struct type_converters_storage: private mpl::generate_linear_hierarchy<TList, type_converter_unit, type_converter_root<T> >::type
{
  typedef typename mpl::generate_linear_hierarchy<TList, detail::type_converter_unit, type_converter_root<T> >::type base;
  
  enum { converters_count = base::converters_count };
  
  type_converters_storage ()
  {
    register_converter (converters_count, items);
  }
  
  type_converter_item items [converters_count + 1]; //for case of no converters
};

struct type_converter_hash_table
{
  const type_converter_item* items;
  unsigned int               count;
  
  type_converter_hash_table (unsigned int in_count, const type_converter_item* in_items)
    : items (in_items)
    , count (in_count)
    {}
};

template <class T, class TList>
type_converter_hash_table get_type_converter_hash_table_helper1 (TList)
{
  typedef type_converters_storage<T, TList> storage;

  storage& s = singleton_default<storage, false>::instance ();
  
  return type_converter_hash_table (storage::converters_count, &s.items [0]);
}

template <class T, bool is_class=type_traits::is_class<T>::value> struct cast_type_clear
{
  typedef T type;
};

template <class T> struct cast_type_clear<T, true>
{
  typedef mpl::type_node<T, mpl::type_node<T*, mpl::type_node<T&, mpl::null_type> > > type;
};

template <class TList> struct cast_types_preprocess;

template <class Head, class Tail> struct cast_types_preprocess<mpl::type_node<Head, Tail> >
{
  typedef mpl::type_node<typename cast_type_clear<typename type_traits::remove_cv<Head>::type>::type, typename cast_types_preprocess<Tail>::type> type;
};

template <> struct cast_types_preprocess<mpl::null_type>
{
  typedef mpl::type_node<mpl::null_type, mpl::null_type> type;
};

template <class T, class TList>
type_converter_hash_table get_type_converter_hash_table_helper2 (TList)
{
  return get_type_converter_hash_table_helper1<T> (typename cast_types_preprocess<TList>::type ());
}

template <bool value> struct cast_types_selector {};

template <class T>
type_converter_hash_table get_type_converter_hash_table (cast_types_selector<true>)
{
  //+adl
  return get_type_converter_hash_table_helper2<T> (get_cast_types (static_cast<typename type_traits::remove_cv<typename type_traits::remove_reference<T>::type>::type*> (0)));
}

template <class T>
type_converter_hash_table get_type_converter_hash_table (cast_types_selector<false>)
{
  //+adl
  return get_type_converter_hash_table_helper2<T> (get_cast_types (T (0)));
}

template <class T> struct type_converter_impl: private type_converter_hash_table
{
  using type_converter_hash_table::count;
  using type_converter_hash_table::items;
  public:
    type_converter_impl ()
      : type_converter_hash_table (get_type_converter_hash_table<T> (cast_types_selector<type_traits::is_class<typename type_traits::remove_reference<T>::type>::value> ()))
    {
    }
  
    const type_converter_item* find (const std::type_info& type) const
    {
      if (!count)
        return 0;
    
      size_t i, back_count = count;
      
      for (i=get_type_hash (type) % count; back_count && items [i].type != &type; i = i + 1 < count ? i + 1 : 0, back_count--);

      if (!back_count)
        return 0;
        
      return &items [i];
    }
};

template <class Ret, class Arg>
const type_converter_item* find_converter ()
{
  if (type_traits::is_const<Arg>::value && !type_traits::is_const<Ret>::value)
    return 0;
    
  if (type_traits::is_volatile<Arg>::value && !type_traits::is_volatile<Ret>::value)
    return 0;

  typedef typename type_traits::remove_cv<Ret>::type To;
  typedef typename type_traits::remove_cv<Arg>::type From;

  type_converter_impl<From>& impl = singleton_default<type_converter_impl<From>, false>::instance ();
  
  const type_converter_item* item = impl.find (typeid (To));
  
  if (!item)
    return 0;
    
  return item;
}

/*
    Преобразование скалярных типов
*/

inline mpl::list<signed char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, 
  mpl::list<float, double, long double>::type>::type
 get_cast_types (long double)
{
  return mpl::list<signed char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, 
      mpl::list<float, double, long double>::type>::type ();
}

/*
    Реализация информации о типе
*/

template <class T>
class type_info_impl: public type_info
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Стандартная информация о типе
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& std_type () const                           { return typeid (T); }
    const char*           name     () const                           { return typeid (T).name (); }
    bool                  before   (const type_info& type) const      { return typeid (T).before (type.std_type ()) != 0; }
    bool                  before   (const std::type_info& type) const { return typeid (T).before (type) != 0; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка принадлежности типа к базовым категориям типов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool is_void () const                         { return xtl::type_traits::is_void<T>::value; }                     
    bool is_integral () const                     { return xtl::type_traits::is_integral<T>::value; }
    bool is_floating_point () const               { return xtl::type_traits::is_floating_point<T>::value; }
    bool is_array () const                        { return xtl::type_traits::is_array<T>::value; }
    bool is_pointer () const                      { return xtl::type_traits::is_pointer<T>::value; }
    bool is_reference () const                    { return xtl::type_traits::is_reference<T>::value; }
    bool is_member_object_pointer () const        { return xtl::type_traits::is_member_object_pointer<T>::value; }
    bool is_member_function_pointer () const      { return xtl::type_traits::is_member_function_pointer<T>::value; }
    bool is_enum () const                         { return xtl::type_traits::is_enum<T>::value; }
    bool is_union () const                        { return xtl::type_traits::is_union<T>::value; }
    bool is_class () const                        { return xtl::type_traits::is_class<T>::value; }
    bool is_function () const                     { return xtl::type_traits::is_function<T>::value; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка принадлежности типа к объединённым категориям типов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool is_arithmetic () const      { return xtl::type_traits::is_arithmetic<T>::value; }
    bool is_fundamental () const     { return xtl::type_traits::is_fundamental<T>::value; }
    bool is_object () const          { return xtl::type_traits::is_object<T>::value; }
    bool is_scalar () const          { return xtl::type_traits::is_scalar<T>::value; }
    bool is_compound () const        { return xtl::type_traits::is_compound<T>::value; }
    bool is_member_pointer () const  { return xtl::type_traits::is_member_pointer<T>::value; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка свойств типа
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool is_const () const                  { return xtl::type_traits::is_const<T>::value; }
    bool is_volatile () const               { return xtl::type_traits::is_volatile<T>::value; }
    bool is_pod () const                    { return xtl::type_traits::is_pod<T>::value; }
    bool is_empty () const                  { return xtl::type_traits::is_empty<T>::value; }
    bool is_polymorphic () const            { return xtl::type_traits::is_polymorphic<T>::value; }
    bool is_abstract () const               { return xtl::type_traits::is_abstract<T>::value; }
    bool has_trivial_constructor () const   { return xtl::type_traits::has_trivial_constructor<T>::value; }
    bool has_trivial_copy () const          { return xtl::type_traits::has_trivial_copy<T>::value; }
    bool has_trivial_assign () const        { return xtl::type_traits::has_trivial_assign<T>::value; }
    bool has_trivial_destructor () const    { return xtl::type_traits::has_trivial_destructor<T>::value; }
    bool has_nothrow_constructor () const   { return xtl::type_traits::has_nothrow_constructor<T>::value; }
    bool has_nothrow_copy () const          { return xtl::type_traits::has_nothrow_copy<T>::value; }
    bool has_nothrow_assign () const        { return xtl::type_traits::has_nothrow_assign<T>::value; }
    bool has_virtual_destructor () const    { return xtl::type_traits::has_virtual_destructor<T>::value; }
    bool is_signed () const                 { return xtl::type_traits::is_signed<T>::value; }
    bool is_unsigned () const               { return xtl::type_traits::is_unsigned<T>::value; }
    unsigned int alignment_of () const      { return xtl::type_traits::alignment_of<T>::value; }
    unsigned int rank () const              { return xtl::type_traits::rank<T>::value; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования константности типов
///////////////////////////////////////////////////////////////////////////////////////////////////
    const type_info& remove_const () const    { return get_type<typename xtl::type_traits::remove_const<T>::type> (); }
    const type_info& remove_volatile () const { return get_type<typename xtl::type_traits::remove_volatile<T>::type> (); }
    const type_info& remove_cv () const       { return get_type<typename xtl::type_traits::remove_cv<T>::type> (); }
    const type_info& add_const () const       { return get_type<typename xtl::type_traits::add_const<T>::type> (); }
    const type_info& add_volatile () const    { return get_type<typename xtl::type_traits::add_volatile<T>::type> (); }
    const type_info& add_cv () const          { return get_type<typename xtl::type_traits::add_cv<T>::type> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования ссылочных типов
///////////////////////////////////////////////////////////////////////////////////////////////////
    const type_info& remove_reference () const { return get_type<typename xtl::type_traits::remove_reference<T>::type> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление размерностей массива
///////////////////////////////////////////////////////////////////////////////////////////////////
    const type_info& remove_extent () const       { return get_type<typename xtl::type_traits::remove_extent<T>::type> (); }
    const type_info& remove_all_extents () const  { return get_type<typename xtl::type_traits::remove_all_extents<T>::type> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
    const type_info& remove_pointer () const  { return get_type<typename xtl::type_traits::remove_pointer<T>::type> (); }
};

}

/*
    Преобразователь типов
*/

template <class To, class From>
To custom_cast (From value)
{
  detail::type_converter_impl<From>& impl = singleton_default<detail::type_converter_impl<From>, false>::instance ();
  
  const detail::type_converter_item* item = detail::find_converter<To, From> ();
  
  if (!item)
    throw bad_any_cast (bad_any_cast::bad_cast, typeid (From), typeid (To));
    
  typedef To (*ConverterFn)(From);
    
  return reinterpret_cast<ConverterFn> (item->handler)(value);
}

template <class To, class From>
bool has_custom_cast ()
{
  const detail::type_converter_item* item = detail::find_converter<To, From> ();

  return item != 0;
}

/*
    Получение информации о типе
*/

template <class T>
const type_info& get_type ()
{
  return singleton_default<detail::type_info_impl<T>, false>::instance ();
}

template <class T>
const type_info& get_type (const T&)
{
  return xtl::get_type<const T> ();
}

template <class T>
const type_info& get_type (T&)
{
  return xtl::get_type<T> ();
}
