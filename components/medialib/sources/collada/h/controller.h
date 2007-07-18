#include <media/collada.h>
#include <stl/string>
#include <stl/vector>
#include <math/mathlib.h>

namespace medialib
{

class ColladaMorphImpl : public ColladaEntityImpl
{
  public:
    ColladaMorphMethod        method;
    stl::string               base_mesh;
    stl::vector <stl::string> targets;
    stl::vector <float>       weights;
};

class ColladaSkinImpl : public ColladaEntityImpl
{
  public:
    stl::string                          base_mesh;
    stl::vector <stl::string>            joints;
    math::mat4f                          bind_shape_matrix;
    stl::vector <math::mat4f>            inv_bind_matrices;
    stl::vector <ColladaVertexInfluence> vertex_influences;
    stl::vector <ColladaJointWeight>     joint_weights;
};

class ColladaControllerImpl : public ColladaEntityImpl
{
  public:
    ColladaControllerType type;
    ColladaSkinImpl       skin;
    ColladaMorphImpl      morph;
};

}
