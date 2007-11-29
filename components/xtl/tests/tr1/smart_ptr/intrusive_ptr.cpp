#include "test.h"

#include <stl/algorithm>
#include <stl/functional>

using namespace stl;

using xtl::intrusive_ptr;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

//

namespace test
{

namespace N
{

class base
{
private:

    size_t use_count_;

    base(base const &);
    base & operator=(base const &);

protected:

    base(): use_count_(0)
    {
    }

    virtual ~base()
    {
    }

public:

    long use_count() const
    {
        return use_count_;
    }

#if !defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)

    inline friend void intrusive_ptr_add_ref(base * p)
    {
        ++p->use_count_;
    }

    inline friend void intrusive_ptr_release(base * p)
    {
        if(--p->use_count_ == 0) delete p;
    }

#else

    void add_ref()
    {
        ++use_count_;
    }

    void release()
    {
        if(--use_count_ == 0) delete this;
    }

#endif
};

} // namespace N

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)

namespace boost
{

inline void intrusive_ptr_add_ref(N::base * p)
{
    p->add_ref();
}

inline void intrusive_ptr_release(N::base * p)
{
    p->release();
}

} // namespace boost

#endif

//

struct X: public virtual N::base
{
};

struct Y: public X
{
};

//

namespace n_element_type
{

void f(X &)
{
}

void test()
{
    typedef intrusive_ptr<X>::element_type T;
    T t;
    f(t);
}

} // namespace n_element_type

namespace n_constructors
{

void default_constructor()
{
    intrusive_ptr<X> px;
    TEST (px.get() == 0);
}

void pointer_constructor()
{
    {
        intrusive_ptr<X> px(0);
        TEST (px.get() == 0);
    }

    {
        intrusive_ptr<X> px(0, false);
        TEST (px.get() == 0);
    }

    {
        X * p = new X;
        TEST (p->use_count() == 0);

        intrusive_ptr<X> px(p);
        TEST (px.get() == p);
        TEST (px->use_count() == 1);
    }

    {
        X * p = new X;
        TEST (p->use_count() == 0);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using intrusive_ptr_add_ref;
#endif
        intrusive_ptr_add_ref(p);
        TEST (p->use_count() == 1);

        intrusive_ptr<X> px(p, false);
        TEST (px.get() == p);
        TEST (px->use_count() == 1);
    }
}

void copy_constructor()
{
    {
        intrusive_ptr<X> px;
        intrusive_ptr<X> px2(px);
        TEST (px2.get() == px.get());
    }

    {
        intrusive_ptr<Y> py;
        intrusive_ptr<X> px(py);
        TEST (px.get() == py.get());
    }

    {
        intrusive_ptr<X> px(0);
        intrusive_ptr<X> px2(px);
        TEST (px2.get() == px.get());
    }

    {
        intrusive_ptr<Y> py(0);
        intrusive_ptr<X> px(py);
        TEST (px.get() == py.get());
    }

    {
        intrusive_ptr<X> px(0, false);
        intrusive_ptr<X> px2(px);
        TEST (px2.get() == px.get());
    }

    {
        intrusive_ptr<Y> py(0, false);
        intrusive_ptr<X> px(py);
        TEST (px.get() == py.get());
    }

    {
        intrusive_ptr<X> px(new X);
        intrusive_ptr<X> px2(px);
        TEST (px2.get() == px.get());
    }

    {
        intrusive_ptr<Y> py(new Y);
        intrusive_ptr<X> px(py);
        TEST (px.get() == py.get());
    }
}

void test()
{
    default_constructor();
    pointer_constructor();
    copy_constructor();
}

} // namespace n_constructors

namespace n_destructor
{

void test()
{
    intrusive_ptr<X> px(new X);
    TEST (px->use_count() == 1);

    {
        intrusive_ptr<X> px2(px);
        TEST (px->use_count() == 2);
    }

    TEST (px->use_count() == 1);
}

} // namespace n_destructor

namespace n_assignment
{

void copy_assignment()
{
}

void conversion_assignment()
{
}

void pointer_assignment()
{
}

void test()
{
    copy_assignment();
    conversion_assignment();
    pointer_assignment();
}

} // namespace n_assignment

namespace n_access
{

void test()
{
    {
        intrusive_ptr<X> px;
        TEST (px? false: true);
        TEST (!px);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using get_pointer;
#endif

        TEST (get_pointer(px) == px.get());
    }

    {
        intrusive_ptr<X> px(0);
        TEST (px? false: true);
        TEST (!px);

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using get_pointer;
#endif

        TEST (get_pointer(px) == px.get());
    }

    {
        intrusive_ptr<X> px(new X);
        TEST (px? true: false);
        TEST (!!px);
        TEST (&*px == px.get());
        TEST (px.operator ->() == px.get());

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
        using get_pointer;
#endif

        TEST (get_pointer(px) == px.get());
    }
}

} // namespace n_access

