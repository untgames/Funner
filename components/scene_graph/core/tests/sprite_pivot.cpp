#include "shared.h"

int main ()
{
  printf ("Results of sprite_pivot_test:\n");

  Sprite::Pointer model = Sprite::Create ();

  model->SetPivotPosition (1, 2, 3);
  model->SetPivotRotation (degree (90.f));
  
  mat4f tm;

  model->EvalWorldTMAfterPivot (tm);

  model->SetPosition (5, 6, 7);

  model->EvalWorldTMAfterPivot (tm);

  vec3f position = tm * vec3f (0),
        axis_x   = tm * vec4f (1, 0, 0, 0),
        axis_y   = tm * vec4f (0, 1, 0, 0),
        axis_z   = tm * vec4f (0, 0, 1, 0);

  printf ("position: [%.2f %.2f %.2f]\n", position.x, position.y, position.z);
  printf ("axis_x:   [%.2f %.2f %.2f]\n", axis_x.x, axis_x.y, axis_x.z);
  printf ("axis_y:   [%.2f %.2f %.2f]\n", axis_y.x, axis_y.y, axis_y.z);
  printf ("axis_z:   [%.2f %.2f %.2f]\n", axis_z.x, axis_z.y, axis_z.z);

  return 0;
}
