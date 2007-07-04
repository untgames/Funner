/*
    CallbackHandler
*/

template <class Arg1,class Arg2,class Arg3,class Arg4>
inline CallbackHandler<Arg1,Arg2,Arg3,Arg4>::CallbackHandler (ptr_fun func)
  : base (func)
 { }

template <class Arg1,class Arg2,class Arg3,class Arg4>
inline CallbackHandler<Arg1,Arg2,Arg3,Arg4>::CallbackHandler (ptr_fun_context func,void* user_data)
  : base (func,user_data)
 { }

template <class Arg1,class Arg2,class Arg3,class Arg4> template <class T,class Fn> 
inline CallbackHandler<Arg1,Arg2,Arg3,Arg4>::CallbackHandler (T& obj,Fn func)
  : base (obj,func)
 { }

/*
    4 arguments version
*/

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::CallbackFunction (ptr_fun func)
{
  context           = 0;
  callback          = (void*)func;
  internal_callback = call_ptr_fun;
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::CallbackFunction (ptr_fun_context func,void* user_data)
{
  context           = user_data;
  callback          = (void*)func;
  internal_callback = call_ptr_fun_context;
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::valid () const
{
  return callback && (context || internal_callback == call_ptr_fun || internal_callback == call_ptr_fun_context);
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4> template <class T,class Fn> 
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::CallbackFunction (T& obj,Fn func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
  
  _mem_func         = func;
  context           = &obj;
  callback          = _func;
  internal_callback = call_mem_fun<T>;
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4) const
{
  return internal_callback (*this,arg1,arg2,arg3,arg4);
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::call_ptr_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
{
  ptr_fun fn = (ptr_fun)func.callback;
  
  return fn (arg1,arg2,arg3,arg4);
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::call_ptr_fun_context (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
{
  ptr_fun_context fn = (ptr_fun_context)func.callback;
  
  return fn (arg1,arg2,arg3,arg4,func.context);
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4> template <class T>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::call_mem_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
    
  T& obj = *(T*)func.context;
  _func  = func.callback;
  
  return (obj.*_mem_func)(arg1,arg2,arg3,arg4);
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::operator == (const CallbackFunction& x) const
{
  return x.internal_callback == internal_callback && x.callback == callback && x.context == context;  
}

template <class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,Arg4>::operator != (const CallbackFunction& x) const
{
  return !((*this) == x);
}

/*
    3 arguments version
*/

template <class Ret,class Arg1,class Arg2,class Arg3>
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::CallbackFunction (ptr_fun func)
{
  context           = 0;
  callback          = (void*)func;
  internal_callback = call_ptr_fun;
}

template <class Ret,class Arg1,class Arg2,class Arg3>
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::CallbackFunction (ptr_fun_context func,void* user_data)
{
  context           = user_data;
  callback          = (void*)func;
  internal_callback = call_ptr_fun_context;
}

template <class Ret,class Arg1,class Arg2,class Arg3> template <class T,class Fn> 
inline CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::CallbackFunction (T& obj,Fn func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
  
  _mem_func         = func;
  context           = &obj;
  callback          = _func;
  internal_callback = call_mem_fun<T>;
}

template <class Ret,class Arg1,class Arg2,class Arg3>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::valid () const
{
  return callback && (context || internal_callback == call_ptr_fun || internal_callback == call_ptr_fun_context);
}

template <class Ret,class Arg1,class Arg2,class Arg3>
inline Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::operator () (Arg1 arg1,Arg2 arg2,Arg3 arg3) const
{
  return internal_callback (*this,arg1,arg2,arg3);
}

template <class Ret,class Arg1,class Arg2,class Arg3>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::call_ptr_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3)
{
  ptr_fun fn = (ptr_fun)func.callback;
  
  return fn (arg1,arg2,arg3);
}

template <class Ret,class Arg1,class Arg2,class Arg3>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::call_ptr_fun_context (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3)
{
  ptr_fun_context fn = (ptr_fun_context)func.callback;
  
  return fn (arg1,arg2,arg3,func.context);
}

template <class Ret,class Arg1,class Arg2,class Arg3> template <class T>
Ret CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::call_mem_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2,Arg3 arg3)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
    
  T& obj = *(T*)func.context;
  _func  = func.callback;
  
  return (obj.*_mem_func)(arg1,arg2,arg3);
}

template <class Ret,class Arg1,class Arg2,class Arg3>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::operator == (const CallbackFunction& x) const
{
  return x.internal_callback == internal_callback && x.callback == callback && x.context == context;  
}

template <class Ret,class Arg1,class Arg2,class Arg3>
inline bool CallbackFunction<Ret,Arg1,Arg2,Arg3,void>::operator != (const CallbackFunction& x) const
{
  return !((*this) == x);
}

/*
    2 arguments version
*/

template <class Ret,class Arg1,class Arg2>
inline CallbackFunction<Ret,Arg1,Arg2,void,void>::CallbackFunction (ptr_fun func)
{
  context           = 0;
  callback          = (void*)func;
  internal_callback = call_ptr_fun;
}

template <class Ret,class Arg1,class Arg2>
inline CallbackFunction<Ret,Arg1,Arg2,void,void>::CallbackFunction (ptr_fun_context func,void* user_data)
{
  context           = user_data;
  callback          = (void*)func;
  internal_callback = call_ptr_fun_context;
}

template <class Ret,class Arg1,class Arg2> template <class T,class Fn> 
inline CallbackFunction<Ret,Arg1,Arg2,void,void>::CallbackFunction (T& obj,Fn func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
  
  _mem_func         = func;
  context           = &obj;
  callback          = _func;
  internal_callback = call_mem_fun<T>;
}

template <class Ret,class Arg1,class Arg2>
inline bool CallbackFunction<Ret,Arg1,Arg2,void,void>::valid () const
{
  return callback && (context || internal_callback == call_ptr_fun || internal_callback == call_ptr_fun_context);
}

template <class Ret,class Arg1,class Arg2>
inline Ret CallbackFunction<Ret,Arg1,Arg2,void,void>::operator () (Arg1 arg1,Arg2 arg2) const
{
  return internal_callback (*this,arg1,arg2);
}

template <class Ret,class Arg1,class Arg2>
Ret CallbackFunction<Ret,Arg1,Arg2,void,void>::call_ptr_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2)
{
  ptr_fun fn = (ptr_fun)func.callback;
  
  return fn (arg1,arg2);
}

template <class Ret,class Arg1,class Arg2>
Ret CallbackFunction<Ret,Arg1,Arg2,void,void>::call_ptr_fun_context (const CallbackFunction& func,Arg1 arg1,Arg2 arg2)
{
  ptr_fun_context fn = (ptr_fun_context)func.callback;
  
  return fn (arg1,arg2,func.context);
}

template <class Ret,class Arg1,class Arg2> template <class T>
Ret CallbackFunction<Ret,Arg1,Arg2,void,void>::call_mem_fun (const CallbackFunction& func,Arg1 arg1,Arg2 arg2)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
    
  T& obj = *(T*)func.context;
  _func  = func.callback;
  
  return (obj.*_mem_func)(arg1,arg2);
}

template <class Ret,class Arg1,class Arg2>
inline bool CallbackFunction<Ret,Arg1,Arg2,void,void>::operator == (const CallbackFunction& x) const
{
  return x.internal_callback == internal_callback && x.callback == callback && x.context == context;  
}

template <class Ret,class Arg1,class Arg2>
inline bool CallbackFunction<Ret,Arg1,Arg2,void,void>::operator != (const CallbackFunction& x) const
{
  return !((*this) == x);
}

/*
    1 argument version
*/

template <class Ret,class Arg1>
inline CallbackFunction<Ret,Arg1,void,void,void>::CallbackFunction (ptr_fun func)
{
  context           = 0;
  callback          = (void*)func;
  internal_callback = call_ptr_fun;
}

template <class Ret,class Arg1>
inline CallbackFunction<Ret,Arg1,void,void,void>::CallbackFunction (ptr_fun_context func,void* user_data)
{
  context           = user_data;
  callback          = (void*)func;
  internal_callback = call_ptr_fun_context;
}

template <class Ret,class Arg1> template <class T,class Fn> 
inline CallbackFunction<Ret,Arg1,void,void,void>::CallbackFunction (T& obj,Fn func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
  
  _mem_func         = func;
  context           = &obj;
  callback          = _func;
  internal_callback = call_mem_fun<T>;
}

template <class Ret,class Arg1>
inline bool CallbackFunction<Ret,Arg1,void,void,void>::valid () const
{
  return callback && (context || internal_callback == call_ptr_fun || internal_callback == call_ptr_fun_context);
}

template <class Ret,class Arg1>
inline Ret CallbackFunction<Ret,Arg1,void,void,void>::operator () (Arg1 arg1) const
{
  return internal_callback (*this,arg1);
}

template <class Ret,class Arg1>
Ret CallbackFunction<Ret,Arg1,void,void,void>::call_ptr_fun (const CallbackFunction& func,Arg1 arg1)
{
  ptr_fun fn = (ptr_fun)func.callback;
  
  return fn (arg1);
}

template <class Ret,class Arg1>
Ret CallbackFunction<Ret,Arg1,void,void,void>::call_ptr_fun_context (const CallbackFunction& func,Arg1 arg1)
{
  ptr_fun_context fn = (ptr_fun_context)func.callback;
  
  return fn (arg1,func.context);
}

template <class Ret,class Arg1> template <class T>
Ret CallbackFunction<Ret,Arg1,void,void,void>::call_mem_fun (const CallbackFunction& func,Arg1 arg1)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
    
  T& obj = *(T*)func.context;
  _func  = func.callback;
  
  return (obj.*_mem_func)(arg1);
}

template <class Ret,class Arg1>
inline bool CallbackFunction<Ret,Arg1,void,void,void>::operator == (const CallbackFunction& x) const
{
  return x.internal_callback == internal_callback && x.callback == callback && x.context == context;  
}

template <class Ret,class Arg1>
inline bool CallbackFunction<Ret,Arg1,void,void,void>::operator != (const CallbackFunction& x) const
{
  return !((*this) == x);
}

/*
    No arguments version
*/

template <class Ret>
inline CallbackFunction<Ret,void,void,void,void>::CallbackFunction (ptr_fun func)
{
  context           = 0;
  callback          = (void*)func;
  internal_callback = call_ptr_fun;
}

template <class Ret>
inline CallbackFunction<Ret,void,void,void,void>::CallbackFunction (ptr_fun_context func,void* user_data)
{
  context           = user_data;
  callback          = (void*)func;
  internal_callback = call_ptr_fun_context;
}

template <class Ret> template <class T,class Fn> 
inline CallbackFunction<Ret,void,void,void,void>::CallbackFunction (T& obj,Fn func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
  
  _mem_func         = func;
  context           = &obj;
  callback          = _func;
  internal_callback = call_mem_fun<T>;
}

template <class Ret>
inline bool CallbackFunction<Ret,void,void,void,void>::valid () const
{
  return callback && (context || internal_callback == call_ptr_fun || internal_callback == call_ptr_fun_context);
}

template <class Ret>
inline Ret CallbackFunction<Ret,void,void,void,void>::operator () () const
{
  return internal_callback (*this);
}

template <class Ret>
Ret CallbackFunction<Ret,void,void,void,void>::call_ptr_fun (const CallbackFunction& func)
{
  ptr_fun fn = (ptr_fun)func.callback;
  
  return fn ();
}

template <class Ret>
Ret CallbackFunction<Ret,void,void,void,void>::call_ptr_fun_context (const CallbackFunction& func)
{
  ptr_fun_context fn = (ptr_fun_context)func.callback;
  
  return fn (func.context);
}

template <class Ret> template <class T>
Ret CallbackFunction<Ret,void,void,void,void>::call_mem_fun (const CallbackFunction& func)
{
  union {
    typename rebind<T>::mem_fun _mem_func;
    void*                       _func;
  };
    
  T& obj = *(T*)func.context;
  _func  = func.callback;
  
  return (obj.*_mem_func)();
}

template <class Ret>
inline bool CallbackFunction<Ret,void,void,void,void>::operator == (const CallbackFunction& x) const
{
  return x.internal_callback == internal_callback && x.callback == callback && x.context == context;  
}

template <class Ret>
inline bool CallbackFunction<Ret,void,void,void,void>::operator != (const CallbackFunction& x) const
{
  return !((*this) == x);
}
