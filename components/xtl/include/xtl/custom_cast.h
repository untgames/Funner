#ifndef XTL_CUSTOM_CAST_HEADER
#define XTL_CUSTOM_CAST_HEADER

#include <memory.h>

#include <xtl/any_cast_exception.h>
#include <xtl/singleton_default.h>
#include <xtl/type_traits>

namespace xtl
{

namespace detail
{

//implementation forwards
struct custom_ref_caster_type_info;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class From, class To> struct static_caster;
template <class From, class To> struct dynamic_caster;
template <class From, class To> struct reinterpret_caster;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Декларация возможности приведения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FromT, class ToT = FromT, template <class, class> class CastTag = static_caster>
class declcast;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение конвертера
///////////////////////////////////////////////////////////////////////////////////////////////////
class custom_ref_caster
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
                          custom_ref_caster ();
    template <class From> custom_ref_caster (From& value);
    template <class From> custom_ref_caster (From*& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типа
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class To> To   cast     () const;
    template <class To> bool has_cast () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип элемента
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& type () const;

  private:
    void*                                      source;
    const detail::custom_ref_caster_type_info* source_type;
};

template <class From>
custom_ref_caster make_custom_ref_caster (From& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class To, class From> To custom_cast (From&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка возможности приведения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class From, class To> bool has_custom_cast ();

#include <xtl/detail/custom_cast.inl>

///////////////////////////////////////////////////////////////////////////////////////////////////
///Стандартные приведения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template class declcast<signed char, char>;
template class declcast<signed char, unsigned char>;
template class declcast<signed char, unsigned short>;
template class declcast<signed char, signed short>;
template class declcast<signed char, unsigned int>;
template class declcast<signed char, signed int>;
template class declcast<signed char, unsigned long>;
template class declcast<signed char, signed long>;
template class declcast<signed char, unsigned long long>;
template class declcast<signed char, signed long long>;
template class declcast<signed char, float>;
template class declcast<signed char, double>;
template class declcast<signed char, long double>;

template class declcast<char, signed char>;
template class declcast<char, unsigned char>;
template class declcast<char, unsigned short>;
template class declcast<char, signed short>;
template class declcast<char, unsigned int>;
template class declcast<char, signed int>;
template class declcast<char, unsigned long>;
template class declcast<char, signed long>;
template class declcast<char, unsigned long long>;
template class declcast<char, signed long long>;
template class declcast<char, float>;
template class declcast<char, double>;
template class declcast<char, long double>;

template class declcast<unsigned char, char>;
template class declcast<unsigned char, signed char>;
template class declcast<unsigned char, unsigned short>;
template class declcast<unsigned char, signed short>;
template class declcast<unsigned char, unsigned int>;
template class declcast<unsigned char, signed int>;
template class declcast<unsigned char, unsigned long>;
template class declcast<unsigned char, signed long>;
template class declcast<unsigned char, unsigned long long>;
template class declcast<unsigned char, signed long long>;
template class declcast<unsigned char, float>;
template class declcast<unsigned char, double>;
template class declcast<unsigned char, long double>;

template class declcast<signed short, char>;
template class declcast<signed short, signed char>;
template class declcast<signed short, unsigned char>;
template class declcast<signed short, unsigned short>;
template class declcast<signed short, unsigned int>;
template class declcast<signed short, signed int>;
template class declcast<signed short, unsigned long>;
template class declcast<signed short, signed long>;
template class declcast<signed short, unsigned long long>;
template class declcast<signed short, signed long long>;
template class declcast<signed short, float>;
template class declcast<signed short, double>;
template class declcast<signed short, long double>;

template class declcast<unsigned short, char>;
template class declcast<unsigned short, unsigned char>;
template class declcast<unsigned short, signed char>;
template class declcast<unsigned short, signed short>;
template class declcast<unsigned short, unsigned int>;
template class declcast<unsigned short, signed int>;
template class declcast<unsigned short, unsigned long>;
template class declcast<unsigned short, signed long>;
template class declcast<unsigned short, unsigned long long>;
template class declcast<unsigned short, signed long long>;
template class declcast<unsigned short, float>;
template class declcast<unsigned short, double>;
template class declcast<unsigned short, long double>;

template class declcast<signed int, char>;
template class declcast<signed int, unsigned char>;
template class declcast<signed int, signed char>;
template class declcast<signed int, unsigned short>;
template class declcast<signed int, signed short>;
template class declcast<signed int, unsigned int>;
template class declcast<signed int, unsigned long>;
template class declcast<signed int, signed long>;
template class declcast<signed int, unsigned long long>;
template class declcast<signed int, signed long long>;
template class declcast<signed int, float>;
template class declcast<signed int, double>;
template class declcast<signed int, long double>;

template class declcast<unsigned int, char>;
template class declcast<unsigned int, unsigned char>;
template class declcast<unsigned int, signed char>;
template class declcast<unsigned int, unsigned short>;
template class declcast<unsigned int, signed short>;
template class declcast<unsigned int, signed int>;
template class declcast<unsigned int, unsigned long>;
template class declcast<unsigned int, signed long>;
template class declcast<unsigned int, unsigned long long>;
template class declcast<unsigned int, signed long long>;
template class declcast<unsigned int, float>;
template class declcast<unsigned int, double>;
template class declcast<unsigned int, long double>;

template class declcast<signed long, char>;
template class declcast<signed long, unsigned char>;
template class declcast<signed long, signed char>;
template class declcast<signed long, unsigned short>;
template class declcast<signed long, signed short>;
template class declcast<signed long, unsigned int>;
template class declcast<signed long, signed int>;
template class declcast<signed long, unsigned long>;
template class declcast<signed long, unsigned long long>;
template class declcast<signed long, signed long long>;
template class declcast<signed long, float>;
template class declcast<signed long, double>;
template class declcast<signed long, long double>;

template class declcast<unsigned long, char>;
template class declcast<unsigned long, unsigned char>;
template class declcast<unsigned long, signed char>;
template class declcast<unsigned long, unsigned short>;
template class declcast<unsigned long, signed short>;
template class declcast<unsigned long, unsigned int>;
template class declcast<unsigned long, signed int>;
template class declcast<unsigned long, signed long>;
template class declcast<unsigned long, unsigned long long>;
template class declcast<unsigned long, signed long long>;
template class declcast<unsigned long, float>;
template class declcast<unsigned long, double>;
template class declcast<unsigned long, long double>;

template class declcast<signed long long, char>;
template class declcast<signed long long, unsigned char>;
template class declcast<signed long long, signed char>;
template class declcast<signed long long, unsigned short>;
template class declcast<signed long long, signed short>;
template class declcast<signed long long, unsigned int>;
template class declcast<signed long long, signed int>;
template class declcast<signed long long, unsigned long>;
template class declcast<signed long long, signed long>;
template class declcast<signed long long, unsigned long long>;
template class declcast<signed long long, float>;
template class declcast<signed long long, double>;
template class declcast<signed long long, long double>;

template class declcast<unsigned long long, char>;
template class declcast<unsigned long long, unsigned char>;
template class declcast<unsigned long long, signed char>;
template class declcast<unsigned long long, unsigned short>;
template class declcast<unsigned long long, signed short>;
template class declcast<unsigned long long, unsigned int>;
template class declcast<unsigned long long, signed int>;
template class declcast<unsigned long long, unsigned long>;
template class declcast<unsigned long long, signed long>;
template class declcast<unsigned long long, signed long long>;
template class declcast<unsigned long long, float>;
template class declcast<unsigned long long, double>;
template class declcast<unsigned long long, long double>;

template class declcast<float, char>;
template class declcast<float, unsigned char>;
template class declcast<float, signed char>;
template class declcast<float, unsigned short>;
template class declcast<float, signed short>;
template class declcast<float, unsigned int>;
template class declcast<float, signed int>;
template class declcast<float, unsigned long>;
template class declcast<float, signed long>;
template class declcast<float, unsigned long long>;
template class declcast<float, signed long long>;
template class declcast<float, double>;
template class declcast<float, long double>;

template class declcast<double, char>;
template class declcast<double, unsigned char>;
template class declcast<double, signed char>;
template class declcast<double, unsigned short>;
template class declcast<double, signed short>;
template class declcast<double, unsigned int>;
template class declcast<double, signed int>;
template class declcast<double, unsigned long>;
template class declcast<double, signed long>;
template class declcast<double, unsigned long long>;
template class declcast<double, signed long long>;
template class declcast<double, float>;
template class declcast<double, long double>;

template class declcast<long double, char>;
template class declcast<long double, unsigned char>;
template class declcast<long double, signed char>;
template class declcast<long double, unsigned short>;
template class declcast<long double, signed short>;
template class declcast<long double, unsigned int>;
template class declcast<long double, signed int>;
template class declcast<long double, unsigned long>;
template class declcast<long double, signed long>;
template class declcast<long double, unsigned long long>;
template class declcast<long double, signed long long>;
template class declcast<long double, float>;
template class declcast<long double, double>;

}

#endif

