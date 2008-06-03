/*
    Derived exception
*/

template <class BaseException,class Tag>
inline DerivedException<BaseException,Tag>::DerivedException (const char* message)
  : BaseException (message)
  { }
  
template <class BaseException,class Tag>
inline DerivedException<BaseException,Tag>::DerivedException (const char* source, const char* message)
  : BaseException (source, message)
  { }  
  
/*
    raise
*/

//даннаz функциz введена для освобождения от интерфейсной зависимости от strlib.h
extern void vraise 
(const char* source,
 const char* format,
 va_list     list,
 void (*)(const char* source,const char* message));

template <class Exception>
void __RaiseException (const char* source,const char* message)
{
  Exception exception (message);

  exception.Touch (source);

  throw exception;
}

template <class Exception>
inline void vraise (const char* source,const char* format,va_list list)
{
  vraise (source,format,list,__RaiseException<Exception>);
}

template <class Exception>
void raise (const char* source,const char* format,...)
{
  va_list list;
  
  va_start          (list,format);
  vraise<Exception> (source,format,list);
}
  

/*
    ¬ыброс стандартных исключений
*/

inline void raise_invalid_argument (const char* source,const char* param)
{
  raise<ArgumentException> (source,"Invalid argument <%s>",param);
}

inline void raise_invalid_argument (const char* source,const char* param,const char* value,const char* comment)
{
  raise<ArgumentException> (source,comment?"Invalid argument <%s>='%s'. %s":"Invalid argument <%s>='%s'",
                            param,value,comment);
}

inline void raise_invalid_argument (const char* source,const char* param,int value,const char* comment)
{
  raise<ArgumentException> (source,comment?"Invalid argument <%s>=%d. %s":"Invalid argument <%s>=%d",
                            param,value,comment);
}

inline void raise_invalid_argument (const char* source,const char* param,size_t value,const char* comment)
{
  raise<ArgumentException> (source,comment?"Invalid argument <%s>=%u. %s":"Invalid argument <%s>=%u",
                            param,value,comment);
}

inline void raise_invalid_argument (const char* source,const char* param,float value,const char* comment)
{
  raise<ArgumentException> (source,comment?"Invalid argument <%s>=%g. %s":"Invalid argument <%s>=%g",
                            param,value,comment);
}

inline void raise_invalid_argument (const char* source,const char* param,double value,const char* comment)
{
  raise<ArgumentException> (source,comment?"Invalid argument <%s>=%g. %s":"Invalid argument <%s>=%g",
                            param,value,comment);
}

inline void raise_out_of_range (const char* source,const char* param)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s> is out of range",param);
}

inline void raise_out_of_range (const char* source,const char* param,int value,int first,int last)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%d is out of range [%d;%d)",param,value,first,last);
}

inline void raise_out_of_range (const char* source,const char* param,size_t value,size_t first,size_t last)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%u is out of range [%u;%u)",param,value,first,last);
}

inline void raise_out_of_range (const char* source,const char* param,float value,float first,float last)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%g is out of range [%g;%g)",param,value,first,last);
}

inline void raise_out_of_range (const char* source,const char* param,double value,double first,double last)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%g is out of range [%g;%g)",param,value,first,last);
}

inline void raise_out_of_range (const char* source,const char* param,size_t index,size_t max_count)
{
  raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%u is out of range [0;%u)",param,index,max_count);
}

inline void raise_null_argument (const char* source,const char* param)
{
  raise<ArgumentNullException> (source,"Null argument <%s>",param);
}

inline void raise_not_implemented (const char* source)
{
  raise<NotImplementedException> (NULL,"Method '%s' doesn't implemented",source);
}

inline void raise_not_supported (const char* source)
{
  raise<NotSupportedException> (NULL,"Method '%s' doesn't supported",source);
}

inline void raise_not_supported (const char* source,const char* format,...)
{
  va_list list;

  va_start (list,format);

  vraise<NotSupportedException> (source,format,list);
}

inline void vraise_not_supported (const char* source,const char* format,va_list list)
{  
  vraise<NotSupportedException> (source,format,list);
}

inline void raise_invalid_operation (const char* source,const char* format,...)
{
  va_list list;

  va_start (list,format);

  vraise<OperationException> (source,format,list);
}

inline void vraise_invalid_operation (const char* source,const char* format,va_list list)
{  
  vraise<OperationException> (source,format,list);
}
