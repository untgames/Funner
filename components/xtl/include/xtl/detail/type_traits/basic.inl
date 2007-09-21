/*
    Проверка принадлежности типа к базовым категориям типов
*/

namespace detail
{

//вспомогательный класс для проверки свойства типа без учёта cv-модификаторов
template <template <class T1> class Tag, class T>
struct type_trait_checker: public false_type {}; //если тип неизвестен - то результат проверки свойства - ложь

template <template <class T1> class Tag, class T>
struct cv_type_trait_checker: public type_trait_checker<Tag, T> {};

template <template <class T1> class Tag, class T>
struct cv_type_trait_checker<Tag, const T>: public type_trait_checker<Tag, T> {};

template <template <class T1> class Tag, class T>
struct cv_type_trait_checker<Tag, volatile T>: public type_trait_checker<Tag, T> {};

template <template <class T1> class Tag, class T>
struct cv_type_trait_checker<Tag, const volatile T>: public type_trait_checker<Tag, T> {};

//специализации для проверки является ли T void-типом
template <> struct type_trait_checker<is_void, void>: public true_type {};

//специализации для проверки является ли T целочисленным типом
template <> struct type_trait_checker<is_integral, unsigned char>:  public true_type {};
template <> struct type_trait_checker<is_integral, unsigned short>: public true_type {};
template <> struct type_trait_checker<is_integral, unsigned int>:   public true_type {};
template <> struct type_trait_checker<is_integral, unsigned long>:  public true_type {};
template <> struct type_trait_checker<is_integral, signed char>:    public true_type {};
template <> struct type_trait_checker<is_integral, signed short>:   public true_type {};
template <> struct type_trait_checker<is_integral, signed int>:     public true_type {};
template <> struct type_trait_checker<is_integral, signed long>:    public true_type {};
template <> struct type_trait_checker<is_integral, bool>:           public true_type {};
template <> struct type_trait_checker<is_integral, char>:           public true_type {};
template <> struct type_trait_checker<is_integral, wchar_t>:        public true_type {};

#ifdef _MSC_VER

template <> struct type_trait_checker<is_integral, unsigned __int64>: public true_type {};
template <> struct type_trait_checker<is_integral, signed __int64>: public true_type {};

#endif

//специализации для проверки является ли T вещественным типом
template <> struct type_trait_checker<is_floating_point, float>:       public true_type {};
template <> struct type_trait_checker<is_floating_point, double>:      public true_type {};
template <> struct type_trait_checker<is_floating_point, long double>: public true_type {};

//специализации для проверки является ли T массивом
template <class T, size_t N> struct type_trait_checker<is_array, T [N]>:                public true_type {};
template <class T, size_t N> struct type_trait_checker<is_array, T const [N]>:          public true_type {};
template <class T, size_t N> struct type_trait_checker<is_array, T volatile [N]>:       public true_type {};
template <class T, size_t N> struct type_trait_checker<is_array, T const volatile [N]>: public true_type {};
template <class T>           struct type_trait_checker<is_array, T []>:                 public true_type {};
template <class T>           struct type_trait_checker<is_array, T const []>:           public true_type {};
template <class T>           struct type_trait_checker<is_array, T volatile []>:        public true_type {};
template <class T>           struct type_trait_checker<is_array, T const volatile []>:  public true_type {};

}

template <class T> struct is_void:           public detail::cv_type_trait_checker<is_void, T> {};
template <class T> struct is_integral:       public detail::cv_type_trait_checker<is_integral, T> {};
template <class T> struct is_floating_point: public detail::cv_type_trait_checker<is_floating_point, T> {};
template <class T> struct is_array:          public detail::cv_type_trait_checker<is_array, T> {};

/*
    Проверка является ли T объединением
*/

template <class T> struct is_union: public detail::type_traits_intrinsics::is_union<T> {};
      
/*
    Проверка наличия знака
*/

namespace detail
{

template <class T, bool is_a_integral=is_integral<T>::value || is_enum<T>::value>
struct is_signed_helper: public bool_constant<static_cast<T> (-1) < 0> {};

template <class T> struct is_signed_helper<T, false>: public false_type {};

template <class T, bool is_a_integral=is_integral<T>::value || is_enum<T>::value>
struct is_unsigned_helper: public bool_constant<(static_cast<T> (-1) > 0)> {};

template <class T> struct is_unsigned_helper<T, false>: public false_type {};

}

