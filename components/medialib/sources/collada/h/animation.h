#include <stl/string>
#include <stl/vector>

namespace medialib
{

class ColladaAnimationChannelImpl : public ColladaEntityImpl
{
  public:
    stl::string           target;
    ColladaAnimationParam param;
    stl::vector <float>   input, output, in_tangent, out_tangent;
};

class ColladaAnimationImpl : public ColladaEntityImpl
{
  public:
    stl::vector <ColladaAnimationChannelImpl> anim_channels;
    stl::vector <ColladaAnimationImpl>        child_animations;
};

class ColladaAnimationClipImpl : public ColladaEntityImpl
{
  public:
    float                     start, end;
    stl::vector <stl::string> instances_anim;
};

}
