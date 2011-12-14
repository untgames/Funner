#include "shared.h"

const char* DST_FILE_NAME = "/io/stdout/test.xmtl";

int main ()
{
  printf ("Results of xmtl_saver_test:\n");
  
  try
  {
    SpriteMaterial::Pointer sprite_material  = SpriteMaterial::Create ();
    Material::Pointer       test_material1   = TestMaterial::Create (),
                            test_material2   = test_material1;

    test_material1->Rename   ("test_material1");
    test_material2->Rename   ("test_material2");
    sprite_material->Rename  ("sprite_material");
        
    sprite_material->SetImage ("image1.tga");
    sprite_material->EnableTiling ();
    sprite_material->SetTileSize (10, 20);
    sprite_material->SetTileOffset (1, 3);
    sprite_material->SetBlendMode (SpriteBlendMode_Translucent);
    
    MaterialLibrary library;
    
    library.Attach ("sprite_material", sprite_material);
    
    library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
