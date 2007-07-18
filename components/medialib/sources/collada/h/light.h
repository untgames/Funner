#include <media/collada.h>
#include <stl/string>
#include <math/mathlib.h>

namespace medialib
{

class ColladaLightImpl : public ColladaEntityImpl
{
  public:
    ColladaLightType type;
    math::vec3f      color;
    float            constant_attenuation, linear_attenuation, quadratic_attenuation, falloff_angle, falloff_exponent;
};

}
