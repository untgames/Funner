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

class ErrorCache: public MyCache
{
  public:
    ErrorCache (const char* in_id) : MyCache (in_id), has_error (true) {}
    
    void UpdateCacheCore ()
    {
      MyCache::UpdateCacheCore ();
      
      if (has_error)
        throw std::bad_alloc ();
    }
    
    void InvalidateCache (bool invalidate_deps = true)
    {
      MyCache::InvalidateCache (invalidate_deps);
      
      has_error = false;
    }
    
  private:
    bool has_error;
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
    
    printf ("Update valid dependency cache\n");        
    
    dependency.UpdateCache ();
    self.UpdateCache ();
    source.UpdateCache ();
    
    printf ("Invalidate cache without dependencies\n");
    
    source.InvalidateCache (false);
    
    dependency.UpdateCache ();
    
    printf ("Invalidate cache with dependencies\n");
    
    source.InvalidateCache ();
    
    dependency.UpdateCache ();

    {
      printf ("Update error cache\n");

      ErrorCache err_cache ("error");

      dependency.AttachCacheSource (err_cache);

      try
      {
        dependency.UpdateCache ();
      }
      catch (std::exception& e)
      {
        printf ("%s\n", e.what ());
      }
      
      printf ("Update cache after error\n");

      dependency.UpdateCache ();
      
      printf ("Invalidate error cache and update\n");

      err_cache.InvalidateCache ();
      dependency.UpdateCache ();    
      
      printf ("Remove error cache and update\n");
    }    
    
    dependency.UpdateCache ();
    
    printf ("Reset cache\n");
    
    self.ResetCache ();
    dependency.ResetCache ();
    
    printf ("All done\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
