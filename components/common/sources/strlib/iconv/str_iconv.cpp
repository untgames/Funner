#include <cerrno>

#include <iconv.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/strconv.h>

using namespace common;

extern "C" size_t iconv_wrapper (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);

namespace components
{

namespace iconv_converter
{

/*
    Константы
*/

const char* ICONV_CONVERTER_NAME = "common.string.converters.iconv";

/*
    Преобразователь
*/

class IconvStringConverter: public xtl::reference_counter, public IStringConverter
{
  public:
///Конструктор
    IconvStringConverter (const char* source_encoding, const char* destination_encoding)
      : cd (0)
    {
      try
      {
        if (!source_encoding)
          throw xtl::make_null_argument_exception ("", "source_encoding");
          
        if (!destination_encoding)
          throw xtl::make_null_argument_exception ("", "destination_encoding");
          
        cd = iconv_open (destination_encoding, source_encoding);        
        
        if (cd == (iconv_t)-1)
          throw xtl::format_operation_exception ("::iconv_open", "ICONV error: %s", strerror (errno));

        iconv (cd, 0, 0, 0, 0);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::IconvStringConverter::IconvStringConverter");
        throw;
      }        
    }    
    
///Деструктор
    ~IconvStringConverter ()
    {
      iconv_close (cd);
    }
    
///Преобразование
    void Convert (const void*& source_buffer_ptr, size_t& source_buffer_size, void*& destination_buffer_ptr, size_t& destination_buffer_size)
    {
      try
      {        
        size_t result = iconv_wrapper (cd, reinterpret_cast<const char**> (&source_buffer_ptr), &source_buffer_size, reinterpret_cast<char**> (&destination_buffer_ptr), &destination_buffer_size);
      
        if (result == (size_t)-1 && errno != E2BIG)
          throw xtl::format_operation_exception ("::iconv", "ICONV error: %s", strerror (errno));
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::IconvStringConverter::Convert");
        throw;        
      }
    }

///Подсчёт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    iconv_t cd;    
};

/*
    Компонент, регистрирующий ICONV-конвертер
*/

class Component
{
  public:
    Component ()
    {
      StringConverterSystem::RegisterConverter ("*", "*", &Convert);
    }
    
  private:
    static IStringConverter* Convert (const char* source_encoding, const char* destination_encoding)
    {
      return new IconvStringConverter (source_encoding, destination_encoding);
    }
};

extern "C" ComponentRegistrator<Component> IconvConverter (ICONV_CONVERTER_NAME);

}

}
