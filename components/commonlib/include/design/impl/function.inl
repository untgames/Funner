/*
    Вспомогательные классы
*/

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик вызовов пустой функции обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
class function_empty_invoker: public invoker<Signature,reference_counter>, public noncopyable
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика для разного набора аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () ()
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type)
    { 
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type)
    { 
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type,arg8_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type,arg2_type,arg3_type,arg4_type,arg5_type,arg6_type,arg7_type,arg8_type,arg9_type)
    {
      throw bad_function_call ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение экземпляра обработчика пустого делегата
///////////////////////////////////////////////////////////////////////////////////////////////////
    static function_empty_invoker& instance () {
      static char buffer [sizeof (function_empty_invoker)];
      static function_empty_invoker* invoker = new (buffer) function_empty_invoker;

      return *invoker;
    }
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение функции удаления объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void on_lost_references () {}

  private:
    function_empty_invoker () {}
};

}

/*
    function
*/

/*
    Конструкторы / деструкторы
*/

template <class Signature>
inline function<Signature>::function ()
  : invoker (&empty_invoker_type::instance ())
  { }

template <class Signature>
inline function<Signature>::function (const function& d)
  : invoker (d.invoker)
  { }

template <class Signature> template <class Fn>
inline function<Signature>::function (Fn fn)
  : invoker (create_invoker (fn))
  { }

template <class Signature> template <class Ptr,class MemFun>
inline function<Signature>::function (const Ptr& ptr,const MemFun& fn)
  : invoker (create_invoker (detail::memfun_functor<result_type,Ptr,MemFun> (ptr,fn)))
  { }

/*
    Присваивание
*/

template <class Signature>
function<Signature>& function<Signature>::assign (const function& d)
{
  if (invoker != d.invoker)
    invoker = d.invoker;

  return *this;
}

template <class Signature> template <class Fn>
function<Signature>& function<Signature>::assign_internal (const Fn& fn)
{
  invoker = create_invoker (fn);

  return *this;
}

template <class Signature> template <class Fn>
inline function<Signature>& function<Signature>::assign (Fn fn)
{
  return assign_internal (fn);
}

template <class Signature> template <class Ptr,class MemFun>
inline function<Signature>& function<Signature>::assign (const Ptr& ptr,const MemFun& fn)
{
  return assign_internal (detail::memfun_functor<result_type,Ptr,MemFun> (ptr,fn));
}

template <class Signature>
function<Signature>& function<Signature>::operator = (const function& d)
{
  return assign (d);
}

template <class Signature> template <class Fn>
function<Signature>& function<Signature>::operator = (Fn fn)
{
  return assign_internal (fn);
}

/*
    Очистка делегата
*/

template <class Signature>
void function<Signature>::clear ()
{
  if (empty ())
    return;

  invoker = &empty_invoker_type::instance ();
}

/*
    Создание специализированных обработчиков
*/

template <class Signature> template <class Fn>
typename function<Signature>::invoker_type* function<Signature>::create_invoker (const Fn& fn)
{
  return new detail::functor_invoker<Signature,Fn,invoker_type> (fn);
}

/*
    Проверка является ли данный делегат пустым
*/

template <class Signature>
inline bool function<Signature>::empty () const
{
  return &empty_invoker_type::instance () == invoker;
}

/*
    Вызов делегата
*/

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () () const
{
  return (*invoker)();
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () (arg1_type arg) const
{
  return (*invoker)(arg);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () (arg1_type arg1,arg2_type arg2) const
{
  return (*invoker)(arg1,arg2);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3) const
{
  return (*invoker)(arg1,arg2,arg3);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4) const
{
  return (*invoker)(arg1,arg2,arg3,arg4);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5) const
{
  return (*invoker)(arg1,arg2,arg3,arg4,arg5);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6) const
{
  return (*invoker)(arg1,arg2,arg3,arg4,arg5,arg6);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7) const
{
  return (*invoker)(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7,
  arg8_type arg8) const
{
  return (*invoker)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type arg1,
  arg2_type arg2,
  arg3_type arg3,
  arg4_type arg4,
  arg5_type arg5,
  arg6_type arg6,
  arg7_type arg7,
  arg8_type arg8,
  arg9_type arg9) const
{
  return (*invoker)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
}

/*
    Сравнение
*/

template <class Signature> template <class Fn>
inline bool function<Signature>::operator == (Fn fn) const
{
  typedef detail::functor_invoker<Signature,Fn,invoker_type> functor_invoker;  

  functor_invoker* casted_invoker = dynamic_cast<functor_invoker*> (invoker.get ());

  return casted_invoker && *casted_invoker == fn;
}

template <class Signature,class Fn>
inline bool operator == (Fn a,const function<Signature>& b)
{
    typedef detail::functor_invoker<Signature,Fn,typename function<Signature>::invoker_type> functor_invoker;    

  functor_invoker* casted_invoker = dynamic_cast<functor_invoker*> (b.invoker.get ());

  return casted_invoker && a == *casted_invoker;
}

template <class Signature> template <class Fn>
inline bool function<Signature>::operator != (Fn fn) const
{
  return !(*this == fn);
}

template <class Signature,class Fn>
inline bool operator != (Fn a,const function<Signature>& b)
{
  return !(a == b);
}
