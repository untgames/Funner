#include "test.h"

//
// Generic range algorithm
//
template< class Rng >
typename range_result_iterator<Rng>::type foo_algo( Rng& r )
{
        //
        // This will only compile for Rng = UDT if the qualified calls
        // find xtl_range_XXX via ADL.
        //
        return size(r) == 0u ? begin(r) : end(r);
}

namespace Foo
{
        //
        // Our sample UDT
        //
        struct X
        {
                typedef vector<int>       data_t;
                typedef data_t::iterator       iterator;
                typedef data_t::const_iterator const_iterator;
                typedef data_t::size_type      size_type;

                data_t vec;

                void push_back( int i )
                { vec.push_back(i); }
        };

        //
        // The required functions. No type-traits need
        // to be defined because X defines the proper set of
        // nested types.
        //
        inline X::iterator begin( X& x )
        {
                return x.vec.begin();
        }


        inline X::const_iterator begin( const X& x )
        {
                return x.vec.begin();
        }


    inline X::iterator end( X& x )
        {
                return x.vec.end();
        }

        inline X::const_iterator end( const X& x )
        {
                return x.vec.end();
        }

        inline X::size_type size( const X& x )
        {
                return x.vec.size();
        }
}

int main ()
{
        printf ("Results of extension_mechanism_test:\n");

        Foo::X x;
        x.push_back(3);
        const Foo::X x2;

        foo_algo( x );
        foo_algo( x2 );
        
        return 0;
}
