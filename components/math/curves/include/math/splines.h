#ifndef MATH_CURVES_SPLINES_HEADER
#define MATH_CURVES_SPLINES_HEADER

#ifdef _MSC_VER
  #pragma push_macros(MATH_CURVES_SPLINE_NO_DETAILS)
#endif

#define  MATH_CURVES_SPLINE_NO_DETAILS
#include <math/basic_spline.h>
#undef   MATH_CURVES_SPLINE_NO_DETAILS

#ifdef _MSC_VER
  #pragma pop_macros
#endif

namespace math
{

template splinef;
template spline2f;
template spline3f;
template spline4f;
template rotation_splinef;

}

#endif