//проверка является ли T знаковым типом
template <class T> struct is_signed: public detail::is_signed_helper<T> {};

//проверка является ли T беззнаковым типом
template <class T> struct is_unsigned: public detail::is_unsigned_helper<T> {};

/*
    Проверка является ли T POD-типом
*/

template <class T> struct is_pod:
         public bool_constant<detail::type_traits_intrinsics::is_pod<T>::value || is_void<T>::value || is_scalar<T>::value> {};
         
template <class T, size_t N> struct is_pod<T [N]>: is_pod<T> {};

/*
    Проверка является ли T пустым типом
*/

template <class T> struct is_empty: public detail::type_traits_intrinsics::is_empty<T> {};

/*
    Проверка является ли T указателем на член класса
*/

template <class T>          struct is_member_pointer:         public is_member_function_pointer<T> {};
template <class T, class U> struct is_member_pointer<U T::*>: public true_type {};

/*
    Проверка является ли T указателем на функцию/указателем на функцию-член класса/указателем на поле класса
*/

namespace detail
{

template <class T> struct type_trait_checker<is_function, T>: public bool_constant<functional_traits<T>::is_function> {};
template <class T> struct type_trait_checker<is_member_function_pointer, T>: 
         public bool_constant<functional_traits<T>::is_memfun> {};

}

template <class T> struct is_function: public detail::cv_type_trait_checker<is_function, T> {};
template <class T> struct is_member_function_pointer: public detail::cv_type_trait_checker<is_member_function_pointer, T> {};
template <class T> struct is_member_object_pointer: 
         public bool_constant<is_member_pointer<T>::value && !is_member_function_pointer<T>::value> {};

/*
    Проверка принадлежности типа к объединённым категориям типов
*/

//проверка является ли T арифметическим типом 
template <class T> struct is_arithmetic: bool_constant<is_integral<T>::value || is_floating_point<T>::value> {};

//проверка является ли T фундаментальны типом 
template <class T> struct is_fundamental: bool_constant<is_arithmetic<T>::value || is_void<T>::value> {};

//проверка является ли T объектным типом (не ссылка, void или функция)
template <class T> struct is_object: bool_constant<!(is_reference<T>::value || is_void<T>::value || is_function<T>::value)> {};

//проверка является ли T скалярным типом
template <class T> struct is_scalar: bool_constant<is_arithmetic<T>::value || is_enum<T>::value || 
                                                       is_pointer<T>::value || is_member_pointer<T>::value> {};

//проверка является ли T не фундаментальным типом
template <class T> struct is_compound: bool_constant<!is_fundamental<T>::value> {};

/*
    Проверка сложных свойств типа, определение которых требует поддержки компилятора
*/

//проверка имеет ли тип T тривиальный конструктор по умолчанию
template <class T> struct has_trivial_constructor: 
         public bool_constant<detail::type_traits_intrinsics::has_trivial_constructor<T>::value || is_pod<T>::value> {};

//проверка имеет ли тип T тривиальный конструктор копирования
template <class T> struct has_trivial_copy:
         public bool_constant<(is_pod<T>::value || detail::type_traits_intrinsics::has_trivial_copy<T>::value) &&
                              !is_volatile<T>::value> {};

//проверка имеет ли тип T тривиальный оператор присваивания
template <class T> struct has_trivial_assign:
         public bool_constant<(is_pod<T>::value || detail::type_traits_intrinsics::has_trivial_assign<T>::value) &&
                               !is_const<T>::value && !is_volatile<T>::value> {};

//проверка имеет ли тип тривиальный деструктор
template <class T> struct has_trivial_destructor:
         public bool_constant<detail::type_traits_intrinsics::has_trivial_destructor<T>::value || is_pod<T>::value> {};

//проверка имеет ли тип виртуальный деструктор
template <class T> struct has_virtual_destructor: public detail::type_traits_intrinsics::has_virtual_destructor<T> {};

//проверка имеет ли тип T бессбойный конструктор по умолчанию
template <class T> struct has_nothrow_constructor:
         public bool_constant<has_trivial_constructor<T>::value || detail::type_traits_intrinsics::has_nothrow_constructor<T>::value> {};

