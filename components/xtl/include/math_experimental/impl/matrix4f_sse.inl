template <>
__forceinline matrix<float,4,4>& matrix<float,4,4>::operator = (const matrix<float,4,4>& m)
{
  row (0) = m [0];
  row (1) = m [1];
  row (2) = m [2];
  row (3) = m [3];

  return *this;
}

template <>
__forceinline void matrix_assign_scalar (matrix<float,4,4>& res,const float& a)
{
  __m128 r = _mm_set_ps (0.0f,0.0f,0.0f,a);

  res [0] = r;
  res [1] = _mm_shuffle_ps (r,r,_MM_SHUFFLE (1,1,0,1));
  res [2] = _mm_shuffle_ps (r,r,_MM_SHUFFLE (1,0,1,1));
  res [3] = _mm_shuffle_ps (r,r,_MM_SHUFFLE (0,1,1,1));
}

template <>
__forceinline void matrix_mul_scalar (matrix<float,4,4>& res,const matrix<float,4,4>& a,const float& b)
{
  __m128 r = _mm_set_ps1 (b);

  res [0] = _mm_mul_ps (a [0],r);  
  res [1] = _mm_mul_ps (a [1],r);  
  res [2] = _mm_mul_ps (a [2],r);  
  res [3] = _mm_mul_ps (a [3],r);  
}

template <>
__forceinline void matrix_add (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  res [0] = _mm_add_ps (a [0],b [0]);  
  res [1] = _mm_add_ps (a [1],b [1]);  
  res [2] = _mm_add_ps (a [2],b [2]);  
  res [3] = _mm_add_ps (a [3],b [2]);  
} 

template <>
__forceinline void matrix_sub (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  res [0] = _mm_sub_ps (a [0],b [0]);  
  res [1] = _mm_sub_ps (a [1],b [1]);  
  res [2] = _mm_sub_ps (a [2],b [2]);  
  res [3] = _mm_sub_ps (a [3],b [2]);  
} 

inline float matrix_det (const matrix<float,4,4>& m) 
{ 
  __m128 r1,r2,r3,r4,r5,r6; 
  float res;
  
  r1 = _mm_shuffle_ps (m [1],m [1],_MM_SHUFFLE (0,3,2,1));
  r2 = _mm_shuffle_ps (m [2],m [2],_MM_SHUFFLE (2,1,0,3));
  r3 = _mm_shuffle_ps (m [3],m [3],_MM_SHUFFLE (1,0,3,2));

  r5 = _mm_mul_ps     (r1,r2);  
 
  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (1,0,3,2));
  r2 = _mm_shuffle_ps (r2,r2,_MM_SHUFFLE (1,0,3,2));

  r4 = _mm_mul_ps     (r1,r2);
  r5 = _mm_sub_ps     (r5,r4);
  r6 = _mm_mul_ps     (r5,r3);

  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (2,1,0,3));
  r3 = _mm_shuffle_ps (r3,r3,_MM_SHUFFLE (0,3,2,1));

  r5 = _mm_mul_ps     (r1,r2);

  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (2,1,0,3));
  r2 = _mm_shuffle_ps (r2,r2,_MM_SHUFFLE (0,3,2,1));

  r4 = _mm_mul_ps     (r1,r2);
  r5 = _mm_sub_ps     (r5,r4);
  r5 = _mm_mul_ps     (r5,r3);
  r6 = _mm_add_ps     (r6,r5);

  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (1,0,3,2));
  r3 = _mm_shuffle_ps (r3,r3,_MM_SHUFFLE (1,0,3,2));

  r5 = _mm_mul_ps     (r1,r2);

  r1 = _mm_shuffle_ps (r1,r1,_MM_SHUFFLE (2,1,0,3));
  r2 = _mm_shuffle_ps (r2,r2,_MM_SHUFFLE (0,3,2,1));

  r4 = _mm_mul_ps     (r1,r2);
  r5 = _mm_sub_ps     (r5,r4);
  r5 = _mm_mul_ps     (r5,r3);
  r6 = _mm_add_ps     (r6,r5);

  r6 = _mm_mul_ps     (r6,m [0]);

  r4 = _mm_shuffle_ps (r6,r6,_MM_SHUFFLE (1,0,3,2));
  r6 = _mm_add_ps     (r6,r4);
  r4 = _mm_shuffle_ps (r6,r6,_MM_SHUFFLE (2,3,0,1));
  r6 = _mm_sub_ps     (r4,r6);

  _mm_store_ss        (&res,r6);

  return res;
}

