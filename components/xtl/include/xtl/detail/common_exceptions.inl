/*
    Неверное значение аргумента переданного функции
*/

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name)
{
  return format_exception<argument_exception> (source, "Invalid argument <%s>", argument_name);
}

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, const char* value, const char* comment)
{
  return format_exception<argument_exception> (source, comment ? "Invalid argument <%s>='%s'. %s" : 
    "Invalid argument <%s>='%s'", argument_name, value, comment);
}

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, int value, const char* comment)
{
  return format_exception<argument_exception> (source, comment ? "Invalid argument <%s>=%d. %s" : 
    "Invalid argument <%s>=%d", argument_name, value, comment);
}

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, size_t value, const char* comment)
{
  return format_exception<argument_exception> (source, comment ? "Invalid argument <%s>=%u. %s" : 
    "Invalid argument <%s>=%u", argument_name, value, comment);
}

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, float value, const char* comment)
{
  return format_exception<argument_exception> (source, comment ? "Invalid argument <%s>=%g. %s" : 
    "Invalid argument <%s>=%g", argument_name, value, comment);
}

inline message_exception<argument_exception> make_argument_exception (const char* source, const char* argument_name, double value, const char* comment)
{
  return format_exception<argument_exception> (source, comment ? "Invalid argument <%s>=%g. %s" : "Invalid argument <%s>=%g",
    argument_name, value, comment);
}

/*
    Нулевое значение аргумента
*/

inline message_exception<null_argument_exception> make_null_argument_exception (const char* source, const char* argument_name)
{
  return format_exception<null_argument_exception> (source, "Null argument <%s>", argument_name);
}

/*
    Значение аргумента выходит за пределы допустимых значений
*/

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name)
{
  return format_exception<bad_range> (source, "Argument <%s> is out of range", argument_name);
}

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, int value, int first, int last)
{
  return format_exception<bad_range> (source, "Argument <%s>=%d is out of range [%d;%d)", argument_name, value, first, last);
}

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, size_t value, size_t first, size_t last)
{
  return format_exception<bad_range> (source, "Argument <%s>=%u is out of range [%u;%u)", argument_name, value, first, last);
}

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, float value, float first, float last)
{
  return format_exception<bad_range> (source,"Argument <%s>=%g is out of range [%g;%g)", argument_name, value, first, last);
}

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, double value, double first, double last)
{
  return format_exception<bad_range> (source,"Argument <%s>=%g is out of range [%g;%g)", argument_name, value, first, last);
}

inline message_exception<bad_range> make_range_exception (const char* source, const char* argument_name, size_t index, size_t max_count)
{
  return format_exception<bad_range> (source,"Argument <%s>=%u is out of range [0;%u)", argument_name, index, max_count);
}

/*
    Неверная операция
*/

inline message_exception<operation_exception> format_operation_exception (const char* source, const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return vformat_operation_exception (source, format, args);
}

inline message_exception<operation_exception> vformat_operation_exception (const char* source, const char* format, va_list list)
{
  return vformat_exception<operation_exception> (source, format, list);
}

/*
    Функция не реализована
*/

inline message_exception<not_implemented_exception> make_not_implemented_exception (const char* source)
{
  return format_exception<not_implemented_exception> (0, "Method '%s' doesn't implemented", source);  
}

/*
    Функция не поддерживается
*/

inline message_exception<not_supported_exception> format_not_supported_exception  (const char* source)
{
  return format_exception<not_supported_exception> ("", "Method '%s' not supported", source);
}

inline message_exception<not_supported_exception> format_not_supported_exception (const char* source, const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return vformat_not_supported_exception (source, format, args);
}

inline message_exception<not_supported_exception> vformat_not_supported_exception (const char* source, const char* format, va_list list)
{
  return vformat_exception<not_supported_exception> (source, format, list);
}

/*
    Платформа не поддерживается
*/

inline message_exception<bad_platform> make_unsupported_platform_exception (const char* source, const char* platform)
{
  return format_exception<bad_platform> (source, platform ? "%s platform not supported" : "Platform not supported", platform);
}
