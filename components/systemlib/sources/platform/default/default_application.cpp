#include <time.h>
#include <platform/platform.h>

using namespace syslib;

/*
    ѕриостановка выполнени€ приложени€ на miliseconds милисекунд
*/

void DefaultPlatform::Sleep (size_t miliseconds)
{
  clock_t end = clock () + CLOCKS_PER_SEC / 1000 * miliseconds;
  
  while (clock () < end);
}

/*
    –абота с очередью сообщений
*/

bool DefaultPlatform::IsMessageQueueEmpty ()
{
  return true;
}

void DefaultPlatform::DoNextEvent ()
{
}

void DefaultPlatform::WaitMessage ()
{
}
