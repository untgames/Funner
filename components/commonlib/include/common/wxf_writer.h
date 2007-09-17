#ifndef COMMONLIB_WXF_WRITER_HEADER
#define COMMONLIB_WXF_WRITER_HEADER

#include <common/streams.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: неверный wxf-тэг
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WrongWxfTagExceptionTag;

typedef DerivedException<Exception, WrongWxfTagExceptionTag> WrongWxfTagException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс, упрощающий сериализацию данных в файлы WXF формата
///////////////////////////////////////////////////////////////////////////////////////////////////
class WxfWriter
{
  public:
    enum { 
      DEFAULT_FRAME_INDENT      = 2, //величина отступа фрейма (по умолчанию)
      DEFAULT_ATTRIBUTES_INDENT = 16 //величина отступа списка атрибутов от имени тэга (по умолчанию)
    };
    
    typedef xtl::function<size_t (const void* buffer, size_t size)> WriteFunction;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    WxfWriter  (const char* file_name, size_t frame_indent=DEFAULT_FRAME_INDENT, size_t attr_indent=DEFAULT_ATTRIBUTES_INDENT);
    WxfWriter  (const WriteFunction& writer, size_t frame_indent=DEFAULT_FRAME_INDENT, size_t attr_indent=DEFAULT_ATTRIBUTES_INDENT);
    ~WxfWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поток вывода
///////////////////////////////////////////////////////////////////////////////////////////////////
    const OutputTextStream& Stream () const;
          OutputTextStream& Stream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры отступов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FrameIndent      () const; //отступ фреймов
    size_t AttributesIndent () const; //отступ от тэга до списка атрибутов

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с фреймами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginFrame (const char* frame_tag);
    void EndFrame   ();

    template <class T> void BeginFrame (const char* frame_tag, const T& value);
    template <class T> void BeginFrame (const char* frame_tag, const T& value, const char* format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализация атрибутов
///////////////////////////////////////////////////////////////////////////////////////////////////
                       void Write (const char* tag);
    template <class T> void Write (const char* tag, const T& value);
    template <class T> void Write (const char* tag, const T& value, const char* format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализация диапазонов атрибутов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T>      void WriteRange (const char* tag, size_t count, const T* array);
    template <class InIter> void WriteRange (const char* tag, InIter first, InIter last);
    template <class T>      void WriteRange (const char* tag, size_t count, const T* array, const char* format);
    template <class InIter> void WriteRange (const char* tag, InIter first, InIter last, const char* format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка коментария
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteComment (const char* comment);
    void WriteComment (const wchar_t* comment);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс буферов потока
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

  private:
    void BeginFrame  ();
    void WriteIndent ();
    void WriteIndent (size_t indent);

    static void CheckTag (const char* tag, const char* function_name);

  private:
    WxfWriter (const WxfWriter&); //no impl
    WxfWriter& operator = (const WxfWriter&); //no impl

  private: 
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перегрузки вывода различных типов атрибутов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void wxf_write (OutputTextStream&, const T& value);
template <class T> void wxf_write (OutputTextStream&, const T& value, const char* format);

void wxf_write (OutputTextStream&, signed char symbol);
void wxf_write (OutputTextStream&, unsigned char symbol);
void wxf_write (OutputTextStream&, const char* string);
void wxf_write (OutputTextStream&, const wchar_t* string);

template <class Traits, class Allocator> void wfx_write (OutputTextStream&, const stl::basic_string<char, Traits, Allocator>& string);
template <class Traits, class Allocator> void wxf_write (OutputTextStream&, const stl::basic_string<wchar_t, Traits, Allocator>& string);

#include <common/detail/wxf_writer.inl>

}

#endif
