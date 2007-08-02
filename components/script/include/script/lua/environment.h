#ifndef SCRIPT_HEADER
#define SCRIPT_HEADER

#include <xtl/functional_fwd>
#include <common/exception.h>
#include <new>

//implementation forwards
struct lua_State;

namespace script
{

namespace lua
{

//implementation forwards
namespace detail
{

class Stack;
class Invoker;

}

struct EnvironmentImpl;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Исключения
//////////////////////////////////////////////////////////////////////////////////////////////////
struct ScriptExceptionTag;                //базовое исключение для lua-исключений
struct RuntimeExceptionTag;               //исключение, возникающее при выполнении скрипта
struct StackOverflowExceptionTag;         //стек аргументов переполнен
struct UndefinedFunctionCallExceptionTag; //попытка вызова незарегистрированной функции

typedef common::DerivedException<common::Exception, ScriptExceptionTag>              ScriptException;
typedef common::DerivedException<ScriptException, StackOverflowExceptionTag>         StackOverflowException;
typedef common::DerivedException<ScriptException, UndefinedFunctionCallExceptionTag> UndefinedFunctionCallException;
typedef common::DerivedException<ScriptException, RuntimeExceptionTag>               RuntimeException;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Селектор типа возвращаемого значения
//////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct Result {};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Скриптовая среда
//////////////////////////////////////////////////////////////////////////////////////////////////
class Environment
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////  
    Environment  ();
    ~Environment ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя среды
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация функций
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature, class Fn> 
    void RegisterFunction (const char* name, Fn fn);

    void UnregisterFunction     (const char* name); //no throw
    void UnregisterAllFunctions (); //no throw

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: зарегистрирована ли в контексте функция с указанным именем
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFunction (const char* name) const; //no throw

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение команды луа
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* message)> LogFunc;

    bool DoString (const char* expression);
    bool DoString (const char* expression, const LogFunc& log);
//    bool DoBuffer (const char* name, const char* buffer, size_t buffer_size);
//    bool DoBuffer (const char* name, const char* buffer, size_t buffer_size, const LogFunc& log);
    bool DoFile   (const char* file_name);    
    bool DoFile   (const char* file_name, const LogFunc& log);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов функции луа
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ret>
    Ret Invoke (const char* name, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1>
    Ret Invoke (const char* name, const T1&, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1, class T2>
    Ret Invoke (const char* name, const T1&, const T2&, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1, class T2, class T3>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1, class T2, class T3, class T4>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1, class T2, class T3, class T4, class T5>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, const T5&, Result<Ret> = Result<Ret> ()) const;

    template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, Result<Ret> = Result<Ret> ()) const;
    
    template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, const T7&, Result<Ret> = Result<Ret> ()) const;
    
    template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&,
                const T5&, const T6&, const T7&, const T8&, Result<Ret> = Result<Ret> ()) const;
    
    template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    Ret Invoke (const char* name, const T1&, const T2&, const T3&, const T4&,
                const T5&, const T6&, const T7&, const T8&, const T9&, Result<Ret> = Result<Ret> ()) const;

    void Invoke (const char* name, Result<void> = Result<void> ()) const;

    template <class T1>
    void Invoke (const char* name, const T1&, Result<void> = Result<void> ()) const;

    template <class T1, class T2>
    void Invoke (const char* name, const T1&, const T2&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3>
    void Invoke (const char* name, const T1&, const T2&, const T3&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3, class T4>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3, class T4, class T5>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, const T5&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&,
                 const T5&, const T6&, const T7&, Result<void> = Result<void> ()) const;

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&,
                 const T5&, const T6&, const T7&, const T8&, Result<void> = Result<void> ()) const;
    
    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void Invoke (const char* name, const T1&, const T2&, const T3&, const T4&,
                 const T5&, const T6&, const T7&, const T8&, const T9&, Result<void> = Result<void> ()) const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Возвращение стека
///////////////////////////////////////////////////////////////////////////////////////////////////
    detail::Stack Stack () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренние функции
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFunctionCore (const char* name, detail::Invoker* invoker, size_t arguments_count);
    void InvokeCore           (size_t args_count, size_t results_count) const;
    
    template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    Ret InvokeDispatch (const char* name, size_t args_count, const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, const T7&, const T8&, const T9&, Result<Ret>) const;
    
    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void InvokeDispatch (const char* name, size_t args_count, const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, const T7&, const T8&, const T9&, Result<void>) const;

  private:
    Environment (const Environment&); //no impl
    Environment& operator = (const Environment&); //no impl

  private:   
    EnvironmentImpl* impl;
};

#include <script/lua/detail/stack.inl>
#include <script/lua/detail/invoker.inl>
#include <script/lua/detail/environment.inl>

}

}

#endif
