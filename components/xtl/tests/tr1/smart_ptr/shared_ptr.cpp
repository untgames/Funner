#include "test.h"

#include <stl/map>
#include <stl/vector>

#ifdef _MSC_VER
  #pragma warning (disable : 4355)
#endif

using namespace stl;

template <class T> struct checked_deleter
{
  void operator () (T* p) const { delete p; }
};

template <class T> struct checked_array_deleter
{
  void operator () (T* p) const { delete [] p; }
};

namespace test
{


namespace n_element_type
{

void f(int &)
{
}

void test()
{
    typedef shared_ptr<int>::element_type T;
    T t;
    f(t);
}

} // namespace n_element_type

namespace n_constructors
{

class incomplete;

void default_constructor()
{
    {
        shared_ptr<int> pi;
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
    }

    {
        shared_ptr<void> pv;
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
    }

    {
        shared_ptr<incomplete> px;
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
    }
}

struct A
{
    int dummy;
};

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

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

// virtual inheritance stresses the implementation

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

template<class T> void pc0_test(T * p)
{
    TEST (p == 0);
    shared_ptr<T> pt(p);
    TEST (pt? false: true);
    TEST (!pt);
    TEST (pt.get() == 0);
    TEST (pt.use_count() == 1);
    TEST (pt.unique());
}

void pointer_constructor()
{
    pc0_test(static_cast<int*>(0));

    pc0_test(static_cast<int const*>(0));
    pc0_test(static_cast<int volatile*>(0));
    pc0_test(static_cast<int const volatile*>(0));

    {
        shared_ptr<int const> pi(static_cast<int*>(0));
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    {
        shared_ptr<int volatile> pi(static_cast<int*>(0));
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    {
        shared_ptr<void> pv(static_cast<int*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        shared_ptr<void const> pv(static_cast<int*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    pc0_test(static_cast<X*>(0));
    pc0_test(static_cast<X const*>(0));
    pc0_test(static_cast<X volatile*>(0));
    pc0_test(static_cast<X const volatile*>(0));

    {
        shared_ptr<X const> px(static_cast<X*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
    }

    {
        shared_ptr<X> px(static_cast<Y*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
    }

    {
        shared_ptr<X const> px(static_cast<Y*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
    }

    {
        shared_ptr<void> pv(static_cast<X*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        shared_ptr<void const> pv(static_cast<X*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        int * p = new int(7);
        shared_ptr<int> pi(p);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == p);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (*pi == 7);
    }

    {
        int * p = new int(7);
        shared_ptr<int const> pi(p);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == p);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (*pi == 7);
    }

    {
        int * p = new int(7);
        shared_ptr<void> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == p);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        int * p = new int(7);
        shared_ptr<void const> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == p);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    TEST (X::instances == 0);

    {
        X * p = new X;
        shared_ptr<X> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
    }

    TEST (X::instances == 0);

    {
        X * p = new X;
        shared_ptr<X const> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
    }

    TEST (X::instances == 0);

    {
        X * p = new X;
        shared_ptr<void> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == p);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);
    }

    TEST (X::instances == 0);

    {
        X * p = new X;
        shared_ptr<void const> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == p);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);

    {
        Y * p = new Y;
        shared_ptr<X> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);

    {
        Y * p = new Y;
        shared_ptr<X const> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);
}

int m = 0;

void deleter(int * p)
{
    TEST (p == 0);
}

void deleter2(int * p)
{
    TEST (p == &m);
    ++*p;
}

struct deleter3
{
    void operator()(incomplete * p)
    {
        TEST (p == 0);
    }
};

// Borland C++ 5.5.1 fails on static_cast<incomplete*>(0)

incomplete * p0 = 0;

void deleter_constructor()
{
    {
        shared_ptr<int> pi(static_cast<int*>(0), deleter);
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    {
        shared_ptr<void> pv(static_cast<int*>(0), &deleter);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        shared_ptr<void const> pv(static_cast<int*>(0), deleter);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        shared_ptr<incomplete> px(p0, deleter3());
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
    }

    {
        shared_ptr<void> pv(p0, deleter3());
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    {
        shared_ptr<void const> pv(p0, deleter3());
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    TEST (m == 0);

    {
        shared_ptr<int> pi(&m, deleter2);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == &m);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    TEST (m == 1);

    {
        shared_ptr<int const> pi(&m, &deleter2);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == &m);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    TEST (m == 2);

    {
        shared_ptr<void> pv(&m, deleter2);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == &m);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    TEST (m == 3);

    {
        shared_ptr<void const> pv(&m, &deleter2);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == &m);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
    }

    TEST (m == 4);
}

void copy_constructor()
{
    {
        shared_ptr<int> pi;

        shared_ptr<int> pi2(pi);
        TEST (pi2 == pi);
        TEST (pi2? false: true);
        TEST (!pi2);
        TEST (pi2.get() == 0);
        TEST (pi2.use_count() == pi.use_count());

        shared_ptr<void> pi3(pi);
        TEST (pi3 == pi);
        TEST (pi3? false: true);
        TEST (!pi3);
        TEST (pi3.get() == 0);
        TEST (pi3.use_count() == pi.use_count());

        shared_ptr<void> pi4(pi3);
        TEST (pi4 == pi3);
        TEST (pi4? false: true);
        TEST (!pi4);
        TEST (pi4.get() == 0);
        TEST (pi4.use_count() == pi3.use_count());
    }

    {
        shared_ptr<void> pv;

        shared_ptr<void> pv2(pv);
        TEST (pv2 == pv);
        TEST (pv2? false: true);
        TEST (!pv2);
        TEST (pv2.get() == 0);
        TEST (pv2.use_count() == pv.use_count());
    }

    {
        shared_ptr<incomplete> px;

        shared_ptr<incomplete> px2(px);
        TEST (px2 == px);
        TEST (px2? false: true);
        TEST (!px2);
        TEST (px2.get() == 0);
        TEST (px2.use_count() == px.use_count());

        shared_ptr<void> px3(px);
        TEST (px3 == px);
        TEST (px3? false: true);
        TEST (!px3);
        TEST (px3.get() == 0);
        TEST (px3.use_count() == px.use_count());
    }

    {
        shared_ptr<int> pi(static_cast<int*>(0));

        shared_ptr<int> pi2(pi);
        TEST (pi2 == pi);
        TEST (pi2? false: true);
        TEST (!pi2);
        TEST (pi2.get() == 0);
        TEST (pi2.use_count() == 2);
        TEST (!pi2.unique());
        TEST (pi2.use_count() == pi.use_count());
        TEST (!(pi < pi2 || pi2 < pi)); // shared ownership test

        shared_ptr<void> pi3(pi);
        TEST (pi3 == pi);
        TEST (pi3? false: true);
        TEST (!pi3);
        TEST (pi3.get() == 0);
        TEST (pi3.use_count() == 3);
        TEST (!pi3.unique());
        TEST (pi3.use_count() == pi.use_count());
        TEST (!(pi < pi3 || pi3 < pi)); // shared ownership test

        shared_ptr<void> pi4(pi2);
        TEST (pi4 == pi2);
        TEST (pi4? false: true);
        TEST (!pi4);
        TEST (pi4.get() == 0);
        TEST (pi4.use_count() == 4);
        TEST (!pi4.unique());
        TEST (pi4.use_count() == pi2.use_count());
        TEST (!(pi2 < pi4 || pi4 < pi2)); // shared ownership test

        TEST (pi3.use_count() == pi4.use_count());
        TEST (!(pi3 < pi4 || pi4 < pi3)); // shared ownership test
    }

    {
        shared_ptr<X> px(static_cast<X*>(0));

        shared_ptr<X> px2(px);
        TEST (px2 == px);
        TEST (px2? false: true);
        TEST (!px2);
        TEST (px2.get() == 0);
        TEST (px2.use_count() == 2);
        TEST (!px2.unique());
        TEST (px2.use_count() == px.use_count());
        TEST (!(px < px2 || px2 < px)); // shared ownership test

        shared_ptr<void> px3(px);
        TEST (px3 == px);
        TEST (px3? false: true);
        TEST (!px3);
        TEST (px3.get() == 0);
        TEST (px3.use_count() == 3);
        TEST (!px3.unique());
        TEST (px3.use_count() == px.use_count());
        TEST (!(px < px3 || px3 < px)); // shared ownership test

        shared_ptr<void> px4(px2);
        TEST (px4 == px2);
        TEST (px4? false: true);
        TEST (!px4);
        TEST (px4.get() == 0);
        TEST (px4.use_count() == 4);
        TEST (!px4.unique());
        TEST (px4.use_count() == px2.use_count());
        TEST (!(px2 < px4 || px4 < px2)); // shared ownership test

        TEST (px3.use_count() == px4.use_count());
        TEST (!(px3 < px4 || px4 < px3)); // shared ownership test
    }

    {
        int * p = new int(7);
        shared_ptr<int> pi(p);

        shared_ptr<int> pi2(pi);
        TEST (pi2 == pi);
        TEST (pi2? true: false);
        TEST (!!pi2);
        TEST (pi2.get() == p);
        TEST (pi2.use_count() == 2);
        TEST (!pi2.unique());
        TEST (*pi2 == 7);
        TEST (pi2.use_count() == pi.use_count());
        TEST (!(pi < pi2 || pi2 < pi)); // shared ownership test
    }

    {
        int * p = new int(7);
        shared_ptr<void> pv(p);
        TEST (pv.get() == p);

        shared_ptr<void> pv2(pv);
        TEST (pv2 == pv);
        TEST (pv2? true: false);
        TEST (!!pv2);
        TEST (pv2.get() == p);
        TEST (pv2.use_count() == 2);
        TEST (!pv2.unique());
        TEST (pv2.use_count() == pv.use_count());
        TEST (!(pv < pv2 || pv2 < pv)); // shared ownership test
    }

    TEST (X::instances == 0);

    {
        X * p = new X;
        shared_ptr<X> px(p);
        TEST (px.get() == p);

        shared_ptr<X> px2(px);
        TEST (px2 == px);
        TEST (px2? true: false);
        TEST (!!px2);
        TEST (px2.get() == p);
        TEST (px2.use_count() == 2);
        TEST (!px2.unique());

        TEST (X::instances == 1);

        TEST (px2.use_count() == px.use_count());
        TEST (!(px < px2 || px2 < px)); // shared ownership test

        shared_ptr<void> px3(px);
        TEST (px3 == px);
        TEST (px3? true: false);
        TEST (!!px3);
        TEST (px3.get() == p);
        TEST (px3.use_count() == 3);
        TEST (!px3.unique());
        TEST (px3.use_count() == px.use_count());
        TEST (!(px < px3 || px3 < px)); // shared ownership test

        shared_ptr<void> px4(px2);
        TEST (px4 == px2);
        TEST (px4? true: false);
        TEST (!!px4);
        TEST (px4.get() == p);
        TEST (px4.use_count() == 4);
        TEST (!px4.unique());
        TEST (px4.use_count() == px2.use_count());
        TEST (!(px2 < px4 || px4 < px2)); // shared ownership test

        TEST (px3.use_count() == px4.use_count());
        TEST (!(px3 < px4 || px4 < px3)); // shared ownership test
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);

    {
        Y * p = new Y;
        shared_ptr<Y> py(p);
        TEST (py.get() == p);

        shared_ptr<X> px(py);
        TEST (px == py);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 2);
        TEST (!px.unique());
        TEST (px.use_count() == py.use_count());
        TEST (!(px < py || py < px)); // shared ownership test

        TEST (X::instances == 1);
        TEST (Y::instances == 1);

        shared_ptr<void const> pv(px);
        TEST (pv == px);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == px.get());
        TEST (pv.use_count() == 3);
        TEST (!pv.unique());
        TEST (pv.use_count() == px.use_count());
        TEST (!(px < pv || pv < px)); // shared ownership test

        shared_ptr<void const> pv2(py);
        TEST (pv2 == py);
        TEST (pv2? true: false);
        TEST (!!pv2);
        TEST (pv2.get() == py.get());
        TEST (pv2.use_count() == 4);
        TEST (!pv2.unique());
        TEST (pv2.use_count() == py.use_count());
        TEST (!(py < pv2 || pv2 < py)); // shared ownership test

        TEST (pv.use_count() == pv2.use_count());
        TEST (!(pv < pv2 || pv2 < pv)); // shared ownership test
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);
}

void weak_ptr_constructor()
{
    {
        weak_ptr<Y> wp;
        TEST (wp.use_count() == 0);

        try
        {
            shared_ptr<Y> p2(wp);
            printf ("shared_ptr<Y> p2(wp) failed to throw\n");
        }
        catch(bad_weak_ptr)
        {
        }

        try
        {
            shared_ptr<X> p3(wp);
            printf ("shared_ptr<X> p3(wp) failed to throw\n");
        }
        catch(bad_weak_ptr)
        {
        }
    }

    {
        shared_ptr<Y> p;
        weak_ptr<Y> wp(p);

        if(wp.use_count() != 0) // 0 allowed but not required
        {
            shared_ptr<Y> p2(wp);
            TEST (p2.use_count() == wp.use_count());
            TEST (p2.get() == 0);

            shared_ptr<X> p3(wp);
            TEST (p3.use_count() == wp.use_count());
            TEST (p3.get() == 0);
        }
    }

    {
        shared_ptr<Y> p(new Y);
        weak_ptr<Y> wp(p);

        {
            shared_ptr<Y> p2(wp);
            TEST (p2? true: false);
            TEST (!!p2);
            TEST (p2.get() == p.get());
            TEST (p2.use_count() == 2);
            TEST (!p2.unique());
            TEST (p2.use_count() == wp.use_count());

            TEST (p.use_count() == p2.use_count());
            TEST (!(p < p2 || p2 < p)); // shared ownership test

            shared_ptr<X> p3(wp);
            TEST (p3? true: false);
            TEST (!!p3);
            TEST (p3.get() == p.get());
            TEST (p3.use_count() == 3);
            TEST (!p3.unique());
            TEST (p3.use_count() == wp.use_count());

            TEST (p.use_count() == p3.use_count());
        }

        p.reset();
        TEST (wp.use_count() == 0);

        try
        {
            shared_ptr<Y> p2(wp);
            printf ("shared_ptr<Y> p2(wp) failed to throw\n");
        }
        catch(bad_weak_ptr)
        {
        }

        try
        {
            shared_ptr<X> p3(wp);
            printf ("shared_ptr<X> p3(wp) failed to throw\n");
        }
        catch(bad_weak_ptr)
        {
        }
    }
}

void auto_ptr_constructor()
{
    {
        auto_ptr<int> p;
        shared_ptr<int> pi(p);
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p;
        shared_ptr<int const> pi(p);
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p;
        shared_ptr<void> pv(p);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p;
        shared_ptr<void const> pv(p);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<X> p;
        shared_ptr<X> px(p);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<X> p;
        shared_ptr<X const> px(p);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<Y> p;
        shared_ptr<X> px(p);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<Y> p;
        shared_ptr<X const> px(p);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<Y> p;
        shared_ptr<void> pv(p);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<Y> p;
        shared_ptr<void const> pv(p);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p(new int(7));
        int * q = p.get();
        shared_ptr<int> pi(p);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == q);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (*pi == 7);
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p(new int(7));
        int * q = p.get();
        shared_ptr<int const> pi(p);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == q);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
        TEST (*pi == 7);
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p(new int(7));
        int * q = p.get();
        shared_ptr<void> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == q);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    {
        auto_ptr<int> p(new int(7));
        int * q = p.get();
        shared_ptr<void const> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == q);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);

    {
        auto_ptr<X> p(new X);
        X * q = p.get();
        shared_ptr<X> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == q);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);

    {
        auto_ptr<X> p(new X);
        X * q = p.get();
        shared_ptr<X const> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == q);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);

