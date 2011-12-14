#include "shared.h"

void dump (const SpriteMaterial& material)
{
  printf ("Sprite material '%s'\n", material.Name ());  
  printf ("  blend_mode: %s\n", get_name (material.BlendMode ()));  
  printf ("  image: '%s'\n", material.Image ());  
  printf ("  tiling: %s\n", material.IsTiled () ? "enabled" : "disabled");  
  
  if (material.IsTiled ())
    printf ("  tile: %ux%u\n", material.TileWidth (), material.TileHeight ());
}

int main ()
{
  printf ("Results of sprite_material_test:\n");
  
  try
  {
    SpriteMaterial::Pointer material = SpriteMaterial::Create ();
    
    material->Rename ("shader1");
    
    material->SetBlendMode (SpriteBlendMode_Additive);
    material->EnableTiling ();
    material->SetTileSize  (10, 20);
    material->SetImage     ("imag1.tga");

    dump (*material);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
