#include "shared.h"

const char* get_string (SpriteMode mode)
{
  switch (mode)
  {
    case SpriteMode_Billboard:         return "billboard";
    case SpriteMode_Oriented:          return "oriented";
    case SpriteMode_OrientedBillboard: return "oriented_billboard";
    default:                           return "unknown";
  }
}

const char* get_string (SpriteUsage usage)
{
  switch (usage)
  {
    case SpriteUsage_Static:   return "static";
    case SpriteUsage_Dynamic:  return "dynamic";
    case SpriteUsage_Stream:   return "stream";
    case SpriteUsage_Batching: return "batching";
    default:                   return "unknown";
  }
}

class MyVisitor: public visitor<void, Sprite>
{
  public:
    void visit (Sprite& sprite)
    {
      printf ("Sprite '%s': \n", sprite.Name ());
      printf ("  material:        '%s'\n", sprite.Material ());
      printf ("  color:           [%.2f %.2f %.2f %.2f]\n", sprite.Color ().x, sprite.Color ().y, sprite.Color ().z, sprite.Color ().w);
      printf ("  up:              [%.2f %.2f %.2f]\n", sprite.OrtUp ().x, sprite.OrtUp ().y, sprite.OrtUp ().z);
      printf ("  mode:            %s\n", get_string (sprite.Mode ()));
      printf ("  usage:           %s\n", get_string (sprite.Usage ()));
      printf ("  descs (count=%u):\n", sprite.SpriteDescsCount ());
      
      for (size_t i=0; i<sprite.SpriteDescsCount (); i++)
      {
        const SpriteDesc& desc = sprite.SpriteDescs () [i];
        
        printf ("    position: [%.2f %.2f %.2f]\n", desc.position.x, desc.position.y, desc.position.z);
        printf ("    size:     [%.2f %.2f]\n", desc.size.x, desc.size.y);
        printf ("    color:    [%.2f %.2f %.2f %.2f]\n", desc.color.x, desc.color.y, desc.color.z, desc.color.w);
        printf ("    tex_offs: [%.2f %.2f]\n", desc.tex_offset.x, desc.tex_offset.y);
        printf ("    tex_size: [%.2f %.2f]\n", desc.tex_size.x, desc.tex_size.y);
        printf ("    normal:   [%.2f %.2f %.2f]\n", desc.normal.x, desc.normal.y, desc.normal.z);
        printf ("    angle:    %.2f deg\n", math::degree (desc.rotation));
      }
    }
};

int main ()
{
  printf ("Results of sprite1_test:\n");
  
  Sprite::Pointer sprite (Sprite::Create ());

  sprite->SetMaterial ("material1");
  sprite->SetAlpha (0.5f);
  sprite->SetTexSize (math::vec2f (0.5f, 0.5f));
  sprite->SetMode (SpriteMode_OrientedBillboard);

  MyVisitor visitor;

  sprite->VisitEach (visitor, NodeTraverseMode_TopToBottom);  

  return 0;
}