    {
        auto_ptr<X> p(new X);
        X * q = p.get();
        shared_ptr<void> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == q);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);

    {
        auto_ptr<X> p(new X);
        X * q = p.get();
        shared_ptr<void const> pv(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == q);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);

    {
        auto_ptr<Y> p(new Y);
        Y * q = p.get();
        shared_ptr<X> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == q);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);

    {
        auto_ptr<Y> p(new Y);
        Y * q = p.get();
        shared_ptr<X const> px(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == q);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
        TEST (p.get() == 0);
    }

    TEST (X::instances == 0);
    TEST (Y::instances == 0);
}

void test()
{
    default_constructor();
    pointer_constructor();
    deleter_constructor();
    copy_constructor();
    weak_ptr_constructor();
    auto_ptr_constructor();
}

} // namespace n_constructors

namespace n_assignment
{

class incomplete;

struct A
{
    int dummy;
};

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

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

void copy_assignment()
{
    {
        shared_ptr<incomplete> p1;

        p1 = p1;

        TEST (p1 == p1);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<incomplete> p2;

        p1 = p2;

        TEST (p1 == p2);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<incomplete> p3(p1);

        p1 = p3;

        TEST (p1 == p3);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
    }

    {
        shared_ptr<void> p1;

        p1 = p1;

        TEST (p1 == p1);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<void> p2;

        p1 = p2;

        TEST (p1 == p2);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<void> p3(p1);

        p1 = p3;

        TEST (p1 == p3);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<void> p4(new int);
        TEST (p4.use_count() == 1);

        p1 = p4;

        TEST (p1 == p4);
        TEST (!(p1 < p4 || p4 < p1));
        TEST (p1.use_count() == 2);
        TEST (p4.use_count() == 2);

        p1 = p3;

        TEST (p1 == p3);
        TEST (p4.use_count() == 1);
    }

    {
        shared_ptr<X> p1;

        p1 = p1;

        TEST (p1 == p1);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<X> p2;

        p1 = p2;

        TEST (p1 == p2);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<X> p3(p1);

        p1 = p3;

        TEST (p1 == p3);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        TEST (X::instances == 0);

        shared_ptr<X> p4(new X);

        TEST (X::instances == 1);

        p1 = p4;

        TEST (X::instances == 1);

        TEST (p1 == p4);
        TEST (!(p1 < p4 || p4 < p1));

        TEST (p1.use_count() == 2);

        p1 = p2;

        TEST (p1 == p2);
        TEST (X::instances == 1);

        p4 = p3;

        TEST (p4 == p3);
        TEST (X::instances == 0);
    }
}

void conversion_assignment()
{
    {
        shared_ptr<void> p1;

        shared_ptr<incomplete> p2;

        p1 = p2;

        TEST (p1 == p2);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        shared_ptr<int> p4(new int);
        TEST (p4.use_count() == 1);

        shared_ptr<void> p5(p4);
        TEST (p4.use_count() == 2);

        p1 = p4;

        TEST (p1 == p4);
        TEST (!(p1 < p5 || p5 < p1));
        TEST (p1.use_count() == 3);
        TEST (p4.use_count() == 3);

        p1 = p2;

        TEST (p1 == p2);
        TEST (p4.use_count() == 2);
    }

    {
        shared_ptr<X> p1;

        shared_ptr<Y> p2;

        p1 = p2;

        TEST (p1 == p2);
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);

        TEST (X::instances == 0);
        TEST (Y::instances == 0);

        shared_ptr<Y> p4(new Y);

        TEST (X::instances == 1);
        TEST (Y::instances == 1);
        TEST (p4.use_count() == 1);

        shared_ptr<X> p5(p4);
        TEST (p4.use_count() == 2);

        p1 = p4;

        TEST (X::instances == 1);
        TEST (Y::instances == 1);

        TEST (p1 == p4);
        TEST (!(p1 < p5 || p5 < p1));

        TEST (p1.use_count() == 3);
        TEST (p4.use_count() == 3);

        p1 = p2;

        TEST (p1 == p2);
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
        TEST (p4.use_count() == 2);

        p4 = p2;
        p5 = p2;

        TEST (p4 == p2);
        TEST (X::instances == 0);
        TEST (Y::instances == 0);
    }
}

