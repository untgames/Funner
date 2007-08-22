#ifndef MEDIALIB_GEOMETRY_VERTEX_HEADER
#define MEDIALIB_GEOMETRY_VERTEX_HEADER

#include <math/mathlib.h>
#include <media/geometry/vertex_format.h>

namespace xtl
{

namespace mpl
{

//implementation forwards
template <class Head, class Tail> struct type_node;
struct null_type;

}

}

namespace medialib
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Положение в пространстве
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, size_t Size=3> struct Position; //положение

template <class T> struct Position<T, 2> { math::vec<T, 2> position; };
template <class T> struct Position<T, 3> { math::vec<T, 3> position; };
template <class T> struct Position<T, 4> { math::vec<T, 4> position; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Нормали и касательные
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct Normal   { math::vec<T, 3> normal;   }; //нормаль
template <class T> struct Tangent  { math::vec<T, 3> tangent;  }; //касательная
template <class T> struct Binormal { math::vec<T, 3> binormal; }; //бинормаль

///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, size_t Size=4> struct Color; //цвет

template <class T> struct Color<T, 3> { math::vec<T, 3> color; };
template <class T> struct Color<T, 4> { math::vec<T, 4> color; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстурные координаты
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t Index, class T, size_t Size=2> struct TexCoord;

template <class T> struct TexCoord<0, T, 2> { math::vec<T, 2> texcoord0; };
template <class T> struct TexCoord<0, T, 3> { math::vec<T, 3> texcoord0; };
template <class T> struct TexCoord<1, T, 2> { math::vec<T, 2> texcoord1; };
template <class T> struct TexCoord<1, T, 3> { math::vec<T, 3> texcoord1; };
template <class T> struct TexCoord<2, T, 2> { math::vec<T, 2> texcoord2; };
template <class T> struct TexCoord<2, T, 3> { math::vec<T, 3> texcoord2; };
template <class T> struct TexCoord<3, T, 2> { math::vec<T, 2> texcoord3; };
template <class T> struct TexCoord<3, T, 3> { math::vec<T, 3> texcoord3; };
template <class T> struct TexCoord<4, T, 2> { math::vec<T, 2> texcoord4; };
template <class T> struct TexCoord<4, T, 3> { math::vec<T, 3> texcoord4; };
template <class T> struct TexCoord<5, T, 2> { math::vec<T, 2> texcoord5; };
template <class T> struct TexCoord<5, T, 3> { math::vec<T, 3> texcoord5; };
template <class T> struct TexCoord<6, T, 2> { math::vec<T, 2> texcoord6; };
template <class T> struct TexCoord<6, T, 3> { math::vec<T, 3> texcoord6; };
template <class T> struct TexCoord<7, T, 2> { math::vec<T, 2> texcoord7; };
template <class T> struct TexCoord<7, T, 3> { math::vec<T, 3> texcoord7; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пустой атрибут
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EmptyAttribute {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершина
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0=EmptyAttribute, class T1=EmptyAttribute, class T2=EmptyAttribute, class T3=EmptyAttribute,
          class T4=EmptyAttribute, class T5=EmptyAttribute, class T6=EmptyAttribute, class T7=EmptyAttribute>
struct Vertex;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Псевдонимы вершинных атрибутов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef Position<unsigned char, 4> Position4ub;
typedef Position<short, 2>         Position2s;
typedef Position<short, 3>         Position3s;
typedef Position<short, 4>         Position4s;
typedef Position<float, 2>         Position2f;
typedef Position<float, 3>         Position3f;
typedef Position<float, 4>         Position4f;
typedef Normal<short>              Normals;
typedef Normal<float>              Normalf;
typedef Tangent<short>             Tangents;
typedef Tangent<float>             Tangentf;
typedef Binormal<short>            Binormals;
typedef Binormal<float>            Binormalf;
typedef Color<unsigned char, 4>    Color4ub;
typedef Color<short, 2>            Color2i;
typedef Color<short, 3>            Color3i;
typedef Color<short, 4>            Color4i;
typedef Color<float, 2>            Color2f;
typedef Color<float, 3>            Color3f;
typedef Color<float, 4>            Color4f;

template <size_t Index> struct TexChannel
{
  typedef TexCoord<Index, short, 2> Coord2s;
  typedef TexCoord<Index, short, 3> Coord3s;
  typedef TexCoord<Index, float, 2> Coord2f;
  typedef TexCoord<Index, float, 3> Coord3f;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение формата вершины
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
void get_format (const Vertex<T0, T1, T2, T3, T4, T5, T6, T7>*, VertexFormat&);

#include <media/geometry/detail/vertex.inl>

}

}

#endif
