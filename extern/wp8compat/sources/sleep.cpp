#include <windows.h>
#include <concrt.h>

extern "C"
{

void Sleep (DWORD milliseconds)
{
  Concurrency::wait (milliseconds);
}

}