namespace n_swap
{

void test()
{
    {
        intrusive_ptr<X> px;
        intrusive_ptr<X> px2;

        px.swap(px2);

        TEST (px.get() == 0);
        TEST (px2.get() == 0);

        using stl::swap;
        swap(px, px2);

        TEST (px.get() == 0);
        TEST (px2.get() == 0);
    }

    {
        X * p = new X;
        intrusive_ptr<X> px;
        intrusive_ptr<X> px2(p);
        intrusive_ptr<X> px3(px2);

        px.swap(px2);

        TEST (px.get() == p);
        TEST (px->use_count() == 2);
        TEST (px2.get() == 0);
        TEST (px3.get() == p);
        TEST (px3->use_count() == 2);

        using stl::swap;
        swap(px, px2);

        TEST (px.get() == 0);
        TEST (px2.get() == p);
        TEST (px2->use_count() == 2);
        TEST (px3.get() == p);
        TEST (px3->use_count() == 2);
    }

    {
        X * p1 = new X;
        X * p2 = new X;
        intrusive_ptr<X> px(p1);
        intrusive_ptr<X> px2(p2);
        intrusive_ptr<X> px3(px2);

        px.swap(px2);

        TEST (px.get() == p2);
        TEST (px->use_count() == 2);
        TEST (px2.get() == p1);
        TEST (px2->use_count() == 1);
        TEST (px3.get() == p2);
        TEST (px3->use_count() == 2);

        using stl::swap;
        swap(px, px2);

        TEST (px.get() == p1);
        TEST (px->use_count() == 1);
        TEST (px2.get() == p2);
        TEST (px2->use_count() == 2);
        TEST (px3.get() == p2);
        TEST (px3->use_count() == 2);
    }
}

} // namespace n_swap

namespace n_comparison
{

template<class T, class U> void test2(intrusive_ptr<T> const & p, intrusive_ptr<U> const & q)
{
    TEST ((p == q) == (p.get() == q.get()));
    TEST ((p != q) == (p.get() != q.get()));
}

template<class T> void test3(intrusive_ptr<T> const & p, intrusive_ptr<T> const & q)
{
    TEST ((p == q) == (p.get() == q.get()));
    TEST ((p.get() == q) == (p.get() == q.get()));
    TEST ((p == q.get()) == (p.get() == q.get()));
    TEST ((p != q) == (p.get() != q.get()));
    TEST ((p.get() != q) == (p.get() != q.get()));
    TEST ((p != q.get()) == (p.get() != q.get()));

    // 'less' moved here as a g++ 2.9x parse error workaround
    less<T*> less;
    TEST ((p < q) == less(p.get(), q.get()));
}

void test()
{
    {
        intrusive_ptr<X> px;
        test3(px, px);

        intrusive_ptr<X> px2;
        test3(px, px2);

        intrusive_ptr<X> px3(px);
        test3(px3, px3);
        test3(px, px3);
    }

    {
        intrusive_ptr<X> px;

        intrusive_ptr<X> px2(new X);
        test3(px, px2);
        test3(px2, px2);

        intrusive_ptr<X> px3(new X);
        test3(px2, px3);

        intrusive_ptr<X> px4(px2);
        test3(px2, px4);
        test3(px4, px4);
    }

    {
        intrusive_ptr<X> px(new X);

        intrusive_ptr<Y> py(new Y);
        test2(px, py);

        intrusive_ptr<X> px2(py);
        test2(px2, py);
        test3(px, px2);
        test3(px2, px2);
    }
}

} // namespace n_comparison

namespace n_static_cast
{

void test()
{
}

} // namespace n_static_cast

namespace n_dynamic_cast
{

void test()
{
}

} // namespace n_dynamic_cast

namespace n_transitive
{

struct X: public N::base
{
    intrusive_ptr<X> next;
};

void test()
{
    intrusive_ptr<X> p(new X);
    p->next = intrusive_ptr<X>(new X);
    TEST (!p->next->next);
    p = p->next;
    TEST (!p->next);
}

} // namespace n_transitive

namespace n_report_1
{

class foo: public N::base
{ 
public: 

    foo(): m_self(this)
    {
    } 

    void suicide()
    {
        m_self = 0;
    }

private:

    intrusive_ptr<foo> m_self;
}; 

void test()
{
    foo * foo_ptr = new foo;
    foo_ptr->suicide();
} 

} // namespace n_report_1

} //namespace test

int main()
{
    printf ("Results of intrusive_ptr_test:\n");

    using namespace test;

    n_element_type::test();
    n_constructors::test();
    n_destructor::test();
    n_assignment::test();
    n_access::test();
    n_swap::test();
    n_comparison::test();
    n_static_cast::test();
    n_dynamic_cast::test();

    n_transitive::test();
    n_report_1::test();

    return 0;
}
