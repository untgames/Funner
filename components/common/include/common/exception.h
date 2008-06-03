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
void raise (const char* source,const char* format,...);

template <class Exception>
void vraise (const char* source,const char* format,va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательные функции для выброса стандартных исключений
///////////////////////////////////////////////////////////////////////////////////////////////////
void raise_invalid_argument   (const char* source,const char* param);
void raise_invalid_argument   (const char* source,const char* param,const char* value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,int value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,size_t value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,float value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,double value,const char* comment=NULL);
void raise_out_of_range        (const char* source,const char* param);
void raise_out_of_range        (const char* source,const char* param,int value,int first,int last);
void raise_out_of_range        (const char* source,const char* param,size_t value,size_t first,size_t last);
void raise_out_of_range        (const char* source,const char* param,float value,float first,float last);
void raise_out_of_range        (const char* source,const char* param,double value,double first,double last);
void raise_out_of_range        (const char* source,const char* param,size_t index,size_t max_count);
void raise_null_argument      (const char* source,const char* param);
void raise_not_implemented    (const char* source);
void raise_invalid_operation  (const char* source, const char* format, ...);
void vraise_invalid_operation (const char* source, const char* format, va_list list);
void raise_not_supported      (const char* source);
void raise_not_supported      (const char* source,const char* format,...);
void vraise_not_supported     (const char* source,const char* format,va_list list);

#include <common/detail/exception.inl>

}

#endif
