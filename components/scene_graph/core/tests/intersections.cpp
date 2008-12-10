#include "shared.h"

void print_intersections (Entity& entity)
{
  NodeArray intersections = entity.GetIntersections ();
  
  printf ("%s has %u intersections: {", entity.Name (), intersections.Size ());
  
  for (size_t i=0, count=intersections.Size (); i<count; i++)
  {
    if (i)
      printf (", ");
      
    printf ("%s", intersections.Item (i).Name ());
  }
  
  printf ("}\n");
}

int main ()
{
  printf ("Results of intersection_test:\n");
  
  try
  {
    Scene scene;
    
    TestEntity::Pointer entity1 = TestEntity::Create (),
                        entity2 = TestEntity::Create (),
                        entity3 = TestEntity::Create ();

    entity1->SetName ("entity1");
    entity2->SetName ("entity2");
    entity3->SetName ("entity3");

    entity1->SetBoundBox (aaboxf (-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f));
    entity2->SetBoundBox (aaboxf (0.1f, 0.1f, -1.0f, 1.0f, 1.0f, 1.0f));
    entity3->SetBoundBox (aaboxf (-1.0f, -1.0f, -1.0f, -0.1f, -0.1f, 1.0f));
    
    entity1->BindToScene (scene);
    entity2->BindToScene (scene);
    entity3->BindToScene (scene);

    print_intersections (*entity1);
    print_intersections (*entity2);
    print_intersections (*entity3);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
