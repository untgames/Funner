/*
    TextSerializer
*/

inline TextSerializer::TextSerializer (OutputTextStream& _stream,const char* _separator)
  : stream (_stream), separator (_separator)
  { }

/*
    Вывод разделителя
*/

inline void TextSerializer::WriteSeparator ()
{
  stream.Print (separator.c_str ());
}

/*
    Сериализация базовых типов
*/

inline void TextSerializer::Write (bool value)
{
  stream.Print (value?"true":"false");
}

inline void TextSerializer::Write (char value)
{
  stream.Printf ("%c",value);
}

inline void TextSerializer::Write (unsigned char value)
{
  stream.Printf ("%c",value);
}

inline void TextSerializer::Write (short value)
{
  stream.Printf ("%d",value);
}

inline void TextSerializer::Write (unsigned short value)
{
  stream.Printf ("%u",value);
}

inline void TextSerializer::Write (int value)
{
  stream.Printf ("%d",value);
}

inline void TextSerializer::Write (unsigned int value)
{
  stream.Printf ("%u",value);
}

inline void TextSerializer::Write (long value)
{
  stream.Printf ("%d",value);
}

inline void TextSerializer::Write (unsigned long value)
{
  stream.Printf ("%Lu",value);
}

inline void TextSerializer::Write (float value)
{
  stream.Printf ("%g",value);
}

inline void TextSerializer::Write (double value)
{
  stream.Printf ("%g",value);
}

inline void TextSerializer::Write (long double value)
{
  Write ((double)value);
}

/*
    Сериализация строк
*/

inline void TextSerializer::Write (const char* string)
{
  stream.Print (string);
}

template <class Traits,class Allocator>
inline void TextSerializer::Write (const stl::basic_string<char,Traits,Allocator>& string)
{
  stream.Print (string.c_str ());
}

/*
    Сериализация математических типов
*/

template <class T,size_t size>
inline void TextSerializer::Write (const math::vec<T,size>& vector)
{
  for (size_t i=0;i<size-1;i++)
  {
    Write (vector [i]);
    WriteSeparator ();
  }
  
  Write (vector [size-1]);
}

template <class T,size_t size>
inline void TextSerializer::Write (const math::matrix<T,size>& matrix)
{
  const T* array = &matrix [0][0];

  for (size_t i=0;i<size*size-1;i++)
  {
    Write (array [i]);
    WriteSeparator ();
  }
  
  Write (array [size*size-1]);
}

template <class T>
inline void TextSerializer::Write (const math::quat<T>& quaternion)
{
  Write (quaternion [0]);
  WriteSeparator ();
  Write (quaternion [1]);
  WriteSeparator ();
  Write (quaternion [2]);
  WriteSeparator ();
  Write (quaternion [3]);  
}

/*
    TextSerializerIterator
*/

template <class T,class SerializerType>
inline TextSerializerIterator<T,SerializerType>::TextSerializerIterator (serializer_type& _serializer)
  : serializer (_serializer), first_output (true)
  { }

template <class T,class SerializerType>
inline TextSerializerIterator<T,SerializerType>& TextSerializerIterator<T,SerializerType>::operator = (const value_type& value)
{
  if (first_output) first_output = false;
  else              serializer.WriteSeparator ();

  serializer.Write (value);

  return *this;
}

template <class T,class SerializerType>
inline TextSerializerIterator<T,SerializerType> make_iterator (SerializerType& serializer)
{
  return TextSerializerIterator<T,SerializerType> (serializer);
}

/*
    BasicWxfSerializer
*/        

inline void __PrintIndent (OutputTextStream& stream,size_t indent)
{
  static char  buf [] = "                                ";
  const size_t BUF_SIZE = sizeof (buf) - 1;

  for (size_t i=0;i<indent/BUF_SIZE;i++)
    stream.Print (buf,BUF_SIZE);

  stream.Print (buf,indent % BUF_SIZE);
}

inline void __CheckWxfTagName (const char* tag,const char* function_name)
{
  if (!tag)
    RaiseNullArgument (function_name,"tag");

  for (const char* s=tag;*s;s++)
    switch (*s)
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y': 
      case 'Z':
      case 'a': case 'b': case 'c': case 'd': case 'e':
      case 'f': case 'g': case 'h': case 'i': case 'j':
      case 'k': case 'l': case 'm': case 'n': case 'o':
      case 'p': case 'q': case 'r': case 's': case 't':
      case 'u': case 'v': case 'w': case 'x': case 'y':
      case 'z':
      case '_':
        break;
      default:
        Raise<WrongWxfTagException> (function_name,"Wrong wxf-format tag '%s'",tag);
    }
}

