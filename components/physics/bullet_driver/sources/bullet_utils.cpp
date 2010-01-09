#include "shared.h"

namespace physics
{

namespace low_level
{

namespace bullet
{

//преобразование векторов
void vector_from_bullet_vector (const btVector3& bullet_vector, math::vec3f& target_vector)
{
  target_vector.x = bullet_vector.x ();
  target_vector.y = bullet_vector.y ();
  target_vector.z = bullet_vector.z ();
}

void bullet_vector_from_vector (const math::vec3f& vector, btVector3& target_vector)
{
  target_vector.setX (vector.x);
  target_vector.setY (vector.y);
  target_vector.setZ (vector.z);
}

//преобразование кватернионов
void quaternion_from_bullet_quaternion (const btQuaternion& bullet_quaternion, math::quatf& target_quaternion)
{
  target_quaternion.x = bullet_quaternion.getX ();
  target_quaternion.y = bullet_quaternion.getY ();
  target_quaternion.z = bullet_quaternion.getZ ();
  target_quaternion.w = bullet_quaternion.getW ();
}

void bullet_quaternion_from_quaternion (const math::quatf& quaternion, btQuaternion& target_quaternion)
{
  target_quaternion.setX (quaternion.x);
  target_quaternion.setY (quaternion.y);
  target_quaternion.setZ (quaternion.z);
  target_quaternion.setW (quaternion.w);
}

}

}

}

