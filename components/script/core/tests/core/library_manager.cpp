#include "shared.h"

class MyComponent: public common::IComponent
{
  public:
    void Load ()
    {
      LibraryManager::RegisterLibrary ("library1", &Binder);
    }

    void Unload () {}

  private:
    static void Binder (Environment&)
    {
      printf ("Bind library\n");
    }
};

int main ()
{
  printf ("Results of library_manager_test:\n");

  try
  {
    common::ComponentManager::Register ("script.binds.library1", new MyComponent);
    
    Environment env;

    env.BindLibraries ("*");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
