#include "test.h"

template <size_t size, size_t align> void check_helper ()
{
  typedef typename aligned_storage<size, align>::type type;

  enum {
    check_align  = alignment_of<type>::value == align,
    check_size   = sizeof (type) == size,
    check_pod    = is_pod<type>::value,
    check_result = check_align && check_size && check_pod
  };

  printf ("%saligned_storage<%lu, %lu>: align=%lu size=%lu is_pod=%s\n", check_result?"":"error: ",
          size, align, alignment_of<type>::value, sizeof (type), is_pod<type>::value?"true":"false");
}

template <size_t align> void check ()
{
  check_helper<align, align> ();
  check_helper<align * 2, align> ();
}

int main ()
{
  printf ("Results of aligned_storage_test:\n");

  check<alignment_of<char>::value> ();
  check<alignment_of<short>::value> ();
  check<alignment_of<int>::value> ();
  check<alignment_of<long>::value> ();
  check<alignment_of<float>::value> ();
  check<alignment_of<double>::value> ();
  check<alignment_of<long double>::value> ();
  check<alignment_of<int[4]>::value> ();
  check<alignment_of<int(*)(int)>::value> ();
  check<alignment_of<int*>::value> ();
  check<alignment_of<VB>::value> ();
  check<alignment_of<VD>::value> ();
  check<alignment_of<enum_UDT>::value> ();
  check<alignment_of<mf2>::value> ();
  check<alignment_of<POD_UDT>::value> ();
  check<alignment_of<empty_UDT>::value> ();
  check<alignment_of<union_UDT>::value> ();
  check<alignment_of<detail::max_align>::value> ();

  return 0;
}
