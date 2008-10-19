#include <cstdio>

//тест проверяет работу компилятора c non-POD на присвоение вида B seasons = { { "spring", "summer", "autumn", "winter" } };

const size_t MAX_NUM = 16;

struct A;

struct Monitor
{
  A*     objects [MAX_NUM];
  size_t count;
  
  Monitor () : count (0) {}
  
  void Insert (A* obj)
  {
    if (count == MAX_NUM)
    {
      printf ("too many objects\n");
      return;
    }

    objects [count++] = obj;
  }
};

Monitor constructed, destructed;
 
struct A
{
  A (const char* in_s)
  {
    constructed.Insert (this);        
  }
  A (const A& in_s)
  {
    constructed.Insert (this);
  }

  ~A ()
  {
    destructed.Insert (this);
  }
};

struct B
{
  A array [4];
};

int main()
{
  printf ("Results of tr1-compiler-check_test:\n");
  
  do
  {
    B seasons = { { "spring", "summer", "autumn", "winter" } };

    B seasons_orig = seasons;  
  } while (0);
  
  A **constructed_iter = constructed.objects + constructed.count - 1,
    **destructed_iter  = destructed.objects;

  if (constructed.count != destructed.count)
    printf ("Constructed/destructed mismath: constructed=%u destructed=%u\n", constructed.count, destructed.count);
  
  for (size_t i=0; i<constructed.count && i<destructed.count; i++, constructed_iter--, destructed_iter++)
    if (*constructed_iter != *destructed_iter)
    {
      printf ("Failed on object %u (constructed=%p, destructed=%p)\n", i, *constructed_iter, *destructed_iter);
      return 0;
    }
    
  printf ("Check ok\n");

  return 0;
}
