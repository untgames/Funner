#include "shared.h"

int main ()
{
  printf ("Results of utility_to_quat_test:\n");
  
  mat4f m = rotate (degree (90.0f), vec3f (0, 0, 1));
  
  dump ("to_quat(m)", to_quat (m));
  dump ("to_quat(angle, axis)", to_quat (degree (90.0f), vec3f (0, 0, 1)));
  dump ("to_quat(pitch, yaw, roll)", to_quat (degree (0.0f), degree (0.0f), degree (90.0f)));

  return 0;
}
