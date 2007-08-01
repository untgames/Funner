#ifndef SCRIPT_HEADER
#define SCRIPT_HEADER

#include <xtl/functional_fwd>
#include <common/exception.h>

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
class Stack;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
///Скриптовая среда
//////////////////////////////////////////////////////////////////////////////////////////////////
class Environment
{
  template <class Ret>
  friend Ret invoke (Environment&, const char* fn_name);
  template <class Ret, class T1>
  friend Ret invoke (Environment&, const char* fn_name, const T1& arg1);
  template <class Ret, class T1, class T2>
  friend Ret invoke (Environment&, const char* fn_name, const T1& arg1, const T2& arg2);
  friend void invoke (Environment& env, const char* fn_name);
  template <class T1>
  friend void invoke (Environment& env, const char* fn_name, const T1& arg1);
  template <class T1, class T2>
  friend void invoke (Environment& env, const char* fn_name, const T1& arg1, const T2& arg2);

  typedef xtl::function<void (const char* env_name, const char* message)> DebugLogFunc;

  public:
    Environment  (const DebugLogFunc&);
    ~Environment ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка пользовательской функции лога дебаг-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    const DebugLogFunc& GetDebugLog ();
    void                SetDebugLog (const DebugLogFunc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя среды
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг функций
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature, class Fn> void BindFunction (const char* name, Fn fn);

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
    void RegisterFunction (const char* name, detail::Invoker* invoker);

          detail::Stack& Stack ();
    const detail::Stack& Stack () const;

    void Invoke (size_t args_count, size_t results_count);
    
  private:
    Environment (const Environment&); //no impl
    Environment& operator = (const Environment&); //no impl

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

#include "detail/script.inl"

}

}

#endif
