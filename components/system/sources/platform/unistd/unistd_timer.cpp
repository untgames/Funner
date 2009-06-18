#include <unistd.h>

#include <platform/platform.h>

using namespace syslib;

/*
    ѕриостановка выполнени€ приложени€ на miliseconds милисекунд
*/

void Platform::Sleep (size_t miliseconds)
{
  usleep (miliseconds * 1000);
}

