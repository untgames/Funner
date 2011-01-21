#ifdef ANDROID

void * lfind (const void *key, const void *base, unsigned int *num, unsigned int width, int (*compare)(const void *, const void *))
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