void auto_ptr_assignment()
{
    {
        shared_ptr<int> p1;

        auto_ptr<int> p2;

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);

        int * p = new int;
        auto_ptr<int> p3(p);

        p1 = p3;
        TEST (p1.get() == p);
        TEST (p1.use_count() == 1);
        TEST (p3.get() == 0);

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);
    }

    {
        shared_ptr<void> p1;

        auto_ptr<int> p2;

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);

        int * p = new int;
        auto_ptr<int> p3(p);

        p1 = p3;
        TEST (p1.get() == p);
        TEST (p1.use_count() == 1);
        TEST (p3.get() == 0);

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);
    }


    {
        shared_ptr<X> p1;

        auto_ptr<Y> p2;

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);
        TEST (X::instances == 0);
        TEST (Y::instances == 0);

        Y * p = new Y;
        auto_ptr<Y> p3(p);

        TEST (X::instances == 1);
        TEST (Y::instances == 1);

        p1 = p3;
        TEST (p1.get() == p);
        TEST (p1.use_count() == 1);
        TEST (X::instances == 1);
        TEST (Y::instances == 1);
        TEST (p3.get() == 0);

        p1 = p2;
        TEST (p1? false: true);
        TEST (!p1);
        TEST (p1.get() == 0);
        TEST (p1.use_count() == 1);
        TEST (X::instances == 0);
        TEST (Y::instances == 0);
    }
}

