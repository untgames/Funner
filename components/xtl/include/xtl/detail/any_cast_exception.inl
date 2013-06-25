/*
    bad_any_cast
*/

/*
    Конструкторы
*/

inline bad_any_cast::bad_any_cast ()
  : source (&typeid (void)), target (&typeid (void))
  {}

inline bad_any_cast::bad_any_cast (const std::type_info& in_source_type, const std::type_info& in_target_type)
  : source (&in_source_type), target (&in_target_type)
  {}

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

/*
    bad_cv_any_cast
*/

/*
    Конструкторы
*/

inline bad_cv_any_cast::bad_cv_any_cast ()
  {}

inline bad_cv_any_cast::bad_cv_any_cast (const std::type_info& source_type, const std::type_info& target_type)
  : bad_any_cast (source_type, target_type)
  {}
