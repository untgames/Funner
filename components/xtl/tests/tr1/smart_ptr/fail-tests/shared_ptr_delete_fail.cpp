#include "test.h"
int main()
{
    shared_ptr<int> p;
    delete p; // must fail
    return 0;
}
