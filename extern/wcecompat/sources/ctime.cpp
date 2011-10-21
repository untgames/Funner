// Added by Taras Nikisha

#include        <time.h>

char * ctime(const time_t *timer)
{
  return asctime(localtime(timer));
}