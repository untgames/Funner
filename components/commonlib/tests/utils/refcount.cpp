#include <common/refcount.h>
#include <stdio.h>

using namespace common;

class SharedResource: public ReferenceCounter
{
  public:
             SharedResource () {}
    explicit SharedResource (size_t init_ref_count) : ReferenceCounter (init_ref_count) {}
  
  private:
    void OnLostReferences () { printf ("OnLostReferences\n"); }
};

void CheckedAddRef (SharedResource& resource)
{
  try
  {
    resource.AddRef ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
}

void CheckedRelease (SharedResource& resource)
{
  try
  {
    resource.Release ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
}

int main ()
{
  printf ("Results of refcount_test:\n");

  SharedResource resource1, resource2 (3);

  CheckedAddRef (resource1);

  resource2 = resource1;
  
  CheckedRelease (resource1);
  CheckedRelease (resource1);
  CheckedRelease (resource2);
  CheckedRelease (resource2);
  CheckedRelease (resource2);
  CheckedRelease (resource2);
  CheckedAddRef  (resource1);

  return 0;
}
