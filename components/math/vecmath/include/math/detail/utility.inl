namespace detail
{

/*
    Преобразования: кватернион - матрица, осевой угол, углы Эйлера
*/

struct quat_to_matrix {
  private:
    template <class T, unsigned int Size>
    static void convert (const quat<T>& q, matrix<T, Size>& res)
    {
      T wx,  wy,  wz,  xx,  yy,  yz,  xy,  xz,  zz,  x2,  y2,  z2;
      T s = T (2) / norm (q);
     
      x2 = q.x * s;    y2 = q.y * s;    z2 = q.z * s;
      xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
      yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
      wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;
      
      res [0][0] = T(1) - (yy + zz);
      res [0][1] = xy - wz;
      res [0][2] = xz + wy;

      res [1][0] = xy + wz;
      res [1][1] = T(1) - (xx + zz);
      res [1][2] = yz - wx;

      res [2][0] = xz - wy;
      res [2][1] = yz + wx;
      res [2][2] = T(1) - (xx + yy);
    }
    
  public:
    template <class T>
    void operator () (const quat<T>& q, matrix<T, 3>& m) const
    {
      convert (q, m);
    }
    
    template <class T>
    void operator () (const quat<T>& q, matrix<T, 4>& m) const
    {
      convert (q, m);
      
      m [0][3] = m [1][3] = m [2][3] = 0;
      m [3][0] = m [3][1] = m [3][2] = 0;
      m [3][3] = T (1);
    }
};

struct matrix_to_quat {
  private:
    template <class T, unsigned int Size> 
    static void convert (const matrix<T, Size>& m, quat<T>& res)
    {
      T tr = m [0][0] + m [1][1] + m [2][2];

      if (tr > 0)
      {
        T  s   = sqrt (tr + T (1));
        res.w  = s * T (0.5);
        s      = T (0.5) / s;
        res.x  = (m [2][1] - m [1][2]) * s;
        res.y  = (m [0][2] - m [2][0]) * s;
        res.z  = (m [1][0] - m [0][1]) * s;
      }
      else
      {
        const size_t nxt [3] = {1,  2,  0};

        size_t i = 0;

        if (m [1][1] > m[0][0]) i = 1;
        if (m [2][2] > m[i][i]) i = 2;

        size_t j = nxt [i];
        size_t k = nxt [j];

        T s = sqrt ((m [i][i] - (m [j][j] + m [k][k])) + T (1));

        res [i] = s * T (0.5);

        if (s != T (0)) s = T (0.5) / s;

        res [3] = (m [k][j] - m [j][k]) * s;
        res [j] = (m [j][i] + m [i][j]) * s;
        res [k] = (m [k][i] + m [i][k]) * s;
      }
    }  
  
  public:
    template <class T>
    void operator () (const matrix<T, 3>& m, quat<T>& res) const
    {
      convert (m, res);
    }
    
    template <class T>
    void operator () (const matrix<T, 4>& m, quat<T>& res) const
    {
      convert (m, res);
    }
};

struct axis_angle_to_quat {
  template <class T> 
  void operator () (const angle<T>& ang, const vector<T, 3>& axis, quat<T>& q) const
  {
    vector<T, 3> v          = normalize (axis);
    angle<T>     half_angle = ang * T (0.5);
    T            sin_a      = sin (half_angle);

    q.x = v.x * sin_a;
    q.y = v.y * sin_a;
    q.z = v.z * sin_a;
    q.w = cos (half_angle);
  }
};

struct euler_angles_to_quat {
  template <class T> 
  void operator () (const angle<T>& pitch, const angle<T>& yaw, const angle<T>& roll, quat<T>& q) const
  {
    angle<T> ex = pitch * T (0.5),
             ey = yaw * T (0.5),
             ez = roll * T (0.5);
             
    T cr   = cos (ex),
      cp   = cos (ey),
      cy   = cos (ez),
      sr   = sin (ex),
      sp   = sin (ey),
      sy   = sin (ez),
      cpcy = cp * cy,
      spsy = sp * sy;

    q.w  = cr * cpcy + sr * spsy;
    q.x  = sr * cpcy - cr * spsy;
    q.y  = cr * sp * cy + sr * cp * sy;
    q.z  = cr * cp * sy - sr * sp * cy;
    q   /= length (q);
  }  
};

/*
    Аффинные преобразования
*/

struct matrix_translate {
  template <class T> 
  void operator () (const vector<T, 3>& v, matrix<T, 4>& res) const
  {
    res [0][3] = v [0];
    res [1][3] = v [1];
    res [2][3] = v [2];
    res [0][0] = res [1][1] = res [2][2] = res [3][3] = T (1);
    res [0][1] = res [0][2] = res [1][0] = res [1][2] = 
    res [2][0] = res [2][1] = res [3][0] = res [3][1] =
    res [3][2] = 0;
  }
};

struct matrix_scale {
  template <class T> 
  void operator () (const vector<T, 3>& v, matrix<T, 4>& res) const
  {
    res [0][0] = v [0];
    res [1][1] = v [1];
    res [2][2] = v [2];
    res [3][3] = T (1);
    res [0][1] = res [0][2] = res [0][3] = 
    res [1][0] = res [1][2] = res [1][3] =
    res [2][0] = res [2][1] = res [2][3] = 
    res [3][0] = res [3][1] = res [3][2] = 0;
  }
};

struct matrix_rotate {
  private:
    template <class T, unsigned int Size> 
    static void convert (const angle<T>& ang, const vector<T, 3>& axis, matrix<T, Size>& res)
    {
      T cosine = cos (ang),
        sine   = sin (ang);

      res [0][0] = axis [0]*axis [0]+(T(1)-axis [0]*axis [0])*cosine;
      res [1][0] = axis [0]*axis [1]*(T(1)-cosine)+axis [2]*sine;
      res [2][0] = axis [0]*axis [2]*(T(1)-cosine)-axis [1]*sine;

      res [0][1] = axis [0]*axis [1]*(T(1)-cosine)-axis [2]*sine;
      res [1][1] = axis [1]*axis [1]+(T(1)-axis [1]*axis [1])*cosine;
      res [2][1] = axis [1]*axis [2]*(T(1)-cosine)+axis [0]*sine;

      res [0][2] = axis [0]*axis [2]*(T(1)-cosine)+axis [1]*sine;
      res [1][2] = axis [1]*axis [2]*(T(1)-cosine)-axis [0]*sine;
      res [2][2] = axis [2]*axis [2]+(T(1)-axis [2]*axis [2])*cosine;
    }
    
  public:
    template <class T>
    void operator () (const angle<T>& ang, const vector<T, 3>& axis, matrix<T, 3>& res) const
    {
      convert (ang, axis, res);
    }
    
    template <class T>
    void operator () (const angle<T>& ang, const vector<T, 3>& axis, matrix<T, 4>& res) const
    {
      convert (ang, axis, res);
      
      res [0][3] = 0;
      res [1][3] = 0;
      res [2][3] = 0;
      res [3][3] = 1;
    }
};

struct matrix_lookat {
  template <class T>
  void operator () (const vector<T, 3>& pos, const vector<T, 3>& center, const vector<T, 3>& up, matrix<T, 4>& res)
  { 
    vector<T, 3> z = normalize (pos - center), 
                 y = up, 
                 x = normalize (cross (y, z));

    y = normalize (cross (z, x));
    
    res [0] = vector<T, 4> (x, 0.0f);
    res [1] = vector<T, 4> (y, 0.0f);
    res [2] = vector<T, 4> (z, 0.0f);
    res [3] = vector<T, 4> (0.0f, 0.0f, 0.0f, 1.0f);

    res = inverse (res * translate (-pos));
  }
};

}

