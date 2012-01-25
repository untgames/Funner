/*
    Конструкторы
*/

template <class I>
inline rational<I>::rational ()
  : n ()
  , d (1)
{
}

template <class I>
inline rational<I>::rational (int_type in_n)
  : n (in_n)
  , d (1)
{
}

template <class I>
inline rational<I>::rational (int_type in_n, int_type in_d)
  : n (in_n)
  , d (in_d)
{
  if (!d)
    throw bad_rational ();
}

/*
    Присваивание
*/

template <class I>
inline rational<I>& rational<I>::operator = (int_type in_n)
{
  n = in_n;
  d = int_type (1);

  return *this;
}

template <class I>
inline rational<I>& rational<I>::assign (int_type in_n, int_type in_d)
{
  if (!in_d)
    throw bad_rational ();

  n = in_n;
  d = in_d;  

  return *this;
}

/*
    Получение данных
*/

template <class I>
inline typename rational<I>::int_type rational<I>::numerator () const
{
  return n;
}

template <class I>
inline typename rational<I>::int_type rational<I>::denominator () const
{
  return d;
}

/*
    Арифметические операции
*/

template <class I>
inline rational<I>& rational<I>::operator += (const rational& r)
{
  if (d == r.d)
  {
    n += r.n  
  }
  else
  {
    n = n * r.d + d * r.n;
    d = d * r.d;
  }

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator -= (const rational& r)
{
  if (d == r.d)
  {
    n -= r.n  
  }
  else
  {
    n = n * r.d - d * r.n;
    d = d * r.d;
  }

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator *= (const rational& r)
{
  n *= r.n;
  d *= r.d;

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator /= (const rational& r)
{
  if (!r.n)
    throw bad_rational ();  

  n *= r.d;
  d *= r.n;
  
  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator += (int_type i)
{
  if (d == 1)
  {
    n += i;
  }
  else
  {
    n += d * i;
  }

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator -= (int_type i)
{
  if (d == 1)
  {
    n -= i;
  }
  else
  {
    n -= d * i;
  }

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator *= (int_type i)
{
  n *= i;

  return *this;
}

template <class I>
inline rational<I>& rational<I>::operator /= (int_type i)
{
  if (!i)
    throw bad_rational ();

  d *= i;
  
  return *this;
}

template <class I>
inline rational<I> rational<I>::operator + (const rational& r) const
{
  return d == r.d ? rational (n + r.n, d) : rational (n * r.d + d * r.n, d * r.d);
}

template <class I>
inline rational<I> rational<I>::operator - (const rational& r) const
{
  return d == r.d ? rational (n - r.n, d) : rational (n * r.d - d * r.n, d * r.d);
}

template <class I>
inline rational<I> rational<I>::operator * (const rational& r) const
{
  return rational (n * r.n, d * r.d);
}

template <class I>
inline rational<I> rational<I>::operator / (const rational& r) const
{
  if (!r.n)
    throw bad_rational ();

  return rational (n * r.d, d * r.n);
}

template <class I, class II>
inline rational<I> operator + (I value, const rational<II>& r)
{
  return r + value;
}

template <class I, class II>
inline rational<I> operator - (I value, const rational<II>& r)
{
  return -r + value;
}

template <class I, class II>
inline rational<I> operator * (I value, const rational<II>& r)
{
  return r * value;
}

template <class I, class II>
inline rational<I> operator / (I value, const rational<II>& r)
{
  if (!r.numerator ())
    throw bad_rational ();

  return rational<I> (value * r.denominator (), r.numerator ());
}

/*
    Инкремент / декремент
*/

template <class I>
inline const rational<I>& rational<I>::operator ++ ()
{
  return *this += int_type (1);
}

template <class I>
inline const rational<I>& rational<I>::operator -- ()
{
  return *this -= int_type (1);
}

/*
    Унарные операции
*/

template <class I>
inline rational<I> rational<I>::operator + () const
{
  return *this;
}

template <class I>
inline rational<I> rational<I>::operator - () const
{
  return rational (-n, d);
}

template <class I>
inline bool rational<I>::operator ! () const
{
  return n == 0;
}

template <class I>
inline rational<I>::operator unspecified_bool_type () const
{
  return n ? &rational::operator ! : 0;
}

/*
    Сравнение
*/

template <class I>
inline bool rational<I>::operator < (const rational& r) const
{
  if (d == r.d)
  {
    return n < r.n;
  }
  else
  {
    return n * r.d < d * r.n;
  }
}

template <class I>
inline bool rational<I>::operator == (const rational& r) const
{
  return n * r.d == d * r.n;
}

template <class I>
inline bool rational<I>::operator != (const rational& r) const
{
  return !(*this == r);
}

template <class I>
inline bool rational<I>::operator > (const rational& r) const
{
  return r < *this;
}

template <class I>
inline bool rational<I>::operator <= (const rational& r) const
{
  return !(r < *this);
}

template <class I>
inline bool rational<I>::operator >= (const rational& r) const
{
  return !(*this < r);
}

template <class I>
inline bool rational<I>::operator == (int_type r) const
{
  return n == r * d;
}

template <class I>
inline bool rational<I>::operator != (int_type r) const
{
  return !(*this == r);
}

template <class I>
inline bool rational<I>::operator < (int_type r) const
{
  if (d == 1)
  {
    return n < r;
  }
  else
  {
    return n < d * r;
  }
}

template <class I>
inline bool rational<I>::operator > (int_type r) const
{
  return r < *this;
}

template <class I>
inline bool rational<I>::operator <= (int_type r) const
{
  return !(r < *this);  
}

template <class I>
inline bool rational<I>::operator >= (int_type r) const
{
  return !(*this < r);
}

/*
    Получение абсолютного значения
*/

template <class I>
inline rational<I> abs (const rational<I>& r)
{
  if      (r.numerator () >= I (0) && r.denominator () >= I (0))  return r;
  else if (r.numerator () >= I (0))                               return rational<I> (r.numerator (), -r.denominator ());
  else if (r.denominator () >= I (0))                             return rational<I> (-r.numerator (), r.denominator ());
  else                                                            return rational<I> (-r.numerator (), -r.denominator ());
}

/*
    Приведение типов
*/

template <class I> template <class T>
inline T rational<I>::cast () const
{
  return static_cast<T> (n) / d;
}

template <class T, class I>
inline T rational_cast (const rational<I>& r)
{
  return r.cast<T> ();
}
