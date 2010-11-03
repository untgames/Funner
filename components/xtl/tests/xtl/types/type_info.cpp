#include <cstdio>

#include <xtl/type_info.h>

using namespace xtl;

struct A
{
  virtual ~A () {}
};

void dump1 (const xtl::type_info& t)
{
  #define DUMP_FIELD(X) if (t.X ()) printf ("  %s\n", #X);

  DUMP_FIELD (is_void);
  DUMP_FIELD (is_integral);
  DUMP_FIELD (is_floating_point);
  DUMP_FIELD (is_array);
  DUMP_FIELD (is_pointer);
  DUMP_FIELD (is_reference);
  DUMP_FIELD (is_member_object_pointer);
  DUMP_FIELD (is_member_function_pointer);
  DUMP_FIELD (is_enum);
  DUMP_FIELD (is_union);
  DUMP_FIELD (is_class);
  DUMP_FIELD (is_function);

  DUMP_FIELD (is_arithmetic);
  DUMP_FIELD (is_fundamental);
  DUMP_FIELD (is_object);
  DUMP_FIELD (is_scalar); 
  DUMP_FIELD (is_compound);
  DUMP_FIELD (is_member_pointer);

  DUMP_FIELD (is_const);
  DUMP_FIELD (is_volatile);
  DUMP_FIELD (is_pod);
  DUMP_FIELD (is_empty);
  DUMP_FIELD (is_polymorphic);
  DUMP_FIELD (is_abstract);
  DUMP_FIELD (has_trivial_constructor);
  DUMP_FIELD (has_trivial_copy);
  DUMP_FIELD (has_trivial_assign);
  DUMP_FIELD (has_trivial_destructor);
//  DUMP_FIELD (has_nothrow_constructor);
//  DUMP_FIELD (has_nothrow_copy);
//  DUMP_FIELD (has_nothrow_assign);
//  DUMP_FIELD (has_virtual_destructor);
  DUMP_FIELD (is_signed);
  DUMP_FIELD (is_unsigned);  
  
  printf ("  alignment_of: %u\n", t.alignment_of ());
  printf ("  rank: %u\n", t.rank ());

  #undef DUMP_FIELD
}

void dump (const char* name, const xtl::type_info& t)
{
  printf ("type '%s'\n", name);
  dump1 (t);
  
  printf ("type remove_const('%s')\n", name);
  dump1 (t.remove_const ());
  
  printf ("type 'remove_const(remove_reference('%s'))\n", name);
  dump1 (t.remove_const ().remove_reference ());
}

int main ()
{
  printf ("Results of type_info_test:\n");

//  const xtl::type_info& int_type = get_type<int> ();    
  
//  dump ("int", int_type);  
  
  const xtl::type_info& a_type = get_type<const A&> ();
  
  dump ("A", a_type);

  return 0;
}
