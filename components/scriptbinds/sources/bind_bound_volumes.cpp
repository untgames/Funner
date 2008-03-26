#include "shared.h"

#include <xtl/implicit_cast.h>

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

const char* BV_STATIC_AXIS_ALIGNED_BOX_CORNER_LIBRARY = "static.BV_Corner";
const char* BV_AXIS_ALIGNED_BOX_LIBRARY               = "BV.AxisAlignedBox";

/*
    —оздание ограничивающего параллелепипеда
*/


template <class T>
axis_aligned_box<T> create_axis_aligned_box ()
{
  return axis_aligned_box<T> ();
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
}  

template <class T>
void bind_axis_aligned_box_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (BV_AXIS_ALIGNED_BOX_LIBRARY);
  
    //регистраци€ функций создани€

  lib.Register ("Create", make_invoker (&create_axis_aligned_box<T>));

    //регистраци€ статических переменных

  bind_static_axis_aligned_box_library (environment);
    
    //регистраци€ операций

  lib.Register ("set_minimum", make_invoker (implicit_cast<void (axis_aligned_box<T>::*) (const axis_aligned_box<T>::vec_type&)> 
                                            (&axis_aligned_box<T>::set_minimum)));
  lib.Register ("set_maximum", make_invoker (implicit_cast<void (axis_aligned_box<T>::*) (const axis_aligned_box<T>::vec_type&)> 
                                            (&axis_aligned_box<T>::set_maximum)));
  lib.Register ("get_minimum", make_invoker (&axis_aligned_box<T>::minimum));
  lib.Register ("get_maximum", make_invoker (&axis_aligned_box<T>::maximum));

  lib.Register ("get_center", make_invoker (&axis_aligned_box<T>::center));
  lib.Register ("get_size",   make_invoker (&axis_aligned_box<T>::size));
  lib.Register ("get_radius", make_invoker (&axis_aligned_box<T>::radius));
  lib.Register ("get_empty",  make_invoker (&axis_aligned_box<T>::empty));

  lib.Register ("SetExtents", make_invoker (implicit_cast<void (axis_aligned_box<T>::*)
               (const axis_aligned_box<T>::vec_type&, const axis_aligned_box<T>::vec_type&)> (&axis_aligned_box<T>::set_extents)));

  lib.Register ("GetCorner", make_invoker (&axis_aligned_box<T>::corner));

  lib.Register ("Reset", make_invoker (&axis_aligned_box<T>::reset));

  lib.Register ("__add", make_invoker (implicit_cast<axis_aligned_box<T> (axis_aligned_box<T>::*) 
                                      (const axis_aligned_box<T>&) const> (&axis_aligned_box<T>::operator +)));
  lib.Register ("__mul", make_invoker (implicit_cast<axis_aligned_box<T> (axis_aligned_box<T>::*) 
                                      (const matrix<T, 4>&) const> (&axis_aligned_box<T>::operator *)));
//  lib.Register ("__eq",  make_invoker (&axis_aligned_box<T>::operator ==));

  lib.Register ("Volume", make_invoker (&bound_volumes::volume<T>));
  lib.Register ("Equal", make_invoker (&bound_volumes::equal<T>));
  lib.Register ("Intersects", make_invoker (implicit_cast<bool (*) (const axis_aligned_box<T>&, const axis_aligned_box<T>&)> (&bound_volumes::intersects)));
  lib.Register ("Contains", make_invoker (implicit_cast<bool (*) (const axis_aligned_box<T>&, const axis_aligned_box<T>&)> (&bound_volumes::contains)));
  lib.Register ("Intersection", make_invoker (implicit_cast<axis_aligned_box<T> (*) (const axis_aligned_box<T>&, const axis_aligned_box<T>&)> (&bound_volumes::intersection)));

    //регистраци€ типов данных

  environment.RegisterType<axis_aligned_box<T>> (BV_AXIS_ALIGNED_BOX_LIBRARY);
}

}

namespace script
{

/*
    –егистраци€ библиотеки ограничивающих объЄмов
*/

void bind_bv_library (Environment& environment)
{
    //регистраци€ библиотек
  
  bind_axis_aligned_box_library<float> (environment);
}

}