void test()
{
    copy_assignment();
    conversion_assignment();
    auto_ptr_assignment();
}

} // namespace n_assignment

namespace n_reset
{

class incomplete;

incomplete * p0 = 0;

void deleter(incomplete *)
{
}

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

private:

    X(X const &);
    X & operator= (X const &);
};

long X::instances = 0;

void plain_reset()
{
    {
        shared_ptr<int> pi;
        pi.reset();
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
    }

    {
        shared_ptr<int> pi(static_cast<int*>(0));
        pi.reset();
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
    }

    {
        shared_ptr<int> pi(new int);
        pi.reset();
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
    }

    {
        shared_ptr<incomplete> px;
        px.reset();
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
    }

    {
        shared_ptr<incomplete> px(p0, deleter);
        px.reset();
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
    }

    {
        shared_ptr<X> px;
        px.reset();
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
    }

    {
        TEST (X::instances == 0);
        shared_ptr<X> px(new X);
        TEST (X::instances == 1);
        px.reset();
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (X::instances == 0);
    }

    {
        shared_ptr<void> pv;
        pv.reset();
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
    }

    {
        TEST (X::instances == 0);
        shared_ptr<void> pv(new X);
        TEST (X::instances == 1);
        pv.reset();
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (X::instances == 0);
    }
}

struct A
{
    int dummy;
};

struct Y: public A, public virtual X
{
    static long instances;

    Y()
    {
        ++instances;
    }

    ~Y()
    {
        --instances;
    }

private:

    Y(Y const &);
    Y & operator= (Y const &);
};

long Y::instances = 0;

void pointer_reset()
{
    {
        shared_ptr<int> pi;

        pi.reset(static_cast<int*>(0));
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());

        int * p = new int;
        pi.reset(p);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == p);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());

        pi.reset(static_cast<int*>(0));
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());
    }

    {
        shared_ptr<X> px;

        px.reset(static_cast<X*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 0);

        X * p = new X;
        px.reset(p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == p);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);

        px.reset(static_cast<X*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 0);
        TEST (Y::instances == 0);

        Y * q = new Y;
        px.reset(q);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == q);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);

        px.reset(static_cast<Y*>(0));
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());
        TEST (X::instances == 0);
        TEST (Y::instances == 0);
    }

    {
        shared_ptr<void> pv;

        pv.reset(static_cast<X*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 0);

        X * p = new X;
        pv.reset(p);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == p);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);

        pv.reset(static_cast<X*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 0);
        TEST (Y::instances == 0);

        Y * q = new Y;
        pv.reset(q);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == q);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 1);
        TEST (Y::instances == 1);

        pv.reset(static_cast<Y*>(0));
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());
        TEST (X::instances == 0);
        TEST (Y::instances == 0);
    }
}

void * deleted = 0;

void deleter2(void * p)
{
    deleted = p;
}

void deleter_reset()
{
    {
        shared_ptr<int> pi;

        pi.reset(static_cast<int*>(0), deleter2);
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());

        deleted = &pi;

        int m = 0;
        pi.reset(&m, deleter2);
        TEST (deleted == 0);
        TEST (pi? true: false);
        TEST (!!pi);
        TEST (pi.get() == &m);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());

        pi.reset(static_cast<int*>(0), deleter2);
        TEST (deleted == &m);
        TEST (pi? false: true);
        TEST (!pi);
        TEST (pi.get() == 0);
        TEST (pi.use_count() == 1);
        TEST (pi.unique());

        pi.reset();
        TEST (deleted == 0);
    }

    {
        shared_ptr<X> px;

        px.reset(static_cast<X*>(0), deleter2);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        deleted = &px;

        X x;
        px.reset(&x, deleter2);
        TEST (deleted == 0);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == &x);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        px.reset(static_cast<X*>(0), deleter2);
        TEST (deleted == &x);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        Y y;
        px.reset(&y, deleter2);
        TEST (deleted == 0);
        TEST (px? true: false);
        TEST (!!px);
        TEST (px.get() == &y);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        px.reset(static_cast<Y*>(0), deleter2);
        TEST (deleted == &y);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        px.reset();
        TEST (deleted == 0);
    }

    {
        shared_ptr<void> pv;

        pv.reset(static_cast<X*>(0), deleter2);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());

        deleted = &pv;

        X x;
        pv.reset(&x, deleter2);
        TEST (deleted == 0);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == &x);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());

        pv.reset(static_cast<X*>(0), deleter2);
        TEST (deleted == &x);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());

        Y y;
        pv.reset(&y, deleter2);
        TEST (deleted == 0);
        TEST (pv? true: false);
        TEST (!!pv);
        TEST (pv.get() == &y);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());

        pv.reset(static_cast<Y*>(0), deleter2);
        TEST (deleted == &y);
        TEST (pv? false: true);
        TEST (!pv);
        TEST (pv.get() == 0);
        TEST (pv.use_count() == 1);
        TEST (pv.unique());

        pv.reset();
        TEST (deleted == 0);
    }

    {
        shared_ptr<incomplete> px;

        px.reset(p0, deleter2);
        TEST (px? false: true);
        TEST (!px);
        TEST (px.get() == 0);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        deleted = &px;
        px.reset(p0, deleter2);
        TEST (deleted == 0);
    }
}

