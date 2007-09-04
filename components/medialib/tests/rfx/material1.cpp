#include "shared.h"

class PassVisitor: public xtl::visitor<void, TestShader, CommonShader>
{
  public:
    void visit (TestShader& shader)   { printf ("    TestShader '%s'\n", shader.Name ()); }
    void visit (CommonShader& shader) { printf ("    CommonShader '%s'\n", shader.Name ()); }
};

void dump (Material& mtl)
{
  printf ("Material '%s' (%u passes)\n", mtl.Name (), mtl.PassesCount ());
  printf ("  sort group %d\n", mtl.SortGroup ());  
  
  for (size_t i=0; i<mtl.PassesCount (); i++)
    printf ("  [%c] pass #%u: shader='%s'\n", mtl.IsPassEnabled (i) ? 'X' : ' ', i, mtl.Shader (i).Name ());
    
  printf ("  visit passes:\n");

  mtl.VisitEachPass (PassVisitor ());
}

int main ()
{
  printf ("Results of material1_test:\n");
  
  Shader::Pointer common_shader1 = CommonShader::Create (),
                  common_shader2 = common_shader1->Clone (CloneMode_Copy),
                  test_shader1   = TestShader::Create (),
                  test_shader2   = test_shader1->Clone (CloneMode_Instance);

  common_shader1->Rename ("common_shader1");
  common_shader2->Rename ("common_shader2");
  test_shader1->Rename   ("test_shader1");
  test_shader2->Rename   ("test_shader2");

  Material mtl1;
  
  mtl1.Rename ("material1");
  mtl1.SetSortGroup (234);
  
  mtl1.AddPass (common_shader1);
  mtl1.AddPass (test_shader1);
  mtl1.AddPass (common_shader2);
  mtl1.AddPass (test_shader2);
  
  mtl1.DisablePass (2);
  mtl1.MovePass (2, 3);

  dump (mtl1);
  
  printf ("search 'common_shader2': %s (pass=%d)\n", mtl1.FindShader ("common_shader2") ? "found" : "not found",
          mtl1.FindPass ("common_shader2"));

  return 0;
}
