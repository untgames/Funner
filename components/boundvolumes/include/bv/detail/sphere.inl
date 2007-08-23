///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class sphere::sphere
{
   sphere_center=vec_type(0,0,0);
   sphere_radius=0;
}

template <class T> class sphere::sphere (const vec_type& center, const element_type& radius)
{
   sphere_center=center;
   sphere_radius=radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
const vec_type& template <class T> class sphere::center () const
{
   return center;
}
element_type    template <class T> class sphere::radius () const;
{
   return radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
void template <class T> class sphere::set_radius  (const element_type& r)
{
   radius=r;
}
void template <class T> class sphere::set_center  (const vec_type& c)
{
   centre=c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
void template <class T> class sphere::reset (const vec_type& center = vec_type (0), const element_type& radius = element_type (0))
{
   sphere_center=center;
   sphere_radius=radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение статистик ограничивающей сферы
///////////////////////////////////////////////////////////////////////////////////////////////////
element_type template <class T> class sphere::volume () const //геометрический объём сферы
{
   return 4*3.14*sphere_radius*sphere_radius*sphere_radius/3; //както не красиво
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
bool template <class T> class sphere::empty (const element_type& eps = default_epsilon) //проверка: r < eps
{
   return sphere_radius<eps?true:false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление примитивов в ограничивающий объём
///  Если текущий радиус < 0 - сбрасываем положение сферы
///////////////////////////////////////////////////////////////////////////////////////////////////
sphere& template <class T> class sphere::operator += (const vec_type& point) const
{
   vec_type vec;
   if(sphere_radius==0)
   {
      sphere_center=vec;
      return *this;
   }
   if(contains(point))
      return *this;
   vec=(sphere_center+point)/2;
   sphere_center=vec+normalize(vec)*(sphere_radius/2);
   return *this;
}

sphere& template <class T> class sphere::operator += (const sphere& sph) const
{
   vec_type vec;
   if(sphere_radius==0)
   {
      sphere_radius=sph.radius();
      sphere_center=sph.centre();
      return *this;
   }
   if(contains(sph))
      return *this;
   vec=(sphere_center+sph.center())/2;
   sphere_center=vec+normalize(vec)*((sphere_radius+sph.radius)/2);
   return *this;
}

//sphere& template <class T> class sphere::operator += (const axis_aligned_box<T>&); //???
sphere  template <class T> class sphere::operator +  (const vec_type& vec) const
{
   return shere(*this)+=vec;
}
sphere  template <class T> class sphere::operator +  (const sphere& sph) const
{
   return shere(*this)+=sph;
}
//sphere  template <class T> class sphere::operator +  (const axis_aligned_box&) const; //???

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка пересечения ограничивающей сферы с различными примитивами
///////////////////////////////////////////////////////////////////////////////////////////////////
bool template <class T> class sphere::intersects (const sphere&)
{
   return sphere_center.length(sphere.center())<sphere_radius+sphere.radius()?true:false;
}
//bool template <class T> class sphere::intersects (const axis_aligned_box<T>&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: содержит ли ограничивающая сфера различные примитивы
///////////////////////////////////////////////////////////////////////////////////////////////////
bool template <class T> class sphere::contains (const vec_type& point)
{
   return point.length(sphere_center)<sphere_radius?true:false;
}
bool template <class T> class sphere::contains (const sphere& sphere)
{
   return sphere_center.length(sphere.center())<sphere_radius-2*sphere.radius()?true:false;   //вроде умножение не нужно
}
//bool template <class T> class sphere::contains (const axis_aligned_box<T>& box) const;





///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
bool template <class T> class sphere::equal (const sphere& sp, const element_type& eps = default_epsilon)
{
   return ((abs(sphere_radius-sp.radius())<eps)&&(center==sp.center()))?true:false
}

bool template <class T> class sphere::operator == (const sphere& sp)
{
   return ((abs(sphere_radius-sp.radius())<eps)&&(center==sp.center()))?true:false  //повторяющийся код, лучше через equal
}

bool template <class T> class sphere::operator != (const sphere& sp)
{
   return ((abs(sphere_radius-sp.radius())<eps)&&(center==sp.center()))?false:true  //повторяющийся код, лучше через оператор ==
}
