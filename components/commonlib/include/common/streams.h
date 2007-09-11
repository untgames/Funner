#ifndef COMMONLIB_STREAMS_HEADER
#define COMMONLIB_STREAMS_HEADER

#include <common/file.h>
#include <common/strlib.h>
#include <math/mathlib.h>
#include <stl/string>
#include <stl/iterator>
#include <stl/stack>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WrongWxfTagExceptionTag; //�������� ��� WXF ����

typedef DerivedException<Exception,WrongWxfTagExceptionTag> WrongWxfTagException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������ � ���������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
//+Unicode, +encoding, +locale info
class OutputTextStream
{
  public:
    enum { DEFAULT_BUFFER_SIZE = 8192 };

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
             OutputTextStream  ();
    explicit OutputTextStream  (size_t buffer_size);
    explicit OutputTextStream  (const char* file_name,size_t buffer_size=DEFAULT_BUFFER_SIZE);
    explicit OutputTextStream  (const File& file,size_t buffer_size=DEFAULT_BUFFER_SIZE);
             ~OutputTextStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Printf  (const char* format,...);
    void VPrintf (const char* format,va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Print (const char* string);
    void Print (const char* string,size_t string_length);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindFile   (const File&);
    void BindFile   (const char* file_name);
    void UnbindFile ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetBufferSize () const;
    void   SetBufferSize (size_t new_buffer_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������� � ���� � ����������� ������� File::Flush
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

  private:
    OutputTextStream (const OutputTextStream&);
    OutputTextStream& operator = (const OutputTextStream&);

  private:
    struct Impl;
    Impl*  impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextSerializer
{
  public:
    TextSerializer (OutputTextStream& stream,const char* separator = " ");
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (bool value);
    void Write (char value);
    void Write (unsigned char value);
    void Write (short value);
    void Write (unsigned short value);
    void Write (int value);
    void Write (unsigned int value);
    void Write (long value);
    void Write (unsigned long value);
    void Write (float value);
    void Write (double value);
    void Write (long double value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (const char* string);

    template <class Traits,class Allocator>
    void Write (const stl::basic_string<char,Traits,Allocator>& string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T,size_t size> void Write (const math::vec<T,size>& vector);
    template <class T,size_t size> void Write (const math::matrix<T,size>& matrix);
    template <class T>             void Write (const math::quat<T>& quaternion);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteSeparator ();

  protected:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputTextStream& Stream () const { return stream; }
        
  private:  
    OutputTextStream& stream;
    stl::string       separator;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ � ��������� ����� ������������ (��������� - STL)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class SerializerType>
class TextSerializerIterator
{
  public:
    typedef T                        value_type;
    typedef void                     difference_type;
    typedef void                     pointer;
    typedef void                     reference;
    typedef stl::output_iterator_tag iterator_category;
    typedef SerializerType           serializer_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    TextSerializerIterator (serializer_type& serializer);

    TextSerializerIterator& operator = (const value_type& value);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextSerializerIterator& operator *  ()    { return *this; }
    TextSerializerIterator& operator ++ ()    { return *this; }
    TextSerializerIterator& operator ++ (int) { return *this; }

  private:
    serializer_type& serializer;
    bool             first_output;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class SerializerType>
TextSerializerIterator<T,SerializerType> make_iterator (SerializerType& serializer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ WXF 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class TextSerializerType>
class BasicWxfSerializer
{
  public:
    typedef TextSerializerType TextSerializer;

    enum { 
      DEFAULT_FRAME_INDENT      = 2, //�������� ������� ������ 
      DEFAULT_ATTRIBUTES_INDENT = 16 //�������� ������� ������ ��������� �� ����� ����
    };
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicWxfSerializer  (const File& file,
                         size_t      frame_indent = DEFAULT_FRAME_INDENT,
                         size_t      attr_indent  = DEFAULT_ATTRIBUTES_INDENT);
    BasicWxfSerializer  (const char* file_name,
                         size_t      frame_indent = DEFAULT_FRAME_INDENT,
                         size_t      attr_indent  = DEFAULT_ATTRIBUTES_INDENT);
    ~BasicWxfSerializer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginFrame (const char* frame_tag);
    void EndFrame   ();

    template <class T> void BeginFrame (const char* frame_tag,const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
                       void Write (const char* tag);
    template <class T> void Write (const char* tag,const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������� � ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T>      void Write (const char* tag,size_t count,const T& value);
    template <class T>      void Write (const char* tag,size_t count,const T* array);
    template <class T>      void Write (const char* tag,size_t count,T* array);
    template <class InIter> void Write (const char* tag,InIter first,InIter last);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteComment (const char* comment);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetLinesCount () const { return line_number; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������������ � ��������� ����� (����������� ������������ ������� ����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CloseFile ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteIndent ();
    void WriteIndent (size_t indent);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    template <class Iter> void WriteDispatch (const char*,Iter,Iter,stl::input_iterator_tag);
    template <class Iter> void WriteDispatch (const char*,Iter,Iter,stl::int_iterator_tag);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ����� � ������������ � �������� WXF
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> 
    void WriteValue (const T&,size_t indent);

    template <class T,size_t size>
    void WriteValue (const math::matrix<T,size>& matrix,size_t indent);
    
    template <class Traits,class Allocator>
    void WriteValue (const stl::basic_string<char,Traits,Allocator>& string,size_t indent);

    void WriteValue (const char* string,size_t indent);
    void WriteValue (char value,size_t indent);
    void WriteValue (unsigned char value,size_t attr_indent);
    
    template <class T>             void WriteEnumValue (const T&);    
    template <class T,size_t size> void WriteEnumValue (const math::matrix<T,size>&);

  private: 
    OutputTextStream stream;
    TextSerializer   serializer;
    size_t           frame_indent;
    size_t           attr_indent;
    size_t           indent;
    size_t           line_number;
    size_t           frame_start_line_number;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� ����� � WXF ������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef BasicWxfSerializer<TextSerializer> WxfSerializer;

#include <common/detail/streams.inl>

}

#endif
