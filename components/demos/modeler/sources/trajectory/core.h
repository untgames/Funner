#ifndef MODELER_CORE_HEADER
#define MODELER_CORE_HEADER

#include <math.h>

#include <vector>

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
    Загрузка модели из файла
*/

void LoadModelData (const char* file_name, ModelData& model_data);

/*
    Простые структуры данных
*/

template <class T> struct Vec3
{
  T x, y, z;
};

template <class T> struct Color4
{
  T r, g, b, a;
};

typedef Vec3<float>    Vec3f;
typedef Color4<float>  Color4f;

/*
    Визуализируемая точка поверхности
*/

struct DrawVertex
{
  Vec3f   position; //положение точки в пространстве
  Color4f color;    //цвет точки
};

/*
    Визуализируемый примитив
*/

enum PrimitiveType
{
  PrimitiveType_LineList
};

struct DrawPrimitive
{
  PrimitiveType type;  //тип примитива
  size_t        first; //номер первого элемента (вершины / индексов)
  size_t        count; //количество элементов (вершин / индексов)
};


/*
    Построение траектории
*/

typedef std::vector<DrawVertex>    DrawVertexArray;
typedef std::vector<DrawPrimitive> DrawPrimitiveArray;

void BuildTrajectory (const ModelData& model_data, double nu1, double nu2, double nu3, size_t vertices_count, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives);

#endif
