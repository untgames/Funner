#include <stdio.h>
#include <common/serializer_manager.h>

using namespace common;

struct A
{
};

void my_loader (const char* name, A&)
{
  printf ("my_loader(%s,A)\n", name);
}

void my_saver (const char* name, const A&)
{
  printf ("my_saver(%s,A)\n", name);
}

typedef ResourceSerializerManager<void (const char*, A&), void (const char*, const A&)> AManager;

int main ()
{
  printf ("Results of serializer_manager_test:\n");
  
  try
  {
    AManager::RegisterLoader ("ext", &my_loader);
    AManager::RegisterSaver  ("ext", &my_saver);

    AManager::GetLoader ("ext")("test", A ());
    AManager::GetSaver ("ext")("test", A ());
    
    AManager::UnregisterAllLoaders ();
    
    AManager::GetSaver ("my.ext", SerializerFindMode_ByName)("test", A ());    
    
    AManager::GetLoader ("ext")("test", A ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  try
  {
    printf ("is-saver-null: %s\n", AManager::FindSaver ("my.ext", SerializerFindMode_ByName) ? "false" : "true");    
    
    SerializerManager::UnregisterAll ("ext");
   
    printf ("is-saver-null: %s\n", AManager::FindSaver ("ext") ? "false" : "true");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
