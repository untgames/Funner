#ifndef MEDIALIB_PHYSICS_SHAPE_HEADER
#define MEDIALIB_PHYSICS_SHAPE_HEADER

#include <math/vector.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип геометрического тела
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

struct Box;
struct Sphere;
struct Capsule;
struct Plane;
class  TriangleMesh;
class  Compound;

}

template <class T> struct ShapeDataType     { typedef T& Type; };
template <class T> struct ShapeDataType<T*> { typedef T* Type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сопоставление типов геометрических тел
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct ShapeTypeId;

template <> struct ShapeTypeId<shapes::Box>           { enum { Type = ShapeType_Box }; };
template <> struct ShapeTypeId<shapes::Sphere>        { enum { Type = ShapeType_Sphere }; };
template <> struct ShapeTypeId<shapes::Capsule>       { enum { Type = ShapeType_Capsule }; };
template <> struct ShapeTypeId<shapes::Plane>         { enum { Type = ShapeType_Plane }; };
template <> struct ShapeTypeId<shapes::TriangleMesh>  { enum { Type = ShapeType_TriangleMesh }; };
template <> struct ShapeTypeId<shapes::Compound>      { enum { Type = ShapeType_Compound }; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Геометрическое тело
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shape
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape  ();
    Shape  (const Shape&);
    ~Shape ();

    Shape& operator = (const Shape&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание копии
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя анимации
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Толщина полей
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Margin    () const;
    void  SetMargin (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение описания
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShapeType Type () const;

    template <class T> const typename ShapeDataType<T>::Type Data    () const;
    template <class T>       typename ShapeDataType<T>::Type Data    ();
    template <class T>       void                            SetData (const T& data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Shape&);

  private:
    struct Impl;
    struct IShapeData;

    template <class T> struct ShapeData;

    Shape (Impl* impl);
    
    void        SetDataCore (IShapeData*);
    IShapeData* DataCore    ();
    
    static void RaiseNullArgument (const char* source, const char* argument_name);
    static void RaiseWrongType (ShapeType source_type, ShapeType required_type);

  private:    
    Impl* impl;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Shape&, Shape&);

#include <media/physics/detail/shape.inl>

}

}

#endif
