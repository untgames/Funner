#ifndef MEDIALIB_PHYSICS_SHAPE_HEADER
#define MEDIALIB_PHYSICS_SHAPE_HEADER

#include <math/vector.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ShapeType
{
  ShapeType_Box,
  ShapeType_Sphere,
  ShapeType_Capsule,
  ShapeType_Plane,
  ShapeType_TriangleMesh,
  ShapeType_Compound
};

namespace shapes
{

class TriangleMesh;
class Compound;

}

template <class T> struct ShapeDataType     { typedef T& Type; };
template <class T> struct ShapeDataType<T*> { typedef T* Type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����� �������������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct ShapeTypeId;

template <> struct ShapeTypeId<shapes::Box>           { enum { Type = ShapeType_Box }; };
template <> struct ShapeTypeId<shapes::Sphere>        { enum { Type = ShapeType_Sphere }; };
template <> struct ShapeTypeId<shapes::Capsule>       { enum { Type = ShapeType_Capsule }; };
template <> struct ShapeTypeId<shapes::Plane>         { enum { Type = ShapeType_Plane }; };
template <> struct ShapeTypeId<shapes::TriangleMesh>  { enum { Type = ShapeType_TriangleMesh }; };
template <> struct ShapeTypeId<shapes::Compound>      { enum { Type = ShapeType_Compound }; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shape
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape  ();
    Shape  (const Shape&);
    ~Shape ();

    Shape& operator = (const Shape&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Margin    () const;
    void  SetMargin (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShapeType Type () const;

    template <class T> const typename ShapeDataType<T>::Type Data    () const;
    template <class T>       typename ShapeDataType<T>::Type Data    ();
    template <class T>       void                            SetData (const T& data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Shape&);

  private:
    struct Impl;
    struct IShapeData;

    template <class T> struct ShapeData;

    Shape (Impl* impl);
    
    void        SetDataCore (IShapeData*);
    IShapeData* DataCore    ();
    
    static void RaiseWrongType (ShapeType source_type, ShapeType required_type);

  private:    
    Impl* impl;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Shape&, Shape&);

#include <media/physics/detail/shape.inl>

}

}

#endif
