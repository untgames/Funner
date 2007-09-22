#include "test.h"

int main ()
{
    printf ("Results of array_test:\n");

    const int sz = 9;
    typedef int   array_t[sz];
    int           my_array[sz]  = { 1,2,3,4,5,6,7,8,9 };
    const array_t ca           = { 1,2,3,4,5,6,7,8,10 }; 

    TEST (begin( my_array ) == my_array );
    TEST (end( my_array ) == my_array + size( my_array ) );
    TEST (empty( my_array ) == false );

    TEST (begin( ca ) == ca );
    TEST (end( ca ) == ca + size( ca ) );
    TEST (empty( ca ) == false);

    return 0;
}
