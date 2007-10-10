#include "shared.h"

class MyVisitor: public xtl::visitor<void, TestMaterial, CommonMaterial, MultiPassMaterial>
{
  public:
    void visit (TestMaterial& material)      { printf ("TestMaterial '%s' (sort_group=%d)\n", material.Name (), material.SortGroup ()); }
    void visit (CommonMaterial& material)    { printf ("CommonMaterial '%s' (sort_group=%d)\n", material.Name (), material.SortGroup ()); }
    void visit (MultiPassMaterial& material)
    {
      printf ("MultiPassMaterial '%s' (%u passes, sort_group=%d)\n", material.Name (), material.PassesCount (), material.SortGroup ());
      
      for (size_t i=0; i<material.PassesCount (); i++)
        printf ("  [%c] pass #%u: shader='%s'\n", material.IsPassEnabled (i) ? 'X' : ' ', i, material.Pass (i)->Name ());
        
      printf ("  visit passes:\n");

      material.VisitEachPass (*this);
    }
};

void dump (Material& mtl)
{
  mtl.Accept (MyVisitor ());
}

int main ()
{
  printf ("Results of material1_test:\n");
  
  Material::Pointer common_material1 = CommonMaterial::Create (),
                    common_material2 = common_material1->Clone (CloneMode_Copy),
                    test_material1   = TestMaterial::Create (),
                    test_material2   = test_material1->Clone (CloneMode_Instance);

  common_material1->Rename ("common_material1");
  common_material2->Rename ("common_material2");
  test_material1->Rename   ("test_material1");
  test_material2->Rename   ("test_material2");

  MultiPassMaterial::Pointer mtl1 = MultiPassMaterial::Create ();
  
  mtl1->Rename ("material1");
  mtl1->SetSortGroup (234);
  
  mtl1->AddPass (common_material1);
  mtl1->AddPass (test_material1);
  mtl1->AddPass (common_material2);
  mtl1->AddPass (test_material2);
  
  mtl1->DisablePass (2);
  mtl1->MovePass (2, 3);

  dump (*mtl1);
  
  printf ("search 'common_material2': %s (pass=%d)\n", mtl1->FindMaterial ("common_material2") ? "found" : "not found",
          mtl1->FindPass ("common_material2"));

  return 0;
}
