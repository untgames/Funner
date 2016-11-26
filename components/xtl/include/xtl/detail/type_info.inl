namespace detail
{

/*
    Реализация информации о типе
*/

template <class T>
class type_info_impl: virtual public type_info
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

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4250) //'xtl::detail::functional_type_info_impl<T>' : inherits 'xtl::detail::type_info_impl<T>::xtl::detail::type_info_impl<T>::std_type' via dominance
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамическая информация о типе функционале
///////////////////////////////////////////////////////////////////////////////////////////////////
template <unsigned int ArgsCount> struct functional_type_info_arguments_holder
{
  const xtl::type_info* argument_types [ArgsCount];
};

template <> struct functional_type_info_arguments_holder<0>
{
  const xtl::type_info* argument_types [1];
};

template <class Traits, unsigned int I, unsigned int N> struct argument_types_initializer
{
  static void init (const xtl::type_info** types)
  {
    types [I] = &get_type<typename Traits::template argument<I+1>::type> ();
    
    argument_types_initializer<Traits, I+1, N>::init (types);
  }
};    

template <class Traits, unsigned int I> struct argument_types_initializer<Traits, I, I>
{
  static void init (const xtl::type_info**) {}
};

template <class T>
class functional_type_info_impl: virtual public functional_type_info, virtual public type_info_impl<T>,
  private functional_type_info_arguments_holder<functional_traits<T>::arguments_count>
{
  typedef functional_traits<T> traits_type;
  using functional_type_info_arguments_holder<traits_type::arguments_count>::argument_types;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    functional_type_info_impl ()
    {
      argument_types_initializer<traits_type, 0, traits_type::arguments_count>::init (argument_types);
    }  
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип возвращаемого значения
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const type_info& result_type () const { return get_type<typename traits_type::result_type> (); }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int arguments_count () const { return traits_type::arguments_count; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение типа аргумента (индексация с нуля)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const xtl::type_info& argument_type (unsigned int index) const
    {
      if (index < traits_type::arguments_count)
        return *argument_types [index];
        
      return get_type<void> ();
    }
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подстановка реализации при получении типа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, bool IsFn = type_traits::is_function<T>::value> struct type_info_impl_selector
{
  typedef type_info_impl<T> type;
};

template <class T> struct type_info_impl_selector<T, true>
{
  typedef functional_type_info_impl<T> type;
};

}

/*
    Получение информации о типе
*/

template <class T>
const type_info& get_type ()
{
  return singleton_default<typename detail::type_info_impl_selector<T>::type, false>::instance ();
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
