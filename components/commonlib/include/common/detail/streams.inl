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

namespace detail
{

template <class InIter, class Separator>
inline void write_range (OutputTextStream& stream, InIter first, InIter last, const char* format, const Separator& separator)
{
  for (;;)
  {
    write (stream, *first, format);

    ++first;

    if (first != last) write (stream, separator);
    else               break;
  }
}

}

template <class InIter>
inline void write_range (OutputTextStream& stream, InIter first, InIter last, const char* format, const char* separator)
{
  detail::write_range (stream, first, last, format, separator);
}

template <class InIter>
inline void write_range (OutputTextStream& stream, InIter first, InIter last, const char* format, const wchar_t* separator)
{
  detail::write_range (stream, first, last, format, separator);
}
