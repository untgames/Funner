#include "shared.h"

struct X: public xtl::dynamic_cast_root
{
  virtual const char* name () const { return "struct X"; }

  virtual const char* test () const { return "X::test"; }
};

struct Y: X
{
  const char* name () const { return "struct Y"; }

  const char* test () const { return "Y::test"; }
};

struct A
{
  A (const char* in_name) : name (in_name) {}

  xtl::shared_ptr<X> f (float& x, const char* y)
  {
    printf ("%s: f(%g,%s)\n", name.c_str (), x, y);
    
    x += 2.0f;
    name += '!';

    return xtl::shared_ptr<X> (new Y);
  }

  stl::string name;
};

void to_string (stl::string& buffer, const X& value)
{
  buffer = value.name ();
}

void to_string (stl::string& buffer, const A& value)
{
  buffer = common::format ("A('%s')", value.name.c_str ());
}

void dump (IStack& stack)
{
  printf ("stack (size=%u):\n", stack.Size ());

  stl::string buffer;

  for (size_t i=0; i<stack.Size (); i++)
  {
    to_string (buffer, stack.GetVariant (i));

    printf ("  item[%u]: '%s'\n", i, buffer.c_str ());
  }
}

void invoke (const char* name, const Invoker& f, IStack& stack)
{
  printf ("invoke '%s':\n", name);

  try
  {
    f (stack);    
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("failed, exception %s: '%s' -> '%s'\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }  
  catch (std::exception& exception)
  {
    printf ("failed, exception: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of invoker2_test:\n");
  
  try
  {
    Invoker f1 = make_invoker (&A::f), f2 = make_invoker (&X::test);
    MyStack stack;
    
    A a ("A1");
    
    stack.Push (xtl::ref (a));
    stack.Push (3.0f);
    stack.Push ("Hello world");

    invoke ("f1", f1, stack);
    
    dump (stack);

    printf ("a.name='%s'\n", a.name.c_str ());
    
    stack.Pop  (stack.Size ());
    stack.Push (xtl::shared_ptr<X> (new Y));
    
    invoke ("f2", f2, stack);    

    dump (stack);
    
    stack.Pop  (stack.Size ());
    stack.Push (xtl::shared_ptr<const X> (new Y));

    invoke ("f2", f2, stack);

    dump (stack);
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("%s: '%s' -> '%s'\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
