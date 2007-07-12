#include "test.h"
#include <stl/utility>

using namespace stl;

int main()
{
    printf ("Results of bind_dm3_test:\n");

    typedef pair<int, int> pair_type;

    pair_type pair( 10, 20 );

    int const & x = bind( &pair_type::first, _1 )( pair );

    TEST ( &pair.first == &x );

    return 0;
}
