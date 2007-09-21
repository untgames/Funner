/*
    Derived exception
*/

template <class BaseException,class Tag>
inline DerivedException<BaseException,Tag>::DerivedException (const char* message)
  : BaseException (message)
  { }
  
/*
    Raise
*/

//даннаz функциz введена для освобождения от интерфейсной зависимости от strlib.h
extern void VRaise 
(const char* source,
 const char* format,
 va_list     list,
 void (*)(const char* source,const char* message));

template <class Exception>
void __RaiseException (const char* source,const char* message)
{
  Exception (message).Raise (source);    
}

template <class Exception>
inline void VRaise (const char* source,const char* format,va_list list)
{
  VRaise (source,format,list,__RaiseException<Exception>);
}

template <class Exception>
void Raise (const char* source,const char* format,...)
{
  va_list list;
  
  va_start          (list,format);
  VRaise<Exception> (source,format,list);
}
  

/*
    ¬ыброс стандартных исключений
*/

inline void RaiseInvalidArgument (const char* source,const char* param)
{
  Raise<ArgumentException> (source,"Invalid argument <%s>",param);
}

inline void RaiseInvalidArgument (const char* source,const char* param,const char* value,const char* comment)
{
  Raise<ArgumentException> (source,comment?"Invalid argument <%s>='%s'. %s":"Invalid argument <%s>='%s'",
                            param,value,comment);
}

inline void RaiseInvalidArgument (const char* source,const char* param,int value,const char* comment)
{
  Raise<ArgumentException> (source,comment?"Invalid argument <%s>=%d. %s":"Invalid argument <%s>=%d",
                            param,value,comment);
}

inline void RaiseInvalidArgument (const char* source,const char* param,size_t value,const char* comment)
{
  Raise<ArgumentException> (source,comment?"Invalid argument <%s>=%u. %s":"Invalid argument <%s>=%u",
                            param,value,comment);
}

inline void RaiseInvalidArgument (const char* source,const char* param,float value,const char* comment)
{
  Raise<ArgumentException> (source,comment?"Invalid argument <%s>=%g. %s":"Invalid argument <%s>=%g",
                            param,value,comment);
}

inline void RaiseInvalidArgument (const char* source,const char* param,double value,const char* comment)
{
  Raise<ArgumentException> (source,comment?"Invalid argument <%s>=%g. %s":"Invalid argument <%s>=%g",
                            param,value,comment);
}

inline void RaiseOutOfRange (const char* source,const char* param)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s> is out of range",param);
}

inline void RaiseOutOfRange (const char* source,const char* param,int value,int first,int last)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%d is out of range [%d;%d)",param,value,first,last);
}

inline void RaiseOutOfRange (const char* source,const char* param,size_t value,size_t first,size_t last)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%u is out of range [%u;%u)",param,value,first,last);
}

inline void RaiseOutOfRange (const char* source,const char* param,float value,float first,float last)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%g is out of range [%g;%g)",param,value,first,last);
}

inline void RaiseOutOfRange (const char* source,const char* param,double value,double first,double last)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%g is out of range [%g;%g)",param,value,first,last);
}

inline void RaiseOutOfRange (const char* source,const char* param,size_t index,size_t max_count)
{
  Raise<ArgumentOutOfRangeException> (source,"Argument <%s>=%u is out of range [0;%u)",param,index,max_count);
}

inline void RaiseNullArgument (const char* source,const char* param)
{
  Raise<ArgumentNullException> (source,"Null argument <%s>",param);
}

inline void RaiseNotImplemented (const char* source)
{
  Raise<NotImplementedException> (NULL,"Method '%s' doesn't implemented",source);
}

inline void RaiseNotSupported (const char* source)
{
  Raise<NotSupportedException> (NULL,"Method '%s' doesn't supported",source);
}

inline void RaiseNotSupported (const char* source,const char* format,...)
{
  va_list list;

  va_start (list,format);

  VRaise<NotSupportedException> (source,format,list);
}

inline void VRaiseNotSupported (const char* source,const char* format,va_list list)
{  
  VRaise<NotSupportedException> (source,format,list);
}

inline void RaiseInvalidOperation (const char* source,const char* format,...)
{
  va_list list;

  va_start (list,format);

  VRaise<OperationException> (source,format,list);
}

inline void VRaiseInvalidOperation (const char* source,const char* format,va_list list)
{  
  VRaise<OperationException> (source,format,list);
}
