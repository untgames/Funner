#ifndef SCRIPT_HEADER
#define SCRIPT_HEADER

//#include <xtl/functional_fwd>

//forward declarations
struct lua_State;

namespace script
{

namespace lua
{

//implementation forwards
namespace detail
{

class Invoker;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
///Элемент стека
//////////////////////////////////////////////////////////////////////////////////////////////////
class StackItem
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие значения аргумента
//////////////////////////////////////////////////////////////////////////////////////////////////
    operator double      () const;
    operator int         () const;
    operator const char* () const;
    operator const void* () const;

    template <class T> operator T () const; //check typeid
    
  protected:
    Argument (struct lua_State* state, size_t argument_number);
    
  private:
    struct lua_State* state;
    size_t            argument_number;
};  

//////////////////////////////////////////////////////////////////////////////////////////////////
///Стек аргументов
//////////////////////////////////////////////////////////////////////////////////////////////////
class Stack
{
  public:
    typedef StackItem Item;
    
//////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов в стеке
//////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение элемента из стека
//////////////////////////////////////////////////////////////////////////////////////////////////
    Item Get (int item_number) const;
    
    template <class T> T Get (int item_number) const //сделать в inl-файле
    {
      return static_cast<T> (Get (item_number)); //вызов: stack.Get<int> (index)
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
///Помещение данных в стек
//////////////////////////////////////////////////////////////////////////////////////////////////
    void Push (double);
    void Push (int);
    void Push (const char*);
    void Push (const void*);
    
    template <class T>
    void Push (const T&);

  private:
    lua_State* state;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Скриптовая среда
//////////////////////////////////////////////////////////////////////////////////////////////////
class Environment
{
  public:
    Environment  (const char* profile);
    ~Environment ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка и выполнение файла скрипта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя среды
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получене стека
///////////////////////////////////////////////////////////////////////////////////////////////////
          lua::Stack& Stack ();
    const lua::Stack& Stack () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка и получение переменных (я бы не делал: получим проблему глобальных переменных)
///////////////////////////////////////////////////////////////////////////////////////////////////
//    template <class T>
//    void SetVariable (const char* name, T value);
    
//    template <class T>
//    T GetVariable (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг функций
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature, class Fn> void BindFunction (const char* name, Fn fn);
      //вызов: env.BindFunction<void (const char*)> ("f", &my_f);

      //...

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение команды луа
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoString (const char* expression);
    void DoFile   (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Environment&);

  private:
    Environment ();
    
    void RegisterFunction (const char* name, Invoker* invoker);

  private:
    struct Impl;
    
    Impl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Environment&, Environment&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов функций луа
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret>
Ret invoke (Environment&, const char* fn_name);

template <class Ret, class T1>
Ret invoke (Environment&, const char* fn_name, const T1& arg1);

template <class Ret, class T1, class T2>
Ret invoke (Environment&, const char* fn_name, const T1& arg1, const T2& arg2);

//пример вызова: int x = invoke<int> (env, "my_f", 1, 3.14, "Hello world");

//...
//+сделать перегрузку для void возвращаемого значения

//пример вызова: invoke (env, "my_void_f", 1, 3.14, "Hello world");

      //перегрузки, упрощающие вызов
template <class Signature, class Fn>
void bind (Environment&, const char* name, Fn);
      
template <class Ret>
void bind (Environment&, const char* name, Ret (*)());

template <class Ret, class T1>
void bind (Environment&, const char* name, Ret (*)(T1));

//////////////////////////////////////////////////////////////////////////////////////////////////
//сделать в detail/script.inl-файле

namespace detail
{

template <class Signature>
inline void bind_call (Stack& stack);

template <class T> struct selector {};

template <class Fn, class Ret>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret ()>)
{
  stack.Push (fn ());
}

template <class Fn, class Ret, class T1>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1)>)
{
  stack.Push (fn (stack.Get<T1> (1)));
}

template <class Fn, class Ret, class T1, class T2>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2)));
}

///и т.д. до

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, t4, T5, T6, T7, T8, T9)>)
{
  stack.Push (fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
              stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9)));
}

//перегрузки для void

template <class Fn, class Ret>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret ()>)
{
  fn ();
}

template <class Fn, class Ret, class T1>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1)>)
{
  fn (stack.Get<T1> (1));
}

template <class Fn, class Ret, class T1, class T2>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2));
}

///и т.д. до

template <class Fn, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void bind_call (Fn& fn, Stack& stack, selector<Ret (T1, T2, T3, t4, T5, T6, T7, T8, T9)>)
{
  fn (stack.Get<T1> (1), stack.Get<T2> (2), stack.Get<T3> (3), stack.Get<T4> (4), stack.Get<T5> (5),
      stack.Get<T6> (6), stack.Get<T7> (7), stack.Get<T8> (8), stack.Get<T9> (9));
}

class Invoker
{
  public:
    virtual void operator () (Stack&) = 0;
};

template <class Signature, class Fn> class InvokerImpl: public Invoker
{
  public:
    InvokerImpl (const Fn& in_fn) : fn (in_fn) {}

    void operator () (Stack& stack) { bind_call (fn, stack, selector<Signature> ()); }

  private:
    Fn fn;
};

}

template <class Signature, class Fn>
inline void Environment::BindFunction (const char* name, Fn fn)
{
  RegisterFunction (name, new InvokerImpl<Signature, Fn> (fn));
}

template <class Ret>
void Environment::BindFunction (const char* name, Ret (*fn)())
{
  BindFunction<Ret ()> (name, fn);
}

template <class Ret, class T1>
void Environment::BindFunction (const char* name, Ret (*fn)(T1))
{
  BindFunction<Ret (T1)> (name, fn);
}

//...
template <class Ret, class T1, class T2>
Ret invoke (Environment&, const char* fn_name, const T1& arg1, const T2& arg2);

template <class Signature, class Fn>
void bind (Environment& env, const char* name, Fn fn)
{
  env.BindFunction (name, fn);
}

template <class Ret, class T1>
void bind (Environment& env, const char* name, Ret (*fn)(T1))
{
  env.BindFunction (name, fn);
}

template <class Ret, class T1, class T2>
Ret invoke (Environment& env, const char* fn_name, const T1& arg1, const T2& arg2)
{
  Stack& stack = env.Stack ();

  stack.Push (arg1);
  stack.Push (arg2);

  env.DoString (fn_name);
}

}

}

#endif
