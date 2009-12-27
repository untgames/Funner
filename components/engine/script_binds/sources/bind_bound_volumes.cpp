#include "shared.h"

#include <bv/axis_aligned_box.h>

using namespace script;
using namespace math;
using namespace xtl;
using namespace bound_volumes;

namespace
{

/*
     онстанты (имена библиотек)
*/

const char* BV_STATIC_AXIS_ALIGNED_BOX_CORNER_LIBRARY = "BoundVolumes.Corner";
const char* BV_AXIS_ALIGNED_BOX_LIBRARY               = "BoundVolumes.AxisAlignedBox";
const char* COMPONENT_NAME                            = "script.binds.BoundVolumes";
const char* BINDER_NAME                               = "BoundVolumes";

/*
    —оздание ограничивающего параллелепипеда
*/

template <class T>
axis_aligned_box<T> create_axis_aligned_box ()
{
  return axis_aligned_box<T> ();
}

template <class T>
axis_aligned_box<T> create_axis_aligned_box
 (const typename axis_aligned_box<T>::vec_type& vmin,
  const typename axis_aligned_box<T>::vec_type& vmax)  
{
  return axis_aligned_box<T> (vmin, vmax);
}

template <class T>
axis_aligned_box<T> create_axis_aligned_box
 (T min_x,
  T min_y,
  T min_z,
  T max_x,
  T max_y,
  T max_z)
{
  return axis_aligned_box<T> (min_x, min_y, min_z, max_x, max_y, max_z);
}

/*
   –егистраци€ библиотеки работы с ограничивающим параллелепипедом
*/

void bind_static_axis_aligned_box_library (Environment& environment)
{
  InvokerRegistry& aabox_box_corner_lib = environment.CreateLibrary (BV_STATIC_AXIS_ALIGNED_BOX_CORNER_LIBRARY);

  aabox_box_corner_lib.Register ("get_nxnynz", make_const (box_corner_nxnynz));
  aabox_box_corner_lib.Register ("get_pxnynz", make_const (box_corner_pxnynz));
  aabox_box_corner_lib.Register ("get_nxpynz", make_const (box_corner_nxpynz));
  aabox_box_corner_lib.Register ("get_pxpynz", make_const (box_corner_pxpynz));
  aabox_box_corner_lib.Register ("get_nxnypz", make_const (box_corner_nxnypz));
  aabox_box_corner_lib.Register ("get_pxnypz", make_const (box_corner_pxnypz));
  aabox_box_corner_lib.Register ("get_nxpypz", make_const (box_corner_nxpypz));
  aabox_box_corner_lib.Register ("get_pxpypz", make_const (box_corner_pxpypz));
}  

template <class T>
void bind_axis_aligned_box_library (Environment& environment)
{
  typedef axis_aligned_box<T>         box_type;
  typedef typename box_type::vec_type vec_type;  

  InvokerRegistry& lib        = environment.CreateLibrary (BV_AXIS_ALIGNED_BOX_LIBRARY);
  InvokerRegistry& global_lib = environment.Library ("global");
  
    //регистраци€ функций создани€

  lib.Register ("Create", make_invoker (
                            make_invoker ((box_type (*)(T, T, T, T, T, T))&create_axis_aligned_box<T>),
                            make_invoker ((box_type (*)(const vec_type&, const vec_type&))&create_axis_aligned_box<T>),
                            make_invoker ((box_type (*)())&create_axis_aligned_box<T>)
                          ));
  global_lib.Register ("AABB", lib, "Create");

    //регистраци€ статических переменных

  bind_static_axis_aligned_box_library (environment);

    //регистраци€ операций

  lib.Register ("set_Minimum", make_invoker (implicit_cast<void (axis_aligned_box<T>::*) (const typename axis_aligned_box<T>::vec_type&)> 
                                            (&box_type::set_minimum)));
  lib.Register ("set_Maximum", make_invoker (implicit_cast<void (box_type::*) (const typename box_type::vec_type&)> 
                                            (&box_type::set_maximum)));
  lib.Register ("get_Minimum", make_invoker (&box_type::minimum));
  lib.Register ("get_Maximum", make_invoker (&box_type::maximum));

  lib.Register ("get_Center", make_invoker (&box_type::center));
  lib.Register ("get_Size",   make_invoker (&box_type::size));
  lib.Register ("get_Radius", make_invoker (&box_type::radius));
  lib.Register ("get_Empty",  make_invoker (&box_type::empty));

  lib.Register ("SetExtents", make_invoker (implicit_cast<void (box_type::*)
               (const typename box_type::vec_type&, const typename box_type::vec_type&)> (&box_type::set_extents)));

  lib.Register ("Corner", make_invoker (&box_type::corner));

  lib.Register ("Reset", make_invoker (&box_type::reset));

  lib.Register ("__add", make_invoker (implicit_cast<box_type (box_type::*) 
                                      (const box_type&) const> (&box_type::operator +)));
  lib.Register ("__mul", make_invoker (implicit_cast<box_type (box_type::*) 
                                      (const matrix<T, 4>&) const> (&box_type::operator *)));
//  lib.Register ("__eq",  make_invoker (&box_type::operator ==));

  lib.Register ("get_Volume", make_invoker (&bound_volumes::volume<T>));
  lib.Register ("Equal", make_invoker (&bound_volumes::equal<T>));
  lib.Register ("Intersects", make_invoker (implicit_cast<bool (*) (const box_type&, const box_type&)> (&bound_volumes::intersects)));
  lib.Register ("Contains", make_invoker (implicit_cast<bool (*) (const box_type&, const box_type&)> (&bound_volumes::contains)));
  lib.Register ("Intersection", make_invoker (implicit_cast<box_type (*) (const box_type&, const box_type&)> (&bound_volumes::intersection)));

    //регистраци€ типов данных

  environment.RegisterType<box_type> (BV_AXIS_ALIGNED_BOX_LIBRARY);
}

/*
     омпонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_axis_aligned_box_library<float> (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> BoundVolumesScriptBind (COMPONENT_NAME);

}

}
