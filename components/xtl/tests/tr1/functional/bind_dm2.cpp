#include "test.h"

#include <stl/string>

using namespace stl;

struct X
{
    int m;
};

struct Y
{
    char m[ 64 ];
};

int main()
{
    printf ("Results of bind_dm2_test:\n");

    X x = { 0 };
    X * px = &x;

    bind< int& >( &X::m, _1 )( px ) = 42;

    TEST ( x.m == 42 );

    bind< int& >( &X::m, ref(x) )() = 17041;

    TEST ( x.m == 17041 );

    X const * pcx = &x;

    TEST ( bind< long >( &X::m, _1 )( pcx ) == 17041L );
    TEST ( bind< long >( &X::m, pcx )() == 17041L );

    Y y = { "test" };
    string v( "test" );

    TEST ( bind< char const* >( &Y::m, &y )() == v );
    TEST ( bind< string >( &Y::m, &y )() == v );

    return 0;
}
