#include <math/quat.h>

#include <math/basic_spline.h>
#include <math/spline.h>

namespace math
{

template class basic_spline<spline_tcb_key<float> >;
template class basic_spline<spline_tcb_key<vector<float, 2> > >;
template class basic_spline<spline_tcb_key<vector<float, 3> > >;
template class basic_spline<spline_tcb_key<vector<float, 4> > >;
//template class basic_spline<spline_tcb_key<quat<float> > >;
template class basic_spline<spline_bezier_key<float> >;
template class basic_spline<spline_bezier_key<vector<float, 2> > >;
template class basic_spline<spline_bezier_key<vector<float, 3> > >;
template class basic_spline<spline_bezier_key<vector<float, 4> > >;
template class basic_spline<spline_step_key<float> >;
template class basic_spline<spline_step_key<vector<float, 2> > >;
template class basic_spline<spline_step_key<vector<float, 3> > >;
template class basic_spline<spline_step_key<vector<float, 4> > >;
template class basic_spline<spline_step_key<matrix<float, 4> > >;
template class basic_spline<spline_linear_key<float> >;
template class basic_spline<spline_linear_key<vector<float, 2> > >;
template class basic_spline<spline_linear_key<vector<float, 3> > >;
template class basic_spline<spline_linear_key<vector<float, 4> > >;
template class basic_spline<spline_linear_key<matrix<float, 4> > >;

}
