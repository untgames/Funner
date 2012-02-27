#include "shared.h"

class MyServer: public media::rms::ICustomServer
{
  public:
    void PrefetchResource (const char* resource_name)
    {
      Print ("PrefetchResource", resource_name);
    }
    
    void LoadResource (const char* resource_name)
    {
      Print ("LoadResource", resource_name);
    }
    
    void UnloadResource (const char* resource_name)
    {
      Print ("UnloadResource", resource_name);
    }
    
  private:
    void Print (const char* message, const char* resource_name)
    {
      printf ("MyServer::%s(%s)\n", message, resource_name);
    }
};

void print_log (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of xscene1_test:\n");

  try
  {
    MyServer custom_server;

    media::rms::ServerGroup server_group ("group1");
    
    server_group.Attach (custom_server);     

    SceneManager manager;
    
    manager.LoadScene ("data/test1.xscene", "", &print_log);
    
    Node::Pointer node = manager.CreateScene ("scene1");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
