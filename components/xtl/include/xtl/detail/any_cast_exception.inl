/*
    Конструкторы
*/

inline bad_any_cast::bad_any_cast (cast_error in_error_id)
  : error_id (in_error_id), source (&typeid (void)), target (&typeid (void))
  {}

inline bad_any_cast::bad_any_cast (cast_error in_error_id, const std::type_info& in_source_type, const std::type_info& in_target_type)
  : error_id (in_error_id), source (&in_source_type), target (&in_target_type)
  {}

inline const char* bad_any_cast::what () const throw ()
{
  switch (error_id)
  {
    case bad_direct_cast:       return "xtl::bad_any_cast; reason: types aren't equal";
    case bad_const_cast:        return "xtl::bad_any_cast; reason: lost cv-qualifiers";
    case bad_dynamic_cast:      return "xtl::bad_any_cast; reason: error at dynamic_cast";
    case bad_to_string_cast:    return "xtl::bad_any_cast; reason: function 'to_string' not found";
    case bad_to_value_cast:     return "xtl::bad_any_cast; reason: function 'to_value' not found";
    case bad_to_reference_cast: return "xtl::bad_any_cast; reason: no way for convert return value to reference after lexical cast";
    default:                    return "xtl::bad_any_cast";
  }
}

/*
    Вид ошибки
*/

inline bad_any_cast::cast_error bad_any_cast::error () const
{
  return error_id;
}

/*
    Исходный и целевой типы
*/

inline const std::type_info& bad_any_cast::source_type () const
{
  return *source;
}

inline const std::type_info& bad_any_cast::target_type () const
{
  return *target;
}
