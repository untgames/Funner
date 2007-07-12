#include "test.h"

class X
{
private:

    int state_;

public:

    X(): state_(0)
    {
    }

    int state() const
    {
        return state_;
    }

    int operator()()
    {
        return state_ += 17041;
    }

    int operator()(int x1)
    {
        return state_ += x1;
    }

    int operator()(int x1, int x2)
    {
        return state_ += x1+x2;
    }

    int operator()(int x1, int x2, int x3)
    {
        return state_ += x1+x2+x3;
    }

    int operator()(int x1, int x2, int x3, int x4)
    {
        return state_ += x1+x2+x3+x4;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5)
    {
        return state_ += x1+x2+x3+x4+x5;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6)
    {
        return state_ += x1+x2+x3+x4+x5+x6;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7)
    {
        return state_ += x1+x2+x3+x4+x5+x6+x7;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8)
    {
        return state_ += x1+x2+x3+x4+x5+x6+x7+x8;
    }

    int operator()(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9)
    {
        return state_ += x1+x2+x3+x4+x5+x6+x7+x8+x9;
    }
};

int f0(int & state_)
{
    return state_ += 17041;
}

int f1(int & state_, int x1)
{
    return state_ += x1;
}

int f2(int & state_, int x1, int x2)
{
    return state_ += x1+x2;
}

int f3(int & state_, int x1, int x2, int x3)
{
    return state_ += x1+x2+x3;
}

int f4(int & state_, int x1, int x2, int x3, int x4)
{
    return state_ += x1+x2+x3+x4;
}

int f5(int & state_, int x1, int x2, int x3, int x4, int x5)
{
    return state_ += x1+x2+x3+x4+x5;
}

int f6(int & state_, int x1, int x2, int x3, int x4, int x5, int x6)
{
    return state_ += x1+x2+x3+x4+x5+x6;
}

int f7(int & state_, int x1, int x2, int x3, int x4, int x5, int x6, int x7)
{
    return state_ += x1+x2+x3+x4+x5+x6+x7;
}

int f8(int & state_, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8)
{
    return state_ += x1+x2+x3+x4+x5+x6+x7+x8;
}

template<class F> void test(F f, int a, int b)
{
    TEST ( f() == a +   b );
    TEST ( f() == a + 2*b );
    TEST ( f() == a + 3*b );
}

void stateful_function_object_test()
{
    test( bind<int>( X() ), 0, 17041 );
    test( bind<int>( X(), 1 ), 0, 1 );
    test( bind<int>( X(), 1, 2 ), 0, 1+2 );
    test( bind<int>( X(), 1, 2, 3 ), 0, 1+2+3 );
    test( bind<int>( X(), 1, 2, 3, 4 ), 0, 1+2+3+4 );
    test( bind<int>( X(), 1, 2, 3, 4, 5 ), 0, 1+2+3+4+5 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6 ), 0, 1+2+3+4+5+6 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7 ), 0, 1+2+3+4+5+6+7 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7, 8 ), 0, 1+2+3+4+5+6+7+8 );
    test( bind<int>( X(), 1, 2, 3, 4, 5, 6, 7, 8, 9 ), 0, 1+2+3+4+5+6+7+8+9 );

    X x;

    int n = x.state();

    test( bind<int>( ref(x) ), n, 17041 );
    n += 3*17041;

    test( bind<int>( ref(x), 1 ), n, 1 );
    n += 3*1;

    test( bind<int>( ref(x), 1, 2 ), n, 1+2 );
    n += 3*(1+2);

    test( bind<int>( ref(x), 1, 2, 3 ), n, 1+2+3 );
    n += 3*(1+2+3);

    test( bind<int>( ref(x), 1, 2, 3, 4 ), n, 1+2+3+4 );
    n += 3*(1+2+3+4);

    test( bind<int>( ref(x), 1, 2, 3, 4, 5 ), n, 1+2+3+4+5 );
    n += 3*(1+2+3+4+5);

    test( bind<int>( ref(x), 1, 2, 3, 4, 5, 6 ), n, 1+2+3+4+5+6 );
    n += 3*(1+2+3+4+5+6);

    test( bind<int>( ref(x), 1, 2, 3, 4, 5, 6, 7 ), n, 1+2+3+4+5+6+7 );
    n += 3*(1+2+3+4+5+6+7);

    test( bind<int>( ref(x), 1, 2, 3, 4, 5, 6, 7, 8 ), n, 1+2+3+4+5+6+7+8 );
    n += 3*(1+2+3+4+5+6+7+8);

    test( bind<int>( ref(x), 1, 2, 3, 4, 5, 6, 7, 8, 9 ), n, 1+2+3+4+5+6+7+8+9 );
    n += 3*(1+2+3+4+5+6+7+8+9);

    TEST( x.state() == n );
}

void stateful_function_test()
{
    test( bind( f0, 0 ), 0, 17041 );
    test( bind( f1, 0, 1 ), 0, 1 );
    test( bind( f2, 0, 1, 2 ), 0, 1+2 );
    test( bind( f3, 0, 1, 2, 3 ), 0, 1+2+3 );
    test( bind( f4, 0, 1, 2, 3, 4 ), 0, 1+2+3+4 );
    test( bind( f5, 0, 1, 2, 3, 4, 5 ), 0, 1+2+3+4+5 );
    test( bind( f6, 0, 1, 2, 3, 4, 5, 6 ), 0, 1+2+3+4+5+6 );
    test( bind( f7, 0, 1, 2, 3, 4, 5, 6, 7 ), 0, 1+2+3+4+5+6+7 );
    test( bind( f8, 0, 1, 2, 3, 4, 5, 6, 7, 8 ), 0, 1+2+3+4+5+6+7+8 );
}

int main()
{
    printf ("Results of bind_stateful_test:\n");

    stateful_function_object_test();
    stateful_function_test();
    return 0;
}
