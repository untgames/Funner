/*
    –азбор базовых типов
*/

inline bool read (const char* string, long& value)
{
  if (!string || !*string)
    return false;    
    
  long tmp = strtol (string, (char**)&string, 0);

  if (!*string)
  {  
    value = tmp;
    return true;
  }

  if (*string != '.')
    return false;

  while (*++string)
    if (!isdigit ((unsigned char)*string))
      return false;

  value = tmp;

  return true;  
}

inline bool read (const char* string, unsigned long& value)
{
  return read (string, *(long*)&value);
}

inline bool read (const char* string, bool& value)
{
  long tmp_value;

  if (!read_and_cast<long> (string, tmp_value))
    return false;
    
  value = tmp_value != 0;

  return true;
}

inline bool read (const char* string, char& value)
{
  if (!string || !*string || string [1])
    return false;

  value = *string;
  
  return true;
}

inline bool read (const char* string, signed char& value)
{
  return read (string, *(char*)&value);
}

inline bool read (const char* string, unsigned char& value)
{
  return read (string, *(char*)&value);
}

#ifndef XTL_NO_WCHAR

inline bool read (const char* string, wchar_t& value)
{
  if (!string || !*string)
    return false;

  wchar_t tmp = 0;
  
  int length = strlen (string), result_length = mbrtowc (&tmp, string, length, 0);
  
  if (length != result_length)
    return false;

  value = tmp;

  return true;
}

#endif

inline bool read (const char* string, short& value)
{
  return read_and_cast<long> (string, value);
}

inline bool read (const char* string, unsigned short& value)
{
  return read_and_cast<long> (string, value);
}

inline bool read (const char* string, int& value)
{
  return read_and_cast<long> (string, value);
}

inline bool read (const char* string, unsigned int& value)
{
  return read_and_cast<long> (string, value);
}

inline bool read (const char* string, double& value)
{
  if (!string || !*string)
    return false;    

  double tmp = strtod (string, (char**)&string);

  if (*string)
    return false;

  value = tmp;

  return true;
}

inline bool read (const char* string, float& value)
{
  return read_and_cast<double> (string, value);
}

inline bool read (const char* string, long double& value)
{
  return read_and_cast<double> (string, value);
}

namespace detail
{

struct wchar_string_converter
{
  enum { BUFFER_SIZE = 32 };
  public:
    wchar_string_converter (const wchar_t* string)
    {
      *buffer = '\0';

      if (!string)
        return;

      int string_length = wcslen (string);
      
      static const int MAX_LENGTH = BUFFER_SIZE-1; //данный размер совпадает с количеством символов multibyte строке, поскольку работа производитс€ только с базовыми типами: числа и простые символы

      if (string_length > MAX_LENGTH)
        string_length = MAX_LENGTH;

      int length = wcsrtombs (buffer, &string, string_length, 0);

      if (length >= 0 && length < BUFFER_SIZE) buffer [length]        = '\0';
      else                                     buffer [BUFFER_SIZE-1] = '\0';
    }

    const char* get () const { return buffer; }

  private:
    char buffer [BUFFER_SIZE];
};

template <class Value>
inline bool read_wchar_string (const wchar_t* string, Value& result_value)
{
  return read (wchar_string_converter (string).get (), result_value);
}

}

inline bool read (const wchar_t* string, bool& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, char& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, signed char& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, unsigned char& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, short& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, unsigned short& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

#ifndef XTL_NO_WCHAR

inline bool read (const wchar_t* string, wchar_t& result_value)
{
  if (!string || !*string || wcslen (string) != 1)
    return false;

  result_value = *string;

  return true;
}

#endif

inline bool read (const wchar_t* string, int& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, unsigned int& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, long& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, unsigned long& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, float& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, double& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

inline bool read (const wchar_t* string, long double& result_value)
{
  return detail::read_wchar_string (string, result_value);
}

/*
    „тение токенов с приведением типов
*/

template <class Base, class Char, class Value>
inline bool read_and_cast (const Char* string, Value& value)
{
  Base base_value;

  if (!read (string, base_value))
    return false;

  value = static_cast<Value> (base_value);

  return true;
}

/*
    „тение токенов с возвратом значени€
*/

template <class Value, class Char>
inline Value get (const Char* string)
{
  Value result;

  if (read (string, result))
    return result;      

  throw bad_lexical_cast (typeid (const Char*), typeid (Value));
}

template <class Value, class Char>
inline Value get (const Char* string, const Value& default_value)
{
  Value result;
  
  return read (string, result) ? result : default_value;
}