template <>
__forceinline void matrix_transpose (matrix<float,4,4>& m)
{
  _MM_TRANSPOSE4_PS (m [0],m [1],m [2],m [3]);
}

template <>
__forceinline void matrix_transpose (matrix<float,4,4>& dst,const matrix<float,4,4>& src)
{
  __m128 r3, r2, r1, r0;

  r0 = _mm_shuffle_ps (src [0],src [1], 0x44);
  r2 = _mm_shuffle_ps (src [0],src [1], 0xEE);
  r1 = _mm_shuffle_ps (src [2],src [3], 0x44);
  r3 = _mm_shuffle_ps (src [2],src [3], 0xEE);

  dst [0] = _mm_shuffle_ps (r0, r1, 0x88);
  dst [1] = _mm_shuffle_ps (r0, r1, 0xDD);
  dst [2] = _mm_shuffle_ps (r2, r3, 0x88);
  dst [3] = _mm_shuffle_ps (r2, r3, 0xDD);
}

template <>
__forceinline void matrix_mul_rr (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  __m128 r0,r1,r2,r3,r4,r5,r6;

  for (size_t i=0;i<4;i++)
  {            
     r0 = _mm_mul_ps     (a [i],b [0]);
     r1 = _mm_mul_ps     (a [i],b [1]);
     r2 = _mm_mul_ps     (a [i],b [2]);
     r3 = _mm_mul_ps     (a [i],b [3]);

     r5 = r0;
     r5 = _mm_movelh_ps  (r5,r1);
     r1 = _mm_movehl_ps  (r1,r0);
     r5 = _mm_add_ps     (r5,r1);

     r4 = r2;
     r2 = _mm_movelh_ps  (r2,r3);
     r3 = _mm_movehl_ps  (r3,r4);
     r3 = _mm_add_ps     (r3,r2);

     r6 = r3;
     r6 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (2,0,2,0));
     r5 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (3,1,3,1));
     r6 = _mm_add_ps     (r6,r5);

     res [i] = r6;
  }
} 

template <>  
__forceinline void matrix_mul_cc (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  matrix_mul_rr (res,transpose (a),transpose (b));
}

template <>
__forceinline void matrix_mul_rc (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  matrix<float,4,4> tmp;

  matrix_transpose (tmp,b);
  matrix_mul_rr    (res,a,tmp);
} 

template <>
inline void matrix_mul_cr (matrix<float,4,4>& res,const matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  matrix_mul_rc (res,b,a);
  matrix_transpose (res);
}

template <>
inline void matrix_mul_rr (matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  __m128 r0,r1,r2,r3,r4,r5,r6;

  for (size_t i=0;i<4;i++)
  {            
    r0 = _mm_mul_ps     (a [i],b [0]);
    r1 = _mm_mul_ps     (a [i],b [1]);
    r2 = _mm_mul_ps     (a [i],b [2]);
    r3 = _mm_mul_ps     (a [i],b [3]);

    r5 = r0;
    r5 = _mm_movelh_ps  (r5,r1);
    r1 = _mm_movehl_ps  (r1,r0);
    r5 = _mm_add_ps     (r5,r1);

    r4 = r2;
    r2 = _mm_movelh_ps  (r2,r3);
    r3 = _mm_movehl_ps  (r3,r4);
    r3 = _mm_add_ps     (r3,r2);

    r6 = r3;
    r6 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (2,0,2,0));
    r5 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (3,1,3,1));
    r6 = _mm_add_ps     (r6,r5);

    a [i] = r6;
  }
} 

template <>
inline void matrix_mul_rc (matrix<float,4,4>& a,const matrix<float,4,4>& b)
{
  matrix<float,4,4> tmp;

  matrix_transpose (tmp,b);
  matrix_mul_rr    (a,tmp);  
} 

