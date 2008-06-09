/*
    message_exception
*/

/*
    Конструкторы
*/

inline message_exception::message_exception (const char* in_message)
{
  message += in_message ? in_message : "";
}

inline message_exception::message_exception (const char* in_source, const char* in_message)
{
  message += in_message ? in_message : "";
  
  if (in_source)
    message_exception::touch (in_source);
}

/*
    Получение сообщения
*/

inline const char* message_exception::what () const throw ()
{
  return message.data ();
}

/*
    Добавление информации о контексте выброса
*/

inline void message_exception::touch  (const char* format, ...) throw ()
{
  va_list args;
  
  va_start (args, format);
  
  message_exception::vtouch (format, args);
}

inline void message_exception::vtouch (const char* format, va_list args) throw ()
{
  if (!format)
    return;

  message.append         ("\n    at ");
  message.append_vformat (format, args);
}

/*
    derived_exception
*/

template <class Tag, class BaseException>
inline derived_exception<Tag, BaseException>::derived_exception (const char* message)
  : BaseException (message)
  {}

template <class Tag, class BaseException>  
inline derived_exception<Tag, BaseException>::derived_exception (const char* source, const char* message)
  : BaseException (source, message)
  {}
