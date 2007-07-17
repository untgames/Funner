/*
    ќпределение типа возвращаемого значени€ функционального объекта:
      1) если тип €вл€етс€ указателем на функцию или указателем на функцию-член,
         то тип результата определ€етс€ из сигнатуры: R f(Arg1, Arg2, ..., ArgN) -> R;
      2) если тип €вл€етс€ сигнатурой F(Arg1, ..., ArgN), то:
         2.1) если F - указатель на функцию или указатель на функцию-член, то тип возвращаемого 
              значени€ определ€етс€ из сигнатуры F;
         2.2) иначе (если F - пользовательский тип данных):
              2.2.1) если количество аргументов == 0 -> тип возвращаемого значени€ void;
              2.2.2) если F содержит тип result_type, то тип возвращаемого значени€ result_type
              2.2.3) иначе тип возвращаемого значени€ result<F(Arg1, ..., ArgN)>::type              
*/

namespace detail
{

//определение наличи€ типа Fn::result_type
struct no_result_type_tag {};

template <class T, class result_type = no_result_type_tag> struct has_result_type: public T
{
  typedef char                       no_type;
  typedef struct { char dummy [2]; } yes_type;

  static yes_type test (...);
  static no_type  test (void (*)(no_result_type_tag));
  
  enum { value = sizeof (test (static_cast<void (*)(result_type)> (0))) == sizeof (yes_type) };
};

//определение типа возвращаемого значени€ пользовательского типа Fn
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

//определение типа возвращаемого значени€ сигнатуры Fn
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

//определение типа возвращаемого значени€ если Fn не сигнатура
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

//комбинаци€ указанных выше случаев
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
