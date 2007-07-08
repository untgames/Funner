#include "test.h"

struct deleter
{
    int data;

    deleter(): data(0)
    {
    }

    void operator()(void *)
    {
        TEST (data == 17041);
    }
};

struct deleter2
{
};

struct X
{
};

int main()
{
    printf ("Results of get_deleter_test:\n");

    {
        shared_ptr<X> p;

        TEST (get_deleter<void>(p) == 0);
        TEST (get_deleter<void const>(p) == 0);
        TEST (get_deleter<int>(p) == 0);
        TEST (get_deleter<int const>(p) == 0);
        TEST (get_deleter<X>(p) == 0);
        TEST (get_deleter<X const>(p) == 0);
        TEST (get_deleter<deleter>(p) == 0);
        TEST (get_deleter<deleter const>(p) == 0);
        TEST (get_deleter<deleter2>(p) == 0);
        TEST (get_deleter<deleter2 const>(p) == 0);
    }

    {
        shared_ptr<X> p(new X);

        TEST (get_deleter<void>(p) == 0);
        TEST (get_deleter<void const>(p) == 0);
        TEST (get_deleter<int>(p) == 0);
        TEST (get_deleter<int const>(p) == 0);
        TEST (get_deleter<X>(p) == 0);
        TEST (get_deleter<X const>(p) == 0);
        TEST (get_deleter<deleter>(p) == 0);
        TEST (get_deleter<deleter const>(p) == 0);
        TEST (get_deleter<deleter2>(p) == 0);
        TEST (get_deleter<deleter2 const>(p) == 0);
    }

    {
        X x;
        shared_ptr<X> p(&x, deleter());

        TEST (get_deleter<void>(p) == 0);
        TEST (get_deleter<void const>(p) == 0);
        TEST (get_deleter<int>(p) == 0);
        TEST (get_deleter<int const>(p) == 0);
        TEST (get_deleter<X>(p) == 0);
        TEST (get_deleter<X const>(p) == 0);
        TEST (get_deleter<deleter2>(p) == 0);
        TEST (get_deleter<deleter2 const>(p) == 0);

        deleter * q = get_deleter<deleter>(p);

        TEST (q != 0);
        TEST (q->data == 0);

        q->data = 17041;

        deleter const * r = get_deleter<deleter const>(p);

        TEST (r == q);
        TEST (r->data == 17041);
    }

    return 0;
}
