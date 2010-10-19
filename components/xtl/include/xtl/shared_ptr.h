/*
    XTL smart pointers
*/

#ifndef XTL_MEMORY_HEADER
#define XTL_MEMORY_HEADER

#include <exception>
#include <typeinfo>

#include <xtl/interlocked.h>

namespace stl
{

//implementation forwards
template <class T> class auto_ptr;

}


namespace xtl
{

//implementation forwards
template <class T> class weak_ptr;
template <class T> class enable_shared_from_this;

namespace detail
{

class shared_counter;

//определение типа ссылки
template <class T> struct shared_ptr_traits                      { typedef T&   reference; };
template <>        struct shared_ptr_traits<void>                { typedef void reference; };
template <>        struct shared_ptr_traits<const void>          { typedef void reference; };
template <>        struct shared_ptr_traits<volatile void>       { typedef void reference; };
template <>        struct shared_ptr_traits<const volatile void> { typedef void reference; };

//тэги приведения интеллектуальных указателей
struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};

//rvalue auto_ptr support based on a technique by Dave Abrahams
template <class T, class R> struct sp_enable_if_auto_ptr {};
template <class T, class R> struct sp_enable_if_auto_ptr<stl::auto_ptr<T>, R> { typedef R type; }; 

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интеллектуальный указатель с подсчётом ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class shared_ptr
{
  template <class T1> friend class shared_ptr;
  template <class T1> friend class weak_ptr;
  typedef T* (shared_ptr::*unspecified_bool_type)() const;
  public:
    typedef T                                                element_type;
    typedef T*                                               pointer;
    typedef typename detail::shared_ptr_traits<T>::reference reference;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
                                       shared_ptr ();
                                       shared_ptr (const shared_ptr&);
    template <class T1>                shared_ptr (const shared_ptr<T1>&);
    template <class T1>                shared_ptr (const shared_ptr<T1>&, detail::static_cast_tag);
    template <class T1>                shared_ptr (const shared_ptr<T1>&, detail::dynamic_cast_tag);
    template <class T1>                shared_ptr (const shared_ptr<T1>&, detail::const_cast_tag);
    template <class T1>       explicit shared_ptr (T1*);
    template <class T1>       explicit shared_ptr (const weak_ptr<T1>&);
    template <class T1>       explicit shared_ptr (stl::auto_ptr<T1>&);
    template <class Ptr>      explicit shared_ptr (Ptr p, typename detail::sp_enable_if_auto_ptr<Ptr, int>::type = 0);
    template <class T1, class Deleter> shared_ptr (T1*, Deleter);
                                      ~shared_ptr ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операторы присваивания
///////////////////////////////////////////////////////////////////////////////////////////////////
                         shared_ptr& operator = (const shared_ptr&);
    template <class T1>  shared_ptr& operator = (const shared_ptr<T1>&);
    template <class T1>  shared_ptr& operator = (stl::auto_ptr<T1>&);
    template <class Ptr> typename detail::sp_enable_if_auto_ptr<Ptr, shared_ptr&>::type operator = (Ptr);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции доступа
///////////////////////////////////////////////////////////////////////////////////////////////////
    pointer   get         () const;
    reference operator *  () const;
    pointer   operator -> () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о состоянии указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    long use_count () const; //количество ссылок на объект
    bool unique    () const; //является ли данный указатель единственным указателем на объект    
    
    operator unspecified_bool_type () const { return ptr ? &shared_ptr::get : 0; } //неявное приведение типа для проверки if (p)
    bool     operator !            () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение удаляющего функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Deleter> Deleter* get_deleter () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка нового значения
///////////////////////////////////////////////////////////////////////////////////////////////////
                                       void reset ();
    template <class T1>                void reset (T1*);
    template <class T1, class Deleter> void reset (T1*, Deleter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (shared_ptr&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T1> bool operator == (const shared_ptr<T1>&) const;
    template <class T1> bool operator != (const shared_ptr<T1>&) const;
    template <class T1> bool operator <  (const shared_ptr<T1>&) const;
    template <class T1> bool operator >  (const shared_ptr<T1>&) const;
    template <class T1> bool operator <= (const shared_ptr<T1>&) const;
    template <class T1> bool operator >= (const shared_ptr<T1>&) const;

  private:
    typedef detail::shared_counter shared_counter;

    template <class T1, class T2> void sp_enable_shared_from_this (const enable_shared_from_this<T1>*, const T2*) const;
                                  void sp_enable_shared_from_this (...) const;

  private:
    pointer         ptr;
    shared_counter* counter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение возникающее при обращении к недействительному указателю
///////////////////////////////////////////////////////////////////////////////////////////////////
class bad_weak_ptr: public std::exception
{
  public:
    const char* what () const throw () { return "xtl::bad_weak_ptr"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртка над указателем с подсчётом ссылок не увеличивающая числа ссылок на объект
///При попытке получения shared_ptr на недействительный объект генерирует исключение bad_weak_ptr
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class weak_ptr
{
  template <class T1> friend class shared_ptr;
  template <class T1> friend class weak_ptr;
  public:
    typedef T element_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
                        weak_ptr  ();
                        weak_ptr  (const weak_ptr&);    
    template <class T1> weak_ptr  (const shared_ptr<T1>&);
    template <class T1> weak_ptr  (const weak_ptr<T1>&);
                        ~weak_ptr ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операторы присваивания
///////////////////////////////////////////////////////////////////////////////////////////////////
                        weak_ptr& operator = (const weak_ptr&);
    template <class T1> weak_ptr& operator = (const weak_ptr<T1>&);
    template <class T1> weak_ptr& operator = (const shared_ptr<T1>&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о состоянии указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    long use_count () const; //количество ссылок на объект
    bool expired   () const { return use_count () == 0; } //результат истина если объект стал недействительным
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс / получение shared_ptr
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          reset ();
    shared_ptr<T> lock  () const; //если !expired (), то генерируется исключение bad_weak_ptr
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (weak_ptr&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T1> bool operator == (const weak_ptr<T1>&) const;
    template <class T1> bool operator != (const weak_ptr<T1>&) const;
    template <class T1> bool operator <  (const weak_ptr<T1>&) const;
    template <class T1> bool operator >  (const weak_ptr<T1>&) const;
    template <class T1> bool operator <= (const weak_ptr<T1>&) const;
    template <class T1> bool operator >= (const weak_ptr<T1>&) const;

  private:
    typedef detail::shared_counter shared_counter;

    template <class T1>      weak_ptr (T1*, shared_counter*);
    template <class T1> void assign   (T1*, detail::shared_counter*);

  private:
    T*              ptr;
    shared_counter* counter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс позволяющий получать shared_ptr на себя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class enable_shared_from_this
{
  template <class T1> friend class shared_ptr;
  public:
    shared_ptr<T>       shared_from_this ();
    shared_ptr<const T> shared_from_this () const;

  protected:
    enable_shared_from_this  () {}
    enable_shared_from_this  (const enable_shared_from_this&) {}
    ~enable_shared_from_this () {}

    enable_shared_from_this& operator = (const enable_shared_from_this&) { return *this; }

  private:
    mutable weak_ptr<T> weak_this;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого интеллектуальных указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void swap (shared_ptr<T>&, shared_ptr<T>&);
template <class T> void swap (weak_ptr<T>&, weak_ptr<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1, class T2> shared_ptr<T1> static_pointer_cast  (const shared_ptr<T2>&);
template <class T1, class T2> shared_ptr<T1> dynamic_pointer_cast (const shared_ptr<T2>&);
template <class T1, class T2> shared_ptr<T1> const_pointer_cast   (const shared_ptr<T2>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение указателя на удаляющий объект / plain-указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Deleter, class T> Deleter* get_deleter (const shared_ptr<T>&);
template <class T>                T*       get_pointer (T*);
template <class T>                T*       get_pointer (const shared_ptr<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение таблиц типов для указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace type_traits
{

template <class T> struct is_pointer;
template <class T> struct remove_pointer;

template <class T> struct is_pointer<shared_ptr<T> >     { enum { value = true }; };
template <class T> struct remove_pointer<shared_ptr<T> > { typedef T type; };

}

#ifdef XTL_HAS_INTERLOCKED
  #include <xtl/detail/shared_counter_interlocked.inl>  
#else
//#elif defined (ARM9)
  #include <xtl/detail/shared_counter_nothreads.inl>
//#else
//  #error Unknown platform
#endif

#include <xtl/detail/shared_counter.inl>
#include <xtl/detail/shared_ptr.inl>
#include <xtl/detail/weak_ptr.inl>

}

namespace tr1
{

using xtl::shared_ptr;
using xtl::weak_ptr;
using xtl::bad_weak_ptr;
using xtl::enable_shared_from_this;
using xtl::static_pointer_cast;
using xtl::dynamic_pointer_cast;
using xtl::const_pointer_cast;
using xtl::get_deleter;
using xtl::swap;

}

#endif
