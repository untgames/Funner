/*
    Лексикографическое преобразование
*/

template <class DstT, class SrcT>
inline DstT lexical_cast (const SrcT& value)
{
  stl::string buffer;
  
  to_string (buffer, value);
  
  try
  {
    DstT target;
  
    to_value (buffer, target);
    
    return target;
  }
  catch (bad_lexical_cast&)
  {
    throw bad_lexical_cast (typeid (SrcT), typeid (DstT));
  }
}

/*
    Преобразования тип -> строка
*/

inline void to_string (stl::string& buffer, const stl::string& value)
{
  buffer = value;
}

inline void to_string (stl::string& buffer, const char* value)
{
  buffer = value ? value : "(null)";
}

#ifndef XTL_NO_WCHAR

inline void to_string (stl::string& buffer, const wchar_t* string)
{
  if (!string)
    string = L"";

  int source_size = wcslen (string);

  buffer.fast_resize (source_size * 4);

  int result_size = (int)wcstombs (&buffer [0], string, source_size);  

  buffer.resize (result_size <= 0 ? 0 : result_size);
}

#endif

inline void to_string (stl::string& buffer, const stl::wstring& value)
{
  to_string (buffer, value.c_str ());
}

inline void to_string (stl::string& buffer, const signed char* value)
{
  to_string (buffer, reinterpret_cast<const char*> (value));
}

inline void to_string (stl::string& buffer, const unsigned char* value)
{
  to_string (buffer, reinterpret_cast<const char*> (value));
}

namespace detail
{

inline void integer_to_string (stl::string& buffer, long value, bool sign)
{
  const size_t MAX_INTEGER_STRING_SIZE = 16; //максимальное количество знаков для символьного представления целого числа 

  char char_buffer [MAX_INTEGER_STRING_SIZE];

  xsnprintf (char_buffer, sizeof (char_buffer), sign ? "%d" : "%u", value);

  buffer = char_buffer;
}

}

inline void to_string (stl::string& buffer, char value)
{
  buffer = value;
}

inline void to_string (stl::string& buffer, signed char value)
{
  buffer = value;
}

inline void to_string (stl::string& buffer, unsigned char value)
{
  buffer = value;
}

#ifndef XTL_NO_WCHAR

inline void to_string (stl::string& buffer, wchar_t value)
{
  wchar_t tmp_buffer [2] = {value, L'\0'};

  to_string (buffer, tmp_buffer);
}

#endif

inline void to_string (stl::string& buffer, short value)
{
  detail::integer_to_string (buffer, value, true);
}

inline void to_string (stl::string& buffer, unsigned short value)
{
  detail::integer_to_string (buffer, value, false);
}

inline void to_string (stl::string& buffer, int value)
{
  detail::integer_to_string (buffer, value, true);
}

inline void to_string (stl::string& buffer, unsigned int value)
{
  detail::integer_to_string (buffer, value, false);
}

inline void to_string (stl::string& buffer, long value)
{
  detail::integer_to_string (buffer, value, true);
}

inline void to_string (stl::string& buffer, unsigned long value)
{
  detail::integer_to_string (buffer, value, false);
}

inline void to_string (stl::string& buffer, const volatile bool& value)
{
  long tmp = value != 0;

  detail::integer_to_string (buffer, tmp, false);
}

inline void to_string (stl::string& buffer, const double& value)
{
  const size_t MAX_REAL_STRING_SIZE = 32; //максимальное количество знаков для символьного представления вещественного числа 
  
  char char_buffer [MAX_REAL_STRING_SIZE];
  
  static const double EPS = 1.0e-06;
  
  xsnprintf (char_buffer, sizeof (char_buffer), value < - EPS || value > EPS ? "%.6g" : "0", value);
  
  char_buffer [MAX_REAL_STRING_SIZE - 1] = 0;
  
  buffer = char_buffer;
}

inline void to_string (stl::string& buffer, float value)
{
  to_string (buffer, static_cast<double> (value));
}

inline void to_string (stl::string& buffer, const long double& value)
{
  to_string (buffer, static_cast<double> (value));
}

inline void to_string (stl::string& buffer, const void* pointer)
{
  buffer.resize (8);
  
  xsnprintf (&buffer [0], buffer.size () + 1, "%p", pointer);
}

template <class T>
inline stl::string to_string (const T& value)
{
  stl::string buffer;
  
  to_string (buffer, value);
  
  return buffer;
}

/*
    Преобразования строка -> тип
*/

namespace detail
{

template <class Char, class Traits, class Allocator, class Value>
inline void to_value (const stl::basic_string<Char, Traits, Allocator>& buffer, Value& value)
{
  if (buffer.empty () || !xtl::io::read (buffer.c_str (), value))
    throw bad_lexical_cast (typeid (stl::basic_string<Char, Traits, Allocator>), typeid (Value));
}

}

inline void to_value (const stl::string& buffer, stl::string& value)
{
  value = buffer;
}

#ifndef XTL_NO_WCHAR

inline void to_value (const stl::string& buffer, stl::wstring& value)
{
  if (buffer.empty ())
  {
    value.clear ();
    return;
  }

  value.fast_resize (buffer.size ());

  int result_size = (int)mbstowcs (&value [0], buffer.c_str (), buffer.size ());

  if (result_size <= 0)
    throw bad_lexical_cast (typeid (stl::string), typeid (stl::wstring));

  value.fast_resize (result_size);
}

#endif

#ifndef XTL_NO_WCHAR

inline void to_value (const stl::string& buffer, wchar_t& value)
{
  detail::to_value (buffer, value);
}

#endif

inline void to_value (const stl::string& buffer, long& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned long& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, char& value)
{
  if (buffer.size () != 1)
    throw bad_lexical_cast (typeid (stl::string), typeid (char));
    
  detail::to_value (buffer, value);  
}

inline void to_value (const stl::string& buffer, signed char& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned char& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, short& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned short& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, int& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned int& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, bool& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, float& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, double& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, long double& value)
{
  detail::to_value (buffer, value);
}

inline void to_value (const stl::string& buffer, void*& pointer)
{ 
  long tmp;

  detail::to_value (buffer, tmp);

  pointer = reinterpret_cast<void*> (tmp);
}

template <class T>
inline T to_value (const stl::string& buffer)
{
  T value;
  
  to_value (buffer, value);
  
  return value;
}

/*
  Вывод диапазонов
*/

template <class Iter>
void to_string (stl::string& buffer, Iter first, Iter last, const char* separator)
{
  if (!separator)
    separator = "";

  buffer.clear ();
  
  stl::string item_buffer;
  bool        need_separator = false;

  for (;first!=last; ++first)
  {  
    if (need_separator)
      buffer += separator;
      
    need_separator = true;
  
    to_string (item_buffer, *first);

    buffer += item_buffer;
  }
}

/*
    Приведение к строке математических типов
*/

template <class String, class T, unsigned int Size>
void to_string (String& buffer, const math::vector<T, Size>& value)
{
  to_string (buffer, &value [0], &value [Size], " ");
}

template <class String, class T, unsigned int Size>
void to_string (String& buffer, const math::matrix<T, Size>& value)
{
  to_string (buffer, &value [0][0], &value [Size-1][Size], " ");
}

template <class String, class T>
void to_string (String& buffer, const math::quat<T>& value)
{
  to_string (buffer, &value [0], &value [4], " ");
}
