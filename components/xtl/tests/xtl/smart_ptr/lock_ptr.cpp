#include "shared.h"

class A: public xtl::reference_counter
{
  public:
    int id;  
  
    A (int in_id) : id (in_id)
    {
      printf ("A::A(%d)\n", id);
    }
    
    ~A ()
    {
      printf ("A::~A(%d)\n", id);
    }
  
  private:
    A (const A&); //no impl
    A& operator = (const A&); //no impl
};

void lock (A& a)
{
  printf ("lock A(%d)\n", a.id);
}

void unlock (A& a)
{
  printf ("unlock A(%d)\n", a.id);
}

int main ()
{
  printf ("Results of lock_ptr_test:\n");
  
  typedef lock_ptr<A, intrusive_ptr<A> > my_ptr;
  
  my_ptr p (intrusive_ptr<A> (new A (1), false)), p1 (p), p2 (intrusive_ptr<A> (new A (2), false));
  
  TEST (!p == false);
  TEST (p2);
  TEST (p == p1);
  TEST (p1 != p2);
  TEST (p1.get () == p);
  TEST (p1.get () != p2);
  TEST (p == p1.get ());
  TEST (p1 != p2.get ());
  TEST ((p1 = p1) == p1);

  return 0;
}