/*
    Преобразования: кватернион - матрица,  осевой угол,  углы Эйлера
*/

template <class T>
void to_matrix (const quat<T>& q, matrix<typename quat<T>::value_type, 4>& res)
{
  detail::quat_to_matrix ()(q, res);
}

template <class T>
void to_matrix (const quat<T>& q, matrix<typename quat<T>::value_type, 3>& res)
{
  detail::quat_to_matrix ()(q, res);
}

template <class T>
matrix<T, 4> to_matrix (const quat<T>& q)
{
  return matrix<T, 4> (q, detail::quat_to_matrix (), return_value_tag ());
}

template <class T>
quat<T> to_quat (const matrix<T, 4>& m)
{
  return quat<T> (m, detail::matrix_to_quat (), return_value_tag ());
}

template <class T>
quat<T> to_quat (const matrix<T, 3>& m)
{
  return quat<T> (m, detail::matrix_to_quat (), return_value_tag ());
}

template <class T>
quat<T> to_quat (const angle<T>& pitch, const angle<T>& yaw, const angle<T>& roll)
{
  return quat<T> (pitch, yaw, roll, detail::euler_angles_to_quat (), return_value_tag ());
}

template <class T>
quat<T> to_quat (const angle<T>& ang, const vector<T, 3>& axis)
{
  return quat<T> (ang, axis, detail::axis_angle_to_quat (), return_value_tag ());
}

