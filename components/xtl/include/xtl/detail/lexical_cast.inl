/*
    bad_lexical_cast
*/

//конструкторы
inline bad_lexical_cast::bad_lexical_cast ()
  : source (&typeid (void)), target (&typeid (void))
  {}

inline bad_lexical_cast::bad_lexical_cast (const std::type_info& source_type, const std::type_info& target_type)
  : source (&source_type), target (&target_type)
  {}

//исходный и целевой типы
inline const std::type_info& bad_lexical_cast::source_type () const
{
  return *source;
}

inline const std::type_info& bad_lexical_cast::target_type () const
{
  return *target;
}

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

inline void to_string (stl::string& buffer, const wchar_t* value)
{
  if (!value)
    value = L"";      

  buffer.resize ((wcslen (value) + 1)* 2);
  
  size_t len = _snprintf (&buffer [0], buffer.size () + 1, "%S", value);
  
  if (len > buffer.size ())
    len = buffer.size ();
    
  buffer.resize (len);
}

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

  _snprintf (char_buffer, sizeof (char_buffer), sign ? "%d" : "%u", value);
  
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

inline void to_string (stl::string& buffer, wchar_t value)
{
  wchar_t tmp_buffer [2] = {value, L'\0'};

  to_string (buffer, tmp_buffer);
}

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

inline void to_string (stl::string& buffer, bool value)
{
  long tmp = value != 0;

  detail::integer_to_string (buffer, tmp, false);
}

inline void to_string (stl::string& buffer, const double& value)
{
  const size_t MAX_REAL_STRING_SIZE = 32; //максимальное количество знаков для символьного представления вещественного числа 
  
  char char_buffer [MAX_REAL_STRING_SIZE];
  
  _snprintf (char_buffer, sizeof (char_buffer), "%.12g", value);
  
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

template <class T>
inline void to_string (stl::string& buffer, const T* pointer)
{
  buffer.resize (8);
  
  _snprintf (&buffer [0], buffer.size () + 1, "%p", pointer);
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

inline void to_value (const stl::string& buffer, stl::string& value)
{
  value = buffer;
}

inline void to_value (const stl::string& buffer, stl::wstring& value)
{
  if (buffer.empty ())
    throw bad_lexical_cast (typeid (stl::string), typeid (stl::wstring));

  value.resize (buffer.size ());
  
  size_t length = _snwprintf (&value [0], value.size (), L"%S", buffer.c_str ());
  
  if (length > value.size ())
    length = value.size ();
    
  value.resize (length);
}

inline void to_value (const stl::string& buffer, wchar_t& value)
{
  if (buffer.empty () || !_snscanf (buffer.c_str (), buffer.size (), "%C", &value))
    throw bad_lexical_cast (typeid (stl::string), typeid (wchar_t));
}

inline void to_value (const stl::string& buffer, long& value)
{
  if (buffer.empty () || !_snscanf (buffer.c_str (), buffer.size (), "%d", &value))
    throw bad_lexical_cast (typeid (stl::string), typeid (long));
}

inline void to_value (const stl::string& buffer, unsigned long& value)
{
  to_value (buffer, *reinterpret_cast<long*> (&value));
}

namespace detail
{

template <class T>
inline void string_to_integer (const stl::string& buffer, T& value)
{
  long tmp;
  
  to_value (buffer, tmp);
  
  value = static_cast<T> (tmp);
}

}

inline void to_value (const stl::string& buffer, char& value)
{
  if (buffer.size () != 1)
    throw bad_lexical_cast (typeid (stl::string), typeid (char));
    
  value = buffer [0];
}

inline void to_value (const stl::string& buffer, signed char& value)
{
  to_value (buffer, reinterpret_cast<char&> (value));
}

inline void to_value (const stl::string& buffer, unsigned char& value)
{
  to_value (buffer, reinterpret_cast<char&> (value));
}

inline void to_value (const stl::string& buffer, short& value)
{
  detail::string_to_integer (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned short& value)
{
  detail::string_to_integer (buffer, value);
}

inline void to_value (const stl::string& buffer, int& value)
{
  detail::string_to_integer (buffer, value);
}

inline void to_value (const stl::string& buffer, unsigned int& value)
{
  detail::string_to_integer (buffer, value);
}

inline void to_value (const stl::string& buffer, bool& value)
{
  long tmp;

  to_value (buffer, tmp);
  
  value = tmp != 0;
}

inline void to_value (const stl::string& buffer, float& value)
{
  if (buffer.empty () || !_snscanf (buffer.c_str (), buffer.size (), "%f", &value))
    throw bad_lexical_cast (typeid (stl::string), typeid (float));
}

inline void to_value (const stl::string& buffer, double& value)
{
  float tmp;
  
  to_value (buffer, tmp);
  
  value = static_cast<double> (tmp);
}

inline void to_value (const stl::string& buffer, long double& value)
{
  float tmp;
  
  to_value (buffer, tmp);
  
  value = static_cast<long double> (tmp);  
}

inline void to_value (const stl::string& buffer, void*& pointer)
{
  long tmp;

  detail::string_to_integer (buffer, tmp);

  pointer = reinterpret_cast<void*> (tmp);
}

template <class T>
inline T to_value (const stl::string& buffer)
{
  T value;
  
  to_value (buffer, value);
  
  return value;
}
