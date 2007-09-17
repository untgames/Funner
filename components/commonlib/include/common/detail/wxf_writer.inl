/*
    Работа с фреймами
*/

template <class T>
inline void WxfWriter::BeginFrame (const char* frame_tag, const T& value)
{
  OutputTextStream& stream = Stream ();

  CheckTag     (frame_tag, "common::WxfWriter::BeginFrame");
  WriteIndent  ();
  write        (stream, frame_tag);
  WriteIndent  (1);
  wxf_write    (stream, value);
  BeginFrame   ();
}

template <class T>
inline void WxfWriter::BeginFrame (const char* frame_tag, const T& value, const char* format)
{
  OutputTextStream& stream = Stream ();

  CheckTag     (frame_tag, "common::WxfWriter::BeginFrame");
  WriteIndent  ();
  write        (stream, frame_tag);
  WriteIndent  (1);
  wxf_write    (stream, value, format);
  BeginFrame   ();
}

/*
    Сериализация атрибутов
*/

template <class T>
inline void WxfWriter::Write (const char* tag, const T& value)
{
  OutputTextStream& stream = Stream ();

  CheckTag (tag, "common::WxfWriter::Write");

  size_t tag_len = strlen (tag), attr_indent = AttributesIndent ();

  WriteIndent  ();
  write        (stream, tag);
  WriteIndent  (tag_len < attr_indent ? attr_indent - tag_len : 1);
  wxf_write    (stream, value);
  write        (stream, "\n");
}

template <class T>
inline void WxfWriter::Write (const char* tag, const T& value, const char* format)
{
  OutputTextStream& stream = Stream ();

  CheckTag (tag, "common::WxfWriter::Write");

  size_t tag_len = strlen (tag), attr_indent = AttributesIndent ();

  WriteIndent  ();
  write        (stream, tag);
  WriteIndent  (tag_len < attr_indent ? attr_indent - tag_len : 1);
  wxf_write    (stream, value, format);
  write        (stream, "\n");
}

/*
    Сериализация диапазонов атрибутов
*/

template <class T>
inline void WxfWriter::WriteRange (const char* tag, size_t count, const T* array)
{
  WriteRange (tag, array, array + count);
}

template <class T>
inline void WxfWriter::WriteRange (const char* tag, size_t count, const T* array, const char* format)
{
  WriteRange (tag, array, array + count, format);
}

template <class InIter>
inline void WxfWriter::WriteRange (const char* tag, InIter first, InIter last)
{
  OutputTextStream& stream = Stream ();

  CheckTag    (tag, "common::WxfWriter::WriteRange");
  WriteIndent ();
  write       (stream, tag);
  write       (stream, "\n");
  WriteIndent ();
  write       (stream, "(\n");

  for (;first != last; ++first)
  {
    WriteIndent ();
    WriteIndent (FrameIndent ());
    wxf_write   (stream, *first);
    write       (stream, "\n");
  }

  WriteIndent ();
  write       (stream, ")\n");
}

template <class InIter>
inline void WxfWriter::WriteRange (const char* tag, InIter first, InIter last, const char* format)
{
  OutputTextStream& stream = Stream ();

  CheckTag    (tag, "common::WxfWriter::WriteRange");
  WriteIndent ();
  write       (stream, tag);
  WriteIndent ();
  write       (stream, "(\n");

  for (;first != last; ++first)
  {
    WriteIndent ();
    WriteIndent (FrameIndent ());
    wxf_write   (stream, *first, format);
    write       (stream, "\n");
  }

  WriteIndent ();
  write       (stream, ")\n");
}

/*
    Перегрузки вывода различных типов атрибутов
*/

template <class T>
inline void wxf_write (OutputTextStream& stream, const T& value)
{
  write (stream, value);
}

template <class T>
inline void wxf_write (OutputTextStream& stream, const T& value, const char* format)
{
  write (stream, value, format);
}

inline void wxf_write (OutputTextStream& stream, signed char symbol)
{
  write (stream, "\"");
  write (stream, symbol);
  write (stream, "\"");
}

inline void wxf_write (OutputTextStream& stream, unsigned char symbol)
{
  write (stream, static_cast<signed char> (symbol));
}

inline void wxf_write (OutputTextStream& stream, const char* string)
{
  write (stream, "\"");
  write (stream, string);
  write (stream, "\"");
}

inline void wxf_write (OutputTextStream& stream, const wchar_t* string)
{
  write (stream, "\"");
  write (stream, string);
  write (stream, "\"");
}

template <class Traits, class Allocator>
inline void wxf_write (OutputTextStream& stream, const stl::basic_string<char, Traits, Allocator>& string)
{
  write (stream, string.c_str ());
}

template <class Traits, class Allocator>
inline void wxf_write (OutputTextStream& stream, const stl::basic_string<wchar_t, Traits, Allocator>& string)
{
  write (stream, string.c_str ());
}