__forceinline void matrix_swaprow (matrix<float,4,4>& a,size_t i,size_t j)
{
  vec<float,4> tmp = a [i];
  a [i]            = a [j];
  a [j]            = tmp;      
}

//template <>
__forceinline void matrix_invert (matrix<float,4,4>& res,const matrix<float,4,4>& src)
{
  matrix<float,4,4> a = src;
  res = 1;

  for (size_t i=0;i<4;i++)
  {
    if (a [i][i] == 0.0f)
      for (size_t k=i+1;k<4;k++)
        if (a [k][i] != 0.0f)
        {
          matrix_swaprow (a,i,k);
          matrix_swaprow (res,i,k);
          break;
        }

    vec<float,4> t = 1.0f / a[i][i];

    res [i] *= t;
    a [i] *= t;

    for (size_t j=0;j<i;j++)
    {
      vec<float,4> t = a [j][i];

      a   [j] -= a   [i] * t;
      res [j] -= res [i] * t;
    }

    for (j++;j<4;j++)
    {
      vec<float,4> t = a [j][i];

      a   [j] -= a   [i] * t;
      res [j] -= res [i] * t;
    }
  }
}

/*template <>
__forceinline void matrix_mul_vec (vec<float,4>& res,const matrix<float,4,4>& m,const vec<float,4>& v)
{
  __m128 r0,r1,r2,r3,r4,r5,r6;

  r0 = _mm_mul_ps     (m [0],v);
  r1 = _mm_mul_ps     (m [1],v);
  r2 = _mm_mul_ps     (m [2],v);
  r3 = _mm_mul_ps     (m [3],v);

  r5 = r0;
  r5 = _mm_movelh_ps  (r5,r1);
  r1 = _mm_movehl_ps  (r1,r0);
  r5 = _mm_add_ps     (r5,r1);

  r4 = r2;
  r2 = _mm_movelh_ps  (r2,r3);
  r3 = _mm_movehl_ps  (r3,r4);
  r3 = _mm_add_ps     (r3,r2);

  r6 = r3;
  r6 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (2,0,2,0));
  r5 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (3,1,3,1));
  r6 = _mm_add_ps     (r6,r5);

  res = r6;
}

template <>
__forceinline void vec_mul_matrix (vec<float,4>& res,const vec<float,4>& v,const matrix<float,4,4>& m)
{
  __m128 r0,r1,r2,r3,r4,r5,r6;
  __m128 t0,t1,t2,t3;
                                                            
  t0  = _mm_shuffle_ps (m [0], m [1], 0x44);          
  t2  = _mm_shuffle_ps (m [0], m [1], 0xEE);          
  t1  = _mm_shuffle_ps (m [2], m [3], 0x44);          
  t3  = _mm_shuffle_ps (m [2], m [3], 0xEE);          
                                                          
  r0 = _mm_shuffle_ps (t0, t1, 0x88);              
  r0 = _mm_mul_ps     (r0,v);

  r1 = _mm_shuffle_ps (t0, t1, 0xDD);              
  r1 = _mm_mul_ps     (r1,v);

  r2 = _mm_shuffle_ps (t2, t3, 0x88);              
  r2 = _mm_mul_ps     (r2,v);

  r3 = _mm_shuffle_ps (t2, t3, 0xDD);       
  r3 = _mm_mul_ps     (r3,v);

  r5 = r0;
  r5 = _mm_movelh_ps  (r5,r1);
  r1 = _mm_movehl_ps  (r1,r0);
  r5 = _mm_add_ps     (r5,r1);

  r4 = r2;
  r2 = _mm_movelh_ps  (r2,r3);
  r3 = _mm_movehl_ps  (r3,r4);
  r3 = _mm_add_ps     (r3,r2);

  r6 = r3;
  r6 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (2,0,2,0));
  r5 = _mm_shuffle_ps (r5,r3,_MM_SHUFFLE (3,1,3,1));
  r6 = _mm_add_ps     (r6,r5);

  res = r6;
}
  */