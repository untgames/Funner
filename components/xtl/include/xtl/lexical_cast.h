#ifndef XTL_LEXICAL_CAST_HEADER
#define XTL_LEXICAL_CAST_HEADER

#include <stdio.h>
#include <exception>
#include <typeinfo>
#include <stl/string>

namespace xtl
{

/*
    В отличие от boost, lexical_cast работает через двойное приведение source_type->stl::string->destination_type
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: неверное лексикографическое преобразование
///////////////////////////////////////////////////////////////////////////////////////////////////
class bad_lexical_cast: public std::exception
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    bad_lexical_cast ();
    bad_lexical_cast (const std::type_info& source_type, const std::type_info& target_type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исходный и целевой типы
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& source_type () const;
    const std::type_info& target_type () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сообщение
///////////////////////////////////////////////////////////////////////////////////////////////////    
    const char* what () const throw () { return "xtl::bad_lexical_cast"; }

  private:
    const std::type_info *source, *target;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования тип -> строка
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_string (stl::string& buffer, const stl::string&);
void to_string (stl::string& buffer, const char* value);
void to_string (stl::string& buffer, const wchar_t* value);
void to_string (stl::string& buffer, const stl::wstring&);
void to_string (stl::string& buffer, const signed char* value);
void to_string (stl::string& buffer, const unsigned char* value);
void to_string (stl::string& buffer, char value);
void to_string (stl::string& buffer, signed char value);
void to_string (stl::string& buffer, unsigned char value);
void to_string (stl::string& buffer, wchar_t value);
void to_string (stl::string& buffer, short value);
void to_string (stl::string& buffer, unsigned short value);
void to_string (stl::string& buffer, int value);
void to_string (stl::string& buffer, unsigned int value);
void to_string (stl::string& buffer, long value);
void to_string (stl::string& buffer, unsigned long value);
void to_string (stl::string& buffer, bool value);
void to_string (stl::string& buffer, float value);
void to_string (stl::string& buffer, const double& value);
void to_string (stl::string& buffer, const long double& value);

template <class T>
void to_string (stl::string& buffer, const T* pointer);

template <class T> stl::string to_string (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования строка -> тип
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_value (const stl::string& buffer, stl::string&);
void to_value (const stl::string& buffer, stl::wstring&);
void to_value (const stl::string& buffer, char& value);
void to_value (const stl::string& buffer, signed char& value);
void to_value (const stl::string& buffer, unsigned char& value);
void to_value (const stl::string& buffer, wchar_t& value);
void to_value (const stl::string& buffer, short& value);
void to_value (const stl::string& buffer, unsigned short& value);
void to_value (const stl::string& buffer, int& value);
void to_value (const stl::string& buffer, unsigned int& value);
void to_value (const stl::string& buffer, long& value);
void to_value (const stl::string& buffer, unsigned long& value);
void to_value (const stl::string& buffer, bool& value);
void to_value (const stl::string& buffer, float& value);
void to_value (const stl::string& buffer, double& value);
void to_value (const stl::string& buffer, long double& value);

void to_value (const stl::string& buffer, void*& pointer);

template <class T> T to_value (const stl::string&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Лексикографическое преобразование
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DstT, class SrcT> DstT lexical_cast (const SrcT&);

#include <xtl/detail/lexical_cast.inl>

}

#endif
