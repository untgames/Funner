#ifndef BOUND_VOLUMES_FRUSTUM_HEADER
#define BOUND_VOLUMES_FRUSTUM_HEADER

#include <mathlib.h>

namespace bound_volumes
{

//forward declarations
template <class T> class axis_aligned_box;
template <class T> class sphere;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class frustum
{
  public:
    typedef T               value_type;
    typedef math::vec<T,3>  vec_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    frustum();
    frustum(const vec_type& _origin, const vec_type& _direction,
            value_type _near_plane, value_type _far_plane,
            value_type _width, value_type _height);
    frustum(const frustum& _frustum);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const vec_type& origin()      const;
    const vec_type& direction()   const;
    value_type      width()       const;
    value_type      height()      const;
    value_type      near_plane()  const;
    value_type      far_plane()   const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void set_origin     (const vec_type& _origin);
    void set_origin     (value_type _origin_x, value_type _origin_y, value_type _origin_z);
    void set_direction  (const vec_type& _direction);
    void set_direction  (value_type _direction_x, value_type _direction_y, value_type _direction_z);
    void set_width      (value_type _width);
    void set_height     (value_type _height);
    void set_size       (value_type _width, value_type _height);
    void set_near_plane (value_type _near_plane);
    void set_far_plane  (value_type _far_plane);
    void set_planes     (value_type _near_plane, value_type _far_plane);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void reset          (const vec_type&  _origin     = vec_type(0),
                         const vec_type&  _direction  = vec_type(0),
                         value_type       _width      = value_type(-1),
                         value_type       _height     = value_type(-1),
                         value_type       _near_plane = value_type(-1),
                         value_type       _far_plane  = value_type(-1));
               
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty() const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    value_type volume () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ����� / ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template< template<> class Arg1<T> >
    frustum& operator += (const Arg1&);
    
    template< template<> class Arg1<T> >
    frustum  operator +  (const Arg1&)            const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    frustum& operator *= (const math::quat<T>&);
    frustum  operator *  (const math::quat<T>&)   const;
    
    axis_aligned_box<T> operator * (const math::matrix<T, 4>&) const;
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool    operator ==  (const frustum<T>&)      const;
    bool    operator !=  (const frustum<T>&)      const;
    
  private:
    vec_type    frustum_origin;
    vec_type    frustum_direction;
    value_type  frustum_width, frustum_height;
    value_type  frustum_near_plane, frustum_far_plane;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef frustum<int>    frustumi;
typedef frustum<float>  frustumf;
typedef frustum<double> frustumd;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, template<> class Arg1<T> >
axis_aligned_box<T> operator * (const math::matrix<T, 4>&, const frustum<T>&);

template <class T>
frustum<T>          operator * (const math::quat<T>&, const frustum<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template < class T, template<> class Arg1<T>, template<> class Arg2<T> >
bool intersects (const Arg1&, const Arg2&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������: �������� �� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template < class T, template<> class Arg1<T>, template<> class Arg2<T> >
bool contains (const Arg1&, const Arg2&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template < class T, template<> class Arg1<T> >
bool equal (const Arg1&, const Arg1&, const T& eps);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <bv/detail/frustum.inl>

}

#endif