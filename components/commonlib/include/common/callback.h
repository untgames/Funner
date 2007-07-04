#ifndef __COMMONLIB_CALLBACK__
#define __COMMONLIB_CALLBACK__

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер функции обратного вызова (4 аргумента)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret=void,class Arg1=void,class Arg2=void,class Arg3=void,class Arg4=void>
class CallbackFunction
{
  public:
    typedef Ret  result_type;
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Arg3 third_argument_type;
    typedef Arg4 fourth_argument_type;
    typedef Ret  (*ptr_fun)(Arg1,Arg2,Arg3,Arg4);
    typedef Ret  (*ptr_fun_context)(Arg1,Arg2,Arg3,Arg4,void* context);
    
    template <class T> struct rebind
    {
      typedef Ret (T::*mem_fun)(Arg1,Arg2,Arg3,Arg4);
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackFunction (ptr_fun = 0);
    CallbackFunction (ptr_fun_context,void* context);
    
    template <class T,class Fn> 
    CallbackFunction (T& obj,Fn mem_fun);        

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret  operator () (Arg1,Arg2,Arg3,Arg4) const;
    bool valid    () const; //проверка корректности состояния
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const CallbackFunction&) const;
    bool operator != (const CallbackFunction&) const;

  private:
    typedef Ret (*internal_callback_fun)(const CallbackFunction&,Arg1,Arg2,Arg3,Arg4);
                         
    static Ret call_ptr_fun         (const CallbackFunction&,Arg1,Arg2,Arg3,Arg4);
    static Ret call_ptr_fun_context (const CallbackFunction&,Arg1,Arg2,Arg3,Arg4);
    
    template <class T> 
    static Ret call_mem_fun (const CallbackFunction&,Arg1,Arg2,Arg3,Arg4);

  private:
    void*                 context;
    void*                 callback;    
    internal_callback_fun internal_callback;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер процедуры обратного вызова (4 аргумента)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1=void,class Arg2=void,class Arg3=void,class Arg4=void>
class CallbackHandler: public CallbackFunction<void,Arg1,Arg2,Arg3,Arg4>
{
  private: 
    typedef CallbackFunction<void,Arg1,Arg2,Arg3,Arg4> base;

  public:
    typedef typename base::ptr_fun         ptr_fun;
    typedef typename base::ptr_fun_context ptr_fun_context;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackHandler (ptr_fun = 0);
    CallbackHandler (ptr_fun_context,void* context);

    template <class T,class Fn>
    CallbackHandler (T& obj,Fn mem_fun);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация адаптера функции обратного вызова (3 аргумента)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class Arg1,class Arg2,class Arg3>
class CallbackFunction<Ret,Arg1,Arg2,Arg3,void>
{
  public:
    typedef Ret  result_type;
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Arg3 third_argument_type;
    typedef Ret  (*ptr_fun)(Arg1,Arg2,Arg3);
    typedef Ret  (*ptr_fun_context)(Arg1,Arg2,Arg3,void* context);
    
