#ifndef MATHLIB_VECMATH_VECTOR_HEADER
#define MATHLIB_VECMATH_VECTOR_HEADER

#include <cmath>

#ifdef VECMATH_SSE
  #include <xmmintrin.h>
#endif

#ifdef _WIN32_WCE
  #include <stdlib.h>
#endif

#ifdef _MSC_VER
  #pragma pack (push, 1)
#endif

#undef min
#undef max

namespace math
{

//For using return value optimization
struct return_value_tag {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Default vector base
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size> struct vector_base       { T x [Size]; };
template <class T>                    struct vector_base<T, 2> { T x, y; };
template <class T>                    struct vector_base<T, 3> { T x, y, z; };
template <class T>                    struct vector_base<T, 4> { T x, y, z, w; };

///////////////////////////////////////////////////////////////////////////////////////////////////
//Vector
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
class vector: public vector_base<T, Size>
{
  public:
    typedef vector_base<T, Size> base;
    typedef T                    value_type;

    enum { size = Size };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors
///////////////////////////////////////////////////////////////////////////////////////////////////
    vector ();
    vector (const value_type&);
    vector (const base&);
    vector (const value_type&, const value_type&, const value_type& = 0, const value_type& = 0); 
    vector (const vector<value_type, size-1>&, const value_type&);

    template <unsigned int Size1> vector (const vector<value_type, Size1>&);

      //For using return value optimization
    template <class T1, class Fn>                     vector (const T1& arg, Fn fn, return_value_tag);                  //fn (arg, *this)
    template <class T1, class T2, class Fn>           vector (const T1& arg1, const T2& arg2, Fn fn, return_value_tag); //fn (arg1, arg2, *this)
    template <class T1, class T2, class T3, class Fn> vector (const T1& arg1, const T2& arg2, const T3& arg3, Fn fn, return_value_tag); //fn (arg1, arg2, arg3, *this)

///////////////////////////////////////////////////////////////////////////////////////////////////
///Assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    vector& operator = (const value_type&); 
    vector& operator = (const base&);

    template <unsigned int Size1> vector& operator = (const vector<value_type, Size1>&); 
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Indexing
///////////////////////////////////////////////////////////////////////////////////////////////////
    value_type&       operator [] (unsigned int index);
    const value_type& operator [] (unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Unary operators
///////////////////////////////////////////////////////////////////////////////////////////////////
    const vector& operator + () const;
          vector  operator - () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Binary operators
///////////////////////////////////////////////////////////////////////////////////////////////////
    vector& operator += (const vector&);
    vector& operator -= (const vector&);
    vector& operator *= (const vector&);
    vector& operator /= (const vector&);
    vector& operator *= (const value_type&);
    vector& operator /= (const value_type&);
    vector  operator +  (const vector&) const;
    vector  operator -  (const vector&) const;
    vector  operator *  (const vector&) const;
    vector  operator /  (const vector&) const;
    vector  operator *  (const value_type&) const;
    vector  operator /  (const value_type&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Comparison
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vector&) const;
    bool operator != (const vector&) const;
};

template <class T> class vector<T, 0> {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Types definition
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef vector<float,2>          vec2f;
typedef vector<double,2>         vec2d;
typedef vector<int,2>            vec2i;
typedef vector<unsigned int,2>   vec2ui;
typedef vector<short,2>          vec2s;
typedef vector<unsigned short,2> vec2us;
typedef vector<char,2>           vec2b;
typedef vector<unsigned char,2>  vec2ub;

typedef vector<float,3>          vec3f;
typedef vector<double,3>         vec3d;
typedef vector<int,3>            vec3i;
typedef vector<unsigned int,3>   vec3ui;
typedef vector<short,3>          vec3s;
typedef vector<unsigned short,3> vec3us;
typedef vector<char,3>           vec3b;
typedef vector<unsigned char,3>  vec3ub;

typedef vector<float,4>          vec4f;
typedef vector<double,4>         vec4d;
typedef vector<int,4>            vec4i;
typedef vector<unsigned int,4>   vec4ui;
typedef vector<short,4>          vec4s;
typedef vector<unsigned short,4> vec4us;
typedef vector<char,4>           vec4b;
typedef vector<unsigned char,4>  vec4ub;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Multiplication scalar by vector
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size> 
vector<T, Size> operator * (const T& a, const vector<T, Size>& v);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Scalar multiplication
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size> 
typename vector<T, Size>::value_type dot (const vector<T, Size>&, const vector<T, Size>&); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Comparison with error bounds
///////////////////////////////////////////////////////////////////////////////////////////////////    
template <class T, unsigned int Size>
bool equal (const vector<T, Size>&, const vector<T, Size>&, const T& eps); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Vector's length
///////////////////////////////////////////////////////////////////////////////////////////////////                
template <class T, unsigned int Size>                          
typename vector<T, Size>::value_type length (const vector<T, Size>&); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Vector's length square
///////////////////////////////////////////////////////////////////////////////////////////////////                
template <class T, unsigned int Size>
typename vector<T, Size>::value_type qlen (const vector<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get normalized vector
///////////////////////////////////////////////////////////////////////////////////////////////////                
template <class T, unsigned int Size>                          
vector<T, Size> normalize (const vector<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get per-component absolute value
///////////////////////////////////////////////////////////////////////////////////////////////////                
template <class T, unsigned int Size>                          
vector<T, Size> abs (const vector<T, Size>&); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get per-component min/max
///////////////////////////////////////////////////////////////////////////////////////////////////                
template <class T, unsigned int Size>
vector<T, Size> min (const vector<T, Size>&, const vector<T, Size>&); 

template <class T, unsigned int Size>
vector<T, Size> max (const vector<T, Size>&, const vector<T, Size>&); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cross product
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
vector<T, 3> cross (const vector<T, 3>& a, const vector<T, 3>& b);

template <class T>
vector<T, 4> cross (const vector<T, 4>& a, const vector<T, 4>& b);

#include <math/detail/vector.inl>

}

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#endif
