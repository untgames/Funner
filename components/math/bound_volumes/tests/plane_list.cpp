#include "shared.h"

int main()
{
  printf ("Results of plane_list_test:\n");

  plane_listf p1, p2, p3;
  printf ("plane_list created by default constructor\n");
  dump (p1);

  printf ("capacity: %u\n", p1.capacity ());
  p1.reserve (15);
  printf ("capacity: %u\n", p1.capacity ());

  printf ("add 1 - %u\n", p1.add (planef (1.f, 2.f, 3.f, 4.f)));

  planef planes [3] = { planef (2.f, 3.f, 4.f, 5.f), planef (3.f, 4.f, 5.f, 6.f), planef (4.f, 5.f, 6.f, 7.f) };

  printf ("add 2 - %u\n", p1.add (sizeof (planes) / sizeof (planes [0]), planes));

  p2 += planes [1];
  p1 += p2;

  dump (p1);

  printf ("removing 2 items\n");

  p1.remove (1, 2);

  dump (p1);

  printf ("removing 1 item\n");

  p1.remove (0);

  dump (p1);

  printf ("clear\n");

  p1.clear ();

  dump (p1);

  p1.add (planef (vec3f (1.f, 0.f, 0.f), 1.f));
  p1.add (planef (vec3f (-1.f, 0.f, 0.f), 1.f));

  printf ("intersects with box1: %s\n", intersects (p1, aaboxf (1.5f, 2.f)) ? "true" : "false");
  printf ("swap intersects with box1: %s\n", intersects (aaboxf (1.5f, 2.f), p1) ? "true" : "false");
  printf ("intersects with box2: %s\n", intersects (p1, aaboxf (0.5f, 1.5f)) ? "true" : "false");
  printf ("intersects with box3: %s\n", intersects (p1, aaboxf (1.5f, 2.f), 0.6f) ? "true" : "false");
  printf ("intersects with empty box: %s\n", intersects (p1, aaboxf ()) ? "true" : "false");
  printf ("intersects with sphere1: %s\n", intersects (p1, spheref (vec3f (2.f, 0.f, 0.f), 0.5f)) ? "true" : "false");
  printf ("swap intersects with sphere1: %s\n", intersects (spheref (vec3f (2.f, 0.f, 0.f), 0.5f), p1) ? "true" : "false");
  printf ("intersects with sphere2: %s\n", intersects (p1, spheref (vec3f (2.f, 0.f, 0.f), 1.5f)) ? "true" : "false");
  printf ("intersects with sphere3: %s\n", intersects (p1, spheref (vec3f (2.f, 0.f, 0.f), 0.5f), 0.6f) ? "true" : "false");
  printf ("intersects with empty sphere: %s\n", intersects (p1, aaboxf ()) ? "true" : "false");
  printf ("contains box1: %s\n", contains (p1, aaboxf (0.5f, 1.5f)) ? "true" : "false");
  printf ("contains box2: %s\n", contains (p1, aaboxf (-0.5f, 0.5f)) ? "true" : "false");
  printf ("contains box3: %s\n", contains (p1, aaboxf (0.5f, 1.5f), 0.6f) ? "true" : "false");
  printf ("contains empty box: %s\n", contains (p1, aaboxf ()) ? "true" : "false");
  printf ("contains sphere1: %s\n", contains (p1, spheref (vec3f (2.f, 0.f, 0.f), 1.5f)) ? "true" : "false");
  printf ("contains sphere2: %s\n", contains (p1, spheref (vec3f (0.1f, 0.f, 0.f), 0.5f)) ? "true" : "false");
  printf ("contains sphere3: %s\n", contains (p1, spheref (vec3f (2.f, 0.f, 0.f), 1.5f), 2.6f) ? "true" : "false");
  printf ("contains empty sphere: %s\n", contains (p1, spheref ()) ? "true" : "false");

  p1.clear ();
  p2.clear ();
  p3.clear ();

  p1.add (planef (1.f, 2.f, 3.f, 4.f));
  p2.add (planef (2.f, 2.f, 3.f, 4.f));
  p3.add (planef (1.1f, 2.f, 3.f, 4.f));

  printf ("p1 equal p3: %s\n", equal (p1, p3, 0.15f) ? "true" : "false");
  printf ("p1 equal p3: %s\n", equal (p1, p3, 0.05f) ? "true" : "false");
  printf ("p1 == p2: %s\n", p1 == p2 ? "true" : "false");
  printf ("p1 != p2: %s\n", p1 != p2 ? "true" : "false");
  printf ("p1 == p1: %s\n", p1 == p1 ? "true" : "false");
  printf ("p1 != p1: %s\n", p1 != p1 ? "true" : "false");

  return 0;
}
