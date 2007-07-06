/*
    Структура, имеющая указанное выравнивание
*/

namespace detail
{

class alignment_dummy;
typedef void (*alignment_function_ptr)();
typedef int (alignment_dummy::*alignment_member_ptr);
typedef int (alignment_dummy::*alignment_member_function_ptr)();

template <class T> struct struct_aligner
{
  T t;
};

template <bool found, size_t alignment, class T> struct type_aligner
{
  enum { value = alignment == alignment_of<T>::value };
  typedef typename mpl::select<value, T, char>::type type;
};

template <size_t alignment, class T> struct type_aligner<true, alignment, T>
{
  typedef char type;
  enum { value = 1 };
};

template <size_t alignment> union lower_alignment
{
  enum
  {
    found1  = type_aligner<false,   alignment, char>::value,
    found2  = type_aligner<found1,  alignment, short>::value,
    found3  = type_aligner<found2,  alignment, int>::value,
    found4  = type_aligner<found3,  alignment, long>::value,
    found5  = type_aligner<found4,  alignment, float>::value,
    found6  = type_aligner<found5,  alignment, double>::value,
    found7  = type_aligner<found6,  alignment, long double>::value,
    found8  = type_aligner<found7,  alignment, void*>::value,
    found9  = type_aligner<found8,  alignment, alignment_function_ptr>::value,
    found10 = type_aligner<found9,  alignment, alignment_member_ptr>::value,
    found11 = type_aligner<found10, alignment, alignment_member_function_ptr>::value,
    found12 = type_aligner<found11, alignment, struct_aligner<char> >::value,
    found13 = type_aligner<found12, alignment, struct_aligner<short> >::value,
    found14 = type_aligner<found13, alignment, struct_aligner<int> >::value,
    found15 = type_aligner<found14, alignment, struct_aligner<long> >::value,
    found16 = type_aligner<found15, alignment, struct_aligner<float> >::value,
    found17 = type_aligner<found16, alignment, struct_aligner<double> >::value,
    found18 = type_aligner<found17, alignment, struct_aligner<long double> >::value,
    found19 = type_aligner<found18, alignment, struct_aligner<void*> >::value,
    found20 = type_aligner<found19, alignment, struct_aligner<alignment_function_ptr> >::value,
    found21 = type_aligner<found20, alignment, struct_aligner<alignment_member_ptr> >::value,
    found22 = type_aligner<found21, alignment, struct_aligner<alignment_member_function_ptr> >::value
  };

  typename type_aligner<found1,  alignment, short>::type                                          field1;
  typename type_aligner<found2,  alignment, int>::type                                            field2;
  typename type_aligner<found3,  alignment, long>::type                                           field3;
  typename type_aligner<found4,  alignment, float>::type                                          field4;
  typename type_aligner<found5,  alignment, double>::type                                         field5;
  typename type_aligner<found6,  alignment, long double>::type                                    field6;
  typename type_aligner<found7,  alignment, void*>::type                                          field7;
  typename type_aligner<found8,  alignment, alignment_function_ptr>::type                         field8;
  typename type_aligner<found9,  alignment, alignment_member_ptr>::type                           field9;
  typename type_aligner<found10, alignment, alignment_member_function_ptr>::type                  field10;
  typename type_aligner<found11, alignment, struct_aligner<char> >::type                          field11;
  typename type_aligner<found12, alignment, struct_aligner<short> >::type                         field12;
  typename type_aligner<found13, alignment, struct_aligner<int> >::type                           field13;
  typename type_aligner<found14, alignment, struct_aligner<long> >::type                          field14;
  typename type_aligner<found15, alignment, struct_aligner<float> >::type                         field15;
  typename type_aligner<found16, alignment, struct_aligner<double> >::type                        field16;
  typename type_aligner<found17, alignment, struct_aligner<long double> >::type                   field17;
  typename type_aligner<found18, alignment, struct_aligner<void*> >::type                         field18;
  typename type_aligner<found19, alignment, struct_aligner<alignment_function_ptr> >::type        field19;
  typename type_aligner<found20, alignment, struct_aligner<alignment_member_ptr> >::type          field20;
  typename type_aligner<found21, alignment, struct_aligner<alignment_member_function_ptr>> ::type field21;
};

union max_align
{
  char                                          field1;
  short                                         field2;
  int                                           field3;
  long                                          field4;
  float                                         field5;
  double                                        field6;
  long double                                   field7;
  void*                                         field8;
  alignment_function_ptr                        field9;
  alignment_member_ptr                          field10;
  alignment_member_function_ptr                 field11;
  struct_aligner<char>                          sfield1;
  struct_aligner<short>                         sfield2;
  struct_aligner<int>                           sfield3;
  struct_aligner<long>                          sfield4;
  struct_aligner<float>                         sfield5;
  struct_aligner<double>                        sfield6;
  struct_aligner<long double>                   sfield7;
  struct_aligner<void*>                         sfield8;
  struct_aligner<alignment_function_ptr>        sfield9;
  struct_aligner<alignment_member_ptr>          sfield10;
  struct_aligner<alignment_member_function_ptr> sfield11;
};

template <size_t TAlign, size_t Align> struct is_aligned
{
  enum { value = (TAlign >= Align) & (TAlign % Align == 0) };
};

template <size_t alignment> class type_with_alignment
{ 
  typedef lower_alignment<alignment> t1;
  typedef typename mpl::select<is_aligned<alignment_of<t1>::value, alignment>::value, t1, max_align>::type align_t;
  
  enum {
    result_alignment = alignment_of<align_t>::value,
    ct_check1        = mpl::compile_time_assert<result_alignment >= alignment>::value,
    ct_check2        = mpl::compile_time_assert<result_alignment % alignment == 0>::value
  };

  public:
    typedef align_t type;
};

template <size_t size, size_t alignment> union aligned_storage_impl
{
  char                                 buf [size];
  typename type_with_alignment<alignment>::type aligner;
};

template <size_t size> union aligned_storage_impl<size, (size_t)-1>
{
  char      buf [size];
  max_align aligner; 
};

}

template <size_t Size, size_t Alignment> struct aligned_storage
{
  private:
    detail::aligned_storage_impl<Size, Alignment> data;

  public:
    typedef detail::aligned_storage_impl<Size, Alignment> type;
    
    enum {
      size      = Size,
      alignment = alignment_of<type>::value
    };
    
    aligned_storage () {}
    ~aligned_storage () {}
    
  private:
    aligned_storage (const aligned_storage&);
    aligned_storage& operator = (const aligned_storage&);
};

template <size_t Size, size_t Alignment> struct is_pod<aligned_storage<Size, Alignment> >: public true_type {};
template <size_t Size, size_t Alignment> struct is_pod<detail::aligned_storage_impl<Size, Alignment> >: public true_type {};
