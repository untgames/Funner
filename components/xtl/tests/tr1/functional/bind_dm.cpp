#include "test.h"

struct X
{
    int m;
};

X f( int v )
{
    X r = { v };
    return r;
}

int main()
{
    printf ("Results of bind_dm_test:\n");

    X x = { 17041 };
    X * px = &x;

    TEST ( bind( &X::m, _1 )( x ) == 17041 );
    TEST ( bind( &X::m, _1 )( px ) == 17041 );

    TEST ( bind( &X::m, x )() == 17041 );
    TEST ( bind( &X::m, px )() == 17041 );
    TEST ( bind( &X::m, ref(x) )() == 17041 );


    X const cx = x;
    X const * pcx = &cx;

    TEST ( bind( &X::m, _1 )( cx ) == 17041 );
    TEST ( bind( &X::m, _1 )( pcx ) == 17041 );

    TEST ( bind( &X::m, cx )() == 17041 );
    TEST ( bind( &X::m, pcx )() == 17041 );
    TEST ( bind( &X::m, ref(cx) )() == 17041 );

    int const v = 42;

    TEST ( bind( &X::m, bind( f, _1 ) )( v ) == v );

    return 0;
}
