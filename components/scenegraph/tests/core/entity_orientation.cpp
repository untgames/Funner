#include <mathlib.h>
#include <xtl/intrusive_ptr.h>
#include "shared.h"

using namespace math;

typedef xtl::com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_orientation_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  printf ("set orientation from axis angle result: ");
  entity->SetOrientation (-390, 0, 0, 1);
  dump_orientation (*entity);
  printf ("\n");

  printf ("set orientation euler angle result: ");
  entity->SetOrientation (10, 20, 30);
  dump_orientation (*entity);
  printf ("\n");  

  printf ("reset orientation result: ");    
  entity->ResetOrientation ();
  dump_orientation (*entity);
  printf ("\n");  

  return 0;
}
