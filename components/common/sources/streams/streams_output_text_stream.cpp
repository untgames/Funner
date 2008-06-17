#include "shared.h"

using namespace common;

const size_t FORMAT_BUFFER_SIZE = 16; //размер буфера форматирования

/*
    Описание реализации текстового потока вывода
*/

struct OutputTextStream::Impl
{
  OutputStreamBuffer buffer; //буфер

  Impl (size_t buffer_size, const WriteFunction& in_writer = &OutputStreamBuffer::DefaultWriter) : buffer (in_writer, buffer_size) {}
};

/*
    Конструктор / деструктор
*/

OutputTextStream::OutputTextStream (size_t buffer_size)
  : impl (new Impl (buffer_size))
  {}

OutputTextStream::OutputTextStream (const WriteFunction& writer, size_t buffer_size)
  : impl (new Impl (buffer_size, writer))
  {}

OutputTextStream::~OutputTextStream ()
{
}

/*
    Буфер потока
*/

const common::OutputStreamBuffer& OutputTextStream::Buffer () const
{
  return impl->buffer;
}

common::OutputStreamBuffer& OutputTextStream::Buffer ()
{
  return impl->buffer;
}

/*
    Вывод строк
*/

void OutputTextStream::Write (const char* string, size_t size)
{
  if (!string && size)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const char*,size_t)", "string");
    
  size_t write_size = impl->buffer.Write (string, size);
    
  if (write_size != size)
    throw xtl::format_exception<StreamWriteException> ("common::OutputTextStream::Write", "Could not write block with size %u to the buffer (write_size=%u)",
                                 size, write_size);
}

void OutputTextStream::Write (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const char*)", "string");
    
  Write (string, strlen (string));
}

void OutputTextStream::Write (const wchar_t*, size_t)
{
  throw xtl::make_not_implemented_exception ("common::OutputTextStream::Write(const wchar_t*,size_t)");
}

void OutputTextStream::Write (const wchar_t* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const wchar_t*)", "string");

  return Write (string, wcslen (string));
}

namespace common
{

/*
    Вывод строк
*/

void write (OutputTextStream& stream, const char* string)
{
  stream.Write (string);
}

void write (OutputTextStream& stream, const wchar_t* string)
{
  stream.Write (string);
}

/*
    Вывод символов
*/

void write (OutputTextStream& stream, char symbol)
{
  stream.Write (&symbol, 1);
}

void write (OutputTextStream& stream, wchar_t symbol)
{
  stream.Write (&symbol, 1);
}

void write (OutputTextStream& stream, size_t count, char symbol)
{
  if (!count)
    return;

  char buf [128];
  const size_t BUF_SIZE = sizeof (buf);
  
  memset (buf, symbol, BUF_SIZE < count ? BUF_SIZE : count);

  for (size_t i=0, passes_count=count/BUF_SIZE; i<passes_count; i++)
    stream.Write (buf, BUF_SIZE);

  stream.Write (buf, count % BUF_SIZE);
}

void write (OutputTextStream& stream, size_t count, wchar_t symbol)
{
  if (!count)
    return;

  const size_t BUF_SIZE = 128;
  wchar_t buf [BUF_SIZE];
  
  for (size_t i=0, fill_size=count<BUF_SIZE?count:BUF_SIZE; i<fill_size; i++)
    buf [i] = symbol;

  for (size_t i=0, passes_count=count/BUF_SIZE; i<passes_count; i++)
    stream.Write (buf, BUF_SIZE);

  stream.Write (buf, count % BUF_SIZE);
}

/*
    Вывод целых чисел
      format:
        ""      - вывод с форматированием по умолчанию
        "hex"   - вывод в шестнадцатиричном формате
        "#"     - вывод с заполнителем по умолчанию (' ') и шириной не меньше 1
        "000"   - вывод с заполнителем 0 и шириной 3
        "+##"   - вывод с заполнителем по умолчанию, шириной 3 и указанием знака
        "hex:#" - вывод с заполнителем по умолчанию, шириной 1 в шастнадцатиричном формате
*/

namespace
{

//приведение формата к printf-форме
void get_int_printf_format (const char*& format, bool sign, char buffer [FORMAT_BUFFER_SIZE])
{
  char* pos         = buffer;
  char  base_format = sign ? 'd' : 'u';

  *pos++ = '%';

  if (!strncmp (format, "hex:", 4))
  {
    base_format  = 'x';
    format      += 4;
  }
  else if (!strncmp (format, "oct:", 4))
  {
    base_format  = 'o';
    format      += 4;
  }
  else if (!strncmp (format, "dec:", 4))
  {
    base_format  = 'u';
    format      += 4;
  }
  
  switch (*format)
  {
    case '+':
    case '-':
      *pos++ = '+';
      format++;
      break;
  }

  xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%u%c",
                             *format == '0' ? "0" : "", strlen (format), base_format);
}

//печать целого числа
void write_int (OutputTextStream& stream, unsigned int value, const char* format, bool sign)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");

  char format_buffer [FORMAT_BUFFER_SIZE], value_buffer [16];

  get_int_printf_format (format, sign, format_buffer);

  xtl::xsnprintf (value_buffer, sizeof (value_buffer), format_buffer, value);

  stream.Write (value_buffer);
}

}

void write (OutputTextStream& stream, int value, const char* format)
{
  write_int (stream, static_cast<unsigned int> (value), format, true);
}

void write (OutputTextStream& stream, unsigned int value, const char* format)
{
  write_int (stream, value, format, false);
}

/*
  Вывод чисел с плавающей точкой
    format (аналогично целым числам, исключая системы счисления):
      отсутствие точки - печать дробной части с форматированием по умолчанию
      ".##" - ширина не больше 2-х (1.2=1.2, 1.211=1.2)
      ".00" - ширина 2 (1.2=1.20, 1.211=1.21)
*/

namespace
{

//приведение формата к printf-форме
void get_float_printf_format (const char*& format, char buffer [FORMAT_BUFFER_SIZE])
{
  char* pos = buffer;

  *pos++ = '%';

  switch (*format)
  {
    case '+':
    case '-':
      *pos++ = '+';
      format++;
      break;
  }
  
  const char *dot = strchr (format, '.');
  
  if (dot)
  {
    const char* frac_format = dot + 1;
    size_t      frac_size   = strlen (frac_format);
    
    xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%u.%uf",
                             *format == '0' ? "0" : "", dot - format + frac_size + 1, frac_size);
  }
  else
  {
    xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%ug",
                             *format == '0' ? "0" : "", strlen (format));
  }
}

}

void write (OutputTextStream& stream, double value, const char* format)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");

  char format_buffer [FORMAT_BUFFER_SIZE], value_buffer [16];

  get_float_printf_format (format, format_buffer);

  xtl::xsnprintf (value_buffer, sizeof (value_buffer), format_buffer, value);

  stream.Write (value_buffer);  
}

void write (OutputTextStream& stream, float value, const char* format)
{
  write (stream, static_cast<double> (value), format);
}

void write (OutputTextStream& stream, long double value, const char* format)
{
  write (stream, static_cast<double> (value), format);
}

/*
    Вывод логических значений
      format:
        "alpha" - вывод в текстовом виде (true, false)
        ""      - вывод в численном виде (0, 1)
*/

void write (OutputTextStream& stream, bool value, const char* format)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");
    
  if (!strcmp (format, "alpha")) stream.Write (value ? "true" : "false");
  else if (!*format)             stream.Write (value ? "1" : "0");
  else
  {
    throw xtl::make_argument_exception ("common::write", "format", format);
  }
}

}
