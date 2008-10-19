#include "shared.h"

int main()
{
  printf ("Results of sphere_test:\n");

  spheref s1, s2 (vec3f (10, 20, 30), 40);
  printf ("sphere created by default constructor\n");
  dump (s1);
  printf ("\n");

  printf ("sphere created by vector and radius constructor\n");
  dump   (s2);
  printf ("\n");

  printf ("volume: %.1f\n", volume (s2));

  printf        ("set_radius\n");
  s2.set_radius (5);
  dump          (s2);
  printf        ("\n");

  printf        ("set_center\n");
  s2.set_center (-10, -20, -30);
  dump          (s2);
  printf        ("\n");

  printf   ("reset\n");
  s2.reset ();
  dump     (s2);
  printf   ("\n");

  printf ("empty: %s\n", s2.empty () ? "true" : "false");

  printf ("add first point\n");

  s1 += vec3f (-1, -2, 0);

  dump (s1);
  printf ("\n");

  printf ("add second point\n");

  s1 = s1 + vec3f (1, -2, 0);

  dump (s1);
  printf ("\n");

  printf ("copy\n");

  s2 = s1;

  dump (s2);
  printf ("\n");

  printf ("rotate\n");

  s1 = s1 * fromAxisAnglef (deg2rad (90.0f), 0, 1, 0);

  dump (s1);
  printf ("\n");

  printf ("add sphere\n");

  s1 = s2 + s1 * fromAxisAnglef (deg2rad (90.0f), 0, 1, 0);

  dump (s1);
  printf ("\n");

  printf ("scale\n");

  aaboxf box = scalef (2, 2, 2) * s1;

  dump (box);
  printf ("\n");

  printf ("add box\n");

  s1 = s1 + box;

  dump (s1);
  printf ("\n");

  printf ("intersects with sphere1: %s\n", intersects (s1, s2) ? "true" : "false");
  printf ("intersects with sphere2: %s\n", intersects (s1, spheref (vec3f (15.0f), 1)) ? "true" : "false");
  printf ("intersects with empty sphere: %s\n", intersects (s1, spheref ()) ? "true" : "false");
  printf ("intersects with box1: %s\n", intersects (s1, box) ? "true" : "false");
  printf ("intersects with box2: %s\n", intersects (s1, aaboxf (9.0f, 10.0f)) ? "true" : "false");
  printf ("intersects with empty box: %s\n", intersects (s1, aaboxf ()) ? "true" : "false");
  printf ("contains sphere1: %s\n", contains (s1, s2) ? "true" : "false");
  printf ("contains sphere2: %s\n", contains (s1, spheref (vec3f (15.0f), 1)) ? "true" : "false");
  printf ("contains empty sphere: %s\n", contains (s1, spheref ()) ? "true" : "false");
  printf ("contains box1: %s\n", contains (s1, box) ? "true" : "false");
  printf ("contains box2: %s\n", contains (s1, aaboxf (9.0f, 10.0f)) ? "true" : "false");
  printf ("contains empty box: %s\n", contains (s1, aaboxf ()) ? "true" : "false");

//  printf ("small sphere changes\n");

  spheref s3 = s1 * fromAxisAnglef (deg2rad (1.0f), 0, 1, 0);

//  dump (s3);
//  printf ("\n");

  printf ("s1 equal s3: %s\n", equal (s1, s3, 0.1f) ? "true" : "false");
  printf ("s1 == s2: %s\n", s1 == s2 ? "true" : "false");
  printf ("s1 != s2: %s\n", s1 != s2 ? "true" : "false");
  printf ("s1 == s1: %s\n", s1 == s1 ? "true" : "false");
  printf ("s1 != s1: %s\n", s1 != s1 ? "true" : "false");

  return 0;
}
