#ifndef DESIGN_SMART_POINTER_HEADER
#define DESIGN_SMART_POINTER_HEADER

//#include <stddef.h>

namespace stl
{

//forward declarations
template <class T> class auto_ptr;

}

namespace design
{

//тэги приведения интеллектуальных указателей
struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица свойств интеллектуального указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ptr> struct pointer_traits
{
  typedef typename Ptr::element_type element_type;
  typedef typename Ptr::pointer      pointer;

  enum { has_destructive_copy = Ptr::has_destructive_copy };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица свойств обычного указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct pointer_traits<T*>
{
  typedef T  element_type;
  typedef T* pointer;

  enum { has_destructive_copy = false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица свойств auto_ptr<T>
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct pointer_traits<stl::auto_ptr<T> >
{
  typedef typename stl::auto_ptr<T>::element_type element_type;
  typedef element_type*                           pointer;

  enum { has_destructive_copy = true };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа параметров операции копирования указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,bool has_destructive_copy>
struct pointer_parameter_type
{ 
  typedef const T& result;
};

template <class T> struct pointer_parameter_type<T,false>
{
  typedef T& result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс интеллектуального указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ptr>
class smart_ptr_storage
{
  template <class Ptr1> friend class smart_ptr_storage;
  typedef Ptr stored_type;
  public:
    typedef typename pointer_traits<Ptr>::element_type element_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаг определяющий изменяет ли копирование этого объекта его состояние
///////////////////////////////////////////////////////////////////////////////////////////////////
    enum { has_destructive_copy = pointer_traits<stored_type>::has_destructive_copy };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение настоящего указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    element_type* get () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тестирование значения указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct test_type;

    operator test_type* () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ptr1> bool operator == (const smart_ptr_storage<Ptr1>&) const;
    template <class Ptr1> bool operator != (const smart_ptr_storage<Ptr1>&) const;
    template <class Ptr1> bool operator <  (const smart_ptr_storage<Ptr1>&) const;
    template <class Ptr1> bool operator >  (const smart_ptr_storage<Ptr1>&) const;
    template <class Ptr1> bool operator <= (const smart_ptr_storage<Ptr1>&) const;
    template <class Ptr1> bool operator >= (const smart_ptr_storage<Ptr1>&) const;

    bool operator == (const element_type*) const;
    bool operator != (const element_type*) const;
    bool operator <  (const element_type*) const;
    bool operator >  (const element_type*) const;
    bool operator <= (const element_type*) const;
    bool operator >= (const element_type*) const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа параметра
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> struct get_parameter_type: public pointer_parameter_type<T,has_destructive_copy> {};

    typedef typename get_parameter_type<smart_ptr_storage>::result parameter_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    smart_ptr_storage ();
    smart_ptr_storage (element_type*);
    smart_ptr_storage (parameter_type);

    template <class Ptr1> smart_ptr_storage (const smart_ptr_storage<Ptr1>&);
    template <class Ptr1> smart_ptr_storage (smart_ptr_storage<Ptr1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    smart_ptr_storage& operator = (element_type*);
    smart_ptr_storage& operator = (parameter_type);

    template <class Ptr1> smart_ptr_storage& operator = (const smart_ptr_storage<Ptr1>&);
    template <class Ptr1> smart_ptr_storage& operator = (smart_ptr_storage<Ptr1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ
///////////////////////////////////////////////////////////////////////////////////////////////////
    element_type&                                 operator *  () const;
    typename pointer_traits<stored_type>::pointer operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (smart_ptr_storage&);

  private:
    stored_type ptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ptr>
bool operator == (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

template <class Ptr>
bool operator != (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

template <class Ptr>
bool operator < (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

template <class Ptr>
bool operator > (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

template <class Ptr>
bool operator <= (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

template <class Ptr>
bool operator >= (const typename smart_ptr_storage<Ptr>::element_type*,const smart_ptr_storage<Ptr>&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T* get_pointer (T*);

template <class Ptr>
typename smart_ptr_storage<Ptr>::element_type* get_pointer (const smart_ptr_storage<Ptr>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1,class T2> T1* static_pointer_cast  (T2*);
template <class T1,class T2> T1* const_pointer_cast   (T2*);
template <class T1,class T2> T1* dynamic_pointer_cast (T2*);

#include <design/impl/ptr_base.inl>

}

#endif
