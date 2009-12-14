#ifndef MATHLIB_VECMATH_ANGLE_HEADER
#define MATHLIB_VECMATH_ANGLE_HEADER

#include <cmath>

#include <math/constants.h>

namespace math
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct degree_tag {};
struct radian_tag {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class angle
{
  public:
    typedef T value_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    angle ();
    angle (const angle&);
    angle (const value_type& value, degree_tag);
    angle (const value_type& value, radian_tag);
    
    template <class T1> angle (const angle<T1>&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void assign (const angle&);
    void assign (const value_type& value, degree_tag);
    void assign (const value_type& value, radian_tag);
    
    template <class T1> void assign (const angle<T1>&);
    
                        angle& operator = (const angle&);
    template <class T1> angle& operator = (const angle<T1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���� � ��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    value_type get       (degree_tag) const;
    value_type get       (radian_tag) const;
    value_type to_radian () const;
    value_type to_degree () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const angle& operator + () const;
          angle  operator - () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    angle& operator += (const angle&);
    angle& operator -= (const angle&);
    angle& operator *= (const value_type&);
    angle& operator /= (const value_type&);
    angle  operator +  (const angle&) const;
    angle  operator -  (const angle&) const;
    angle  operator *  (const value_type&) const;
    angle  operator /  (const value_type&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const angle&) const;
    bool operator != (const angle&) const;
    bool operator <  (const angle&) const;
    bool operator >  (const angle&) const;
    bool operator <= (const angle&) const;
    bool operator >= (const angle&) const;
    
  private:
    explicit angle (const value_type&);

  private:
    value_type value;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef angle<float>          anglef;
typedef angle<double>         angled;
typedef angle<int>            anglei;
typedef angle<unsigned int>   angleui;
typedef angle<short>          angles;
typedef angle<unsigned short> angleus;
typedef angle<char>           angleb;
typedef angle<unsigned char>  angleub;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
angle<T> operator * (const typename angle<T>::value_type&, const angle<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
anglef radian (float  value);
angled radian (double value);
anglef degree (float  value);
angled degree (double value);

template <class T>
typename angle<T>::value_type radian (const angle<T>&);

template <class T>
typename angle<T>::value_type degree (const angle<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> angle<T>                      abs  (const angle<T>&);
template <class T> typename angle<T>::value_type sin  (const angle<T>&);
template <class T> typename angle<T>::value_type sinh (const angle<T>&);
template <class T> typename angle<T>::value_type cos  (const angle<T>&);
template <class T> typename angle<T>::value_type cosh (const angle<T>&);
template <class T> typename angle<T>::value_type tan  (const angle<T>&);
template <class T> typename angle<T>::value_type tanh (const angle<T>&);

#include <math/detail/angle.inl>

}

#endif
