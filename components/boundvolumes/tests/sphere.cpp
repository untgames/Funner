#include "shared.h"

//radius = length (m * vec3f (0, 0, radius))

void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

void dump (const spheref& sph)
{
  printf ("center=");
  dump (sph.center());
  printf (" radius=%g",sph.radius());
  printf("\n");
}

void main()
{
   spheref sph1,sph2(vec3f(10,10,10),20);
   printf("sphere created by default constructor\n");
   dump(sph1);
   printf("sphere created by vector and radius constructor\n");
   dump(sph2);
   
   printf("empty=%d\n",sph1.empty(0.01));
   sph1.set_radius(5);
   sph1.set_center(vec3f(10,10,30));
   printf("set sphere properties\n");
   dump(sph1);
   printf("volume=%g\n",sph1.volume());
   printf("contains sph2=%d\n",sph1.contains(sph2));
   printf("contains point=%d\n",sph1.contains(vec3f(10,10,10)));

   printf("equal=%d\n",sph1.equal(sph2));
   printf("==%d\n",sph1==sph2?true:false);
   printf("!=%d\n",sph1!=sph2?true:false);

   printf("intersects=%d\n",sph1.intersects(sph2));
}