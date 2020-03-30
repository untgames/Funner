#include "shared.h"

using namespace engine;
using namespace script;
using namespace stl;

namespace
{

/*
    Constants
*/

const char* XTL_RATIONAL_LONG_LONG_LIBRARY = "Xtl.Rational.LongLong";
const char* BINDER_NAME                    = "Xtl.Rational";
const char* COMPONENT_NAME                 = "script.binds.Xtl.Rational";

/*
    Unary and binary operators
*/

template <class Ret, class Arg1, class Arg2, template <class, class, class> class Operation>
inline Invoker make_binary_invoker ()
{
  return make_invoker<Ret (Arg1, Arg2)> (Operation<Arg1, Arg2, Ret> ());
}

template <class Ret, class Arg1, class Arg2, template <class, class> class Operation>
inline Invoker make_binary_invoker ()
{
  return make_invoker<Ret (Arg1, Arg2)> (Operation<Arg1, Arg2> ());
}

template <class Ret, class Arg, template <class, class> class Operation>
inline Invoker make_unary_invoker ()
{
  return make_invoker<Ret (Arg)> (Operation<Arg, Ret> ());
}

/*
    Create function
*/

template <class T>
xtl::rational<T> create_rational_n (T n)
{
  return xtl::rational<T> (n);
}

template <class T>
xtl::rational<T> create_rational_nd (T n, T d)
{
  return xtl::rational<T> (n, d);
}

template <class T>
xtl::rational<T> create_default_rational ()
{
  return xtl::rational<T> ();
}

template <class T>
double get_rational_numerator (const xtl::rational<T>& r)
{
  return (double)r.numerator ();
}

template <class T>
double get_rational_denominator (const xtl::rational<T>& r)
{
  return (double)r.denominator ();
}

/*
    Register library for work with rational type
*/

template <class T>
void bind_rational_library (InvokerRegistry& rational_lib)
{
  typedef xtl::rational<T> rational_type;

    //create function registration

  rational_lib.Register ("Create", make_invoker (make_invoker (&create_rational_nd<T>),
                                                 make_invoker (&create_rational_n<T>),
                                                 make_invoker (&create_default_rational<T>)));

    //register getters

  rational_lib.Register ("get_Numerator",   make_invoker (&get_rational_numerator<T>));
  rational_lib.Register ("get_Denominator", make_invoker (&get_rational_denominator<T>));

    //register operators

  rational_lib.Register ("__unm", make_unary_invoker<rational_type, rational_type, negate> ());
  rational_lib.Register ("__add", make_invoker (make_binary_invoker<rational_type, rational_type, rational_type, plus> (),
                                           make_binary_invoker<rational_type, rational_type, T, plus> (),
                                           make_binary_invoker<rational_type, T, rational_type, plus> ()));
  rational_lib.Register ("__sub", make_invoker (make_binary_invoker<rational_type, rational_type, rational_type, minus> (),
                                           make_binary_invoker<rational_type, rational_type, T, minus> (),
                                           make_binary_invoker<rational_type, T, rational_type, minus> ()));
  rational_lib.Register ("__mul", make_invoker (make_binary_invoker<rational_type, rational_type, rational_type, multiplies> (),
                                           make_binary_invoker<rational_type, rational_type, T, multiplies> (),
                                           make_binary_invoker<rational_type, T, rational_type, multiplies> ()));
  rational_lib.Register ("__div", make_invoker (make_binary_invoker<rational_type, rational_type, rational_type, divides> (),
                                           make_binary_invoker<rational_type, rational_type, T, divides> (),
                                           make_binary_invoker<rational_type, T, rational_type, divides> ()));
  rational_lib.Register ("__eq", make_binary_invoker<bool, rational_type, rational_type, equal_to> ());
  rational_lib.Register ("__lt", make_binary_invoker<bool, rational_type, rational_type, less> ());
  rational_lib.Register ("__le", make_binary_invoker<bool, rational_type, rational_type, less_equal> ());

    //register functions

  rational_lib.Register ("Abs",      make_invoker (&xtl::abs<T>));
  rational_lib.Register ("Assign",   make_invoker (&rational_type::assign));
  rational_lib.Register ("ToNumber", make_invoker (&rational_type::template cast<double>));
}

/*
    Register xtl::rational library
*/

void bind_xtl_rational_library (Environment& environment)
{
    //create libraries
    
  InvokerRegistry rational_ll_lib = environment.CreateLibrary (XTL_RATIONAL_LONG_LONG_LIBRARY);
  
    //register libraries
  
  bind_rational_library<long long> (rational_ll_lib);
  
    //register data types

  environment.RegisterType<xtl::rational<long long> > (XTL_RATIONAL_LONG_LONG_LIBRARY);
}

}

namespace components
{

namespace xtl_script_bind
{

/*
    Component
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_xtl_rational_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> XtlRationalScriptBind (COMPONENT_NAME);

}

}

}
