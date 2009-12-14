namespace detail
{

template <class T> T deg2rad (const T& value)
{
  static T MULTIPLIER = constants<T>::pi / T (180);

  return value * MULTIPLIER;
}

template <class T> T rad2deg (const T& value) 
{ 
  static T MULTIPLIER = T (180) / constants<T>::pi;

  return value * MULTIPLIER;
}

}

/*
    Конструкторы
*/

template <class T>
angle<T>::angle ()
  : value (0)
{
}

template <class T>
angle<T>::angle (const value_type& in_value)
  : value (in_value)
{
}

template <class T>
angle<T>::angle (const value_type& in_value, degree_tag)
  : value (detail::deg2rad (in_value))
{
}

template <class T>
angle<T>::angle (const value_type& in_value, radian_tag)
  : value (in_value)
{
}

template <class T>
angle<T>::angle (const angle& ang)
  : value (ang.value)
{
}

template <class T> template <class T1>
angle<T>::angle (const angle<T1>& ang)
  : value (static_cast<value_type> (ang.to_radian ()))
{
}

/*
    Присваивание
*/

template <class T>
void angle<T>::assign (const angle& ang)
{
  value = ang.value;
}

template <class T> template <class T1>
void angle<T>::assign (const angle<T1>& ang)
{
  value = static_cast<T> (ang.to_radian ());
}

template <class T>
void angle<T>::assign (const value_type& in_value, degree_tag)
{
  value = detail::deg2rad (in_value);
}

template <class T>
void angle<T>::assign (const value_type& in_value, radian_tag)
{
  value = in_value;
}

template <class T>
angle<T>& angle<T>::operator = (const angle& ang)
{
  assign (ang);
  return *this;
}
                    
template <class T> template <class T1>
angle<T>& angle<T>::operator = (const angle<T1>& ang)
{
  assign (ang);
  return *this;
}

/*
    Получение значения угла в различных системах измерений
*/

template <class T>
typename angle<T>::value_type angle<T>::get (degree_tag) const
{
  return detail::rad2deg (value);
}

template <class T>
typename angle<T>::value_type angle<T>::get (radian_tag) const
{
  return value;
}

template <class T>
typename angle<T>::value_type angle<T>::to_radian () const
{
  return value;
}

template <class T>
typename angle<T>::value_type angle<T>::to_degree () const
{
  return detail::rad2deg (value);
}

/*
    Унарные операции
*/

template <class T>
const angle<T>& angle<T>::operator + () const
{
  return *this;
}

template <class T>
angle<T> angle<T>::operator - () const
{
  return angle (-value);
}

/*
    Бинарные операции
*/

template <class T>
angle<T>& angle<T>::operator += (const angle& ang)
{
  value += ang.value;
  return *this;
}

template <class T>
angle<T>& angle<T>::operator -= (const angle& ang)
{
  value -= ang.value;
  return *this;
}

template <class T>
angle<T>& angle<T>::operator *= (const value_type& a)
{
  value *= a;
  return *this;
}

template <class T>
angle<T>& angle<T>::operator /= (const value_type& a)
{
  value /= a;
  return *this;
}

template <class T>
angle<T> angle<T>::operator +  (const angle& ang) const
{
  return angle (value + ang.value);
}

template <class T>
angle<T> angle<T>::operator - (const angle& ang) const
{
  return angle (value - ang.value);
}

template <class T>
angle<T> angle<T>::operator * (const value_type& a) const
{
  return angle (value * a);
}

template <class T>
angle<T> operator * (const typename angle<T>::value_type& a, const angle<T>& ang)
{
  return ang * a;
}

template <class T>
angle<T> angle<T>::operator / (const value_type& a) const
{
  return angle (value / a);
}

/*
    Сравнение
*/

template <class T>
bool angle<T>::operator == (const angle& ang) const
{
  return value == ang.value;
}

template <class T>
bool angle<T>::operator != (const angle& ang) const
{
  return !(*this == ang);
}

template <class T>
bool angle<T>::operator < (const angle& ang) const
{
  return value < ang.value;
}

template <class T>
bool angle<T>::operator > (const angle& ang) const
{
  return ang < *this;
}

template <class T>
bool angle<T>::operator <= (const angle& ang) const
{
  return !(ang < *this);
}

template <class T>
bool angle<T>::operator >= (const angle& ang) const
{
  return !(*this < ang);
}

/*
    Конвертации углов
*/

anglef radian (float value)
{
  return anglef (value, radian_tag ());
}

angled radian (double value)
{
  return angled (value, radian_tag ());
}

anglef degree (float value)
{
  return anglef (value, degree_tag ());
}

angled degree (double value)
{
  return angled (value, degree_tag ());
}

template <class T>
typename angle<T>::value_type radian (const angle<T>& ang)
{
  return ang.to_radian ();
}

template <class T>
typename angle<T>::value_type degree (const angle<T>& ang)
{
  return ang.to_degree ();
}

/*
    Основные операции
*/

template <class T>
angle<T> abs (const angle<T>& ang)
{
  T value = ang.to_radian ();

  return angle<T> (value < 0 ? -value : value, radian_tag ());
}

template <class T>
typename angle<T>::value_type sin (const angle<T>& ang)
{
  return ::sin (ang.to_radian ());
}

template <class T>
typename angle<T>::value_type sinh (const angle<T>& ang)
{
  return ::sinh (ang.to_radian ());
}

template <class T>
typename angle<T>::value_type cos (const angle<T>& ang)
{
  return ::cos (ang.to_radian ());
}

template <class T>
typename angle<T>::value_type cosh (const angle<T>& ang)
{
  return ::cosh (ang.to_radian ());
}

template <class T>
typename angle<T>::value_type tan (const angle<T>& ang)
{
  return ::tan (ang.to_radian ());
}

template <class T>
typename angle<T>::value_type tanh (const angle<T>& ang)
{
  return ::tanh (ang.to_radian ());
}
