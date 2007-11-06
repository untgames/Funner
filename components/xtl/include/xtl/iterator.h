#ifndef XTL_ITERATOR_HEADER
#define XTL_ITERATOR_HEADER

#include <typeinfo>
#include <stl/iterator_traits.h>

namespace stl
{

//forward declaration
template <class T1, class T2> struct pair;

}

namespace xtl
{

namespace detail
{

//implementation forwards
template <class T> struct iterator_interface;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключения:
///  iterator_exception - базовый класс исключений итератора
///  bad_iterator_operation - неверная операция (категория инстанцированного итератора не позволяет выполнить указанную операцию)
///  bad_iterator_dereference - попытка взятия значения в пустом итераторе
///////////////////////////////////////////////////////////////////////////////////////////////////
struct iterator_exception: public std::exception
{
  const char* what () const throw () { return "xtl::iterator_exception"; }
};

struct bad_iterator_operation: public iterator_exception
{
  const char* what () const throw () { return "xtl::bad_iterator_operation"; }
};

struct bad_iterator_dereference: public bad_iterator_operation
{
  const char* what () const throw () { return "xtl::bad_iterator_dereference"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип значений итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct iterator_value_type          { typedef T type; };
template <class T> struct iterator_value_type<const T> { typedef T type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Полиморфный итератор
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class iterator
{
  template <class T> friend class iterator;
  typedef bool (iterator::*safe_bool)() const;
  public:
    typedef typename iterator_value_type<T>::type value_type;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;
    typedef T*                                    pointer;
    typedef T&                                    reference;
    typedef stl::bidirectional_iterator_tag       iterator_category;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator  ();
    iterator  (const iterator&);
    ~iterator ();
    
    template <class Iter>           iterator (Iter iter);
    template <class Iter, class Fn> iterator (Iter iter, Fn selector);
    template <class Iter>           iterator (Iter iter, Iter first, Iter last);
    template <class Iter, class Fn> iterator (Iter iter, Iter first, Iter last, Fn selector);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator& operator = (const iterator&);

    template <class Iter> iterator& operator = (Iter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Селекторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    reference operator  * () const;
    pointer   operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инкремент / декремент
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator& operator ++ ();
    iterator& operator -- ();
    iterator  operator ++ (int);
    iterator  operator -- (int);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа хранимого итератора и возвращение указателя на него
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& target_type () const;

    template <class Iter>       Iter* target ();
    template <class Iter> const Iter* target () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка хранится ли итератор эквивалентный заданному
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Iter> bool contains (const Iter&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const iterator&) const;
    bool operator != (const iterator&) const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (iterator&);
    
  private:
    typedef detail::iterator_interface<value_type> iterator_interface;
    
    template <class Iter> static iterator_interface* create_dispatch (const Iter&, const Iter*);
                          static iterator_interface* create_dispatch (const iterator<value_type>&, const iterator<value_type>*);

  private:
    iterator_interface* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter, class T> bool operator == (const iterator<T>&, const Iter&);
template <class Iter, class T> bool operator != (const iterator<T>&, const Iter&);
template <class Iter, class T> bool operator == (const Iter&, const iterator<T>&);
template <class Iter, class T> bool operator != (const Iter&, const iterator<T>&);
template <class T>             bool operator == (const iterator<T>&, const iterator<const T>&);
template <class T>             bool operator == (const iterator<const T>&, const iterator<T>&);
template <class T>             bool operator != (const iterator<T>&, const iterator<const T>&);
template <class T>             bool operator != (const iterator<const T>&, const iterator<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void swap (iterator<T>&, iterator<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter>          iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter);
template <class T, class Iter> iterator<T>                                               make_iterator (Iter iter);

template <class Iter, class Fn>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Fn selector);

template <class T, class Iter, class Fn>
iterator<T> make_iterator (Iter iter, Fn selector);

template <class Iter>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last);

template <class T, class Iter>
iterator<T> make_iterator (Iter iter, Iter first, Iter last);

template <class Iter, class Fn>
iterator<typename stl::iterator_traits<Iter>::value_type> make_iterator (Iter iter, Iter first, Iter last, Fn selector);

template <class T, class Iter, class Fn>
iterator<T> make_iterator (Iter iter, Iter first, Iter last, Fn selector);

#include <xtl/detail/iterator.inl>

}

#endif