void test()
{
    plain_reset();
    pointer_reset();
    deleter_reset();
}

} // namespace n_reset

namespace n_access
{

struct X
{
};

void test()
{
    {
        shared_ptr<X> px;
        TEST (px.get() == 0);
        TEST (px? false: true);
        TEST (!px);
        TEST (get_pointer(px) == px.get());
    }

    {
        shared_ptr<X> px(static_cast<X*>(0));
        TEST (px.get() == 0);
        TEST (px? false: true);
        TEST (!px);
        TEST (get_pointer(px) == px.get());
    }

    {
        shared_ptr<X> px(static_cast<X*>(0), checked_deleter<X>());
        TEST (px.get() == 0);
        TEST (px? false: true);
        TEST (!px);
        TEST (get_pointer(px) == px.get());
    }

    {
        X * p = new X;
        shared_ptr<X> px(p);
        TEST (px.get() == p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (&*px == px.get());
        TEST (px.operator ->() == px.get());
        TEST (get_pointer(px) == px.get());
    }

    {
        X * p = new X;
        shared_ptr<X> px(p, checked_deleter<X>());
        TEST (px.get() == p);
        TEST (px? true: false);
        TEST (!!px);
        TEST (&*px == px.get());
        TEST (px.operator ->() == px.get());
        TEST (get_pointer(px) == px.get());
    }
}

} // namespace n_access

namespace n_use_count
{

struct X
{
};

void test()
{
    {
        shared_ptr<X> px(static_cast<X*>(0));
        TEST (px.use_count() == 1);
        TEST (px.unique());

        shared_ptr<X> px2(px);
        TEST (px2.use_count() == 2);
        TEST (!px2.unique());
        TEST (px.use_count() == 2);
        TEST (!px.unique());
    }

    {
        shared_ptr<X> px(new X);
        TEST (px.use_count() == 1);
        TEST (px.unique());

        shared_ptr<X> px2(px);
        TEST (px2.use_count() == 2);
        TEST (!px2.unique());
        TEST (px.use_count() == 2);
        TEST (!px.unique());
    }

    {
        shared_ptr<X> px(new X, checked_deleter<X>());
        TEST (px.use_count() == 1);
        TEST (px.unique());

        shared_ptr<X> px2(px);
        TEST (px2.use_count() == 2);
        TEST (!px2.unique());
        TEST (px.use_count() == 2);
        TEST (!px.unique());
    }
}

} // namespace n_use_count

namespace n_swap
{

struct X
{
};

void test()
{
    {
        shared_ptr<X> px;
        shared_ptr<X> px2;

        px.swap(px2);

        TEST (px.get() == 0);
        TEST (px2.get() == 0);

        swap(px, px2);

        TEST (px.get() == 0);
        TEST (px2.get() == 0);
    }

    {
        X * p = new X;
        shared_ptr<X> px;
        shared_ptr<X> px2(p);
        shared_ptr<X> px3(px2);

        px.swap(px2);

        TEST (px.get() == p);
        TEST (px.use_count() == 2);
        TEST (px2.get() == 0);
        TEST (px3.get() == p);
        TEST (px3.use_count() == 2);

        swap(px, px2);

        TEST (px.get() == 0);
        TEST (px2.get() == p);
        TEST (px2.use_count() == 2);
        TEST (px3.get() == p);
        TEST (px3.use_count() == 2);
    }

    {
        X * p1 = new X;
        X * p2 = new X;
        shared_ptr<X> px(p1);
        shared_ptr<X> px2(p2);
        shared_ptr<X> px3(px2);

        px.swap(px2);

        TEST (px.get() == p2);
        TEST (px.use_count() == 2);
        TEST (px2.get() == p1);
        TEST (px2.use_count() == 1);
        TEST (px3.get() == p2);
        TEST (px3.use_count() == 2);

        swap(px, px2);

        TEST (px.get() == p1);
        TEST (px.use_count() == 1);
        TEST (px2.get() == p2);
        TEST (px2.use_count() == 2);
        TEST (px3.get() == p2);
        TEST (px3.use_count() == 2);
    }
}

} // namespace n_swap

namespace n_comparison
{

struct X
{
    int dummy;
};

struct Y
{
    int dummy2;
};

struct Z: public X, public virtual Y
{
};

void test()
{
    {
        shared_ptr<X> px;
        TEST (px == px);
        TEST (!(px != px));
        TEST (!(px < px));

        shared_ptr<X> px2;

        TEST (px.get() == px2.get());
        TEST (px == px2);
        TEST (!(px != px2));
        TEST (!(px < px2 && px2 < px));
    }

    {
        shared_ptr<X> px;
        shared_ptr<X> px2(px);

        TEST (px2 == px2);
        TEST (!(px2 != px2));
        TEST (!(px2 < px2));

        TEST (px.get() == px2.get());
        TEST (px == px2);
        TEST (!(px != px2));
        TEST (!(px < px2 && px2 < px));
    }

    {
        shared_ptr<X> px;
        shared_ptr<X> px2(new X);

        TEST (px2 == px2);
        TEST (!(px2 != px2));
        TEST (!(px2 < px2));

        TEST (px.get() != px2.get());
        TEST (px != px2);
        TEST (!(px == px2));
        TEST (px < px2 || px2 < px);
        TEST (!(px < px2 && px2 < px));
    }

    {
        shared_ptr<X> px(new X);
        shared_ptr<X> px2(new X);

        TEST (px.get() != px2.get());
        TEST (px != px2);
        TEST (!(px == px2));
        TEST (px < px2 || px2 < px);
        TEST (!(px < px2 && px2 < px));
    }

    {
        shared_ptr<X> px(new X);
        shared_ptr<X> px2(px);

        TEST (px2 == px2);
        TEST (!(px2 != px2));
        TEST (!(px2 < px2));

        TEST (px.get() == px2.get());
        TEST (px == px2);
        TEST (!(px != px2));
        TEST (!(px < px2 || px2 < px));
    }

    {
        shared_ptr<X> px(new X);
        shared_ptr<Y> py(new Y);
        shared_ptr<Z> pz(new Z);

        TEST (px.get() != pz.get());
        TEST (px != pz);
        TEST (!(px == pz));

        TEST (py.get() != pz.get());
        TEST (py != pz);
        TEST (!(py == pz));

        TEST (px < py || py < px);
        TEST (px < pz || pz < px);
        TEST (py < pz || pz < py);

        TEST (!(px < py && py < px));
        TEST (!(px < pz && pz < px));
        TEST (!(py < pz && pz < py));

        shared_ptr<void> pvx(px);

        TEST (pvx == pvx);
        TEST (!(pvx != pvx));
        TEST (!(pvx < pvx));

        shared_ptr<void> pvy(py);
        shared_ptr<void> pvz(pz);

        TEST (pvx < pvy || pvy < pvx);
        TEST (pvx < pvz || pvz < pvx);
        TEST (pvy < pvz || pvz < pvy);

        TEST (!(pvx < pvy && pvy < pvx));
        TEST (!(pvx < pvz && pvz < pvx));
        TEST (!(pvy < pvz && pvz < pvy));
    }

    {
        shared_ptr<Z> pz(new Z);
        shared_ptr<X> px(pz);

        TEST (px == px);
        TEST (!(px != px));
        TEST (!(px < px));

        shared_ptr<Y> py(pz);

        TEST (px.get() == pz.get());
        TEST (px == pz);
        TEST (!(px != pz));

        TEST (py.get() == pz.get());
        TEST (py == pz);
        TEST (!(py != pz));

        TEST (!(px < py || py < px));
        TEST (!(px < pz || pz < px));
        TEST (!(py < pz || pz < py));

        shared_ptr<void> pvx(px);
        shared_ptr<void> pvy(py);
        shared_ptr<void> pvz(pz);

        // pvx and pvy aren't equal...
        TEST (pvx.get() != pvy.get());
        TEST (pvx != pvy);
        TEST (!(pvx == pvy));

        // ... but they share ownership ...
        TEST (!(pvx < pvy || pvy < pvx));

        // ... with pvz
        TEST (!(pvx < pvz || pvz < pvx));
        TEST (!(pvy < pvz || pvz < pvy));
    }
}

} // namespace n_comparison

namespace n_static_cast
{

struct X
{
};

struct Y: public X
{
};

void test()
{
    {
        shared_ptr<void> pv;

        shared_ptr<int> pi = static_pointer_cast<int>(pv);
        TEST (pi.get() == 0);

        shared_ptr<X> px = static_pointer_cast<X>(pv);
        TEST (px.get() == 0);
    }

    {
        shared_ptr<int> pi(new int);
        shared_ptr<void> pv(pi);

        shared_ptr<int> pi2 = static_pointer_cast<int>(pv);
        TEST (pi.get() == pi2.get());
        TEST (!(pi < pi2 || pi2 < pi));
        TEST (pi.use_count() == 3);
        TEST (pv.use_count() == 3);
        TEST (pi2.use_count() == 3);
    }

    {
        shared_ptr<X> px(new X);
        shared_ptr<void> pv(px);

        shared_ptr<X> px2 = static_pointer_cast<X>(pv);
        TEST (px.get() == px2.get());
        TEST (!(px < px2 || px2 < px));
        TEST (px.use_count() == 3);
        TEST (pv.use_count() == 3);
        TEST (px2.use_count() == 3);
    }

    {
        shared_ptr<X> px(new Y);

        shared_ptr<Y> py = static_pointer_cast<Y>(px);
        TEST (px.get() == py.get());
        TEST (px.use_count() == 2);
        TEST (py.use_count() == 2);

        shared_ptr<X> px2(py);
        TEST (!(px < px2 || px2 < px));
    }
}

} // namespace n_static_cast

namespace n_const_cast
{

struct X;

void test()
{
    {
        shared_ptr<void const volatile> px;

        shared_ptr<void> px2 = const_pointer_cast<void>(px);
        TEST (px2.get() == 0);
    }

    {
        shared_ptr<int const volatile> px;

        shared_ptr<int> px2 = const_pointer_cast<int>(px);
        TEST (px2.get() == 0);
    }

    {
        shared_ptr<X const volatile> px;

        shared_ptr<X> px2 = const_pointer_cast<X>(px);
        TEST (px2.get() == 0);
    }

    {
        shared_ptr<void const volatile> px(new int);

        shared_ptr<void> px2 = const_pointer_cast<void>(px);
        TEST (px.get() == px2.get());
        TEST (!(px < px2 || px2 < px));
        TEST (px.use_count() == 2);
        TEST (px2.use_count() == 2);
    }

    {
        shared_ptr<int const volatile> px(new int);

        shared_ptr<int> px2 = const_pointer_cast<int>(px);
        TEST (px.get() == px2.get());
        TEST (!(px < px2 || px2 < px));
        TEST (px.use_count() == 2);
        TEST (px2.use_count() == 2);
    }
}

} // namespace n_const_cast

namespace n_dynamic_cast
{

struct V
{
    virtual ~V() {}
};

struct W: public V
{
};

void test()
{
    {
        shared_ptr<V> pv;
        shared_ptr<W> pw = dynamic_pointer_cast<W>(pv);
        TEST (pw.get() == 0);
    }

    {
        shared_ptr<V> pv(static_cast<V*>(0));

        shared_ptr<W> pw = dynamic_pointer_cast<W>(pv);
        TEST (pw.get() == 0);

        shared_ptr<V> pv2(pw);
        TEST (pv < pv2 || pv2 < pv);
    }

    {
        shared_ptr<V> pv(static_cast<W*>(0));

        shared_ptr<W> pw = dynamic_pointer_cast<W>(pv);
        TEST (pw.get() == 0);

        shared_ptr<V> pv2(pw);
        TEST (pv < pv2 || pv2 < pv);
    }

    {
        shared_ptr<V> pv(new V);

        shared_ptr<W> pw = dynamic_pointer_cast<W>(pv);
        TEST (pw.get() == 0);

        shared_ptr<V> pv2(pw);
        TEST (pv < pv2 || pv2 < pv);
    }

    {
        shared_ptr<V> pv(new W);

        shared_ptr<W> pw = dynamic_pointer_cast<W>(pv);
        TEST (pw.get() == pv.get());
        TEST (pv.use_count() == 2);
        TEST (pw.use_count() == 2);

        shared_ptr<V> pv2(pw);
        TEST (!(pv < pv2 || pv2 < pv));
    }
}

} // namespace n_dynamic_cast

namespace n_map
{

struct X
{
};

void test()
{
    vector< shared_ptr<int> > vi;

    {
        shared_ptr<int> pi1(new int);
        shared_ptr<int> pi2(new int);
        shared_ptr<int> pi3(new int);

        vi.push_back(pi1);
        vi.push_back(pi1);
        vi.push_back(pi1);
        vi.push_back(pi2);
        vi.push_back(pi1);
        vi.push_back(pi2);
        vi.push_back(pi1);
        vi.push_back(pi3);
        vi.push_back(pi3);
        vi.push_back(pi2);
        vi.push_back(pi1);
    }

    vector< shared_ptr<X> > vx;

    {
        shared_ptr<X> px1(new X);
        shared_ptr<X> px2(new X);
        shared_ptr<X> px3(new X);

        vx.push_back(px2);
        vx.push_back(px2);
        vx.push_back(px1);
        vx.push_back(px2);
        vx.push_back(px1);
        vx.push_back(px1);
        vx.push_back(px1);
        vx.push_back(px2);
        vx.push_back(px1);
        vx.push_back(px3);
        vx.push_back(px2);
    }

    map< shared_ptr<void>, long > m;

    {
        for(vector< shared_ptr<int> >::iterator i = vi.begin(); i != vi.end(); ++i)
        {
            ++m[*i];
        }
    }

    {
        for(vector< shared_ptr<X> >::iterator i = vx.begin(); i != vx.end(); ++i)
        {
            ++m[*i];
        }
    }

    {
        for(map< shared_ptr<void>, long >::iterator i = m.begin(); i != m.end(); ++i)
        {
            TEST (i->first.use_count() == i->second + 1);
        }
    }
}

} // namespace n_map

namespace n_transitive
{

struct X
{
    X(): next() {}
    shared_ptr<X> next;
};

void test()
{
    shared_ptr<X> p(new X);
    p->next = shared_ptr<X>(new X);
    TEST (!p->next->next);
    p = p->next;
    TEST (!p->next);
}

} // namespace n_transitive

namespace n_report_1
{

class foo
{ 
public: 

