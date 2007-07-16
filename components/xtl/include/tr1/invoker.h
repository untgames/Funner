#ifndef MYTR1_INVOKER_HEADER
#define MYTR1_INVOKER_HEADER

#include <tr1/funcall.h>

namespace tr1
{

//implementation forwards
template <class T> class reference_wrapper;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс обработчика полиморфного вызова функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Base> struct invoker;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчика вызова функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Fn, class Base>
class functional_invoker: public Base
{
  typedef Base base;
  public:
    typedef Fn                          functional_type;
    typedef typename base::result_type  result_type;
    typedef typename base::arg1_type    arg1_type;
    typedef typename base::arg2_type    arg2_type;
    typedef typename base::arg3_type    arg3_type;
    typedef typename base::arg4_type    arg4_type;
    typedef typename base::arg5_type    arg5_type;
    typedef typename base::arg6_type    arg6_type;
    typedef typename base::arg7_type    arg7_type;
    typedef typename base::arg8_type    arg8_type;
    typedef typename base::arg9_type    arg9_type;

    functional_invoker (const Fn&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
          functional_type& function ();
    const functional_type& function () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () ();
    result_type operator () (arg1_type);
    result_type operator () (arg1_type, arg2_type);
    result_type operator () (arg1_type, arg2_type, arg3_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type);
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type);

  private:
    Fn fn;
};

#include <tr1/detail/invoker.inl>

}

#endif