//проверка имеет ли тип T бессбойный конструктор копирования
template <class T> struct has_nothrow_copy:
         public bool_constant<has_trivial_copy<T>::value || detail::type_traits_intrinsics::has_nothrow_copy<T>::value> {};

//проверка имеет ли тип T бессбойный оператор присваивания
template <class T> struct has_nothrow_assign:
         public bool_constant<has_trivial_assign<T>::value || detail::type_traits_intrinsics::has_nothrow_assign<T>::value> {};

/*
    Преобразование массивов
*/

namespace detail
{

//удаление всех размерностей массива
template <class T, bool is_array=is_array<T>::value>
struct remove_all_extents_helper: public remove_all_extents_helper<typename remove_extent<T>::type> {};

template <class T> struct remove_all_extents_helper<T, false> { typedef T type; };

//рекурсивное определение ранга массива (количество размерностей)
template <class T, size_t N>           struct rank_helper:                          public integral_constant<size_t, N> {};
template <class T, size_t R, size_t N> struct rank_helper<T [R], N>:                public rank_helper<T, N+1> {};
template <class T, size_t R, size_t N> struct rank_helper<T const [R], N>:          public rank_helper<T, N+1> {};
template <class T, size_t R, size_t N> struct rank_helper<T volatile [R], N>:       public rank_helper<T, N+1> {};
template <class T, size_t R, size_t N> struct rank_helper<T const volatile [R], N>: public rank_helper<T, N+1> {};
template <class T, size_t N>           struct rank_helper<T [], N>:                 public rank_helper<T, N+1> {};
template <class T, size_t N>           struct rank_helper<T const [], N>:           public rank_helper<T, N+1> {};
template <class T, size_t N>           struct rank_helper<T volatile [], N>:        public rank_helper<T, N+1> {};
template <class T, size_t N>           struct rank_helper<T const volatile [], N>:  public rank_helper<T, N+1> {};

//определение размерности массива
template <class T, size_t N>           struct extent_helper:                          public integral_constant<size_t, 0> {};
template <class T, size_t R, size_t N> struct extent_helper<T [R], N>:                public extent_helper<T, N-1> {};
template <class T, size_t R, size_t N> struct extent_helper<T const [R], N>:          public extent_helper<T, N-1> {};
template <class T, size_t R, size_t N> struct extent_helper<T volatile [R], N>:       public extent_helper<T, N-1> {};
template <class T, size_t R, size_t N> struct extent_helper<T const volatile [R], N>: public extent_helper<T, N-1> {};
template <class T, size_t N>           struct extent_helper<T [], N>:                 public extent_helper<T, N-1> {};
template <class T, size_t N>           struct extent_helper<T const [], N>:           public extent_helper<T, N-1> {};
template <class T, size_t N>           struct extent_helper<T volatile [], N>:        public extent_helper<T, N-1> {};
template <class T, size_t N>           struct extent_helper<T const volatile [], N>:  public extent_helper<T, N-1> {};
template <class T, size_t R>           struct extent_helper<T [R], 0>:                public integral_constant<size_t, R> {};
template <class T, size_t R>           struct extent_helper<T const [R], 0>:          public integral_constant<size_t, R> {};
template <class T, size_t R>           struct extent_helper<T volatile [R], 0>:       public integral_constant<size_t, R> {};
template <class T, size_t R>           struct extent_helper<T const volatile [R], 0>: public integral_constant<size_t, R> {};
template <class T>                     struct extent_helper<T [], 0>:                 public integral_constant<size_t, 0> {};
template <class T>                     struct extent_helper<T const [], 0>:           public integral_constant<size_t, 0> {};
template <class T>                     struct extent_helper<T volatile [], 0>:        public integral_constant<size_t, 0> {};
template <class T>                     struct extent_helper<T const volatile [], 0>:  public integral_constant<size_t, 0> {};

}

//удаление размерности массива
template <class T>           struct remove_extent        { typedef T type; };
template <class T, size_t N> struct remove_extent<T [N]> { typedef T type; };
template <class T>           struct remove_extent<T []>  { typedef T type; };

//удаление всех размерностей массива
template <class T> struct remove_all_extents: public detail::remove_all_extents_helper<T> {};

//определение ранга массива
template <class T> struct rank: public detail::rank_helper<T, 0> {};

//определение размера I-й размерности массива
template <class T, unsigned I> struct extent: public detail::extent_helper<T, I> {};