    foo(): m_self(this)
    {
    } 

    void suicide()
    {
        m_self.reset();
    }

private:

    shared_ptr<foo> m_self;
}; 

void test()
{
    foo * foo_ptr = new foo;
    foo_ptr->suicide();
} 

} // namespace n_report_1

// Test case by Per Kristensen
namespace n_report_2
{

class foo
{
public:

    void setWeak(shared_ptr<foo> s)
    {
        w = s;
    }

private:

    weak_ptr<foo> w;
};
 
class deleter
{
public:

    deleter(int& in_lock): lock(&in_lock)
    {
    }

    void operator() (foo * p)
    {
        ++*lock;
        delete p;
        --*lock;
    } 
    
  private:
    int* lock;
};
 
void test()
{
    int lock = 0;
    {

    shared_ptr<foo> s(new foo, deleter(lock));
    s->setWeak(s);
    s.reset();
    }
    
    TEST (lock == 0);
}

} // namespace n_report_2

namespace n_spt_incomplete
{

class file;

shared_ptr<file> fopen(char const * name, char const * mode);
void fread(shared_ptr<file> f, void * data, long size);

int file_instances = 0;

void test()
{
    TEST (file_instances == 0);

    {
        shared_ptr<file> pf = fopen("name", "mode");
        TEST (file_instances == 1);
        fread(pf, 0, 17041);
    }

    TEST (file_instances == 0);
}

} // namespace n_spt_incomplete

namespace n_spt_pimpl
{

class file
{
private:

