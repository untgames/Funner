#include "test.h"

class X
{
private:

    void operator& ();
    void operator& () const;

public:

    void operator()()
    {
    }

    void operator()() const
    {
    }

    void operator()(int)
    {
    }

    void operator()(int) const
    {
    }

    void operator()(int, int)
    {
    }

    void operator()(int, int) const
    {
    }

    void operator()(int, int, int)
    {
    }

    void operator()(int, int, int) const
    {
    }

    void operator()(int, int, int, int)
    {
    }

    void operator()(int, int, int, int) const
    {
    }

    void operator()(int, int, int, int, int)
    {
    }

    void operator()(int, int, int, int, int) const
    {
    }

    void operator()(int, int, int, int, int, int)
    {
    }

    void operator()(int, int, int, int, int, int) const
    {
    }

    void operator()(int, int, int, int, int, int, int)
    {
    }

    void operator()(int, int, int, int, int, int, int) const
    {
    }

    void operator()(int, int, int, int, int, int, int, int)
    {
    }

    void operator()(int, int, int, int, int, int, int, int) const
    {
    }

    void operator()(int, int, int, int, int, int, int, int, int)
    {
    }

    void operator()(int, int, int, int, int, int, int, int, int) const
    {
    }
};

template<class F> void test_const( F const & f )
{
    f();
}

template<class F> void test( F f )
{
    f();
    test_const( f );
}

int main()
{
    printf ("Results of bind_unary_addr_test:\n");

    test( bind<void>( X() ) );
    test( bind<void>( X(), 1 ) );
    test( bind<void>( X(), 1, 2 ) );
    test( bind<void>( X(), 1, 2, 3 ) );
    test( bind<void>( X(), 1, 2, 3, 4 ) );
    test( bind<void>( X(), 1, 2, 3, 4, 5 ) );
    test( bind<void>( X(), 1, 2, 3, 4, 5, 6 ) );
    test( bind<void>( X(), 1, 2, 3, 4, 5, 6, 7 ) );
    test( bind<void>( X(), 1, 2, 3, 4, 5, 6, 7, 8 ) );
    test( bind<void>( X(), 1, 2, 3, 4, 5, 6, 7, 8, 9 ) );

    return 0;
}
