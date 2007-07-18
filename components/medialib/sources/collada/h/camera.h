#include <stl/string>
#include <media/collada.h>

namespace medialib
{

class ColladaCameraImpl : public ColladaEntityImpl
{
  public:
    ColladaCameraType type;
    float             x, y, aspect_ratio, znear, zfar;  // для перспективы x и y - xfov yfov, а для ортографии - xmag ymag
};

}