    class impl;
    shared_ptr<impl> pimpl_;

public:

    file(char const * name, char const * mode);

    // compiler generated members are fine and useful

    void read(void * data, long size);

    long total_size() const;
};

int file_instances = 0;

void test()
{
    TEST (file_instances == 0);

    {
        file f("name", "mode");
        TEST (file_instances == 1);
        f.read(0, 152);

        file f2(f);
        TEST (file_instances == 1);
        f2.read(0, 894);

        TEST (f.total_size() == 152+894);

        {
            file f3("name2", "mode2");
            TEST (file_instances == 2);
        }

        TEST (file_instances == 1);
    }

    TEST (file_instances == 0);
}

} // namespace n_spt_pimpl

namespace n_spt_abstract
{

class X
{
public:

    virtual void f(int) = 0;
    virtual int g() = 0;

protected:

    virtual ~X() {}
};

shared_ptr<X> createX();

int X_instances = 0;

void test()
{
    TEST (X_instances == 0);

    {
        shared_ptr<X> px = createX();

        TEST (X_instances == 1);

        px->f(18);
        px->f(152);

        TEST (px->g() == 170);
    }

    TEST (X_instances == 0);
}

} // namespace n_spt_abstract

namespace n_spt_preventing_delete
{

int X_instances = 0;

class X
{
private:

    X()
    {
        ++X_instances;
    }

    ~X()
    {
        --X_instances;
    }

    class deleter;
    friend class deleter;

    class deleter
    {
    public:

        void operator()(X * p) { delete p; }
    };

public:

    static shared_ptr<X> create()
    {
        shared_ptr<X> px(new X, X::deleter());
        return px;
    }
};

void test()
{
    TEST (X_instances == 0);

    {
        shared_ptr<X> px = X::create();
        TEST (X_instances == 1);
    }

    TEST (X_instances == 0);
}

} // namespace n_spt_preventing_delete

namespace n_spt_array
{

int X_instances = 0;

struct X
{
    X()
    {
        ++X_instances;
    }

    ~X()
    {
        --X_instances;
    }
};

void test()
{
    TEST (X_instances == 0);

    {
        shared_ptr<X> px(new X[4], checked_array_deleter<X>());
        TEST (X_instances == 4);
    }

    TEST (X_instances == 0);
}

} // namespace n_spt_array

namespace n_spt_static
{

class X
{
public:

    X()
    {
    }

private:

    void operator delete(void *)
    {
        // Comeau 4.3.0.1 wants a definition
        printf ("n_spt_static::X::operator delete() called.\n");
    }
};

struct null_deleter
{
    void operator()(void const *) const
    {
    }
};

static X x;

void test()
{
    shared_ptr<X> px(&x, null_deleter());
}

} // namespace n_spt_static

namespace n_spt_intrusive
{

int X_instances = 0;

struct X
{
    long count;

    X(): count(0)
    {
        ++X_instances;
    }

