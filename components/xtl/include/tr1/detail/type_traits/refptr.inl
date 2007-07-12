#ifndef MYTR1_TYPE_TRAITS_REFPTR_HEADER
#define MYTR1_TYPE_TRAITS_REFPTR_HEADER

/*
    Преобразование указателей и ссылок
*/

//добавление/удаление ссылки
template <class T> struct remove_reference                   { typedef T    type; };
template <class T> struct remove_reference<T&>               { typedef T    type; };
template <class T> struct add_reference                      { typedef T&   type; };
template <class T> struct add_reference<T&>                  { typedef T&   type; };
template <>        struct add_reference<void>                { typedef void type; };
template <>        struct add_reference<const void>          { typedef void type; };
template <>        struct add_reference<volatile void>       { typedef void type; };
template <>        struct add_reference<const volatile void> { typedef void type; };

//добавление/удаление указателя
template <class T> struct remove_pointer                    { typedef T type; };
template <class T> struct remove_pointer<T*>                { typedef T type; };
template <class T> struct remove_pointer<T* const>          { typedef T type; };
template <class T> struct remove_pointer<T* volatile>       { typedef T type; };
template <class T> struct remove_pointer<T* const volatile> { typedef T type; };
template <class T> struct add_pointer                       { typedef typename remove_reference<T>::type* type; };

//проверка является ли T указателем
template <class T> struct is_pointer:                    public false_type {};
template <class T> struct is_pointer<T*>:                public true_type {};
template <class T> struct is_pointer<T* const>:          public true_type {};
template <class T> struct is_pointer<T* volatile>:       public true_type {};
template <class T> struct is_pointer<T* const volatile>: public true_type {};

//проверка является ли T ссылкой
template <class T> struct is_reference:     public false_type {};
template <class T> struct is_reference<T&>: public true_type {};

#endif
