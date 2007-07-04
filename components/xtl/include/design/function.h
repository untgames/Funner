#ifndef DESIGN_FUNCTION_HEADER
#define DESIGN_FUNCTION_HEADER

#include <design/functraits.h>
#include <design/refcount.h>
#include <design/utility.h>
#include <design/intrusive_ptr.h>
#include <exception>
#include <new>

namespace design
{

namespace detail
{

//implementation forwards
template <class Signature,class Base> struct invoker;
template <class Signature>            class  function_empty_invoker;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение, генерируемое при вызове пустой функции обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
struct bad_function_call: public std::exception
{
  const char* what () const throw () { return "bad function call"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функция обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
class function: public type_manip::function_traits<Signature>
{
  private:
    struct dummy { void nonnull () {} };
    
    typedef void (dummy::*safe_bool)();
    typedef detail::invoker<Signature,reference_counter>  invoker_type;
    typedef detail::function_empty_invoker<Signature>     empty_invoker_type;

  public:
    typedef Signature                               signature_type;
    typedef type_manip::function_traits<Signature>  traits_type;
    typedef typename traits_type::result_type       result_type;
    typedef typename traits_type::argument<1>::type arg1_type;
    typedef typename traits_type::argument<2>::type arg2_type;
    typedef typename traits_type::argument<3>::type arg3_type;
    typedef typename traits_type::argument<4>::type arg4_type;
    typedef typename traits_type::argument<5>::type arg5_type;
    typedef typename traits_type::argument<6>::type arg6_type;
    typedef typename traits_type::argument<7>::type arg7_type;
    typedef typename traits_type::argument<8>::type arg8_type;
    typedef typename traits_type::argument<9>::type arg9_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    function  ();
    function  (const function&);

    template <class Fn>               function (Fn fn);
    template <class Ptr,class MemFun> function (const Ptr&,const MemFun&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
                        function& operator = (const function&);
    template <class Fn> function& operator = (Fn);

                                      function& assign (const function&);
    template <class Fn>               function& assign (Fn);
    template <class Ptr,class MemFun> function& assign (const Ptr&,const MemFun&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка является ли даннаая функция обратного вызова пустой
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const { return empty () ? 0 : &dummy::nonnull; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () () const;
    result_type operator () (arg1_type) const;
    result_type operator () (arg1_type,arg2_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type,arg8_type) const;
    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type,arg8_type,arg9_type) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух функций обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
//    void swap (function&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> bool operator == (Fn) const;
    template <class Fn> bool operator != (Fn) const;
    
    template <class Signature,class Fn> friend bool operator == (Fn,const function<Signature>&);
    template <class Signature,class Fn> friend bool operator != (Fn,const function<Signature>&);

  private:
    template <class Fn> function& assign_internal (const Fn&);
    template <class Fn> static invoker_type* create_invoker (const Fn& fn);

  private:
    intrusive_ptr<invoker_type> invoker;
};

#ifndef DESIGN_INVOKER_HEADER
#define DESIGN_INVOKER_HEADER
  #include <design/impl/invoker.inl>
#endif

#include <design/impl/function.inl>

}

#endif
