#include "test.h"

using namespace stl;

struct X
{
    static long instances;

    X()
    {
        ++instances;
    }

    ~X()
    {
        --instances;
    }

    static auto_ptr<X> create()
    {
        return auto_ptr<X>( new X );
    }

private:

    X( X const & );
    X & operator=( X const & );
};

long X::instances = 0;

int main()
{
    printf ("Results of auto_ptr_rv_test:\n");

    TEST ( X::instances == 0 );

    {
        shared_ptr<X> p( X::create() );
        TEST ( X::instances == 1 );

        p = X::create();
        TEST ( X::instances == 1 );

        p.reset();
        TEST ( X::instances == 0 );

        p = X::create();
        TEST ( X::instances == 1 );
    }

    TEST ( X::instances == 0 );

    {
        shared_ptr<X const> p( X::create() );
        TEST ( X::instances == 1 );

        p = X::create();
        TEST ( X::instances == 1 );

        p.reset();
        TEST ( X::instances == 0 );

        p = X::create();
        TEST ( X::instances == 1 );
    }

    TEST ( X::instances == 0 );

    {
        shared_ptr<void> p( X::create() );
        TEST ( X::instances == 1 );

        p = X::create();
        TEST ( X::instances == 1 );

        p.reset();
        TEST ( X::instances == 0 );

        p = X::create();
        TEST ( X::instances == 1 );
    }

    TEST ( X::instances == 0 );

    {
        shared_ptr<void const> p( X::create() );
        TEST ( X::instances == 1 );

        p = X::create();
        TEST ( X::instances == 1 );

        p.reset();
        TEST ( X::instances == 0 );

        p = X::create();
        TEST ( X::instances == 1 );
    }

    TEST ( X::instances == 0 );

    return 0;
}
