template<class T>
plane<T>::plane()
{
  control_point=point<T>(0,0,0);
  normal_vector=vec<T,3>(0,0,0);

}

template<class T>
plane<T>::plane(const vec<T,3>& normal, const point<T>& control)
{
  normal_vector=normal;
  control_point=control;
}

template<class T>
plane<T>::plane(const point<T>& p1, const point<T>& p2, const point<T>& p3)
{
  normal_vector=create_vector(p1,p2)^create_vector(p2,p3);
  control_point=p1;
}

template<class T>
plane<T>::plane(const vec<T,3>& v1, const vec<T,3>& v2, const point<T> p)
{
  normal_vector=v1^v2;
  control_point=p;
}

template<class T>
plane<T>::plane(const point<T>& p1, const point<T>& p2,const vec<T,3> v)
{
  normal_vector=create_vector(p1,p2)^v;
  control_point=p1;
}

template<class T>
plane<T>::plane(const plane<T>& p)
{
  normal_vector=p.get_vector();
  control_point=p.get_point();
}
