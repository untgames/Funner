#include "test.h"

int cnt = 0;

struct X
{
  X  () { ++cnt; }
  ~X () { --cnt; }

  virtual int id () const { return 1; }

  private:
    X (const X&);
    X& operator = (const X&);
};

struct Y: public X
{
  Y  () { ++cnt; }
  ~Y () { --cnt; }

  int id () const { return 2; }
};

int* get_object ()
{
  ++cnt;
  return &cnt;
}

void release_object (int* p)
{
  TEST (p == &cnt);
  --cnt;
}

template <class T> void test_is_X(shared_ptr<T> const & p)
{
  TEST (p->id() == 1);
  TEST ((*p).id() == 1);
}

template <class T> void test_is_X(weak_ptr<T> const & p)
{
  TEST (p.get() != 0);
  TEST (p.get()->id() == 1);
}

template <class T> void test_is_Y(shared_ptr<T> const & p)
{
  TEST (p->id() == 2);
  TEST ((*p).id() == 2);
}

template <class T> void test_is_Y(weak_ptr<T> const & p)
{
  shared_ptr<T> q = p.lock();
  TEST (q.get() != 0);
  TEST (q->id() == 2);
}

template <class T> void test_eq(T const & a, T const & b)
{
  TEST (a == b);
  TEST (!(a != b));
  TEST (!(a < b));
  TEST (!(b < a));
}

template <class T> void test_ne(T const & a, T const & b)
{
  TEST (!(a == b));
  TEST (a != b);
  TEST (a < b || b < a);
  TEST (!(a < b && b < a));
}

template <class T, class U> void test_shared(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
  TEST (!(a < b));
  TEST (!(b < a));
}

template <class T, class U> void test_nonshared(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
  TEST (a < b || b < a);
  TEST (!(a < b && b < a));
}

template <class T, class U> void test_eq2(T const & a, U const & b)
{
  TEST (a == b);
  TEST (!(a != b));
}

template <class T, class U> void test_ne2(T const & a, U const & b)
{
  TEST (!(a == b));
  TEST (a != b);
}

template <class T> void test_is_zero(shared_ptr<T> const & p)
{
  TEST (!p);
  TEST (p.get() == 0);
}

template <class T> void test_is_nonzero(shared_ptr<T> const & p)
{
  TEST (p? true: false);
  TEST (p.get() != 0);
}

int main()
{
  printf ("Results of shared_ptr_basic_test:\n");

  {
    shared_ptr<X> p(new Y);
    shared_ptr<X> p2(new X);

    test_is_nonzero(p);
    test_is_nonzero(p2);
    test_is_Y(p);
    test_is_X(p2);
    test_ne(p, p2);

    {
        shared_ptr<X> q(p);
        test_eq(p, q);
    }

    shared_ptr<Y> p3 = dynamic_pointer_cast<Y>(p);
    shared_ptr<Y> p4 = dynamic_pointer_cast<Y>(p2);

    test_is_nonzero(p3);
    test_is_zero(p4);

    TEST (p.use_count() == 2);
    TEST (p2.use_count() == 1);
    TEST (p3.use_count() == 2);

    test_is_Y(p3);
    test_eq2(p, p3);
    test_ne2(p2, p4);

    shared_ptr<void> p5(p);

    test_is_nonzero(p5);
    test_eq2(p, p5);

    weak_ptr<X> wp1(p2);

    TEST (!wp1.expired());
    TEST (wp1.use_count() != 0);

    p.reset();
    p2.reset();
    p3.reset();
    p4.reset();

    test_is_zero(p);
    test_is_zero(p2);
    test_is_zero(p3);
    test_is_zero(p4);

    TEST (p5.use_count() == 1);

    TEST (wp1.expired());
    TEST (wp1.use_count() == 0);

    try
    {
      shared_ptr<X> sp1(wp1);
      printf ("error: shared_ptr<X> sp1(wp1) failed to throw\n");
    }
    catch(bad_weak_ptr const &)
    {
    }

    test_is_zero(wp1.lock());

    weak_ptr<X> wp2 = static_pointer_cast<X>(p5);

    TEST (wp2.use_count() == 1);
    test_is_Y(wp2);
    test_nonshared(wp1, wp2);

    // Scoped to not affect the subsequent use_count() tests.
    {
      shared_ptr<X> sp2(wp2);
      test_is_nonzero(wp2.lock());
    }

    weak_ptr<Y> wp3 = dynamic_pointer_cast<Y>(wp2.lock());

    TEST (wp3.use_count() == 1);
    test_shared(wp2, wp3);

    weak_ptr<X> wp4(wp3);

    TEST (wp4.use_count() == 1);
    test_shared(wp2, wp4);

    wp1 = p2;
    test_is_zero(wp1.lock());

    wp1 = p4;
    wp1 = wp3;
    wp1 = wp2;

    TEST (wp1.use_count() == 1);
    test_shared(wp1, wp2);

    weak_ptr<X> wp5;

    bool b1 = wp1 < wp5;
    bool b2 = wp5 < wp1;

    p5.reset();

    TEST (wp1.use_count() == 0);
    TEST (wp2.use_count() == 0);
    TEST (wp3.use_count() == 0);

    // Test operator< stability for std::set< weak_ptr<> >
    // Thanks to Joe Gottman for pointing this out

    TEST (b1 == (wp1 < wp5));
    TEST (b2 == (wp5 < wp1));

    {
      // note that both get_object and release_object deal with int*
      shared_ptr<void> p6(get_object(), release_object);
    }
  }

  TEST (cnt == 0);

  return 0;
}
