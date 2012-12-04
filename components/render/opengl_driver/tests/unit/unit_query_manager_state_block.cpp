#include "shared.h"

struct State
{
  IPredicate* predicate;
  bool        predicate_value;
  
  void Init (IDevice& device)
  {
    predicate       = device.GetImmediateContext ()->GetPredicate ();
    predicate_value = device.GetImmediateContext ()->GetPredicateValue ();
  }  

  void Check (const State& src)
  {
    printf ("GetPredicate: %d\n", predicate == src.predicate);
    printf ("GetPredicateValue: %d\n", predicate_value == src.predicate_value);    
  }
};

void print (const char* message)
{
}

int main ()
{
  printf ("Results of unit_query_manager_state_block_test:\n");
  
  try
  {
    Test test;
    
    PredicatePtr predicate (test.device->CreatePredicate ());
    bool         predicate_value = true;
    
    test.device->GetImmediateContext ()->SetPredication (predicate.get (), predicate_value);

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;

    mask.predication = true;

    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);
    
    state_block->Capture (test.device->GetImmediateContext ());

    test.device->GetImmediateContext ()->SetPredication (0, 0);

    printf ("after reset\n");
    
    State dst_state;
    
    dst_state.Init (*test.device);    
    dst_state.Check (src_state);

    state_block->Apply (test.device->GetImmediateContext ());

    printf ("after apply\n");    

    dst_state.Init (*test.device);
    dst_state.Check (src_state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
