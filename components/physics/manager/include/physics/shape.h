#ifndef PHYSICS_MANAGER_SHAPE_HEADER
#define PHYSICS_MANAGER_SHAPE_HEADER

namespace physics
{

//implementation forwards
class ShapeImpl;
class ShapeListImpl;

class Shape
{
  public:
    Shape (const Shape&);
    ~Shape ();    
    Shape& operator = (const Shape&);  
          
///////////////////////////////////////////////////////////////////////////////////////////////////
///Толщина полей
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetMargin (float value);
    float Margin    () const;
    
    void Swap (Shape&);

  protected:
    Shape (ShapeImpl*);
    
  private:
    ShapeImpl* impl;
};

void swap (Shape&, Shape&);

class ShapeList;
{
  public:
    size_t Add       (const Shape&, const Transform&);
    size_t Add       (const Shape&, const math::vec3f& position = math::vec3f (), const math::quatf& orientation = math::quatf ());
    size_t Add       (const Shape&, const math::quatf& orientation);
    void   Remove    (size_t index);
    void   Clear ();

    size_t Size () const;
    
          Shape& operator [] (size_t index);
    const Shape& operator [] (size_t index) const;  
    //+shape transform

  private:
    ShapeListImpl* impl;  
};

void swap (ShapeList&, ShapeList&);

}

#endif
