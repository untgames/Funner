#include "test.h"

enum adl_types
{
    unused,
    xtl_namespace,
    templated_namespace,
    non_templated_namespace,
    global_namespace
};

namespace boost
{
    namespace range_detail
    {
        template< class Range >
        inline typename Range::iterator begin( Range& r )
        {
            return xtl_namespace;  
        }
        
        template< class Range >
        inline typename Range::iterator begin( const Range& r )
        {
            return xtl_namespace;  
        }

    }
    
    template< class Range >
    inline typename Range::iterator begin( Range& r )
    {
        using range_detail::begin; // create ADL hook
        return begin( r );
    }
    
    template< class Range >
    inline typename Range::iterator begin( const Range& r )
    {
        using range_detail::begin; // create ADL hook
        return begin( r );
    }
}


namespace find_templated
{
    template< class T >
    struct range
    {
        typedef adl_types iterator;

        range()                { /* allow const objects */ }
        iterator begin()       { return unused; }
        iterator begin() const { return unused; }
        iterator end()         { return unused; }
        iterator end() const   { return unused; }
    };
        
    //
    // A fully generic version here will create
    // ambiguity.
    //
    template< class T >
    inline typename range<T>::iterator begin( range<T>& r )
    {
        return templated_namespace;
    }
    
    template< class T >
    inline typename range<T>::iterator begin( const range<T>& r )
    {
        return templated_namespace;
    }

}

namespace find_non_templated
{
    struct range
    {
        typedef adl_types iterator;
        
        range()                { /* allow const objects */ }
        iterator begin()       { return unused; }
        iterator begin() const { return unused; }
        iterator end()         { return unused; }
        iterator end() const   { return unused; }
    };
    
    inline range::iterator begin( range& r )
    {
        return non_templated_namespace;
    }
    
    
    inline range::iterator begin( const range& r )
    {
        return non_templated_namespace;
    }
}

struct range
{
    typedef adl_types iterator;

    range()                { /* allow const objects */ }
    iterator begin()       { return unused; }
    iterator begin() const { return unused; }
    iterator end()         { return unused; }
    iterator end() const   { return unused; }
};

inline range::iterator begin( range& r )
{
    return global_namespace;
}   

inline range::iterator begin( const range& r )
{
    return global_namespace;
}   

int main ()
{
    printf ("Results of adl_conformance_test:\n");

    find_templated::range<int>       r;
    const find_templated::range<int> r2;
    find_non_templated::range        r3;
    const find_non_templated::range  r4;
    range                            r5;
    const range                      r6;
    
    //
    // Notice how ADL kicks in even when we have qualified 
    // notation!
    //
    

    TEST ( boost::begin( r )  != xtl_namespace );
    TEST ( boost::begin( r2 ) != xtl_namespace );
    TEST ( boost::begin( r3 ) != xtl_namespace );
    TEST ( boost::begin( r4 ) != xtl_namespace );
    TEST ( boost::begin( r5 ) != xtl_namespace );
    TEST ( boost::begin( r6 ) != xtl_namespace );
    
    TEST ( boost::begin( r ) == templated_namespace ) ;
    TEST ( boost::begin( r2 ) == templated_namespace );
    TEST ( boost::begin( r3 ) == non_templated_namespace );
    TEST ( boost::begin( r4 ) == non_templated_namespace );
    TEST ( boost::begin( r5 ) == global_namespace );
    TEST ( boost::begin( r6 ) == global_namespace );
    
    return 0;
}