template <class T>
void to_axis_angle (const quat<T>& q, angle<T>& ang, vector<T, 3>& axis, const T& eps)
{
  T vl = sqrt (q.x*q.x + q.y*q.y + q.z*q.z);

  if (vl > eps)
  {
    T ivl = 1.0f/vl;
    
    axis [0] = q.x*ivl;
    axis [1] = q.y*ivl;
    axis [2] = q.z*ivl;

    if (q.w < 0) ang = radian (T (2) * atan2 (-vl, -q.w)); //-PI, 0
    else         ang = radian (T (2) * atan2 ( vl,  q.w)); //0, PI
  }
  else
  {
    axis = T (0);
    ang  = angle<T> ();
  }
}

template <class T> 
void to_euler_angles (const quat<T>& q, angle<T>& pitch, angle<T>& yaw, angle<T>& roll)
{
  T w2 = q.w * q.w, 
    x2 = q.x * q.x, 
    y2 = q.y * q.y, 
    z2 = q.z * q.z;

  pitch = radian (atan (T (2) * (q.y*q.z + q.x*q.w)/(-x2 - y2 + z2 + w2)));
  yaw   = radian (asin (T (-2) * (q.x*q.z - q.y*q.w)));
  roll  = radian (atan (T (2) * (q.x*q.y + q.z*q.w)/(x2 - y2 - z2 + w2)));
}

/*
    Аффинные преобразования
*/

template <class T>
matrix<T, 4> translate (const vector<T, 3>& v)
{
  return matrix<T, 4> (v, detail::matrix_translate (), return_value_tag ());
}

template <class T>
matrix<T, 4> scale (const vector<T, 3>& v)
{
  return matrix<T, 4> (v, detail::matrix_scale (), return_value_tag ());
}

template <class T>
matrix<T, 4> rotate (const angle<T>& ang, const vector<T, 3>& axis)
{
  return matrix<T, 4> (ang, axis, detail::matrix_rotate (), return_value_tag ());
}

template <class T>
matrix<T, 4> lookat (const vector<T, 3>& pos, const vector<T, 3>& center, const vector<T, 3>& up)
{
  return matrix<T, 4> (pos, center, up, detail::matrix_lookat (), return_value_tag ());
}

/*
    Определение угла между векторами
*/

template <class T, unsigned int Size>
angle<T> find_angle (const vector<T, Size>& a, const vector<T, Size>& b, const T& eps)
{
  T mult_len = sqrt (qlen (a) * qlen (b));
  
  if (mult_len < eps)
    return angle<T> ();

  float angle_cos = dot (a, b) / mult_len;

  if (angle_cos > 1.f)
    angle_cos = 1.f;
  else if (angle_cos < -1.f)
    angle_cos = -1.f;

  return radian (T (acos (angle_cos)));
}
