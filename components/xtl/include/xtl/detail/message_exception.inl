/*
===================================================================================================
    message_exception_base
===================================================================================================
*/

/*
    Конструкторы
*/

inline message_exception_base::message_exception_base (const char* in_message)
{
  if (in_message)
    message += in_message;
}

inline message_exception_base::message_exception_base (const char* format, va_list args)
{
  if (format)
    message.append_vformat (format, args);
}

inline message_exception_base::message_exception_base (const char* in_source, const char* in_message)
{
  if (in_message)
    message += in_message;    

  if (in_source && *in_source)
    message_exception_base::touch (in_source);
}

inline message_exception_base::message_exception_base (const char* source, const char* format, va_list args)
{
  if (format)
    message.append_vformat (format, args);
    
  if (source && *source)
    message_exception_base::touch (source);
}

/*
    Получение сообщения
*/

inline const char* message_exception_base::what () const throw ()
{
  return message.data ();
}

/*
    Добавление информации о контексте выброса
*/

inline void message_exception_base::touch (const char* format, ...) throw ()
{
  va_list args;
  
  va_start (args, format);
  
  message_exception_base::vtouch (format, args);
}

inline void message_exception_base::vtouch (const char* format, va_list args) throw ()
{
  if (!format || !*format)
    return;

  message.append         ("\n    at ");
  message.append_vformat (format, args);
}

/*
===================================================================================================
    message_exception
===================================================================================================
*/

template <class BaseException>
inline message_exception<BaseException>::message_exception (const char* message)
  : message_exception_base (message)
  {}

template <class BaseException>
inline message_exception<BaseException>::message_exception (const char* format, va_list args)
  : message_exception_base (format, args)
  {}

template <class BaseException>
inline message_exception<BaseException>::message_exception (const char* source, const char* message)
  : message_exception_base (source, message)
  {}

template <class BaseException>
inline message_exception<BaseException>::message_exception (const char* source, const char* format, va_list args)
  : message_exception_base (source, format, args)
  {}

/*
    Создание исключения с форматированным сообщением
*/

template <class Exception>
inline message_exception<Exception> vformat_exception (const char* source, const char* format, va_list args)
{
  return message_exception<Exception> (source, format, args);
}

template <class Exception>
inline message_exception<Exception> format_exception (const char* source, const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return message_exception<Exception> (source, format, args);
}
