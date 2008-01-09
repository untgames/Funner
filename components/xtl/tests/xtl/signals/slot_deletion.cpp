#include "test.h"

typedef signal<void ()> my_signal;

struct Fn
{
  Fn (int id) : impl (new Impl (id)) { }
  Fn (const Fn& f) : impl (f.impl) { ++impl->ref_count; }
  
  ~Fn () {
    if (!--impl->ref_count)
      delete impl;
  }
  
  void operator ()() const { printf ("%d ", impl->id); }
    
  private:
    Fn& operator = (const Fn&); //no impl
    
    struct Impl
    {
      int ref_count, id;
      
      Impl (int in_id) : ref_count (1), id (in_id) {
        printf ("Fn(%d)\n", id);
      }
      
      ~Impl ()
      {
        printf ("~Fn(%d)\n", id);
      }
    };    
    
    Impl* impl;
};

void do_signal (my_signal& sig)
{
  sig ();
  printf ("\n");
}

int main ()
{
  printf ("Results of slot_deletion_test:\n");
  
  try
  {
    my_signal::slot_type s1 (Fn (1));
    
    {
      my_signal sig;

      sig.connect (s1);
      sig.connect (Fn (2));

      {
        my_signal::slot_type s2 (Fn (3));

        sig.connect (s2);
        
        do_signal (sig);

        printf ("end of slot s2\n");
      }
      
      do_signal (sig);
      
      printf ("end of signal\n");
    }    
    
    printf ("end of slot s1\n");    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
