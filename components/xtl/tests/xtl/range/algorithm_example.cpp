#include "test.h"

namespace
{
    //
    // example: extrating bounds in a generic algorithm
    //
    template< typename Range, typename T >
    inline typename range_iterator<Range>::type
    find( Range& c, const T& value )
    {
       return stl::find( begin( c ), end( c ), value );
    }
    
    template< typename Range, typename T >
    inline typename range_const_iterator<Range>::type 
    find( const Range& c, const T& value )
    {
       return stl::find( begin( c ), end( c ), value );
    }
                   
    // 
    // replace first value and return its index
    //                                
    template< class Range, class T >
    inline typename range_size<Range>::type
    my_generic_replace( Range& c, const T& value, const T& replacement )
    {
       typename range_iterator<Range>::type found = find( c, value );
       
       if( found != end( c ) )
           *found = replacement;
       return distance( begin( c ), found );
    }                  
}


int main ()
{
    printf ("Results of algorithm_example_test:\n");

    //
    // usage
    //
    const unsigned N = 5;                     
    vector<int> my_vector;
    int values[] = { 1,2,3,4,5,6,7,8,9 };
    my_vector.assign( values, values + 9 );
    typedef vector<int>::iterator iterator;
    pair<iterator,iterator>       my_view( begin( my_vector ), 
                                                begin( my_vector ) + N );
    char  str_val[] = "a string";
    string str      = str_val;
    
    TEST ( my_generic_replace( my_vector, 4, 2 ) == 3u );
    TEST ( my_generic_replace( my_view, 4, 2 ) == N );
    TEST ( my_generic_replace( str, 'a', 'b' ) == 0u );
    
    return 0;
}
