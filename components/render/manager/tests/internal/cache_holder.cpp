#include "shared.h"

class MyCache: public CacheHolder
{
  public:
    MyCache (const char* in_id) : id (in_id) {}
    
  public:
    void UpdateCacheCore ()
    {
      printf ("...update cache %s\n", id.c_str ());
    }
    
    void ResetCacheCore ()
    {
      printf ("...reset cache %s\n", id.c_str ());      
    }    
    
    using CacheHolder::ResetCache;
    using CacheHolder::UpdateCache;
    using CacheHolder::InvalidateCache;
  
  private:
    stl::string id;
};

int main ()
{
  printf ("Results of cache_holder_test:\n");
  
  try
  {
    printf ("Create caches\n");
    
    MyCache self ("self"), source ("source"), dependency ("dependency");
    
    self.AttachCacheSource (source);
    dependency.AttachCacheSource (self);
    
    printf ("Update dependency cache\n");    
    
    dependency.UpdateCache ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
