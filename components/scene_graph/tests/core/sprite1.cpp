#include "shared.h"

class MyVisitor: public visitor<void, Sprite>
{
  public:
    void visit (Sprite& sprite)
    {
      printf ("Sprite '%s': \n", sprite.Name ());
      printf ("  material: '%s'\n", sprite.Material ());
      printf ("  frame:    %u\n", sprite.Frame ());
      printf ("  color:    [%.2f %.2f %.2f %.2f]\n", sprite.Color ().x, sprite.Color ().y, sprite.Color ().z, sprite.Color ().w);
      printf ("  descs (count=%u):\n", sprite.SpriteDescsCount ());
      
      for (size_t i=0; i<sprite.SpriteDescsCount (); i++)
      {
        const SpriteModel::SpriteDesc& desc = sprite.SpriteDescs () [i];
        
        printf ("    position: [%.2f %.2f %.2f]\n", desc.position.x, desc.position.y, desc.position.z);
        printf ("    size:     [%.2f %.2f]\n", desc.size.x, desc.size.y);
        printf ("    frame:    %u\n", desc.frame);
        printf ("    color:    [%.2f %.2f %.2f %.2f]\n", desc.color.x, desc.color.y, desc.color.z, desc.color.w);
      }
    }
};

int main ()
{
  printf ("Results of sprite1_test:\n");
  
  Sprite::Pointer sprite (Sprite::Create ());

  sprite->SetMaterial ("material1");
  sprite->SetFrame (12);
  sprite->SetAlpha (0.5f);
  
  MyVisitor visitor;
  
  sprite->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
