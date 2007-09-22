#include "test.h"

int main ()
{
    printf ("Results of iterator_pair_test:\n");

    typedef vector<int> vec_t;
    vec_t                    vec;
    vec.push_back( 4 );
    typedef pair<vec_t::iterator,vec_t::iterator>
                             pair_t;
    typedef pair<vec_t::const_iterator,vec_t::const_iterator>
                             const_pair_t;
    typedef const pair_t     const_pair_tt;
    pair_t                   pair       = make_pair( begin( vec ), end( vec ) );
    const_pair_t             const_pair = make_pair( begin( vec ), end( vec ) );
    const_pair_tt            constness_pair( pair );
    

    TEST (( is_same< range_value<pair_t>::type, 
                          iterator_traits<pair_t::first_type>::value_type>::value ));
    TEST (( is_same< range_iterator<pair_t>::type, pair_t::first_type >::value ));
    TEST (( is_same< range_const_iterator<pair_t>::type, pair_t::first_type >::value ));
    TEST (( is_same< range_difference<pair_t>::type,                           
                          iterator_traits<pair_t::first_type>::difference_type >::value ));
    TEST (( is_same< range_size<pair_t>::type, size_t >::value ));
    TEST (( is_same< range_result_iterator<pair_t>::type, pair_t::first_type >::value ));
    TEST (( is_same< range_result_iterator<const_pair_t>::type, const_pair_t::first_type >::value ));
    
    TEST (( is_same< range_value<const_pair_tt>::type, 
                          iterator_traits<const_pair_t::first_type>::value_type>::value ));
    TEST (( is_same< range_iterator<const_pair_tt>::type, const_pair_tt::first_type >::value ));
    TEST (( is_same< range_const_iterator<const_pair_tt>::type, const_pair_tt::first_type >::value ));
    TEST (( is_same< range_difference<const_pair_tt>::type,                           
                          iterator_traits<const_pair_tt::first_type>::difference_type >::value ));
    TEST (( is_same< range_size<const_pair_tt>::type, size_t >::value ));
    TEST (( is_same< range_result_iterator<const_pair_tt>::type, const_pair_tt::first_type >::value ));
    TEST (( is_same< range_result_iterator<const_pair_tt>::type, const_pair_tt::first_type >::value ));

    TEST ( begin( pair ) == pair.first );
    TEST ( end( pair )   == pair.second );
    TEST ( empty( pair ) == (pair.first == pair.second) );
    TEST ( size( pair )  == size_t( distance( pair.first, pair.second ) ) );
    
    TEST ( begin( const_pair ) == const_pair.first );
    TEST ( end( const_pair )   == const_pair.second );
    TEST ( empty( const_pair ) == (const_pair.first == const_pair.second) );
    TEST ( size( const_pair )  == size_t( distance( const_pair.first, const_pair.second ) ) );

    TEST ( begin( constness_pair ) == constness_pair.first );
    TEST ( end( constness_pair )   == constness_pair.second );
    TEST ( empty( constness_pair ) == (constness_pair.first == const_pair.second) );
    TEST ( size( constness_pair )  == size_t( distance( constness_pair.first, constness_pair.second ) ) );

    return 0;
}
