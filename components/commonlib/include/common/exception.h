#ifndef COMMONLIB_EXCEPTION_HEADER
#define COMMONLIB_EXCEPTION_HEADER

#include <exception>
#include <stddef.h>
#include <stdarg.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тэги унаследованных исключений
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ArgumentExceptionTag;             //неверное значение аргумента переданного функции
struct ArgumentNullExceptionTag;         //нулевое значение аргумента
struct ArgumentOutOfRangeExceptionTag;   //значение аргумента выходит за пределы допустимых значений
struct OperationExceptionTag;            //неверная операция
struct NotImplementedExceptionTag;       //функция не реализована
struct NotSupportedExceptionTag;         //функция не поддерживается
struct PlatformNotSupportedExceptionTag; //платформа не поддерживается

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс исключений библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
class Exception: public std::exception
{
  public:
    Exception  (const char* message);
    Exception  (const char* source, const char* message);
    Exception  (const Exception&);
    ~Exception () throw ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Exception& operator = (const Exception&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сообщение об ошибке
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Message () const throw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление информации о контексте выброса
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Touch  (const char* format, ...);
    void VTouch (const char* format, va_list args);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределение методов std::exception
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* what () const throw ();
    
  private:
    void TouchImpl (const char* source);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конкретные классы исключений
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class BaseException,class Tag>
class DerivedException: public BaseException
{
  public:
    DerivedException (const char* message);
    DerivedException (const char* source, const char* message);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конкретные классы исключений
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef DerivedException<Exception,ArgumentExceptionTag>                   ArgumentException;
typedef DerivedException<ArgumentException,ArgumentNullExceptionTag>       ArgumentNullException;
typedef DerivedException<ArgumentException,ArgumentOutOfRangeExceptionTag> ArgumentOutOfRangeException;
typedef DerivedException<Exception,OperationExceptionTag>                  OperationException;
typedef DerivedException<Exception,NotImplementedExceptionTag>             NotImplementedException;
typedef DerivedException<Exception,NotSupportedExceptionTag>               NotSupportedException;
typedef DerivedException<Exception,PlatformNotSupportedExceptionTag>       PlatformNotSupportedException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выброс исключения с форматированием сообщения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Exception>
void Raise (const char* source,const char* format,...);

template <class Exception>
void VRaise (const char* source,const char* format,va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательные функции для выброса стандартных исключений
///////////////////////////////////////////////////////////////////////////////////////////////////
void RaiseInvalidArgument   (const char* source,const char* param);
void RaiseInvalidArgument   (const char* source,const char* param,const char* value,const char* comment=NULL);
void RaiseInvalidArgument   (const char* source,const char* param,int value,const char* comment=NULL);
void RaiseInvalidArgument   (const char* source,const char* param,size_t value,const char* comment=NULL);
void RaiseInvalidArgument   (const char* source,const char* param,float value,const char* comment=NULL);
void RaiseInvalidArgument   (const char* source,const char* param,double value,const char* comment=NULL);
void RaiseOutOfRange        (const char* source,const char* param);
void RaiseOutOfRange        (const char* source,const char* param,int value,int first,int last);
void RaiseOutOfRange        (const char* source,const char* param,size_t value,size_t first,size_t last);
void RaiseOutOfRange        (const char* source,const char* param,float value,float first,float last);
void RaiseOutOfRange        (const char* source,const char* param,double value,double first,double last);
void RaiseOutOfRange        (const char* source,const char* param,size_t index,size_t max_count);
void RaiseNullArgument      (const char* source,const char* param);
void RaiseNotImplemented    (const char* source);
void RaiseInvalidOperation  (const char* source, const char* format, ...);
void VRaiseInvalidOperation (const char* source, const char* format, va_list list);
void RaiseNotSupported      (const char* source);
void RaiseNotSupported      (const char* source,const char* format,...);
void VRaiseNotSupported     (const char* source,const char* format,va_list list);

#include <common/detail/exception.inl>

}

#endif
