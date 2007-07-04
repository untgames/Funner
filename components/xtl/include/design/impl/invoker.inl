namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс обработчиков вызова функторов для разного числа аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct invoker_empty_root {};

template <class Signature,class Base=invoker_empty_root> struct invoker;

//интерфейс обработчика вызова функтора без аргументов
template <class Ret,class Base>
struct invoker<Ret (),Base>: public Base
{
  virtual Ret operator () () = 0;
};

//интерфейс обработчика вызова функтора с 1-м аргументом
template <class Arg,class Ret,class Base>
struct invoker<Ret (Arg),Base>: public Base
{
  virtual Ret operator () (Arg) = 0;
};

//интерфейс обработчика вызова функтора с 2-мя аргументами
template <class Arg1,class Arg2,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2) = 0;
};

//интерфейс обработчика вызова функтора с 3-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3) = 0;
};

//интерфейс обработчика вызова функтора с 4-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4) = 0;
};

//интерфейс обработчика вызова функтора с 5-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4,Arg5),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4,Arg5) = 0;
};

//интерфейс обработчика вызова функтора с 6-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) = 0;
};

//интерфейс обработчика вызова функтора с 7-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) = 0;
};

//интерфейс обработчика вызова функтора с 8-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) = 0;
};

//интерфейс обработчика вызова функтора с 9-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret,class Base>
struct invoker<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9),Base>: public Base
{
  virtual Ret operator () (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик вызовов функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature,class Fn,class Base=invoker_empty_root>
class functor_invoker: public Base
{
  public:
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

    functor_invoker (const Fn& _fn) : fn (_fn) { }
    functor_invoker (const functor_invoker& invoker) : Base (invoker), fn (invoker.fn) {}

    functor_invoker& operator = (const functor_invoker& invoker)
    {
      Base::operator = (invoker);
      fn = invoker.fn;
      return *this;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика для разного набора аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () ()
    {
      return fn ();
    }

    result_type operator () (arg1_type arg)
    {
      return fn (arg);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2)
    {
      return fn (arg1,arg2);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3)
    {
      return fn (arg1,arg2,arg3);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4)
    {
      return fn (arg1,arg2,arg3,arg4);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5)
    {
      return fn (arg1,arg2,arg3,arg4,arg5);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5,arg6_type arg6)
    {
      return fn (arg1,arg2,arg3,arg4,arg5,arg6);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5,arg6_type arg6,arg7_type arg7)
    {
      return fn (arg1,arg2,arg3,arg4,arg5,arg6,arg7);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5,arg6_type arg6,arg7_type arg7,arg8_type arg8)
    {
      return fn (arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
    }

    result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5,arg6_type arg6,arg7_type arg7,arg8_type arg8,arg9_type arg9)
    {
      return fn (arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const Fn& f) const { return fn == f; }

  private:
    Fn fn;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функтор вызывающий функцию член класса
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class ObjPtr,class MemFunPtr>
class memfun_functor
{
  public:
    memfun_functor (const ObjPtr& _object_ptr,const MemFunPtr& _fn) : object_ptr (_object_ptr), fn (_fn) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret operator () () const {
      return ((*object_ptr).*fn)();
    }

    template <class Arg>
    Ret operator () (Arg arg) const {
      return ((*object_ptr).*fn)(arg);
    }

    template <class Arg1,class Arg2>
    Ret operator () (Arg1 arg1,Arg2 arg2) const {
      return ((*object_ptr).*fn)(arg1,arg2);
    }

    template <class Arg1,class Arg2,class Arg3>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3);
    }

    template <class Arg1,class Arg2,class Arg3,class Arg4>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4);    
    }
    
    template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Arg5 arg5) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4,arg5);
    }

    template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4,arg5,arg6);
    }

    template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6,Arg7 arg7) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
    }

    template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6,Arg7 arg7,Arg8 arg8) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
    }

    template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9>
    Ret operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6,Arg7 arg7,Arg8 arg8,Arg9 arg9) const {
      return ((*object_ptr).*fn)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух функторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const memfun_functor& f) const {
      return object_ptr == f.object_ptr && fn == f.fn;
    }

  private:
    ObjPtr    object_ptr;
    MemFunPtr fn;
};

}
