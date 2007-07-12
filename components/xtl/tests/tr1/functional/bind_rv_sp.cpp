#include "test.h"

struct X
{
    int v_;

    X( int v ): v_( v )
    {
    }

    int f()
    {
        return v_;
    }
};

struct Y
{
    shared_ptr<X> f()
    {
        return shared_ptr<X>( new X( 42 ) );
    }
};

int main()
{
    printf ("Results of bind_rv_sp_test:\n");

    Y y;

    TEST ( bind( &X::f, bind( &Y::f, &y ) )() == 42 );

    return 0;
}
