#include "shared.h"

int main()
{
  printf ("Results of ortho_frustum_test:\n");

  float right  = 10,
        left   = -10,
        top    = 10,
        bottom = -10,
        z_near = -10,
        z_far  = 10,
        width  = right - left,
        height = top - bottom,
        depth  = z_far - z_near;

  math::mat4f proj_matrix;

  //выбрана матрица проецирования, используемая gluOrtho2D
  proj_matrix [0] = vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = vec4f (0, 0, 2.0f / depth, - (z_near + z_far) / depth);
  proj_matrix [3] = vec4f (0, 0, 0, 1);

  plane_listf p1 = make_frustum (proj_matrix);

  printf ("Frustum planes:\n");
  dump (p1);

  printf ("intersects with box1: %s\n", intersects (p1, aaboxf (15.f, 20.f)) ? "true" : "false");
  printf ("intersects with box2: %s\n", intersects (p1, aaboxf (5.f, 15.f)) ? "true" : "false");
  printf ("intersects with sphere1: %s\n", intersects (p1, spheref (vec3f (20.f, 0.f, 0.f), 5.f)) ? "true" : "false");
  printf ("intersects with sphere2: %s\n", intersects (p1, spheref (vec3f (20.f, 0.f, 0.f), 15.f)) ? "true" : "false");
  printf ("contains box1: %s\n", contains (p1, aaboxf (5.f, 15.f)) ? "true" : "false");
  printf ("contains box2: %s\n", contains (p1, aaboxf (-5.f, 5.f)) ? "true" : "false");
  printf ("contains sphere1: %s\n", contains (p1, spheref (vec3f (20.f, 0.f, 0.f), 15.f)) ? "true" : "false");
  printf ("contains sphere2: %s\n", contains (p1, spheref (vec3f (1.f, 0.f, 0.f), 5.f)) ? "true" : "false");

  return 0;
}
