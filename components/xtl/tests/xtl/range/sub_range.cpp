#include "test.h"

int main ()
{
    printf ("Results of sub_range_test:\n");
      
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
 
    typedef sub_range<string>       srange;
    typedef sub_range<const string> csrange;
    srange s     = r;
    TEST ( r == r );
    TEST ( s == r );
    s            = make_iterator_range( str );
    csrange s2   = r;
    s2           = r2;
    s2           = make_iterator_range( cstr );
    TEST ( r2 == r2 );
    TEST ( s2 != r2 );
    s2           = make_iterator_range( str );
    TEST ( !(s != s) );
    
    TEST ( r.begin() == s.begin() );
    TEST ( r2.begin()== s2.begin() );
    TEST ( r.end()   == s.end() );
    TEST ( r2.end()  == s2.end() );
    TEST ( r.size() == s.size() );
    TEST ( r2.size() == s2.size() );
    
    if( !r )
        TEST ( false );
    if( !r2 )
        TEST ( false );
    if( !s )
        TEST ( false );
    if( !s2 )
        TEST ( false );
    
    for (irange::iterator i=r.begin (); i!=r.end (); ++i)
      printf ("%c", *i);
      
    printf (" ");
      
    for (cirange::iterator i=r2.begin (); i!=r2.end (); ++i)
      printf ("%c", *i);
      
    printf (" ");      
    
    for (srange::iterator i=s.begin (); i!=s.end (); ++i)
      printf ("%c", *i);    
      
    printf (" ");            
    
    for (csrange::iterator i=s2.begin (); i!=s2.end (); ++i)
      printf ("%c", *i);        
      
    printf ("\n");    
    
    string res  = copy_range<string>( r );
    TEST ( equal( res.begin(), res.end(), r.begin() ) );
    
    r.empty();
    s.empty();
    r.size();
    s.size();
    
    irange singular_irange;
    TEST ( singular_irange.empty() );
    TEST ( singular_irange.size() == 0 );
    
    srange singular_srange;
    TEST ( singular_srange.empty() );
    TEST ( singular_srange.size() == 0 );

    TEST ( empty( singular_irange ) );
    TEST ( empty( singular_srange ) );

    srange rr = make_iterator_range( str );
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
    rrr.front() = 'H';
    rrr.back()  = 'D';
    rrr[1]      = 'E';

    TEST ( rrr == string ("HEllo worlD") );
    
    return 0;
}   
