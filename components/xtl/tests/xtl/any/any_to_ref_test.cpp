#include "shared.h"

void test_cast_to_reference()
{
    int i = 7;
    any a(i), b(a);
    const any c(i);
    
    int&                ra1 = any_cast<int&               >(a);
    int const&          ra2 = any_cast<int const&         >(a);
    int const volatile& ra3 = any_cast<int const volatile&>(a);
    
    TEST (&ra1 == &ra2 && &ra2 == &ra3);

    int&                rb1 = any_cast<int&               >(b);
    int const&          rb2 = any_cast<int const&         >(b);
    int const volatile& rb3 = any_cast<int const volatile&>(b);
    
    TEST (&rb1 == &rb2 && &rb2 == &rb3);
    
    int const&          rc1 = any_cast<int const&         >(c);
    int const volatile& rc2 = any_cast<int const volatile&>(c);
    
    TEST (&rc1 == &rc2);
    
    TEST (&ra1 != &rb1 && &rb1 != &rc1 && &rc1 != &ra1);
    
    TEST (&ra1 != &i && &rb1 != &i && &rc1 != &i);
    
    ++ra1;
    int v = any_cast<int>(a);
    TEST (v == i + 1);
    
    --rb1;
    int v2 = any_cast<int>(b);
    TEST (v2 == i - 1);
    
    TEST_THROW (any_cast<char&>(a), bad_any_cast);
}

int main ()
{
    printf ("Results of any_to_ref_test:\n");

    test_cast_to_reference ();

    return 0;
}
