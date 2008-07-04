#include "shared.h"

class MyVisitor: public visitor<void, Sprite>
{
  public:
    void visit (Sprite& sprite)
    {
      printf ("Sprite: material='%s' frame=%u alpha=%.1f\n", sprite.Material (), sprite.Frame (), sprite.Alpha ());
    }
};

int main ()
{
  printf ("Results of sprite_test:\n");
  
  Sprite::Pointer sprite (Sprite::Create ());

  sprite->SetMaterial ("material1");
  sprite->SetFrame (12);
  sprite->SetAlpha (0.5f);
  
  MyVisitor visitor;
  
  sprite->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
