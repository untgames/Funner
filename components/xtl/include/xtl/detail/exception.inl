inline void exception::touch  (const char* format, ...) throw ()
{
  va_list args;

  va_start (args, format);

  vtouch (format, args);
}
