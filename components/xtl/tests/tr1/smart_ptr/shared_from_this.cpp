#include "test.h"

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

shared_ptr<Y> createY();

void test()
{
    shared_ptr<Y> py = createY();
    TEST (py.get() != 0);
    TEST (py.use_count() == 1);

    shared_ptr<X> px = py->getX();
    TEST (px.get() != 0);
    TEST (py.use_count() == 2);

    px->f();

    shared_ptr<Y> py2 = dynamic_pointer_cast<Y>(px);
    TEST (py.get() == py2.get());
    TEST (!(py < py2 || py2 < py));
    TEST (py.use_count() == 3);
}

void test2();
void test3();

int main()
{
    printf ("Results of shared_from_this_test:\n");

    try
    {
      test();
      test2();
      test3();
    }
    catch (std::exception& exception)
    {
      printf ("exception: %s\n", exception.what ());
    }
    return 0;
}

// virtual inheritance to stress the implementation
// (prevents Y* -> impl*, enable_shared_from_this<impl>* -> impl* casts)

class impl: public X, public virtual Y, public virtual enable_shared_from_this<impl>
{
public:

    virtual void f()
    {
    }

    virtual shared_ptr<X> getX()
    {
        shared_ptr<impl> pi = shared_from_this();
        TEST (pi.get() == this);
        return pi;
    }
};

// intermediate impl2 to stress the implementation

class impl2: public impl
{
};

shared_ptr<Y> createY()
{
    shared_ptr<Y> pi(new impl2);
    return pi;
}

void test2()
{
    shared_ptr<Y> pi(static_cast<impl2*>(0));
}

//

struct V: public enable_shared_from_this<V>
{
};

void test3()
{
    shared_ptr<V> p(new V);

    shared_ptr<V> q = p->shared_from_this();
    TEST (p == q);
    TEST (!(p < q) && !(q < p));

    V v2(*p);

    try
    {
        shared_ptr<V> r = v2.shared_from_this();
        printf ("v2.shared_from_this() failed to throw");
    }
    catch(bad_weak_ptr const &)
    {
    }

    try
    {
        *p = V();
        shared_ptr<V> r = p->shared_from_this();
        TEST (p == r);
        TEST (!(p < r) && !(r < p));
    }
    catch(bad_weak_ptr const &)
    {
        printf ("p->shared_from_this() threw bad_weak_ptr after *p = V()");
    }
}
