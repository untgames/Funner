/*
   Преобразования объектов
*/
  
inline Transform::Transform ()
  {}

inline Transform::Transform (const math::vec3f& in_position, const math::quatf& in_orientation)
  : position (in_position)
  , orientation (in_orientation)
  {}

inline Transform::Transform (const math::quatf& in_orientation)
  : orientation (in_orientation)
  {}
