#include <stl/string>
#include <stl/vector>
#include "node.h"

namespace medialib
{

class ColladaVisualSceneImpl : public ColladaEntityImpl
{
  public:
    stl::vector <ColladaNodeImpl> nodes;
};

}