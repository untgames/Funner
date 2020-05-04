#if defined (ANDROID) || defined (_WIN32_WCE)

# include <sys/types.h>

void * lfind (const void *key, const void *base, size_t *num, size_t width, int (*compare)(const void *, const void *))
{
  unsigned int place = 0;

  while (place < *num)
  {
    if (compare(key, base) == 0)
    {
      return (void *)base;
    }
    else
    {
      base = (char*)base + width;
      place++;
    }
  }
  
  return 0;
}

#endif
