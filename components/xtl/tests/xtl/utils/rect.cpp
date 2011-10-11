#include <stdio.h>

#include <xtl/rect.h>

typedef xtl::rect<int> my_rect;

void dump (const char* name, const my_rect& r)
{
  printf ("%s: x=%d, y=%d, width=%d, height=%d\n", name, r.x, r.y, r.width, r.height);
}

int main ()
{
  printf ("Results of rect_test:\n");
  
  my_rect r1, r2 (1, 2, 3, 4);    
  
  dump ("r1", r1);
  dump ("r2", r2);
  
  r1.x = 5;
  
  r2 = r1;
  
  dump ("r1", r1);
  dump ("r2", r2);  
  
  printf ("r1 == r2: %s\n", r1 == r2 ? "true" : "false");
  printf ("r1 != r2: %s\n", r1 != r2 ? "true" : "false");
  
  r2.height = 40;
  
  printf ("r1 == r2: %s\n", r1 == r2 ? "true" : "false");
  printf ("r1 != r2: %s\n", r1 != r2 ? "true" : "false");  
  
  xtl::rect<float> r3 (1.0f, 2.0f, 3.2f, 4.1f);
  
  my_rect r4 = r3;
  
  r1 = r3;
  
  dump ("r1", r1);
  dump ("r4", r4);
  
  printf ("left=%d, top=%d, right=%d, bottom=%d\n", r1.left (), r1.top (), r1.right (), r1.bottom ());
  
  r2.y = 30;
  
  dump ("r1", r1);
  dump ("r2", r2);  
  
  swap (r1, r2);
  
  dump ("r1", r1);
  dump ("r2", r2);    
  
  return 0;
}
