#ifndef MODELER_CORE_HEADER
#define MODELER_CORE_HEADER

#include <cstdio>
#include <cstring>
#include <cmath>

#include <vector>

namespace modeler
{

/*
    Параметры системы
*/

struct ModelData
{
  double  A, B, C;     //момент инерции A,B,C
  double  h, g;        //постоянные интегрирования h,g
  double  mx,my,mz;    //координаты центра тяжести
  int     ini;         //???
  double  len, dm;     //длина интервала интегрирования и масштаб
};

/*
    Простые структуры данных
*/

template <class T> struct Vec3
{
  T x, y, z;
};

typedef Vec3<double> Vec3d;
typedef Vec3<float>  Vec3f;

/*
    Визуализируемая точка поверхности
*/

struct DrawVertex
{
  Vec3f position; //положение точки в пространстве
  Vec3f normal;   //нормаль к поверхности в точке
};

/*
    Визуализируемый примитив
*/

enum PrimitiveType
{
  PrimitiveType_TriangleStrip
};

struct DrawPrimitive
{
  PrimitiveType type;  //тип примитива
  size_t        first; //номер первого элемента (вершины / индексов)
  size_t        count; //количество элементов (вершин / индексов)
};

/*
    Загрузка модели из файла
*/

void LoadModelData (const char* file_name, ModelData& model_data);

/*
    Построение мега-поверхности
*/

typedef std::vector<DrawVertex>    DrawVertexArray;
typedef std::vector<DrawPrimitive> DrawPrimitiveArray;

void BuildMegaSurface (const ModelData& model_data, int uanz, int vanz, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives);

}

#endif
