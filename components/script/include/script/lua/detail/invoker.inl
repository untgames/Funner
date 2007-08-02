namespace detail
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс вызова между lua и С++
//////////////////////////////////////////////////////////////////////////////////////////////////
class Invoker
{
  public:
    virtual ~Invoker () {}
  
//////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t operator () (Stack&) = 0; //возвращает количество возвращаемых результатов
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Класс необходимый для управления перегрузкой
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> struct Selector {};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Перегрузки вызовов для разных сигнатур
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class Fn, class Ret>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret ()>)
{
  stack.Push (fn ());
  return 1;
}

template <class Fn, class Ret, class T1>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1)>)
{
  stack.Push (fn (stack.Get<T1> (1)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4, T5)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4, T5, T6)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4, T5, T6, T7)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline size_t invoke (Fn& fn, Stack& stack, Selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8, T9)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9)));
  return 1;
}

template <class Fn>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void ()>)
{
  fn ();
  return 0;
}

template <class Fn, class T1>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1)>)
{
  fn (stack.Get<T1> (1));
  return 0;
}

template <class Fn, class T1, class T2>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2));
  return 0;
}

template <class Fn, class T1, class T2, class T3>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4, T5)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4, T5, T6)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4, T5, T6, T7)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4, T5, T6, T7, T8)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7), stack.Get<T8> (8));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline size_t invoke (Fn& fn, Stack& stack, Selector<void (T1, T2, T3, T4, T5, T6, T7, T8, T9)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9));
  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
///Класс вызова функтора С++ из lua
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Fn> class InvokerImpl: public Invoker
{
  public:
    InvokerImpl (const Fn& in_fn) : fn (in_fn) {}

//////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t operator () (Stack& stack) { return invoke (fn, stack, Selector<Signature> ()); }

  private:
    Fn fn;
};

}