    template <class T> struct rebind
    {
      typedef Ret (T::*mem_fun)(Arg1,Arg2,Arg3);
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackFunction (ptr_fun = 0);
    CallbackFunction (ptr_fun_context,void* context);
    
    template <class T,class Fn> 
    CallbackFunction (T& obj,Fn mem_fun);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret  operator () (Arg1,Arg2,Arg3) const;
    bool valid    () const; //проверка корректности состояния    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const CallbackFunction&) const;
    bool operator != (const CallbackFunction&) const;

  private:
    typedef Ret (*internal_callback_fun)(const CallbackFunction&,Arg1,Arg2,Arg3);
                         
    static Ret call_ptr_fun         (const CallbackFunction&,Arg1,Arg2,Arg3);
    static Ret call_ptr_fun_context (const CallbackFunction&,Arg1,Arg2,Arg3);
    
    template <class T> 
    static Ret call_mem_fun (const CallbackFunction&,Arg1,Arg2,Arg3);

  private:
    void*                 context;
    void*                 callback;    
    internal_callback_fun internal_callback;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация адаптера функции обратного вызова (2 аргумента)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class Arg1,class Arg2>
class CallbackFunction<Ret,Arg1,Arg2,void,void>
{
  public:
    typedef Ret  result_type;
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Ret  (*ptr_fun)(Arg1,Arg2);
    typedef Ret  (*ptr_fun_context)(Arg1,Arg2,void* context);
    
    template <class T> struct rebind
    {
      typedef Ret (T::*mem_fun)(Arg1,Arg2);
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackFunction (ptr_fun = 0);
    CallbackFunction (ptr_fun_context,void* context);

    template <class T,class Fn> 
    CallbackFunction (T& obj,Fn mem_fun);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret  operator () (Arg1,Arg2) const;
    bool valid    () const; //проверка корректности состояния    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const CallbackFunction&) const;
    bool operator != (const CallbackFunction&) const;

  private:
    typedef Ret (*internal_callback_fun)(const CallbackFunction&,Arg1,Arg2);
                         
    static Ret call_ptr_fun         (const CallbackFunction&,Arg1,Arg2);
    static Ret call_ptr_fun_context (const CallbackFunction&,Arg1,Arg2);

    template <class T> 
    static Ret call_mem_fun (const CallbackFunction&,Arg1,Arg2);

  private:
    void*                 context;
    void*                 callback;    
    internal_callback_fun internal_callback;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация адаптера функции обратного вызова (1 аргумент)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class Arg1>
class CallbackFunction<Ret,Arg1,void,void,void>
{
  public:
    typedef Ret  result_type;
    typedef Arg1 first_argument_type;
    typedef Ret  (*ptr_fun)(Arg1);
    typedef Ret  (*ptr_fun_context)(Arg1,void* context);
    
    template <class T> struct rebind
    {
      typedef Ret (T::*mem_fun)(Arg1);
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackFunction (ptr_fun = 0);
    CallbackFunction (ptr_fun_context,void* context);

    template <class T,class Fn> 
    CallbackFunction (T& obj,Fn mem_fun);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret  operator () (Arg1) const;
    bool valid    () const; //проверка корректности состояния
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const CallbackFunction&) const;
    bool operator != (const CallbackFunction&) const;

  private:
    typedef Ret (*internal_callback_fun)(const CallbackFunction&,Arg1);
                         
    static Ret call_ptr_fun         (const CallbackFunction&,Arg1);
    static Ret call_ptr_fun_context (const CallbackFunction&,Arg1);
    
    template <class T> 
    static Ret call_mem_fun (const CallbackFunction&,Arg1);

  private:
    void*                 context;
    void*                 callback;    
    internal_callback_fun internal_callback;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация адаптера функции обратного вызова (без аргументов)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret>
class CallbackFunction<Ret,void,void,void,void>
{
  public:
    typedef Ret  result_type;
    typedef Ret  (*ptr_fun)();
    typedef Ret  (*ptr_fun_context)(void* context);
    
    template <class T> struct rebind
    {
      typedef Ret (T::*mem_fun)();
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackFunction (ptr_fun = 0);
    CallbackFunction (ptr_fun_context,void* context);

    template <class T,class Fn> 
    CallbackFunction (T& obj,Fn mem_fun);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Ret  operator () () const;
    bool valid    () const; //проверка корректности состояния

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const CallbackFunction&) const;
    bool operator != (const CallbackFunction&) const;

  private:
    typedef Ret (*internal_callback_fun)(const CallbackFunction&);

    static Ret call_ptr_fun         (const CallbackFunction&);
    static Ret call_ptr_fun_context (const CallbackFunction&);

    template <class T> 
    static Ret call_mem_fun (const CallbackFunction&);

  private:
    void*                 context;
    void*                 callback;    
    internal_callback_fun internal_callback;
};

#include <common/impl/callback.inl>

}

#endif
