#include <stdio.h>
#include <xtl/visitor.h>
#include <stl/vector>
#include <xtl/type_list.h>

using namespace xtl;
using namespace stl;

typedef basic_visitor<> my_visitor;

struct A
{
  virtual ~A () {}

  virtual void accept (my_visitor& visitor) { visitor (*this); }
};

struct B : A
{
  void accept (my_visitor& visitor) { visitor (*this); }
};

struct C : B
{
  void accept (my_visitor& visitor) { visitor (*this); }
};

struct my_visitor_impl: public visitor<void, A, mpl::list<B, C>::type>
{
  void visit (A&) { printf ("visit A\n"); }
  void visit (B&) { printf ("visit B\n"); }
  void visit (C&) { printf ("visit C\n"); }
};

template <class Iter>
void visit_each (Iter first, Iter last, my_visitor& visitor)
{
  for (;first != last; ++first) (*first)->accept (visitor);
}

int main ()
{
  printf ("Results of visitor1_test:\n");

  A a;
  B b;
  C c;

  typedef stl::vector<A*> my_vector;

  my_vector v;

  v.push_back (&c);
  v.push_back (&a);
  v.push_back (&b);

  printf ("visit vector elements\n");

  my_visitor_impl vis;

  visit_each (v.begin (), v.end (), vis);

  return 0;
}
