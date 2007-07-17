/*
    ѕроверка дополнительных сложных свойств типа с учЄтом возможностей компил€тора MSVC
*/

//определение €вл€етс€ ли T классом
template <class T> struct is_class: public bool_constant<__is_class (T)> {};

//определение €вл€етс€ ли T перечислением
template <class T> struct is_enum: public bool_constant<__is_enum (T)> {};

//определение €вл€етс€ ли T полиморфным классом
template <class T> struct is_polymorphic: public bool_constant<__is_polymorphic (T)> {};

//определение €вл€етс€ ли тип T абстрактным
template <class T> struct is_abstract: public bool_constant<__is_abstract (T)> {};
