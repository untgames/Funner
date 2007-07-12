#include "test.h"

int f( int x )
{
    return x + x;
}

int g( int x )
{
    return 2 * x;
}

int main()
{
    printf ("Results of bind_rel_test:\n");

    int x = 4;
    int y = x + x;

    // bind op value

    TEST ( ( bind( f, _1 ) == y )( x ) );
    TEST ( !( ( bind( f, _1 ) != y )( x ) ) );

    TEST ( !( ( bind( f, _1 ) < y )( x ) ) );
    TEST ( ( bind( f, _1 ) < y + 1 )( x ) );

    TEST ( !( ( bind( f, _1 ) > y )( x ) ) );
    TEST ( ( bind( f, _1 ) > y - 1 )( x ) );

    TEST ( !( ( bind( f, _1 ) <= y - 1 )( x ) ) );
    TEST ( ( bind( f, _1 ) <= y )( x ) );
    TEST ( ( bind( f, _1 ) <= y + 1 )( x ) );

    TEST ( !( ( bind( f, _1 ) >= y + 1 )( x ) ) );
    TEST ( ( bind( f, _1 ) >= y )( x ) );
    TEST ( ( bind( f, _1 ) >= y - 1 )( x ) );

    // bind op ref

    TEST ( ( bind( f, _1 ) == ref( y ) )( x ) );
    TEST ( !( ( bind( f, _1 ) != ref( y ) )( x ) ) );
    TEST ( !( ( bind( f, _1 ) < ref( y ) )( x ) ) );
    TEST ( !( ( bind( f, _1 ) > ref( y ) )( x ) ) );
    TEST ( ( bind( f, _1 ) <= ref( y ) )( x ) );
    TEST ( ( bind( f, _1 ) >= ref( y ) )( x ) );

    // bind op placeholder

    TEST ( ( bind( f, _1 ) == _2 )( x, y ) );
    TEST ( !( ( bind( f, _1 ) != _2 )( x, y ) ) );
    TEST ( !( ( bind( f, _1 ) < _2 )( x, y ) ) );
    TEST ( !( ( bind( f, _1 ) > _2 )( x, y ) ) );
    TEST ( ( bind( f, _1 ) <= _2 )( x, y ) );
    TEST ( ( bind( f, _1 ) >= _2 )( x, y ) );

    // bind op bind

    // important: bind( f, _1 ) and bind( g, _1 ) have the same type
    TEST ( ( bind( f, _1 ) == bind( g, _1 ) )( x ) );
    TEST ( !( ( bind( f, _1 ) != bind( g, _1 ) )( x ) ) );
    TEST ( !( ( bind( f, _1 ) < bind( g, _1 ) )( x ) ) );
    TEST ( ( bind( f, _1 ) <= bind( g, _1 ) )( x ) );
    TEST ( !( ( bind( f, _1 ) > bind( g, _1 ) )( x ) ) );
    TEST ( ( bind( f, _1 ) >= bind( g, _1 ) )( x ) );

    return 0;
}
