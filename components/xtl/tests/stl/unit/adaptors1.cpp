#include <stl/functional>
#include <stdio.h>

using namespace stl;

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

int f1 (A&)
{
  printf ("int f1 (A&)\n");
  return 1;
}

int f2 (A&,B&)
{
  printf ("int f1 (A&,B&)\n");
  return 2;
}

int f3 (const A&)
{
  printf ("int int f3 (const A&)\n");
  return 1;
}

int f4 (const A&,const B&)
{
  printf ("int f4 (const A&,const B&)\n");
  return 2;
}

void f5 (A&)
{
  printf ("void f5 (A&)\n");
}

void f6 (A&,B&)
{
  printf ("void f6 (A&,B&)\n");
}

void f7 (const A&)
{
  printf ("void f7 (A&,B&)\n");
}

void f8 (const A&,const B&)
{
  printf ("void f8 (const A&,const B&)\n");
}

void f9 (const A&,const B&,const C&)
{
  printf ("void f9 (const A&,const B&,const C&)\n");
}

void f10 (const A&,const B&,const C&,const D&)
{
  printf ("void f10 (const A&,const B&,const C&,const D&)\n");
}

void f11 (const A&,const B&,const C&,const D&,const E&)
{
  printf ("void f11 (const A&,const B&,const C&,const D&,const E&)\n");
}

struct Class 
{
  Class  ()             { printf ("Class ()\n"); }
  Class  (const Class&) { printf ("Class (const Class&)\n"); }  
  ~Class ()             { printf ("~Class ()\n"); }
  
  Class& operator = (const Class&) { return printf ("Class& Class::operator = (const Class&)\n"), *this; }

  int  f0  ()                  { printf ("int  Class::f0  ()\n"); return 0; }  
  int  f1  (const A&)          { printf ("int  Class::f1  (const A&)\n"); return 1; }
  void f2  ()                  { printf ("void Class::f2  ()\n"); }
  void f3  (const A&)          { printf ("void Class::f3  (const A&)\n"); }
  void f4  (A&,B&)             { printf ("void Class::f4  (A&,B&)\n"); }
  void f5  (A&,B&,C&)          { printf ("void Class::f5  (A&,B&,C&)\n"); }  
  int  f6  (A&,B&,C&,D&)       { printf ("int  Class::f6  (A&,B&,C&,D&)\n"); return 1; }
  int  f7  () const            { printf ("int  Class::f7  () const\n"); return 0; }  
  int  f8  (const A&) const    { printf ("int  Class::f8  (const A&) const\n"); return 1; }
  void f9  () const            { printf ("void Class::f9  () const\n"); }
  void f10 (const A&) const    { printf ("void Class::f10 (const A&) const\n"); }  
  void f11 (A&,B&) const       { printf ("void Class::f11 (A&,B&) const\n"); }
  void f12 (A&,B&,C&) const    { printf ("void Class::f12 (A&,B&,C&) const\n"); }  
  int  f13 (A&,B&,C&,D&) const { printf ("int  Class::f13 (A&,B&,C&,D&) const\n"); return 1; }  
};

int main ()
{
  printf ("Results of adaptors_test:\n");

  Class obj;
  const Class& const_obj = obj;
  A a;
  B b;
  C c;
  D d;
  E e;

  printf  ("ptr_fun test:\n");    
  ptr_fun (f1)(a);
  ptr_fun (f2)(a,b);
  ptr_fun (f3)(a);
  ptr_fun (f4)(a, b);
  ptr_fun (f5)(a);
  ptr_fun (f6)(a,b);
  ptr_fun (f7)(a);
  ptr_fun (f8)(a,b);
  ptr_fun (f9)(a,b,c);
  ptr_fun (f10)(a,b,c,d);
  ptr_fun (f11)(a,b,c,d,e);

  printf  ("mem_fun test:\n");
  mem_fun (&Class::f0)(&obj);
  mem_fun (&Class::f1)(&obj,a);
  mem_fun (&Class::f2)(&obj);
  mem_fun (&Class::f3)(&obj,a);
  mem_fun (&Class::f4)(&obj,a,b);
  mem_fun (&Class::f5)(&obj,a,b,c);
  mem_fun (&Class::f6)(&obj,a,b,c,d);
  mem_fun (&Class::f7)(&const_obj);
  mem_fun (&Class::f8)(&const_obj,a);
  mem_fun (&Class::f9)(&const_obj);
  mem_fun (&Class::f10)(&const_obj,a);
  mem_fun (&Class::f11)(&const_obj,a,b);
  mem_fun (&Class::f12)(&const_obj,a,b,c);  
  mem_fun (&Class::f13)(&const_obj,a,b,c,d);

  printf      ("mem_fun_ref test:\n");
  mem_fun_ref (&Class::f0)(obj);
  mem_fun_ref (&Class::f1)(obj,a);
  mem_fun_ref (&Class::f2)(obj);
  mem_fun_ref (&Class::f3)(obj,a);
  mem_fun_ref (&Class::f4)(obj,a,b);
  mem_fun_ref (&Class::f5)(obj,a,b,c);
  mem_fun_ref (&Class::f6)(obj,a,b,c,d);
  mem_fun_ref (&Class::f7)(const_obj);
  mem_fun_ref (&Class::f8)(const_obj,a);
  mem_fun_ref (&Class::f9)(const_obj);
  mem_fun_ref (&Class::f10)(const_obj,a);
  mem_fun_ref (&Class::f11)(const_obj,a,b);
  mem_fun_ref (&Class::f12)(const_obj,a,b,c);  
  mem_fun_ref (&Class::f13)(const_obj,a,b,c,d);

  return 0;
}
