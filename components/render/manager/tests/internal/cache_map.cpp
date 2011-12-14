#include "shared.h"

struct A: public xtl::reference_counter
{
  typedef xtl::intrusive_ptr<A> Pointer;

  A (int in_id) : id (in_id) { printf ("A(%d)\n", id); }
  ~A () { printf ("~A(%d)\n", id); }
  
  static Pointer Create (int id)
  {
    return Pointer (new A (id), false);
  }

  int id;
};

int main ()
{
  printf ("Results of cache_map_test:\n");
  
  try
  {
    CacheManagerPtr manager (new CacheManager, false);
    
    manager->SetFrameDelay (1);
    
    CacheMap<int, A::Pointer> map1 (manager);
    
    printf ("--- Insert elements in cache ---\n");
    
    manager->UpdateMarkers ();
    
    map1 [0] = A::Create (0);
    map1 [1] = A::Create (1);
    map1 [2] = A::Create (2);
    
    printf ("--- Flush cache (frame_delay = 1) ---\n");
    
    manager->FlushCaches ();
    
    printf ("--- Flush cache (frame_delay = 1, next frame) ---\n");
    
    manager->UpdateMarkers ();
    
    map1 [1];
    
    manager->FlushCaches ();    
    
    printf ("--- Flush cache (frame_delay = 1, next next frame) ---\n");
    
    manager->UpdateMarkers ();
    
    manager->FlushCaches ();        
    
    printf ("--- All done ---\n");    
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
