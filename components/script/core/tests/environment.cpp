#include "shared.h"

void handler (EnvironmentLibraryEvent event_id, const char* id, InvokerRegistry& registry)
{
  switch (event_id)
  {
    case EnvironmentLibraryEvent_OnCreate:
      printf ("  create '%s'\n", id);
      break;
    case EnvironmentLibraryEvent_OnRemove:
      printf ("  remove '%s'\n", id);
      break;    
    default: break;
  }
}

int main (int, char** argv)
{
  printf ("Results of environment:\n");
  
  try
  {
    Environment env;
    
    env.RegisterEventHandler (EnvironmentLibraryEvent_OnCreate, &handler);
    env.RegisterEventHandler (EnvironmentLibraryEvent_OnRemove, &handler);

    printf ("Create libraries:\n");

    env.CreateLibrary ("library1");
    env.CreateLibrary ("library2");
    
    printf ("Register types:\n");
    
    env.RegisterType (typeid (int),    "library1");    
    env.RegisterType (typeid (float),  "library1");
    env.RegisterType (typeid (double), "library2");

    printf ("Unregister libraries:\n");
    
    env.RemoveLibrary ("library2");
    
    printf ("Check types:\n");
    
    env.UnregisterType (typeid (float));
    
    if (env.FindLibrary (typeid (float))) printf ("  'float' type found\n");
    else                                  printf ("  'float' type not found\n");

    if (env.FindLibrary (typeid (int))) printf ("  'int' type found\n");
    else                                printf ("  'int' type not found\n");
    
    if (env.FindLibrary (typeid (double))) printf ("  'double' type found\n");
    else                                   printf ("  'double' type not found\n");    
    
    printf ("Traverse libraries:\n");
    
    for (Environment::LibraryIterator i=env.CreateLibraryIterator (); i; ++i)
      printf ("  library '%s'\n", env.LibraryId (i));
              
    printf ("Search 'f1', 'f2':\n");

    if (env.FindLibrary ("library1")) printf ("  library 'library1' found\n");
    else                              printf ("  library 'library1' not found\n");
    
    if (env.FindLibrary ("library2")) printf ("  library 'library2' found\n");
    else                              printf ("  library 'library2' not found\n");    

    printf ("Exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
