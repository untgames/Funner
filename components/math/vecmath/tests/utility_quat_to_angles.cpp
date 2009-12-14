#include "shared.h"

int main ()
{
  printf ("Results of utility_quat_to_angles_test:\n");
  
  quatf q = to_quat (degree (90.0f), vec3f (0, 0, 1));
  
  dump ("q", q);
  
  vec3f  axis;
  anglef axis_angle;
  
  to_axis_angle (q, axis_angle, axis);
  
  printf ("axis angle: ");
  dump   (axis_angle);
  printf (" ");
  dump   (axis);
  printf ("\n");
  
  anglef pitch, yaw, roll;
  
  to_euler_angles (q, pitch, yaw, roll);
  
  printf ("euler angles: ");
  dump   (pitch);
  printf (" ");
  dump   (yaw);
  printf (" ");
  dump   (roll);
  printf ("\n");

  return 0;
}
