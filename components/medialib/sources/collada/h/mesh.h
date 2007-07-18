#include <stl/vector>
#include <stl/string>
#include <math/mathlib.h>
#include <media/collada.h>

using namespace stl;
using namespace math;
using namespace medialib;

////////////////////////////////////////////////////////////////////////////////////////
///Данные текстурного канала
////////////////////////////////////////////////////////////////////////////////////////
struct TexData
{
  vector <ColladaTexVertex> tex_vertices;  //массив текстурных данных
  string                    channel_name;  //имя данного текстурного канала
};

namespace medialib
{

////////////////////////////////////////////////////////////////////////////////////////
///Внутренняя структура поверхности
////////////////////////////////////////////////////////////////////////////////////////
class ColladaSurfaceImpl : public ColladaEntityImpl
{
  public:
    vector <ColladaVertex> vertices;      //массив вершин
    vector <TexData>       tex_data;      //массив текстурных данных
    vector <vec4f>         colors;        //цвета вершин, могут отутствовать
    vector <size_t>        indexes;       //массив индексов для построения
    ColladaPrimitiveType   type;          //тип примитивов, составляющих поверхность
    string                 material;      //имя материала
};

////////////////////////////////////////////////////////////////////////////////////////
///Внутренняя структура меша
////////////////////////////////////////////////////////////////////////////////////////
class ColladaMeshImpl : public ColladaEntityImpl
{
  public:
    vector <ColladaSurfaceImpl> surfaces;  //массив дочерних поверхностей
};

}
