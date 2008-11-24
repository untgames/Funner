#include <cstdio>
#include <exception>

#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/rms/manager.h>

using namespace media::rms;

class MyServer: public ICustomServer
{
  public:
    MyServer (int in_id = -1) : id (in_id) {}

    void PrefetchResources (size_t count, const char** resource_names)
    {
      Print ("PrefetchResources", count, resource_names);
    }
    
    void LoadResources (size_t count, const char** resource_names)
    {
      Print ("LoadResources", count, resource_names);
    }
    
    void UnloadResources (size_t count, const char** resource_names)
    {
      Print ("UnloadResources", count, resource_names);
    }
    
  private:
    void Print (const char* message, size_t count, const char** resource_names)
    {
      if (id >= 0) printf ("MyServer%u::%s(%u, {", id, message, count);
      else         printf ("MyServer::%s(%u, {", message, count);

      for (size_t i=0; i<count; i++)
      {
        if (i)
          printf (", ");

        printf ("%s", resource_names [i]);
      }
      
      printf ("})\n");
    }        
    
  private:
    int id;
};
