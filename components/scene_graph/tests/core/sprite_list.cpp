#include "shared.h"

class MyVisitor: public visitor<void, SpriteList>
{
  public:
    void visit (SpriteList& sprite_list)
    {
      printf ("SpriteList (material='%s'):\n", sprite_list.Material ());

      for (size_t i=0; i<sprite_list.SpritesCount (); i++)
      {
        const SpriteDesc& s = sprite_list.Sprites ()[i];

        printf ("  #%u: position=[%.0f %.0f %.0f] size=[%.0f %.0f] color=[%.2f %.2f %.2f %.2f] tex_offset=[%.0f %.0f] tex_size=[%.0f %.0f] normal=[%.0f %.0f %.0f] rotation=%.0f\n",
          i, s.position.x, s.position.y, s.position.z, s.size.x, s.size.y, s.color.x, s.color.y, s.color.z, s.color.w, s.tex_offset.x, s.tex_offset.y, s.tex_size.x, s.tex_size.y,
          s.normal.x, s.normal.y, s.normal.z, math::degree (s.rotation));
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
    SpriteDesc s;
    
    s.position   = vec3f (float (i), 0, float (i));
    s.normal     = vec3f (0, float (i), 0);
    s.size       = vec2f (float (i * i), float (i));
    s.color      = vec4f (1.0f, 0.0f, 0.5f, 0.25f);
    s.tex_offset = vec2f (float (i), 0);
    s.tex_size   = vec2f (0, float (i));
    s.rotation   = degree (float (i));

    list->Insert (s);
  }    

  MyVisitor visitor;

  list->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
