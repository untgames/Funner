#include "test.h"

int main ()
{
    printf ("Results of reversible_range_test:\n");

    typedef vector<char>                          vec_t;
    typedef vec_t::iterator                       iterator;
    typedef pair<iterator,iterator>               pair_t;
    typedef range_reverse_iterator<pair_t>::type  rev_iterator;
    typedef pair<rev_iterator,rev_iterator>       rev_pair_t;
    
    vec_t                            vec;
    pair_t                           p    = make_pair( vec.begin(), vec.end() );
    rev_pair_t                       rp   = make_pair( rbegin( p ), rend( p ) );
    string                            str  = "mutable";
    const string                      cstr = "not mutable";
    char                              a[]  = "mutable";
    const char                        ca[] = "not mutable";                        
    wstring                           wstr = L"mutable";
    const wstring                     cwstr= L"not mutable";
    wchar_t                           wa[] = L"mutable";
    const wchar_t                     cwa[]= L"not mutable";
         
    TEST ( rbegin( vec ) == range_reverse_iterator<vec_t>::type( vec.end() ) );
    TEST ( rend( vec ) == range_reverse_iterator<vec_t>::type( vec.begin() ) );
    TEST ( distance( rbegin( vec ), rend( vec ) ) == distance( begin( vec ), end( vec ) ) );

    TEST ( rbegin( p ) == begin( rp ) );
    TEST ( rend( p ) == end( rp ) );
    TEST ( distance( rbegin( p ), rend( p ) ) == distance( begin( rp ), end( rp ) ) );
    TEST ( distance( begin( p ), end( p ) ) == distance( rbegin( rp ), rend( rp ) ) );

    TEST ( &*begin( str ) == &*( rend( str ) - 1 ) );
    TEST ( &*( end( str ) - 1 ) == &*rbegin( str ) );
    TEST ( &*begin( cstr ) == &*( rend( cstr ) - 1 ) );
    TEST ( &*( end( cstr ) - 1 ) == &*rbegin( cstr ) );

    TEST ( &*begin( a ) == &*( rend( a ) - 1 ) );
    TEST ( &*( end( a ) - 1 ) == &*rbegin( a ) );
    TEST ( &*begin( ca ) == &*( rend( ca ) - 1 ) );
    TEST ( &*( end( ca ) - 1 ) == &*rbegin( ca ) );

    TEST ( &*begin( wstr ) == &*( rend( wstr ) - 1 ) );
    TEST ( &*( end( wstr ) - 1 ) == &*rbegin( wstr ) );
    TEST ( &*begin( cwstr ) == &*( rend( cwstr ) - 1 ) );
    TEST ( &*( end( cwstr ) - 1 ) == &*rbegin( cwstr ) );
    
    TEST ( &*begin( wa ) == &*( rend( wa ) - 1 ) );
    TEST ( &*( end( wa ) - 1 ) == &*rbegin( wa ) );
    TEST ( &*begin( cwa ) == &*( rend( cwa ) - 1 ) );
    TEST ( &*( end( cwa ) - 1 ) == &*rbegin( cwa ) );

    return 0;
}