    ~X()
    {
        --X_instances;
    }
};

void intrusive_ptr_add_ref(X * p)
{
    ++p->count;
}

void intrusive_ptr_release(X * p)
{
    if(--p->count == 0) delete p;
}

template<class T> struct intrusive_deleter
{
    void operator()(T * p)
    {
        if(p != 0) intrusive_ptr_release(p);
    }
};

shared_ptr<X> make_shared_from_intrusive(X * p)
{
    if(p != 0) intrusive_ptr_add_ref(p);
    shared_ptr<X> px(p, intrusive_deleter<X>());
    return px;
}

void test()
{
    TEST (X_instances == 0);

    {
        X * p = new X;
        TEST (X_instances == 1);
        TEST (p->count == 0);
        shared_ptr<X> px = make_shared_from_intrusive(p);
        TEST (px.get() == p);
        TEST (p->count == 1);
        shared_ptr<X> px2(px);
        TEST (px2.get() == p);
        TEST (p->count == 1);
    }

    TEST (X_instances == 0);
}

} // namespace n_spt_intrusive

namespace n_spt_another_sp
{

template<class T> class another_ptr: private shared_ptr<T>
{
private:

    typedef shared_ptr<T> base_type;

public:

    explicit another_ptr(T * p = 0): base_type(p)
    {
    }

    void reset()
    {
        base_type::reset();
    }

    T * get() const
    {
        return base_type::get();
    }
};

class event_handler
{
public:

    virtual ~event_handler() {}
    virtual void begin() = 0;
    virtual void handle(int event) = 0;
    virtual void end() = 0;
};

int begin_called = 0;
int handle_called = 0;
int end_called = 0;

class event_handler_impl: public event_handler
{
public:

    virtual void begin()
    {
        ++begin_called;
    }

    virtual void handle(int event)
    {
        handle_called = event;
    }

    virtual void end()
    {
        ++end_called;
    }
};

another_ptr<event_handler> get_event_handler()
{
    another_ptr<event_handler> p(new event_handler_impl);
    return p;
}

shared_ptr<event_handler> current_handler;

void install_event_handler(shared_ptr<event_handler> p)
{
    p->begin();
    current_handler = p;
}

void handle_event(int event)
{
    current_handler->handle(event);
}

void remove_event_handler()
{
    current_handler->end();
    current_handler.reset();
}

template<class P> class smart_pointer_deleter
{
private:

    P p_;

public:

    smart_pointer_deleter(P const & p): p_(p)
    {
    }

    void operator()(void const *)
    {
        p_.reset();
    }
};

void test()
{
    another_ptr<event_handler> p = get_event_handler();

    shared_ptr<event_handler> q(p.get(), smart_pointer_deleter< another_ptr<event_handler> >(p));

    p.reset();

    TEST (begin_called == 0);

    install_event_handler(q);

    TEST (begin_called == 1);

    TEST (handle_called == 0);

    handle_event(17041);

    TEST (handle_called == 17041);

    TEST (end_called == 0);

    remove_event_handler();

    TEST (end_called == 1);
}

} // namespace n_spt_another_sp

namespace n_spt_shared_from_this
{

class X
{
public:

    virtual void f() = 0;

protected:

    virtual ~X() {}
};

class Y
{
public:

    virtual shared_ptr<X> getX() = 0;

protected:

    virtual ~Y() {}
};

class impl: public X, public Y
{
private:

    weak_ptr<impl> weak_this;

    impl(impl const &);
    impl & operator=(impl const &);

    impl() {}

public:

    static shared_ptr<impl> create()
    {
        shared_ptr<impl> pi(new impl);
        pi->weak_this = pi;
        return pi;
    }

    virtual void f() {}

    virtual shared_ptr<X> getX()
    {
        shared_ptr<X> px = weak_this.lock();
        return px;
    }
};

void test()
{
    shared_ptr<Y> py = impl::create();
    TEST (py.get() != 0);
    TEST (py.use_count() == 1);

    shared_ptr<X> px = py->getX();
    TEST (px.get() != 0);
    TEST (py.use_count() == 2);

    shared_ptr<Y> py2 = dynamic_pointer_cast<Y>(px);
    TEST (py.get() == py2.get());
    TEST (!(py < py2 || py2 < py));
    TEST (py.use_count() == 3);
}

} // namespace n_spt_shared_from_this

namespace n_spt_wrap
{

void test()
{
}

} // namespace n_spt_wrap

} //namespace test

int main()
{
    printf ("Results of shared_ptr_test:\n");

    using namespace test;

    n_element_type::test();
    n_constructors::test();
    n_assignment::test();
    n_reset::test();
    n_access::test();
    n_use_count::test();
    n_swap::test();
    n_comparison::test();
    n_static_cast::test();
    n_const_cast::test();
    n_dynamic_cast::test();

    n_map::test();

    n_transitive::test();
    n_report_1::test();
    n_report_2::test();

    n_spt_incomplete::test();
    n_spt_pimpl::test();
    n_spt_abstract::test();
    n_spt_preventing_delete::test();
    n_spt_array::test();
    n_spt_static::test();
    n_spt_intrusive::test();
    n_spt_another_sp::test();
    n_spt_shared_from_this::test();
    n_spt_wrap::test();

    return 0;
}

namespace test
{


namespace n_spt_incomplete
{

class file
{
public:

    file(): fread_called(false)
    {
        ++file_instances;
    }

    ~file()
    {
        TEST (fread_called);
        --file_instances;
    }

    bool fread_called;
};

shared_ptr<file> fopen(char const *, char const *)
{
    shared_ptr<file> pf(new file);
    return pf;
}

void fread(shared_ptr<file> pf, void *, long)
{
    pf->fread_called = true;
}

} // namespace n_spt_incomplete

namespace n_spt_pimpl
{

class file::impl
{
private:

    impl(impl const &);
    impl & operator=(impl const &);

    long total_size_;

public:

    impl(char const *, char const *): total_size_(0)
    {
        ++file_instances;
    }

    ~impl()
    {
        --file_instances;
    }

    void read(void *, long size)
    {
        total_size_ += size;
    }

    long total_size() const
    {
        return total_size_;
    }
};

file::file(char const * name, char const * mode): pimpl_(new impl(name, mode))
{
}

void file::read(void * data, long size)
{
    pimpl_->read(data, size);
}

long file::total_size() const
{
    return pimpl_->total_size();
}

} // namespace n_spt_pimpl

namespace n_spt_abstract
{

class X_impl: public X
{
private:

    X_impl(X_impl const &);
    X_impl & operator=(X_impl const &);

    int n_;

public:

    X_impl(): n_(0)
    {
        ++X_instances;
    }

    ~X_impl()
    {
        --X_instances;
    }

    virtual void f(int n)
    {
        n_ += n;
    }

    virtual int g()
    {
        return n_;
    }
};

shared_ptr<X> createX()
{
    shared_ptr<X> px(new X_impl);
    return px;
}

} // namespace n_spt_abstract

} // namespace test
