#ifndef XTL_LEXICAL_CAST_HEADER
#define XTL_LEXICAL_CAST_HEADER

#include <stdio.h>

#include <stl/string>

#include <xtl/string.h>
#include <xtl/token_parser.h>

namespace xtl
{

/*
    В отличие от boost, lexical_cast работает через двойное приведение source_type->stl::string->destination_type
*/

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
void to_string (stl::string& buffer, const void* pointer);

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
