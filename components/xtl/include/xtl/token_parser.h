#ifndef XTL_TOKEN_PARSER_HEADER
#define XTL_TOKEN_PARSER_HEADER

#include <ctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

namespace xtl
{

namespace io
{

/*
    Разбор базовых типов из строки
*/

bool read (const char*    string, bool& result_value);
bool read (const char*    string, char& result_value);
bool read (const char*    string, signed char& result_value);
bool read (const char*    string, unsigned char& result_value);
bool read (const char*    string, wchar_t& result_value);
bool read (const char*    string, short& result_value);
bool read (const char*    string, unsigned short& result_value);
bool read (const char*    string, int& result_value);
bool read (const char*    string, unsigned int& result_value);
bool read (const char*    string, long& result_value);
bool read (const char*    string, unsigned long& result_value);
bool read (const char*    string, float& result_value);
bool read (const char*    string, double& result_value);
bool read (const char*    string, long double& result_value);
bool read (const wchar_t* string, bool& result_value);
bool read (const wchar_t* string, wchar_t& result_value);
bool read (const wchar_t* string, char& result_value);
bool read (const wchar_t* string, signed char& result_value);
bool read (const wchar_t* string, unsigned char& result_value);
bool read (const wchar_t* string, short& result_value);
bool read (const wchar_t* string, unsigned short& result_value);
bool read (const wchar_t* string, int& result_value);
bool read (const wchar_t* string, unsigned int& result_value);
bool read (const wchar_t* string, long& result_value);
bool read (const wchar_t* string, unsigned long& result_value);
bool read (const wchar_t* string, float& result_value);
bool read (const wchar_t* string, double& result_value);
bool read (const wchar_t* string, long double& result_value);

#include <xtl/detail/io/token_parser.inl>

}

}

#endif
