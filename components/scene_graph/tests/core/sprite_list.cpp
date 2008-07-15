#include "shared.h"

class MyVisitor: public visitor<void, SpriteList>
{
  public:
    void visit (SpriteList& sprite_list)
    {
      printf ("SpriteList (material='%s'):\n", sprite_list.Material ());

      for (size_t i=0; i<sprite_list.Size (); i++)
      {
        const SpriteList::Item& s = sprite_list.Data ()[i];

        printf ("  #%u: position=[%.0f %.0f %.0f] size=[%.0f %.0f] color=[%.2f %.2f %.2f %.2f] frame=%u\n",
          i, s.position.x, s.position.y, s.position.z, s.size.x, s.size.y, s.color.x, s.color.y, s.color.z, s.color.w, s.frame);
      }
    }
};

int main ()
{
  printf ("Results of sprite_list_test:\n");
  
  SpriteList::Pointer list = SpriteList::Create ();

  list->SetMaterial ("material1");

  list->Reserve (10);

  for (size_t i=0; i<10; i++)
  {
    SpriteList::Item s;
    
    s.position = vec3f (float (i), 0, float (i));
    s.size     = vec2f (float (i * i), float (i));
    s.frame    = i % 3;
    s.color    = vec4f (1.0f, 0.0f, 0.5f, 0.25f);

    list->Insert (s);
  }    

  MyVisitor visitor;

  list->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
