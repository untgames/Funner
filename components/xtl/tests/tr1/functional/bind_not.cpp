#include "test.h"

template<class F, class A1, class R> void test( F f, A1 a1, R r )
{
    TEST ( f(a1) == r );
}

bool f( bool v )
{
    return v;
}

int g( int v )
{
    return v;
}

int main()
{
    printf ("Results of bind_not_test:\n");

    test( !bind( f, true ), 0, !f( true ) );
    test( !bind( g, _1 ), 5, !g( 5 ) );
    test( bind( f, !bind( f, true ) ), 0, f( !f( true ) ) );
    test( bind( f, !bind( f, _1 ) ), true, f( !f( true ) ) );

    return 0;
}
