#ifndef XTL_FUNCTION_HEADER
#define XTL_FUNCTION_HEADER

#include <exception>
#include <new>
#include <typeinfo>

#include <stl/string_fwd>

#include <xtl/default_cast_type.h>
#include <xtl/function_equal.h>
#include <xtl/invoker.h>
#include <xtl/reference_counter.h>
#include <xtl/utility>

namespace xtl
{

namespace detail
{

//implementation forwards
struct function_invoker_base;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение, генерируемое при вызове пустой функции обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
struct bad_function_call: public std::exception
{
  const char* what () const throw () { return "xtl::bad_function_call"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функция обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> class function
{
  typedef void (function::*safe_bool)();
  typedef xtl::invoker<Signature, detail::function_invoker_base> invoker_type;
  public:
    typedef Signature                          signature_type;
    typedef typename invoker_type::arg1_type   arg1_type;
    typedef typename invoker_type::arg2_type   arg2_type;
    typedef typename invoker_type::arg3_type   arg3_type;
    typedef typename invoker_type::arg4_type   arg4_type;
    typedef typename invoker_type::arg5_type   arg5_type;
    typedef typename invoker_type::arg6_type   arg6_type;
    typedef typename invoker_type::arg7_type   arg7_type;
    typedef typename invoker_type::arg8_type   arg8_type;
    typedef typename invoker_type::arg9_type   arg9_type;
    typedef typename invoker_type::result_type result_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
                                function  ();
                                function  (const function&);
    template <class Fn>         function  (Fn fn);
    template <class Signature1> function  (const function<Signature1>&);
                                ~function ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
                                function& operator = (const function&);
    template <class Fn>         function& operator = (Fn);
    template <class Signature1> function& operator = (const function<Signature1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа хранимого функционального объекта и возвращение указателя на него
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& target_type () const;

    template <class T>       T* target ();
    template <class T> const T* target () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка хранит ли функция функциональный объект эквивалентный заданному
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> bool contains (const Fn&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дамп
///////////////////////////////////////////////////////////////////////////////////////////////////
    void to_string (stl::string&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка является ли даннаая функция обратного вызова пустой
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const { return empty () ? 0 : &function::dummy; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () () const;
    result_type operator () (arg1_type) const;
    result_type operator () (arg1_type, arg2_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух функций обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (function&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> bool operator == (Fn) const;
    template <class Fn> bool operator != (Fn) const;

    template <class Signature1, class Fn> friend bool operator == (Fn, const function<Signature1>&);
    template <class Signature1, class Fn> friend bool operator != (Fn, const function<Signature1>&);

  private:                        
    template <class Fn> static invoker_type* create_invoker (Fn&);
    template <class Fn> static invoker_type* create_invoker (const reference_wrapper<Fn>&);
    template <class Fn> static invoker_type* create_invoker (Fn&, true_type);
    template <class Fn> static invoker_type* create_invoker (Fn&, false_type);
                        static invoker_type* create_invoker (null_ptr_type);

    template <class Fn> class invoker_impl;
    class empty_invoker_impl;
    
    void dummy () {}

  private:
    invoker_type* invoker;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух функций обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
void swap (function<Signature>&, function<Signature>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дамп
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
void to_string (stl::string& buffer, const function<Signature>& fn);

#include <xtl/detail/function.inl>

}

namespace tr1
{

using xtl::bad_function_call;
using xtl::function;
using xtl::swap;
using xtl::null;

}

#endif
