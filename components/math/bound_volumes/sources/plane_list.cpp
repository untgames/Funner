#include <bv/basic_plane_list.h>
#include <bv/plane_list.h>

namespace bound_volumes
{

//classes
template class plane_list<float>;
template class plane_list<double>;

//functions
template void swap (plane_list<float>&, plane_list<float>&);
template bool contains (const plane_list<float>&, const math::vector<float, 3>&, const float& eps);
template bool contains (const plane_list<float>&, const axis_aligned_box<float>&, const float& eps);
template bool contains (const plane_list<float>&, const sphere<float>&, const float& eps);
template bool intersects (const plane_list<float>&, const axis_aligned_box<float>&, const float& eps);
template bool intersects (const plane_list<float>&, const sphere<float>&, const float& eps);
template bool equal (const plane_list<float>&, const plane_list<float>&, const float& eps);
template plane_list<float> make_frustum (const math::matrix<float, 4>& view_projection);

template void swap (plane_list<double>&, plane_list<double>&);
template bool contains (const plane_list<double>&, const math::vector<double, 3>&, const double& eps);
template bool contains (const plane_list<double>&, const axis_aligned_box<double>&, const double& eps);
template bool contains (const plane_list<double>&, const sphere<double>&, const double& eps);
template bool intersects (const plane_list<double>&, const axis_aligned_box<double>&, const double& eps);
template bool intersects (const plane_list<double>&, const sphere<double>&, const double& eps);
template bool equal (const plane_list<double>&, const plane_list<double>&, const double& eps);
template plane_list<double> make_frustum (const math::matrix<double, 4>& view_projection);

}
