/*
    Перегрузки вывода различных типов атрибутов
*/

template <class T>
inline void xml_write (OutputTextStream& stream, const T& value)
{
  write (stream, value);
}

template <class T>
inline void xml_write (OutputTextStream& stream, const T& value, const char* format)
{
  write (stream, value, format);
}

template <class T>
inline void xml_write_data (OutputTextStream& stream, const T& value)
{
  write (stream, value);
}

template <class T>
inline void xml_write_data (OutputTextStream& stream, const T& value, const char* format)
{
  write (stream, value, format);
}

inline void xml_write_data (OutputTextStream& stream, const char* value)
{
  write (stream, '\'');
  write (stream, value);
  write (stream, '\'');
}

inline void xml_write_data (OutputTextStream& stream, const char* value, const char* format)
{
  xml_write_data (stream, value);
}

template <class Char, class Traits, class Allocator>
inline void xml_write_data (OutputTextStream& stream, const stl::basic_string<Char, Traits, Allocator>& value)
{
  xml_write_data (stream, value.c_str ());
}

template <class Char, class Traits, class Allocator>
inline void xml_write_data (OutputTextStream& stream, const stl::basic_string<Char, Traits, Allocator>& value, const char* format)
{
  xml_write_data (stream, value);
}

/*
    Сериализация атрибутов
*/

template <class T>
inline void XmlWriter::WriteAttribute (const char* name, const T& value)
{
  BeginAttribute (name);
  
  try
  {
    xml_write    (Stream (), value);
    EndAttribute ();
  }
  catch (...)
  {
    EndAttribute ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteAttribute (const char* name, const T& value, const char* format)
{
  BeginAttribute (name);

  try
  {
    xml_write    (Stream (), value, format);
    EndAttribute ();
  }
  catch (...)
  {
    EndAttribute ();
    throw;
  }  
}

/*
    Запись данных в узел
*/

template <class T>
inline void XmlWriter::WriteData (const T& value)
{
  BeginData ();
  
  try
  {
    xml_write_data (Stream (), value);
    EndData   ();
  }
  catch (...)
  {
    EndData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteData (const T& value, const char* format)
{
  BeginData ();
  
  try
  {
    xml_write_data (Stream (), value, format);
    EndData   ();
  }
  catch (...)
  {
    EndData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteCData (const T& value)
{
  BeginCData ();
  
  try
  {
    xml_write (Stream (), value);
    EndCData  ();
  }
  catch (...)
  {
    EndCData ();
    throw;
  }
}

template <class T>
inline void XmlWriter::WriteCData (const T& value, const char* format)
{
  BeginCData ();
  
  try
  {
    xml_write (Stream (), value, format);
    EndCData  ();
  }
  catch (...)
  {
    EndCData ();
    throw;
  }
}

/*
    Обобщённая сериализация узлов (BeginNode+WriteData+EndNode)
*/

inline void XmlWriter::WriteNode (const char* name)
{
  BeginNode (name);
  EndNode ();
}

template <class T>
inline void XmlWriter::WriteNode (const char* name, const T& value)
{
  BeginNode (name);
  
  try
  {
    WriteData (value);
    EndNode   ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNode (const char* name, const T& value, const char* format)
{
  BeginNode (name);
  
  try
  {
    WriteData (value, format);
    EndNode   ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNodeCData (const char* name, const T& value)
{
  BeginNode (name);
  
  try
  {
    WriteCData (value);
    EndNode    ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

template <class T>
inline void XmlWriter::WriteNodeCData (const char* name, const T& value, const char* format)
{
  BeginNode (name);
  
  try
  {
    WriteCData (value, format);
    EndNode    ();
  }
  catch (...)
  {
    EndNode ();
    throw;
  }  
}

/*
    Блок сериализации узлов Xml
*/

inline XmlNodeScope::XmlNodeScope (XmlWriter& in_writer, const char* frame_tag)
  : writer (in_writer)
{
  writer.BeginNode (frame_tag);  
}

inline XmlNodeScope::~XmlNodeScope ()
{
  writer.EndNode ();
}
