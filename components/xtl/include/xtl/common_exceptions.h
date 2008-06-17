#ifndef XTL_COMMON_EXCEPTIONS_HEADER
#define XTL_COMMON_EXCEPTIONS_HEADER

#include <xtl/message_exception.h>

namespace xtl
{

/*
    Иерархия базовых классов исключений
*/

struct argument_exception:               virtual public exception {};
struct null_argument_exception:                  public argument_exception {};
struct argument_range_exception:                 public argument_exception {};
struct operation_exception:              virtual public exception {};
struct not_supported_exception:                  public operation_exception {};
struct platform_not_supported_exception:         public not_supported_exception {};
struct not_implemented_exception:        virtual public exception {};

typedef argument_exception               bad_argument;
typedef argument_range_exception         bad_range;
typedef operation_exception              bad_operation;
typedef not_implemented_exception        bad_implementation;
typedef platform_not_supported_exception bad_platform;

/*
    Создание исключений
*/

///неверное значение аргумента переданного функции
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name);
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, const char* value, const char* comment=0);
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, int value, const char* comment=0);
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, size_t value, const char* comment=0);
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, float value, const char* comment=0);
message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, double value, const char* comment=0);

///нулевое значение аргумента
message_exception<null_argument_exception> make_null_argument_exception (const char* source, const char* argument_name);

///значение аргумента выходит за пределы допустимых значений
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name);
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, int value, int first, int last);
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, size_t value, size_t first, size_t last);
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, float value, float first, float last);
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, double value, double first, double last);
message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, size_t index, size_t max_count);

///неверная операция
message_exception<operation_exception> format_operation_exception  (const char* source, const char* format, ...);
message_exception<operation_exception> vformat_operation_exception (const char* source, const char* format, va_list list);

///функция не реализована
message_exception<not_implemented_exception> make_not_implemented_exception (const char* source);

///aункция не поддерживается
message_exception<not_supported_exception> format_not_supported_exception  (const char* source);
message_exception<not_supported_exception> format_not_supported_exception  (const char* source, const char* format, ...);
message_exception<not_supported_exception> vformat_not_supported_exception (const char* source, const char* format, va_list list);

///gлатформа не поддерживается
message_exception<bad_platform> make_unsupported_platform_exception (const char* source, const char* platform=0);

#include <xtl/detail/common_exceptions.inl>

}

#endif
