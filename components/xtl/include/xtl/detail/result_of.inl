/*
    Определение типа возвращаемого значения функционального объекта:
      1) если тип является указателем на функцию или указателем на функцию-член,
         то тип результата определяется из сигнатуры: R f(Arg1, Arg2, ..., ArgN) -> R;
      2) если тип является сигнатурой F(Arg1, ..., ArgN), то:
         2.1) если F - указатель на функцию или указатель на функцию-член, то тип возвращаемого 
              значения определяется из сигнатуры F;
         2.2) иначе (если F - пользовательский тип данных):
              2.2.1) если количество аргументов == 0 -> тип возвращаемого значения void;
              2.2.2) если F содержит тип result_type, то тип возвращаемого значения result_type
              2.2.3) иначе тип возвращаемого значения result<F(Arg1, ..., ArgN)>::type              
*/

namespace detail
{

template <class T> struct has_result_type
{
  private:
    typedef char                       no_type;
    typedef struct { char dummy [2]; } yes_type;  
  
    template <class T1> struct type_wrapper { typedef T1 type; };  

    template <class T1> static yes_type test (type_wrapper<T1> const volatile*, type_wrapper<typename T1::result_type>* = 0);

    static no_type test (...);

  public:
    enum { value = sizeof (test (static_cast<type_wrapper<T>*> (0))) == sizeof (yes_type) };
};

//определение типа возвращаемого значения пользовательского типа Fn
template <class Fn, class FnArgs, bool HasResultType=has_result_type<Fn>::value> struct user_fn_result_of
{
  typedef typename Fn::result_type type;
};

template <class Fn, class FnArgs> struct user_fn_result_of<Fn, FnArgs, false>
{
  typedef typename Fn::template result<FnArgs>::type type;
};

template <class Fn> struct user_fn_result_of<Fn, Fn(void), false>
{
  typedef void type;
};

//определение типа возвращаемого значения сигнатуры Fn
template <class Fn, class FnArgs, class Traits=functional_traits<Fn>,
          bool IsMemFunOrPtrFun=Traits::is_ptrfun || Traits::is_memfun> struct signature_result_of
{
  typedef typename Traits::result_type type;
};

template <class Fn, class FnArgs, class Traits> struct signature_result_of<Fn, FnArgs, Traits, false>
{
  typedef typename user_fn_result_of<Fn, FnArgs>::type type;
};

template <class T, class Ret, class FnArgs, class Traits> struct signature_result_of<Ret T::*, FnArgs, Traits, false>
{
  typedef Ret& type;
};

//определение типа возвращаемого значения если Fn не сигнатура
template <class Fn, class Traits, bool IsMemFunOrPtrFun = Traits::is_memfun || Traits::is_ptrfun> 
struct non_signature_result_of
{
  typedef typename Traits::result_type type;
};

template <class Fn, class Traits> struct non_signature_result_of<Fn, Traits, false>
{
  typedef typename Fn::result_type type;
};

template <class T, class Ret, class Traits> struct non_signature_result_of<Ret T::*, Traits, false>
{
  typedef Ret& type;
};

//комбинация указанных выше случаев
template <class Fn, class Traits = functional_traits<Fn>, bool IsSignature = Traits::is_function> struct result_of
{
  typedef typename signature_result_of<typename Traits::result_type, Fn>::type type;
};

template <class Fn, class Traits> struct result_of<Fn, Traits, false>
{
  typedef typename non_signature_result_of<Fn, Traits>::type type;
};

}

template <class Fn> struct result_of: public detail::result_of<Fn> {};