template <class TextSerializerType>
inline BasicWxfSerializer<TextSerializerType>::BasicWxfSerializer
 (const File&  file,
  size_t       _frame_indent,
  size_t       _attr_indent)
    : stream (file), serializer (stream," "), frame_indent (_frame_indent), attr_indent (_attr_indent),
      indent (0), line_number (1), frame_start_line_number (line_number)
  { }

template <class TextSerializerType>
inline BasicWxfSerializer<TextSerializerType>::BasicWxfSerializer 
 (const char*  file_name,
  size_t       _frame_indent,
  size_t       _attr_indent)
    : stream (file_name), serializer (stream," "), frame_indent (_frame_indent), attr_indent (_attr_indent),
      indent (0), line_number (1), frame_start_line_number (line_number)
  { }

template <class TextSerializerType>
inline BasicWxfSerializer<TextSerializerType>::~BasicWxfSerializer ()
{
  while (indent) EndFrame ();
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::WriteIndent ()
{
  __PrintIndent (stream,indent);
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::WriteIndent (size_t indent)
{
  __PrintIndent (stream,indent);
}

template <class TextSerializerType> template <class T>
inline void BasicWxfSerializer<TextSerializerType>::WriteValue (const T& value,size_t value_indent)
{
  WriteIndent      (value_indent);
  serializer.Write (value);
  stream.Print     ("\n");
  
  line_number++;
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::WriteValue (const char* string,size_t value_indent)
{
  WriteIndent      (value_indent);
  stream.Print     ("\"");
  serializer.Write (string);
  stream.Print     ("\"\n");
  
  line_number++;
}

template <class TextSerializerType> template <class Traits,class Allocator>
inline void BasicWxfSerializer<TextSerializerType>::WriteValue 
 (const stl::basic_string<char,Traits,Allocator>& string,
  size_t                                          value_indent)
{
  WriteIndent      (value_indent);
  stream.Print     ("\"");
  serializer.Write (string);
  stream.Print     ("\"\n");

  line_number++;
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::WriteValue (char value,size_t value_indent)
{
  WriteIndent      (value_indent);
  stream.Print     ("\"");
  serializer.Write (value);
  stream.Print     ("\"\n");
  
  line_number++;
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::WriteValue (unsigned char value,size_t value_indent)
{
  WriteIndent      (value_indent);
  stream.Print     ("\"");
  serializer.Write (value);
  stream.Print     ("\"\n");
  
  line_number++;
}

template <class TextSerializerType> template <class T,size_t size>
void BasicWxfSerializer<TextSerializerType>::WriteValue (const math::matrix<T,size>& matrix,size_t)
{  
  stream.Print ("\n");
  WriteIndent  ();
  stream.Print ("(\n");

  indent += frame_indent;
    
  for (size_t i=0;i<size;i++) WriteEnumValue (matrix [i]);

  indent -= frame_indent;

  WriteIndent  ();
  stream.Print (")\n");
    
  line_number += 3;
}

template <class TextSerializerType> template <class T>
inline void BasicWxfSerializer<TextSerializerType>::WriteEnumValue (const T& value)
{
  WriteValue (value,indent);
}

template <class TextSerializerType> template <class T,size_t size>
void BasicWxfSerializer<TextSerializerType>::WriteEnumValue (const math::matrix<T,size>& matrix)
{
  WriteIndent  ();
  stream.Print ("(\n");

  indent += frame_indent;    

  for (size_t i=0;i<size;i++) WriteEnumValue (matrix [i]);

  indent -= frame_indent;

  WriteIndent  ();
  stream.Print (")\n");
  
  line_number += 2;
}

template <class TextSerializerType>
void BasicWxfSerializer<TextSerializerType>::BeginFrame (const char* frame_tag)
{    
  __CheckWxfTagName (frame_tag,"BasicWxfSerializer::BeginFrame");

  WriteIndent   ();
  stream.Printf ("%s\n",frame_tag);  
  WriteIndent   ();
  stream.Print  ("{\n");

  indent                  += frame_indent;
  line_number             += 2;
  frame_start_line_number  = line_number;
}

template <class TextSerializerType> template <class T>
void BasicWxfSerializer<TextSerializerType>::BeginFrame (const char* frame_tag,const T& value)
{
  __CheckWxfTagName (frame_tag,"BasicWxfSerializer::BeginFrame");

  WriteIndent  ();
  stream.Print (frame_tag);
  WriteValue   (value,1);
  WriteIndent  ();
  stream.Print ("{\n");
  
  line_number++;

  indent                  += frame_indent;
  frame_start_line_number  = line_number;
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::EndFrame ()
{
  if (!indent)
    return;
    
  indent -= frame_indent;
  
  WriteIndent  ();  
  stream.Print ("}\n");
  
  line_number++;
}

template <class TextSerializerType>
void BasicWxfSerializer<TextSerializerType>::Write (const char* tag)
{
  __CheckWxfTagName (tag,"BasicWxfSerializer::Write");
    
  WriteIndent  ();
  stream.Printf ("%s\n",tag);

  line_number++;
}

template <class TextSerializerType> template <class T>
void BasicWxfSerializer<TextSerializerType>::Write (const char* tag,const T& value)
{
  __CheckWxfTagName (tag,"BasicWxfSerializer::Write");
    
  size_t tag_len = strlen (tag);

  WriteIndent  ();
  stream.Print (tag);
  WriteValue   (value,tag_len < attr_indent ? attr_indent - tag_len : 1);
}

template <class TextSerializerType> template <class T>
void BasicWxfSerializer<TextSerializerType>::Write (const char* tag,size_t count,const T& value)
{
  __CheckWxfTagName (tag,"BasicWxfSerializer::Write");

  WriteIndent   ();
  stream.Printf ("%s\n",tag);
  WriteIndent   ();
  stream.Print  ("(\n");
  
  indent += frame_indent;

  while (count--) WriteEnumValue (value);

  indent -= frame_indent;

  WriteIndent  ();
  stream.Print (")\n");
  
  line_number += 3;
}

template <class TextSerializerType> template <class T>
void BasicWxfSerializer<TextSerializerType>::Write (const char* tag,size_t count,const T* array)
{
  __CheckWxfTagName (tag,"BasicWxfSerializer::Write");

  WriteIndent   ();
  stream.Printf ("%s\n",tag);  
  WriteIndent   ();  
  stream.Print  ("(\n");
  
  indent += frame_indent;

  while (count--) WriteEnumValue (*array++);

  indent -= frame_indent;

  WriteIndent  ();
  stream.Print (")\n");
  
  line_number += 3;
}

template <class TextSerializerType> template <class T>
inline void BasicWxfSerializer<TextSerializerType>::Write (const char* tag,size_t count,T* array)
{
  Write (tag,count,(const T*)array);
}

template <class TextSerializerType> template <class InIter>
void BasicWxfSerializer<TextSerializerType>::WriteDispatch (const char* tag,InIter first,InIter last,stl::input_iterator_tag)
{
  __CheckWxfTagName (tag,"BasicWxfSerializer::Write");

  WriteIndent   ();
  stream.Printf ("%s\n",tag);  
  WriteIndent   ();  
  stream.Print  ("(\n");
  
  indent += frame_indent;
  
  for (;first != last;++first) WriteEnumValue (*first);
  
  indent -= frame_indent;

  WriteIndent  ();
  stream.Print (")\n");
  
  line_number += 3;
}

template <class TextSerializerType> template <class Iter>
inline void BasicWxfSerializer<TextSerializerType>::WriteDispatch (const char* tag,Iter count,Iter value,stl::int_iterator_tag)
{
  Write (tag,(size_t)count,value);
}

template <class TextSerializerType> template <class InIter>
inline void BasicWxfSerializer<TextSerializerType>::Write (const char* tag,InIter first,InIter last)
{
  WriteDispatch (tag,first,last,stl::iterator_category<InIter> ());
}

template <class TextSerializerType>
void BasicWxfSerializer<TextSerializerType>::WriteComment (const char* comment)
{
  if (!comment)
    RaiseNullArgument ("BasicWxfSerializer::WriteComment","comment");
    
  if (line_number != frame_start_line_number)
  {
    stream.Print ("\n");
    line_number++;
  }
  
  WriteIndent ();

  if (strchr (comment,'\n'))
  { 
    stream.Print ("/*\n");
    
    const char *line_start = comment, *current_char = comment;

    for (;*current_char;current_char++)
      switch (*current_char)
      {
        case '\n':
          WriteIndent  (indent + frame_indent);
          stream.Print (line_start,current_char-line_start);
          stream.Print ("\n");

          line_number++;

          line_start = current_char + 1;

          break;
        default:
          break;
      }

    if (line_start != current_char)
    {
      WriteIndent   (indent + frame_indent);
      stream.Printf ("%s\n",line_start);

      line_number++;
    }

    WriteIndent   ();
    stream.Printf ("*/\n\n");

    line_number += 3;
  }
  else
  {
    stream.Printf ("//%s\n\n",comment);

    line_number += 2;
  }
}

template <class TextSerializerType>
inline void BasicWxfSerializer<TextSerializerType>::CloseFile ()
{
  while (indent) EndFrame ();

  stream.UnbindFile ();
}
