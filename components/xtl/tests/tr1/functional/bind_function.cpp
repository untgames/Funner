#include "test.h"

int f( int x )
{
    return x;
}

int g( int x )
{
    return x + 1;
}

int main()
{
    printf ("Results of bind_function_test:\n");

    function<int ()> fn;

    TEST ( !fn.contains( bind( f, 1 ) ) );
    TEST ( !fn.contains( bind( f, 2 ) ) );
    TEST ( !fn.contains( bind( g, 1 ) ) );

    fn = bind( f, 1 );

    TEST ( fn() == 1 );

    TEST ( fn.contains( bind( f, 1 ) ) );
    TEST ( !fn.contains( bind( f, 2 ) ) );
    TEST ( !fn.contains( bind( g, 1 ) ) );

    fn = bind( f, 2 );

    TEST ( fn() == 2 );

    TEST ( !fn.contains( bind( f, 1 ) ) );
    TEST ( fn.contains( bind( f, 2 ) ) );
    TEST ( !fn.contains( bind( g, 1 ) ) );

    fn = bind( g, 1 );

    TEST ( fn() == 2 );

    TEST ( !fn.contains( bind( f, 1 ) ) );
    TEST ( !fn.contains( bind( f, 2 ) ) );
    TEST ( fn.contains( bind( g, 1 ) ) );

    return 0;
}
