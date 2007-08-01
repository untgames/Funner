namespace detail
{

template <class Signature>
inline size_t bind_call (Stack& stack);

template <class Signature>
inline size_t arg_count ();

template <class T> struct selector {};

template <class Fn, class Ret>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret ()>)
{
  stack.Push (fn ());
  return 1;
}

template <class Fn, class Ret, class T1>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1)>)
{
  stack.Push (fn (stack.Get<T1> (1)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4, T5)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4, T5, T6)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4, T5, T6, T7)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8)));
  return 1;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline size_t bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8, T9)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9)));
  return 1;
}

template <class Fn>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void ()>)
{
  fn ();
  return 0;
}

template <class Fn, class T1>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1)>)
{
  fn (stack.Get<T1> (1));
  return 0;
}

template <class Fn, class T1, class T2>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2));
  return 0;
}

template <class Fn, class T1, class T2, class T3>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4, T5)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4, T5, T6)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4, T5, T6, T7)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4, T5, T6, T7, T8)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7), stack.Get<T8> (8));
  return 0;
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline size_t bind_call (Fn& fn, Stack& stack, selector<void (T1, T2, T3, T4, T5, T6, T7, T8, T9)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5), stack.Get<T6> (6),
      stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9));
  return 0;
}

template <class Fn, class Ret>
inline size_t arg_count (Fn& fn, selector<Ret ()>)
{
  return 0;
}

template <class Fn, class Ret, class T1>
inline size_t arg_count (Fn& fn, selector<Ret (T1)>)
{
  return 1;
}

template <class Fn, class Ret, class T1, class T2>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2)>)
{
  return 2;
}

template <class Fn, class Ret, class T1, class T2, class T3>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3)>)
{
  return 3;
}
template <class Fn, class Ret, class T1, class T2, class T3, class T4>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4)>)
{
  return 4;
}
template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4, T5)>)
{
  return 5;
}
template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4, T5, T6)>)
{
  return 6;
}
template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4, T5, T6, T7)>)
{
  return 7;
}
template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8)>)
{
  return 8;
}

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline size_t arg_count (Fn& fn, selector<Ret (T1, T2, T3, T4, T5, T6, T7, T8, T9)>)
{
  return 9;
}

class Invoker
{
  public:
    virtual size_t operator () (Stack&) = 0;
    virtual size_t ArgCount () = 0;
};

template <class Signature, class Fn> class InvokerImpl: public Invoker
{
  public:
    InvokerImpl (const Fn& in_fn) : fn (in_fn) {}

    size_t operator () (Stack& stack) { return bind_call (fn, stack, selector<Signature> ()); }
    size_t ArgCount () { return arg_count (fn, selector <Signature> ()); }

  private:
    Fn fn;
};

}

template <class Ret>
Ret invoke (Environment& env, const char* fn_name)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  env.Invoke (0, 1);

  Ret ret_value = stack->Get (-1);
  stack->Pop (1);

  return ret_value;
}

template <class Ret, class T1>
Ret invoke (Environment& env, const char* fn_name, const T1& arg1)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  stack->Push (arg1);

  env.Invoke (1, 1);

  Ret ret_value = stack->Get (-1);
  stack->Pop (1);

  return ret_value;
}

template <class Ret, class T1, class T2>
Ret invoke (Environment& env, const char* fn_name, const T1& arg1, const T2& arg2)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  stack->Push (arg1);
  stack->Push (arg2);

  env.Invoke (2, 1);

  Ret ret_value = stack->Get (-1);
  stack->Pop (1);

  return ret_value;
}

void invoke (Environment& env, const char* fn_name);

template <class T1>
void invoke (Environment& env, const char* fn_name, const T1& arg1)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  stack->Push (arg1);

  env.Invoke (1, 0);
}

template <class T1, class T2>
void invoke (Environment& env, const char* fn_name, const T1& arg1, const T2& arg2)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  stack->Push (arg1);
  stack->Push (arg2);

  env.Invoke (2, 0);
}

template <class Signature, class Fn>
inline void Environment::BindFunction (const char* name, Fn fn)
{
  RegisterFunction (name, new detail::InvokerImpl<Signature, Fn> (fn));
}

struct IUserData
{
  virtual ~IUserData () {}
};

template <class T> struct UserDataImpl: public IUserData
{
  UserDataImpl (const T& in_value) : value (in_value) {}
  
  T value;
};

/*template <class T> void Stack::Push (const T& value)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");

  
  char* buffer = (char*) Alloc (sizeof (UserDataImpl<T>));

  if (!buffer)
    Raise <Exception> ("Stack::Push", "Can't alloc memory for pushing object.");

  UserDataImpl<T>* object = new (buffer) UserDataImpl<T> (value);
}

template <class T> T Stack::Get (int index) const
{
  return (StackItem) (Get (index));
}

template <class T>
StackItem::operator T () const
{
  UserDataImpl<T> const *object = dynamic_cast<const UserDataImpl<T>*> (reinterpret_cast<const IUserData*> ((const void*)(*this)));

  if (!object)
    Raise <Exception> ("Stack::Get", "Stack item has other type than asked."); //wrong type

  return object->value;
} */

template <class T> void Stack::Push (const T& value)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");

  void* buffer = Alloc (sizeof (UserDataImpl<T>));

  if (!buffer)
    Raise <Exception> ("Stack::Push", "Can't alloc memory for pushing object.");

  new (buffer) UserDataImpl<T> (value);
}

template <class T> T Stack::Get (int index) const
{
  return (StackItem) (Get (index));
}

template <class T>
StackItem::operator T () const
{
  UserDataImpl<T> const *object = dynamic_cast<const UserDataImpl<T>*> (*reinterpret_cast<const IUserData**> ((const void**)(const void*)(*this)));

  if (!object)
    Raise <Exception> ("Stack::Get", "Stack item has other type than asked."); //wrong type

  return object->value;
}

