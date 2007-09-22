#include "test.h"

int main ()
{
    printf ("Results of iterator_range_test:\n");

    typedef string::iterator               iterator;
    typedef string::const_iterator         const_iterator;
    typedef iterator_range<iterator>       irange;
    typedef iterator_range<const_iterator> cirange;
    string       str  = "hello world";
    const string cstr = "const world";
    irange r    = make_iterator_range( str );
    r           = make_iterator_range( str.begin(), str.end() );
    cirange r2  = make_iterator_range( cstr );
    r2          = make_iterator_range( cstr.begin(), cstr.end() );
    r2          = make_iterator_range( str );
 
    TEST ( !r.empty() );
    TEST ( !r2.empty() );

    if( !r )
        TEST ( false && "operator !");
    if( !r2 )
        TEST ( false && "operator !");

    TEST ( r.size() == size( r ) );
    TEST ( r2.size() == size( r2 ) );
    
    TEST ( distance( r.begin(), r.end() ) == distance( begin( r2 ), end( r2 ) ) );
    
    for (irange::iterator i=r.begin (); i!=r.end (); ++i)
      printf ("%c", *i);
      
    printf (" ");
      
    for (cirange::iterator i=r2.begin (); i!=r2.end (); ++i)
      printf ("%c", *i);
      
    printf ("\n");
    
    string res  = copy_range<string>( r );
    TEST ( equal( res.begin(), res.end(), r.begin() ) );

    irange rr = make_iterator_range( str );
    TEST ( rr.equal( r ) );

    rr  = make_iterator_range( str.begin(), str.begin() + 5 );
    TEST ( rr == string ("hello") );
    TEST ( rr != string ("hell") );
    TEST ( rr < string ("hello dude") );
    TEST ( string ("hello") == rr );
    TEST ( string ("hell")  != rr );
    TEST ( ! (string ("hello dude") < rr ) );
    irange rrr = rr;
    TEST ( rrr == rr );
    TEST ( !( rrr != rr ) );
    TEST ( !( rrr < rr ) );

    const irange cr = make_iterator_range( str );
    TEST ( cr.front() == 'h' );
    TEST ( cr.back() == 'd' );
    TEST ( cr[1] == 'e' );

    rrr = make_iterator_range( str, 1, -1 );
    TEST ( rrr == string ("ello worl") );
    rrr = make_iterator_range( rrr, -1, 1 );
    TEST ( rrr == str );
    
    return 0;
}
