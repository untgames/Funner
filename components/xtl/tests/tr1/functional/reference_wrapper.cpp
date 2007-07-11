#include "test.h"

namespace {

template< typename T, typename U >
void ref_test(reference_wrapper<U>)
{
    typedef typename reference_wrapper<U>::type type;
    TEST ((is_same<U,type>::value));
    TEST ((is_same<T,type>::value));
}

template< typename T >
void assignable_test(T x)
{
    x = x;
}

template< typename R, typename Ref >
void cxx_reference_test(Ref)
{
    TEST ((is_same<R,Ref>::value));
}

} // namespace

int main()
{
    printf ("Results of reference_wrapper_test:\n");

    int i = 0;
    int& ri = i;

    int const ci = 0;
    int const& rci = ci;

    // 'ref/cref' functions test
    ref_test<int>(ref(i));
    ref_test<int>(ref(ri));
    ref_test<int const>(ref(ci));
    ref_test<int const>(ref(rci));

    ref_test<int const>(cref(i));
    ref_test<int const>(cref(ri));
    ref_test<int const>(cref(ci));
    ref_test<int const>(cref(rci));

    // test 'assignable' requirement
    assignable_test(ref(i));
    assignable_test(ref(ri));
    assignable_test(cref(i));
    assignable_test(cref(ci));
    assignable_test(cref(rci));

    // ordinary references/function template arguments deduction test
    cxx_reference_test<int>(i);
    cxx_reference_test<int>(ri);
    cxx_reference_test<int>(ci);
    cxx_reference_test<int>(rci);

    cxx_reference_test<int&, int&>(i);
    cxx_reference_test<int&, int&>(ri);
    cxx_reference_test<int const&, int const&>(i);
    cxx_reference_test<int const&, int const&>(ri);
    cxx_reference_test<int const&, int const&>(ci);
    cxx_reference_test<int const&, int const&>(rci);

    return 0;
}
