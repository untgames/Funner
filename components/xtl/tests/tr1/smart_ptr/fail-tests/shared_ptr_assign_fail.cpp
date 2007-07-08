#include "test.h"

int main()
{
    shared_ptr<int> p;
    p = new int(42); // assignment must fail
    return 0;
}
