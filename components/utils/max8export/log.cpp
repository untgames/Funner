#include "export.h"
#include <stdio.h>

static FILE* file = NULL;
static int level = 0;
static int tab_size = 4;
static bool new_line = true;

DumpStream dump;

bool logOpen (const char* file_name,int _tab)
{
  file     = fopen (file_name,"wt");
  tab_size = _tab;  
  new_line = true;
  level    = 0;

  return file!=NULL;    
}

void logClose ()
{
  for (int i=0;i<level;i++)
    logEndFrame ();

  fclose (file);
}

int logPrintf (const char* fmt,...)
{
  va_list list;
  char    buf [8192];

  va_start (list,fmt);

  int ret = vsprintf (buf,fmt,list);

  va_end (list);

  logPuts (buf);

  return ret;
}

void logPuts (const char* src)
{
  char buf [8192], *dst=buf;

  for (;*src;src++)
  {
    if (new_line)
    {
      memset (dst,' ',level*tab_size);
      dst += level*tab_size;
    }

    new_line = *src == '\n';

    switch (*src)
    {
      case '\r': continue;
/*      case '\t': 
        memset (dst,' ',tab_size);
        dst += tab_size;
        break;*/
      default:
        *dst++ = *src;
        break;
    }
  }

  *dst=0;

  fputs (buf,file);
}

void logBeginFrame (const char* name,const char* params)
{
  logPrintf ("%s %s\n{",name,params);
  level++;
  logPuts   ("\n");
}

void logEndFrame ()
{
  if (level)
  {
    level--;
    logPuts ("}\n");
  }
}

int logFrameLevel ()
{
  return level;
}
