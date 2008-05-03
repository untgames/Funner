#include <stdio.h>
#include <common/component.h>
#include <xtl/iterator.h>

using namespace common;

static const char* COMPONENT_NAME = "common.tests.MyComponent";

struct MyComponent: public AutoRegisteredComponent
{
  const char* Name () { return COMPONENT_NAME; }
  
  void Load ()
  {
    printf ("Load MyComponent\n");

    ComponentManager::Unregister (this);
    ComponentManager::Register (this);
    ComponentManager::Load (COMPONENT_NAME);
    ComponentManager::Unload (COMPONENT_NAME);
  }
  
  void Unload ()
  {
    ComponentManager::Unregister (this);
    ComponentManager::Register (this);
    ComponentManager::Load (COMPONENT_NAME);
    ComponentManager::Unload (COMPONENT_NAME);    
    
    printf ("Unload MyComponent\n");
  }
};

void dump ()
{
  printf ("Components:\n");

  for (ComponentManager::Iterator iter=ComponentManager::CreateIterator (); iter; ++iter)
    printf ("  component '%s' %s\n", iter->Name (), iter->IsLoaded () ? "loaded" : "unloaded");
}

int main ()
{
  printf ("Results of component_manager_test:\n");
  
  {
    MyComponent my_component;
    
    printf ("Before load\n");
    
    dump ();
  
    ComponentLoader loader ("*");
    
    printf ("After load\n");
    
    dump ();
    
    ComponentManager::Unload ("*");
    
    dump ();
  }
  
  printf ("After unregister\n");
  
  dump ();

  return 0;
}
