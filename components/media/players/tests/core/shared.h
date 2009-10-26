#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/players.h>

using namespace media::players;
	
void dump (const char* name, const Playlist& list)
{
  printf ("%s={", name, list.Size ());
  
  for (size_t i=0, count=list.Size (); i<count; i++)
  {
    if (i)
      printf (", ");
  
    printf ("'%s'", list.Item (i));
  }
  
  printf ("}\n");
}
