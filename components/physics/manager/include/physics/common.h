#ifndef PHYSICS_MANAGER_COMMON_HEADER
#define PHYSICS_MANAGER_COMMON_HEADER

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
  math::vec3f position;
  math::quatf orientation;
  
  Transform ();
  Transform (const math::vec3f& position, const math::quatf& orientation = math::quatf ());
  Transform (const math::quatf& orientation);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Типы событий столкновений
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CollisionEventType
{
  CollisionEventType_Begin,    //начало столкновения
  CollisionEventType_Process,  //в процессе столкновения
  CollisionEventType_End,      //конец столкновения

  CollisionEventType_Num
};

}

#endif
