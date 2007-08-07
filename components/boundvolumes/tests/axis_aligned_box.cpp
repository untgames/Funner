#include "shared.h"

void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

void dump (const aaboxf& box)
{
  printf ("min=");
  dump (box.minimum ());
  printf (" max=");
  dump (box.maximum ());
}

int main ()
{
  printf ("Results of axis_aligned_box_test:\n");  
  
  aaboxf box, box1 (vec3f (-10, -20, -30), vec3f (30, 20, 10)), box2 (1, 2, 3, 4, 5, 6);  
  
  printf ("default constructor\n");  
  dump (box);  
  printf ("\nvector constructor\n");
  dump (box1);
  printf ("\nscalar constructor\n");
  dump (box2);
  printf ("\n");
  
  printf ("set_extents\n");
  box.set_extents (-1, -2, -3, 3, 2, 1);  
  dump (box);  
  printf ("\n");    
  
  printf ("reset\n");
  box.reset ();
  dump (box);  
  printf ("\n");    
  
  printf (box.empty () ? "box is empty\n" : "box is not empty\n");
  
  printf ("set_minimum\n");
  box.set_minimum (-2, -4, -6);
  dump (box);
  printf ("\n");  
  
  printf ("set_maximum\n");
  box.set_maximum (6, 8, 10);
  dump (box);
  printf ("\n");
  
  printf (box.empty () ? "box is empty\n" : "box is not empty\n");

  printf ("statistics\n");
  printf ("  center: ");
  dump (box.center ());
  printf ("\n  size: ");
  dump (box.size ());
  printf ("\n  radius: %.2f\n", box.radius ());
  printf ("  volume: %g\n", box.volume ());
  
  printf ("corners:\n");
  
  static const char* corner_name [] = {
    "(Xmin, Ymin, Zmin)",
    "(Xmax, Ymin, Zmin)",
    "(Xmin, Ymax, Zmin)",
    "(Xmax, Ymax, Zmin)",
    "(Xmin, Ymin, Zmax)",
    "(Xmax, Ymin, Zmax)",
    "(Xmin, Ymax, Zmax)",
    "(Xmax, Ymax, Zmax)",
  };
  
  vec3f corners [8];
  
  box.get_corners (corners);
  
  for (int i=0; i<8; i++)
  {
    vec3f corner = box.corner ((box_corner)i); 
    
    printf ("  %s=", corner_name [i]);
    dump   (corner);
    printf ("\n");
    
    if (corners [i] != corner)
    {
      printf ("  fuctions corner and get_corners has different results at compute corner %s:\n", corner_name [i]);
      printf ("    corner function result: ");
      dump   (corner);
      printf ("\n    get_corners result: ");
      dump   (corners [i]);
      printf ("\n");
    }
  }
  
  vec3f test_point (-20, 30, 0);
  
  printf ("add test point\n");
  
  box += test_point;
  
  dump (box);
  printf ("\n");  
  
  printf ("add box\n");
  
  box += box1;
  
  dump (box);
  printf ("\n");
  
  printf ("multiply on quaternion\n");
  box *= fromAxisAnglef (deg2rad (90.0f), 1, 0, 0);
  dump (box);
  printf ("\n");
  
  printf ("muliply on matrix\n");
  box *= translatef (10, 20, 30);
  dump (box);
  printf ("\n");  
  
  printf ("intersects:\n");
  printf ("  box with box1: %d, intersection: ", box.intersects (box1));
  dump   (intersection (box, box1));
  printf ("\n");
  printf ("  box1 with box: %d, intersection: ", box1.intersects (box));
  dump   (intersection (box1, box));
  printf ("\n");

  printf ("intersects:\n");
  printf ("  box with box2: %d, intersection: ", box.intersects (box2));
  dump   (intersection (box, box2));
  printf ("\n");
  printf ("  box2 with box: %d, intersection: ", box2.intersects (box));
  dump   (intersection (box2, box));
  printf ("\n");
  
  printf ("contains point: %d\n", box.contains (vec3f (10.0f)));
  printf ("contains box1: %d\n", box.contains (box1));
  printf ("contains intersection (box, box1): %d\n", box.contains (intersection (box, box1)));
  
  printf ("box == box: %d\n", box == box);
  printf ("box != box: %d\n", box != box);  
  printf ("box == box1: %d\n", box == box1);
  printf ("box != box1: %d\n", box != box1);
  
  return 0;
}
