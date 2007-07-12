#include "test.h"

//

long __fastcall f_0()
{
    return 17041L;
}

long __fastcall f_1(long a)
{
    return a;
}

long __fastcall f_2(long a, long b)
{
    return a + 10 * b;
}

long __fastcall f_3(long a, long b, long c)
{
    return a + 10 * b + 100 * c;
}

long __fastcall f_4(long a, long b, long c, long d)
{
    return a + 10 * b + 100 * c + 1000 * d;
}

long __fastcall f_5(long a, long b, long c, long d, long e)
{
    return a + 10 * b + 100 * c + 1000 * d + 10000 * e;
}

long __fastcall f_6(long a, long b, long c, long d, long e, long f)
{
    return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f;
}

long __fastcall f_7(long a, long b, long c, long d, long e, long f, long g)
{
    return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g;
}

long __fastcall f_8(long a, long b, long c, long d, long e, long f, long g, long h)
{
    return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g + 10000000 * h;
}

long __fastcall f_9(long a, long b, long c, long d, long e, long f, long g, long h, long i)
{
    return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g + 10000000 * h + 100000000 * i;
}

void function_test()
{
    int const i = 1;

    TEST ( bind(f_0)(i) == 17041L );
    TEST ( bind(f_1, _1)(i) == 1L );
    TEST ( bind(f_2, _1, 2)(i) == 21L );
    TEST ( bind(f_3, _1, 2, 3)(i) == 321L );
    TEST ( bind(f_4, _1, 2, 3, 4)(i) == 4321L );
    TEST ( bind(f_5, _1, 2, 3, 4, 5)(i) == 54321L );
    TEST ( bind(f_6, _1, 2, 3, 4, 5, 6)(i) == 654321L );
    TEST ( bind(f_7, _1, 2, 3, 4, 5, 6, 7)(i) == 7654321L );
    TEST ( bind(f_8, _1, 2, 3, 4, 5, 6, 7, 8)(i) == 87654321L );
    TEST ( bind(f_9, _1, 2, 3, 4, 5, 6, 7, 8, 9)(i) == 987654321L );
}

int main()
{
    printf ("Results of bind_fastcall_test:\n");
    function_test();
    return 0;
}
