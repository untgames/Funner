#include "test.h"

int main ()
{
    printf ("Results of std_container_test:\n");

    typedef vector<int> vec_t;
    vec_t                    vec;
    vec.push_back( 3 );
    vec.push_back( 4 );
    
    const vec_t cvec ( vec ); 

    TEST (( is_same< range_value<vec_t>::type, vec_t::value_type >::value ));
    TEST (( is_same< range_iterator<vec_t>::type, vec_t::iterator >::value ));
    TEST (( is_same< range_const_iterator<vec_t>::type, vec_t::const_iterator >::value ));
    TEST (( is_same< range_difference<vec_t>::type, vec_t::difference_type >::value ));
    TEST (( is_same< range_size<vec_t>::type, vec_t::size_type >::value ));
    TEST (( is_same< range_result_iterator<vec_t>::type, vec_t::iterator >::value ));
    TEST (( is_same< range_result_iterator<const vec_t>::type, vec_t::const_iterator >::value ));
    
    TEST (( is_same< range_value<const vec_t>::type, vec_t::value_type >::value ));
    TEST (( is_same< range_iterator<const vec_t>::type, vec_t::iterator >::value ));
    TEST (( is_same< range_const_iterator<const vec_t>::type, vec_t::const_iterator >::value ));
    TEST (( is_same< range_difference<const vec_t>::type, vec_t::difference_type >::value ));
    TEST (( is_same< range_size<const vec_t>::type, vec_t::size_type >::value ));

    TEST ( begin( vec ) == vec.begin() );
    TEST ( end( vec )   == vec.end() );
    TEST ( empty( vec ) == vec.empty() );
    TEST ( size( vec ) == vec.size() );
    
    TEST ( begin( cvec ) == cvec.begin() );
    TEST ( end( cvec )   == cvec.end() );
    TEST ( empty( cvec ) == cvec.empty() );
    TEST ( size( cvec ) == cvec.size() );
    
    return 0;
}
