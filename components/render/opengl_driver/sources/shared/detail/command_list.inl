namespace detail
{

/*
    јбстрактный класс команды
*/

class ICommand
{
  public:
    virtual ~ICommand () {}

    virtual void Execute () = 0;
};

/*
    –еализаци€ команд
*/

template <class Fn, class T1=void, class T2=void, class T3=void, class T4=void> class Command;

//команда без аргументов
template <class Fn>
class Command<Fn>: public ICommand
{
  public:
    Command (Fn in_fn) : fn (in_fn) {}
    
    void Execute () { fn (); }

  private:
    Fn fn;
};

//команда с одним аргументом
template <class Fn, class T1>
class Command<Fn, T1>: public ICommand
{
  public:
    Command (Fn in_fn, const T1& in_arg1) : fn (in_fn), arg1 (in_arg1) {}
    
    void Execute () { fn (arg1); }
  
  private:
    Fn fn;
    T1 arg1;
};

//команда с двум€ аргументами
template <class Fn, class T1, class T2>
class Command<Fn, T1, T2>: public ICommand
{
  public:
    Command (Fn in_fn, const T1& in_arg1, const T2& in_arg2) : fn (in_fn), arg1 (in_arg1), arg2 (in_arg2) {}
    
    void Execute () { fn (arg1, arg2); }
  
  private:
    Fn fn;
    T1 arg1;
    T2 arg2;
};

//команда с трем€ аргументами
template <class Fn, class T1, class T2, class T3>
class Command<Fn, T1, T2, T3>: public ICommand
{
  public:
    Command (Fn in_fn, const T1& in_arg1, const T2& in_arg2, const T3& in_arg3) :
      fn (in_fn), arg1 (in_arg1), arg2 (in_arg2), arg3 (in_arg3) {}
    
    void Execute () { fn (arg1, arg2, arg3); }
  
  private:
    Fn fn;
    T1 arg1;
    T2 arg2;
    T3 arg3;
};

//команда с 4-м€ аргументами
template <class Fn, class T1, class T2, class T3, class T4>
class Command: public ICommand
{
  public:
    Command (Fn in_fn, const T1& in_arg1, const T2& in_arg2, const T3& in_arg3, const T4& in_arg4) :
      fn (in_fn), arg1 (in_arg1), arg2 (in_arg2), arg3 (in_arg3), arg4 (in_arg4) {}

    void Execute () { fn (arg1, arg2, arg3, arg4); }
  
  private:
    Fn fn;
    T1 arg1;
    T2 arg2;
    T3 arg3;
    T4 arg4;
};

}

/*
    ƒобавление команды
*/

template <class Fn>
inline void CommandListBuilder::Add (Fn fn)
{
  AddCore (new detail::Command<Fn, 0> (fn));
}

template <class Fn, class T1>
inline void CommandListBuilder::Add (Fn fn, const T1& arg1)
{
  AddCore (new detail::Command<Fn, T1> (fn, arg1));
}

template <class Fn, class T1, class T2>
inline void CommandListBuilder::Add (Fn fn, const T1& arg1, const T2& arg2)
{
  AddCore (new detail::Command<Fn, T1, T2> (fn, arg1, arg2));
}

template <class Fn, class T1, class T2, class T3>
inline void CommandListBuilder::Add (Fn fn, const T1& arg1, const T2& arg2, const T3& arg3)
{
  AddCore (new detail::Command<Fn, T1, T2, T3> (fn, arg1, arg2, arg3));
}

template <class Fn, class T1, class T2, class T3, class T4>
inline void CommandListBuilder::Add (Fn fn, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
{
  AddCore (new detail::Command<Fn, T1, T2, T3, T4> (fn, arg1, arg2, arg3, arg4));
}
