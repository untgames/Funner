#ifndef XTL_BIND_HEADER
#define XTL_BIND_HEADER

#include <xtl/tuple>
#include <xtl/funcall.h>
#include <xtl/result_of.h>

namespace xtl
{

namespace detail
{

//implementation forwards
template <class Ret, class Fn, class T1=mpl::null_type, class T2=mpl::null_type, class T3=mpl::null_type, class T4=mpl::null_type,
          class T5=mpl::null_type, class T6=mpl::null_type, class T7=mpl::null_type, class T8=mpl::null_type, class T9=mpl::null_type>
struct binder;

struct unspecified_result;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подстановщик аргументов функциональных объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace placeholders
{

template <size_t I> struct argument {};

#ifdef _MSC_VER

static argument<1> _1;
static argument<2> _2;
static argument<3> _3;
static argument<4> _4;
static argument<5> _5;
static argument<6> _6;
static argument<7> _7;
static argument<8> _8;
static argument<9> _9;

#else

namespace
{

#ifdef __GNUC__
  #define XTL_UNUSED_ATTRIBUTE __attribute__((unused))
#else
  #define XTL_UNUSED_ATTRIBUTE
#endif

XTL_UNUSED_ATTRIBUTE argument<1> _1;
XTL_UNUSED_ATTRIBUTE argument<2> _2;
XTL_UNUSED_ATTRIBUTE argument<3> _3;
XTL_UNUSED_ATTRIBUTE argument<4> _4;
XTL_UNUSED_ATTRIBUTE argument<5> _5;
XTL_UNUSED_ATTRIBUTE argument<6> _6;
XTL_UNUSED_ATTRIBUTE argument<7> _7;
XTL_UNUSED_ATTRIBUTE argument<8> _8;
XTL_UNUSED_ATTRIBUTE argument<9> _9;

#undef XTL_UNUSED_ATTRIBUTE

}

#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка свойств типа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct is_placeholder;     //является ли тип подстановщиком аргементов функционального объекта
template <class T> struct is_bind_expression; //является ли тип выражением, полученным после вызова bind

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание bind-выражений
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn>
detail::binder<Ret, Fn> bind (Fn);

template <class Ret, class Fn, class T1>
detail::binder<Ret, Fn, T1> bind (Fn, T1);

template <class Ret, class Fn, class T1, class T2>
detail::binder<Ret, Fn, T1, T2> bind (Fn, T1, T2);

template <class Ret, class Fn, class T1, class T2, class T3>
detail::binder<Ret, Fn, T1, T2, T3> bind (Fn, T1, T2, T3);

template <class Ret, class Fn, class T1, class T2, class T3, class T4>
detail::binder<Ret, Fn, T1, T2, T3, T4> bind (Fn, T1, T2, T3, T4);

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
detail::binder<Ret, Fn, T1, T2, T3, T4, T5> bind (Fn, T1, T2, T3, T4, T5);

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6> bind (Fn, T1, T2, T3, T4, T5, T6);

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn, T1, T2, T3, T4, T5, T6, T7);

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn, T1, T2, T3, T4, T5, T6, T7, T8);

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9);

template <class Fn>
detail::binder<detail::unspecified_result, Fn> bind (Fn);

template <class Fn, class T1>
detail::binder<detail::unspecified_result, Fn, T1> bind (Fn, T1);

template <class Fn, class T1, class T2>
detail::binder<detail::unspecified_result, Fn, T1, T2> bind (Fn, T1, T2);

template <class Fn, class T1, class T2, class T3>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3> bind (Fn, T1, T2, T3);

template <class Fn, class T1, class T2, class T3, class T4>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4> bind (Fn, T1, T2, T3, T4);

template <class Fn, class T1, class T2, class T3, class T4, class T5>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5> bind (Fn, T1, T2, T3, T4, T5);

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6> bind (Fn, T1, T2, T3, T4, T5, T6);

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn, T1, T2, T3, T4, T5, T6, T7);

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn, T1, T2, T3, T4, T5, T6, T7, T8);

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка равенства bind-выражений
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret1, class Fn1, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19,
          class Ret2, class Fn2, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
bool function_equal (const detail::binder<Ret1, Fn1, T11, T12, T13, T14, T15, T16, T17, T18, T19>&,
                     const detail::binder<Ret2, Fn2, T21, T22, T23, T24, T25, T26, T27, T28, T29>&);

#include <xtl/detail/bind.inl>

}

namespace tr1
{

using xtl::bind;
using xtl::is_placeholder;
using xtl::is_bind_expression;

namespace placeholders
{

using namespace xtl::placeholders;

}

}

namespace
{

using namespace xtl::placeholders;

}

#endif
