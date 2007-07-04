#pragma pack(push,1)

////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер для SSE-ориентированных векторных баз
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __MATHLIB_SSE_ALIGN__
template <> 
struct vec_base<float,4>
{
             vec_base  () {}
             vec_base  (const __m128&);

  vec_base& operator = (const __m128&);
  vec_base& operator = (const vec_base&);

             operator float*        ()       { return &x; }
             operator const float*  () const { return &x; }
             operator const __m128& () const { return p;  }
             operator       __m128& ()       { return p;  }

  union {
    __m128 p;
    struct {
      float  x,y,z,w;
    };  
  };
};

#else

template <> 
struct vec_base<float,4>
{
             vec_base  () {}
             vec_base  (const __m128&);

  vec_base& operator = (const __m128&);

             operator float*       ()       { return &x; }
             operator const float* () const { return &x; }
             operator const __m128 () const;

  float  x,y,z,w;
};

#endif

#pragma pack(pop)

template <>
__forceinline void vec_assign_scalar (vec<float,4>& res,const float& a)
{
  res = _mm_set_ps1 (a);
}

template <>
__forceinline  void vec_add (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b) 
{ 
  res = _mm_add_ps (a,b);
}

template <>
__forceinline void vec_sub (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b) 
{ 
  res = _mm_sub_ps (a,b);
}

template <>
__forceinline void vec_mul (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b) 
{ 
  res = _mm_mul_ps (a,b);
}

template <>
__forceinline void vec_div (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b) 
{ 
  res = _mm_div_ps (a,b);
}

template <>
__forceinline void vec_abs (vec<float,4>& res,const vec<float,4>& src)
{
  static union {
    size_t i;
    float  f;  
  } mask = {0x7FFFFFFF};

  res = _mm_and_ps (src,_mm_set_ps1 (mask.f));
}

template <>
__forceinline void vec_neg (vec<float,4>& res,const vec<float,4>& src)
{
  static union {
    size_t i;
    float  f;  
  } mask = {0x80000000};

  res = _mm_xor_ps (src,_mm_set_ps1 (mask.f));
}

template <> 
__forceinline void vec_min (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b)
{
  res = _mm_min_ps (a,b);
}

template <> 
__forceinline void vec_max (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b)
{
  res = _mm_max_ps (a,b);
}

#ifndef __MATHLIB_SSE_ALIGN__

__forceinline vec_base<float,4>::vec_base<float,4> (const __m128& p)
{
  _mm_storeu_ps (&x,p);
}

__forceinline vec_base<float,4>::operator const __m128 () const
{
  return _mm_loadu_ps (&x);
} 

template <>
__forceinline void vec_copy (vec<float,4>& res,const vec<float,4>& src)
{
  _mm_storeu_ps (res,src);
}

#else

__forceinline vec_base<float,4>::vec_base<float,4> (const __m128& _p)
{
  p = _p;
}

__forceinline vec_base<float,4>& vec_base<float,4>::operator = (const __m128& _p)
{
  p = _p;
  return *this;
}

__forceinline vec_base<float,4>& vec_base<float,4>::operator = (const vec_base<float,4>& v)
{
  p = v.p;
  return *this;
} 

template <>
__forceinline void vec_copy (vec<float,4>& res,const vec<float,4>& src)
{
  res.p = src.p;
}

#endif

__forceinline void vec_cross_product (vec<float,4>& res,const vec<float,4>& a,const vec<float,4>& b)
{
  __m128 r0,r1,r2;

  r0 = _mm_shuffle_ps (a,a,_MM_SHUFFLE (3,1,0,2));
  r1 = _mm_shuffle_ps (b,b,_MM_SHUFFLE (3,0,2,1));
  r2 = _mm_mul_ps     (r1,r0);

  r0 = _mm_shuffle_ps (r0,r0,_MM_SHUFFLE (3,1,0,2));
  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (3,0,2,1));
  r1 = _mm_mul_ps     (r1,r0);

  r1 = _mm_sub_ps     (r1,r2);

  res = r1;
}
