#ifndef __MYSTL_ALLOC__
#define __MYSTL_ALLOC__

#include <new>
#include <stl/utility>
#include <stl/config.h>

#ifndef __MYSTL_STANDALONE__
  #include <common/heap.h>
#endif

namespace stl
{

//forward declarations
template <class T>                            class allocator;
template <class T,class AllocFn,class FreeFn> class simple_allocator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый аллокатор
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class allocator
{
  public:
    typedef typename non_const_type<T>::value_type  value_type;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef value_type*                             pointer;
    typedef value_type&                             reference;
    typedef const value_type*                       const_pointer;
    typedef const value_type&                       const_reference;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование аллокатора типа к аллокатору типа Other
///////////////////////////////////////////////////////////////////////////////////////////////////   
    template <class Other> struct rebind
    { 
      typedef typename default_allocator<Other>::allocator_type other; 
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы и присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////           
    allocator () throw () {}
    allocator (const allocator&) throw () {}
    
    template <class Other> allocator (const allocator<Other>&) throw () {}
    
    template <class Other> allocator& operator = (const allocator<Other>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение адреса / максимально возможное количество размещённых объектов3
///////////////////////////////////////////////////////////////////////////////////////////////////       
    pointer       address  (reference) const;
    const_pointer address  (const_reference) const;
    size_type     max_size () const throw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение / освобождение памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    pointer allocate   (size_type count,const void* hint = 0);
    void    deallocate (pointer ptr,size_type count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструирование / разрушение объектов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void construct (pointer ptr,const value_type& val);
    void destroy   (pointer ptr);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух аллокаторов 
///  - память выделенная через allocator<T> может освобождаться через allocator<Other>
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Other> bool operator == (const allocator<Other>&) const { return true; }
    template <class Other> bool operator != (const allocator<Other>&) const { return false; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание аллокатора данного типа
///Необходмио для обхода C2064 в MSVC71 (проблемы с дефолтным аргументом конструкторов контейнеров)
///////////////////////////////////////////////////////////////////////////////////////////////////               
    static allocator create () { return allocator (); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация аллокатора для типа void
///////////////////////////////////////////////////////////////////////////////////////////////////    
template <> class allocator<void>
{
  public:
    typedef void              value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;

    template <class Other> struct rebind  { typedef allocator<Other> other; };

    allocator () throw () {}
    allocator (const allocator&) throw () {}

    template <class Other> allocator (const allocator<Other>&) throw () {}

    template <class Other> allocator& operator = (const allocator<Other>&) { return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Простейший случай пользовательского аллокатора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class AllocFn=void* (*)(size_t),class FreeFn=void (*)(void*)>
class simple_allocator: public allocator<T>
{
  template <class T1,class AllocFn1,class FreeFn1> friend class simple_allocator;
  public:
    typedef typename allocator<T>::size_type        size_type;
    typedef typename allocator<T>::difference_type  difference_type;
    typedef typename allocator<T>::pointer          pointer;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование аллокатора типа к аллокатору типа Other
///////////////////////////////////////////////////////////////////////////////////////////////////   
    template <class Other> struct rebind
    { 
      typedef simple_allocator<Other,AllocFn,FreeFn> other;
    };  

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы и присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////           
    simple_allocator (AllocFn alloc,FreeFn free) throw ();
    simple_allocator (const simple_allocator&) throw ();
    
    template <class Other> 
    simple_allocator (const simple_allocator<Other,AllocFn,FreeFn>&) throw ();
    
    template <class Other> 
    simple_allocator& operator = (const simple_allocator<Other,AllocFn,FreeFn>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение / освобождение памяти
///////////////////////////////////////////////////////////////////////////////////////////////////        
    pointer allocate   (size_type count,const void* hint = 0);
    void    deallocate (pointer ptr,size_type count);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух аллокаторов 
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Other> bool operator == (const simple_allocator<Other,AllocFn,FreeFn>&) const;
    template <class Other> bool operator != (const simple_allocator<Other,AllocFn,FreeFn>&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание аллокатора данного типа (причина создания метода см. в классе allocator<T>)
///////////////////////////////////////////////////////////////////////////////////////////////////               
    static simple_allocator create () { return simple_allocator (); }
  
  private:
    AllocFn alloc;
    FreeFn  free;
};

#include <stl/detail/alloc.inl>
  
}

#endif
