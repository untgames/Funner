#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/rms/manager.h>

using namespace media::rms;

class MyServer: public ICustomServer
{
  public:
    MyServer (int in_id = -1) : id (in_id) {}

    void PrefetchResource (const char* resource_name)
    {
      if (!strcmp (resource_name, "bad_resource"))
        throw xtl::format_operation_exception ("MyServer::PrefetchResource", "Attempt to load bad resource");    
    
      Print ("PrefetchResource", resource_name);
    }
    
    void LoadResource (const char* resource_name)
    {
      if (!strcmp (resource_name, "bad_resource"))
        throw xtl::format_operation_exception ("MyServer::LoadResource", "Attempt to load bad resource");
    
      Print ("LoadResource", resource_name);
    }
    
    void UnloadResource (const char* resource_name)
    {
      if (!strcmp (resource_name, "bad_resource"))
        throw xtl::format_operation_exception ("MyServer::UnloadResource", "Attempt to load bad resource");    
    
      Print ("UnloadResource", resource_name);
    }
    
  private:
    void Print (const char* message, const char* resource_name)
    {
      if (id >= 0) printf ("MyServer%u::%s(%s)\n", id, message, resource_name);
      else         printf ("MyServer::%s(%s)\n", message, resource_name);
    }        
    
  private:
    int id;
};
