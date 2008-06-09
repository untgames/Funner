#ifndef XTL_COMMON_EXCEPTIONS_HEADER
#define XTL_COMMON_EXCEPTIONS_HEADER

#include <xtl/message_exception.h>

namespace xtl
{

/*
    Базовые классы исключений
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Неверное значение аргумента переданного функции
///////////////////////////////////////////////////////////////////////////////////////////////////
struct argument_exception_tag;

typedef derived_exception<argument_exception_tag> argument_exception, bad_argument;

argument_exception make_argument_exception (const char* source, const char* argument_name);
argument_exception make_argument_exception (const char* source, const char* argument_name, const char* value, const char* comment=0);
argument_exception make_argument_exception (const char* source, const char* argument_name, int value, const char* comment=0);
argument_exception make_argument_exception (const char* source, const char* argument_name, size_t value, const char* comment=0);
argument_exception make_argument_exception (const char* source, const char* argument_name, float value, const char* comment=0);
argument_exception make_argument_exception (const char* source, const char* argument_name, double value, const char* comment=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Нулевое значение аргумента
///////////////////////////////////////////////////////////////////////////////////////////////////
struct null_argument_exception_tag;

typedef derived_exception<null_argument_exception_tag, argument_exception> null_argument_exception;

null_argument_exception make_null_argument_exception (const char* source, const char* argument_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Значение аргумента выходит за пределы допустимых значений
///////////////////////////////////////////////////////////////////////////////////////////////////
struct argument_range_exception_tag;

typedef derived_exception<argument_range_exception_tag, argument_exception> argument_range_exception, bad_range;

bad_range make_range_exception (const char* source, const char* argument_name);
bad_range make_range_exception (const char* source, const char* argument_name, int value, int first, int last);
bad_range make_range_exception (const char* source, const char* argument_name, size_t value, size_t first, size_t last);
bad_range make_range_exception (const char* source, const char* argument_name, float value, float first, float last);
bad_range make_range_exception (const char* source, const char* argument_name, double value, double first, double last);
bad_range make_range_exception (const char* source, const char* argument_name, size_t index, size_t max_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Неверная операция
///////////////////////////////////////////////////////////////////////////////////////////////////
struct operation_exception_tag;

typedef derived_exception<operation_exception_tag> operation_exception, bad_operation;

operation_exception format_operation_exception  (const char* source, const char* format, ...);
operation_exception vformat_operation_exception (const char* source, const char* format, va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функция не реализована
///////////////////////////////////////////////////////////////////////////////////////////////////
struct not_implemented_exception_tag;

typedef derived_exception<not_implemented_exception_tag> not_implemented_exception;

not_implemented_exception make_not_implemented_exception (const char* source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функция не поддерживается
///////////////////////////////////////////////////////////////////////////////////////////////////
struct not_supported_exception_tag;

typedef derived_exception<not_supported_exception_tag, bad_operation> not_supported_exception;

not_supported_exception format_not_supported_exception  (const char* source, const char* format, ...);
not_supported_exception vformat_not_supported_exception (const char* source, const char* format, va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа не поддерживается
///////////////////////////////////////////////////////////////////////////////////////////////////
struct platform_not_supported_exception_tag;

typedef derived_exception<platform_not_supported_exception_tag, not_supported_exception> platform_not_supported_exception, bad_platform;

bad_platform make_unsupported_platform_exception (const char* source, const char* platform=0);

#include <xtl/detail/common_exceptions.inl>

}

#endif
