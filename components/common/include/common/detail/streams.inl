/*
    OutputTextStream
*/

/*
    Вывод строк
*/

template <class Traits, class Allocator>
inline void write (OutputTextStream& stream, const stl::basic_string<char, Traits, Allocator>& string)
{
  stream.Write (string.c_str (), string.size ());
}

template <class Traits, class Allocator>
inline void write (OutputTextStream& stream, const stl::basic_string<wchar_t, Traits, Allocator>& string)
{
  stream.Write (string.c_str (), string.size ());
}

/*
    Вывод интервалов
*/

template <class InIter>
inline void write_range (OutputTextStream& stream, InIter first, InIter last)
{
  if (first == last)
    return;

  for (;;)
  {
    write (stream, *first);

    ++first;

    if (first != last) stream.Write (" ");
    else               break;
  }  
}

template <class InIter>
inline void write_range (OutputTextStream& stream, InIter first, InIter last, const char* format)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write_range", "format");

  if (first == last)
    return;
    
  static char default_separator = ' ';
    
  const char *separator_start = &default_separator, *separator_end = separator_start + 1;
    
  if (*format == '[')
  {
    for (const char* s=format+1; *s; s++)
      if (*s == ']')
      {
        separator_start = format + 1;
        separator_end   = s;
        format          = s + 1;

        break;
      }
  }
  
  size_t separator_size = separator_end - separator_start;

  for (;;)
  {
    write (stream, *first, format);

    ++first;

    if (first != last) stream.Write (separator_start, separator_size);
    else               break;
  }  
}

template <class FwdIter>
inline void write (OutputTextStream& stream, const xtl::iterator_range<FwdIter>& range, const char* format)
{
  write_range (stream, range.begin (), range.end (), format);
}

template <class FwdIter>
inline void write (OutputTextStream& stream, const xtl::iterator_range<FwdIter>& range)
{
  write_range (stream, range.begin (), range.end ());
}

/*
    Сериализация математических типов
*/

template <class T, unsigned int Size>
inline void write (common::OutputTextStream& stream, const math::vector<T, Size>& value, const char* format)
{
  write_range (stream, &value [0], &value [Size], format);
}

template <class T, unsigned int Size>
inline void write (common::OutputTextStream& stream, const math::matrix<T, Size>& value, const char* format)
{
  write_range (stream, &value [0][0], &value [Size-1][Size], format);
}

template <class T>
inline void write (common::OutputTextStream& stream, const math::quat<T>& value, const char* format)
{
  write_range (stream, &value [0], &value [4], format);
}
