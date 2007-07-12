#include "test.h"

struct X
{
    int operator()()
    {
        return 17041;
    }

    int operator()() const
    {
        return -17041;
    }

    int operator()(int x1)
    {
        return x1;
    }

    int operator()(int x1) const
    {
        return -x1;
    }

    int operator()(int x1, int x2)
    {
        return x1+x2;
    }

    int operator()(int x1, int x2) const
    {
        return -(x1+x2);
    }

    int operator()(int x1, int x2, int x3)
    {
        return x1+x2+x3;
    }

    int operator()(int x1, int x2, int x3) const
    {
        return -(x1+x2+x3);
    }

    int operator()(int x1, int x2, int x3, int x4)
    {
        return x1+x2+x3+x4;
    }

    int operator()(int x1, int x2, int x3, int x4) const
    {
        return -(x1+x2+x3+x4);
    }

    int operator()(int x1, int x2, int x3, int x4, int x5)
    {
        return x1+x2+x3+x4+x5;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5) const
    {
        return -(x1+x2+x3+x4+x5);
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6)
    {
        return x1+x2+x3+x4+x5+x6;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6) const
    {
        return -(x1+x2+x3+x4+x5+x6);
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7)
    {
        return x1+x2+x3+x4+x5+x6+x7;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7) const
    {
        return -(x1+x2+x3+x4+x5+x6+x7);
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8)
    {
        return x1+x2+x3+x4+x5+x6+x7+x8;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8) const
    {
        return -(x1+x2+x3+x4+x5+x6+x7+x8);
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9)
    {
        return x1+x2+x3+x4+x5+x6+x7+x8+x9;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9) const
    {
        return -(x1+x2+x3+x4+x5+x6+x7+x8+x9);
    }
};

template<class F> void test(F f, int r)
{
    const F& cf = f;
    TEST ( cf() == -r );
    TEST ( f() == r );
}

int main()
{
    printf ("Results of bind_cv_test:\n");

    test( bind<int>( X() ), 17041 );
    test( bind<int>( X(), 1 ), 1 );
    test( bind<int>( X(), 1, 2 ), 1+2 );
    test( bind<int>( X(), 1, 2, 3 ), 1+2+3 );
    test( bind<int>( X(), 1, 2, 3, 4 ), 1+2+3+4 );
    test( bind<int>( X(), 1, 2, 3, 4, 5 ), 1+2+3+4+5 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6 ), 1+2+3+4+5+6 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7 ), 1+2+3+4+5+6+7 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7, 8 ), 1+2+3+4+5+6+7+8 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7, 8, 9 ), 1+2+3+4+5+6+7+8+9 );

    return 0;
}
