#include <stl/vector>
#include <stl/string>
#include <math/mathlib.h>
#include <media/collada_geometry.h>

using namespace stl;
using namespace math;

////////////////////////////////////////////////////////////////////////////////////////
///Внутренняя структура сплайна
////////////////////////////////////////////////////////////////////////////////////////
class ColladaSplineImpl
{
  public:
    bool                          closed;         //Показывает, существует ли сегмент, соединяющий первую и последнюю контрольные вершины. По умолчанию false (открыт).
    vector <vec4f>                cvpos;          //Позиции контрольных вершин
    vector <ColladaInterpolation> interpolations; //Тип интерполяции
};
