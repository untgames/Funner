#include "shared.h"

const char* DST_FILE_NAME = "/io/stdout/test.mtl";

int main ()
{
  printf ("Results of mtl_saver_test:\n");
  
  try
  {
    CommonMaterial::Pointer common_material1 = CommonMaterial::Create (),
                            common_material2 = xtl::static_pointer_cast<CommonMaterial> (clone (common_material1));
    Material::Pointer       test_material1   = TestMaterial::Create (),
                            test_material2   = test_material1->Clone (CloneMode_Instance);

    common_material1->Rename ("common_material1");
    common_material2->Rename ("common_material2");
    test_material1->Rename   ("test_material1");
    test_material2->Rename   ("test_material2");
    
    common_material2->SetColor (CommonMaterialColor_Diffuse, vec3f (0.5f));
    
    Texmap& texmap = common_material1->Map (CommonMaterialMap_Diffuse);
    
    common_material1->SetMapWeight (CommonMaterialMap_Diffuse, 0.5f);
    
    texmap.SetTransform  (math::translatef (1, 2, 3));
    texmap.SetSource     (Texcoord_S, TexcoordSource_SphereMap);
    texmap.SetSource     (Texcoord_T, 2);    
    texmap.SetSource     (Texcoord_Q, TexcoordSource_ReflectionMap);
    texmap.SetWrap       (Texcoord_S, TexcoordWrap_Mirror);
    texmap.SetWrap       (Texcoord_T, TexcoordWrap_ClampToBorder);
    texmap.SetFilterType (TexmapFilter_Min, TexmapFilterType_Bilinear);
    texmap.SetFilterType (TexmapFilter_Mip, TexmapFilterType_Trilinear);
    texmap.SetFilterType (TexmapFilter_Mag, TexmapFilterType_Anisotropic);
    
    texmap.SetImage ("image1.jpg");

    MultiPassMaterial::Pointer mtl1 = MultiPassMaterial::Create ();
    
    mtl1->Rename ("material1");
    mtl1->SetSortGroup (234);
    
    mtl1->AddPass (common_material1);
    mtl1->AddPass (test_material1);
    mtl1->AddPass (common_material2);
    mtl1->AddPass (test_material2);
    mtl1->AddPass (mtl1);
    
    mtl1->DisablePass (2);
    mtl1->MovePass (2, 3);    

    MaterialLibrary library;
    
    library.Attach ("material1", mtl1);
    library.Attach ("material2", mtl1);
    library.Attach ("common_material2", common_material2);
    
    library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
