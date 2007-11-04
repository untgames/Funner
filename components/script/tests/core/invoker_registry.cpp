#include "shared.h"

void registry_handler (InvokerRegistryEvent event_id, const char* name)
{
  switch (event_id)
  {
    case InvokerRegistryEvent_OnRegisterInvoker:   printf ("  register '%s'\n", name); break;
    case InvokerRegistryEvent_OnUnregisterInvoker: printf ("  unregister '%s'\n", name); break;
    default: break;
  }
}

void registry1_handler (InvokerRegistryEvent event_id, const char* name)
{
  printf ("  registry1:");
  
  registry_handler (event_id, name);
}

int main (int, char** argv)
{
  printf ("Results of invoker_registry:\n");
  
  try
  {
    InvokerRegistry registry, registry1;
    Invoker         invoker = make_invoker (&main);
    
    registry.RegisterHandler  (InvokerRegistryEvent_OnRegisterInvoker, &registry_handler);
    registry.RegisterHandler  (InvokerRegistryEvent_OnUnregisterInvoker, &registry_handler);
    registry1.RegisterHandler (InvokerRegistryEvent_OnRegisterInvoker, &registry1_handler);
    registry1.RegisterHandler (InvokerRegistryEvent_OnUnregisterInvoker, &registry1_handler);
    
    printf ("Register invokers:\n");
    
    registry.Register ("f1", invoker);
    registry.Register ("f2", invoker);
    registry.Register ("f3", invoker);

    printf ("Unregister invokers:\n");
    
    registry.Unregister ("f2");
    
    printf ("Traverse invokers:\n");
    
    for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
      printf ("  invoker '%s': arguments_count=%u, results_count=%u\n", registry.InvokerId (i), i->ArgumentsCount (),
              i->ResultsCount ());
              
    printf ("Search 'f1', 'f2':\n");

    if (registry.Find ("f1")) printf ("  invoker 'f1' found\n");
    else                      printf ("  invoker 'f1' not found\n");
    
    if (registry.Find ("f2")) printf ("  invoker 'f2' found\n");
    else                      printf ("  invoker 'f2' not found\n");    
    
    printf ("Copy registry:\n");
    
    registry1 = registry;    
    
    printf ("Unregister invoker 'f1' from registry1:\n");

    registry1.Unregister ("f1");

    printf ("Exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
