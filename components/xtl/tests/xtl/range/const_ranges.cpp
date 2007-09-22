#include "test.h"

template< class T >
const T& as_const( const T& r )
{
    return r;
}

int main ()
{
    printf ("Results of const_ranges:\n");

    string       foo( "foo" );
    const string bar( "bar" ); 

    TEST ( const_begin( foo )  == begin( as_const( foo ) ) );
    TEST ( const_end( foo )    == end( as_const( foo ) ) );
    TEST ( const_rbegin( foo ) == rbegin( as_const( foo ) ) );
    TEST ( const_rend( foo )   == rend( as_const( foo ) ) );

    TEST ( const_begin( bar )  == begin( as_const( bar ) ) );
    TEST ( const_end( bar )    == end( as_const( bar ) ) );
    TEST ( const_rbegin( bar ) == rbegin( as_const( bar ) ) );
    TEST ( const_rend( bar )   == rend( as_const( bar ) ) );
    
    return 0;
}
