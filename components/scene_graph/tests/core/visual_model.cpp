#include "shared.h"

namespace
{

class VisualModelTest: public VisualModel
{
  public:
    static Pointer Create () { return new VisualModelTest; }
};

}

int main ()
{
  printf ("Results of visual_model_test:\n");
  
  VisualModel::Pointer model (VisualModelTest::Create ());

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  {
    Scissor::Pointer scissor (Scissor::Create ());

    model->SetScissor (&*scissor, NodeBindMode_WeakRef);

    printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");
  }

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  {
    Scissor::Pointer scissor (Scissor::Create ());

    model->SetScissor (&*scissor, NodeBindMode_AddRef);
  }

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  return 0;
}
