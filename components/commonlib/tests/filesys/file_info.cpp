#include <string.h>
#include "test.h"

const char* file_names [] = {"media/tests/filesys","media/tests/filesys/test.txt"};

int main ()
{
  printf ("Results of file_info_test:\n");

  FileInfo info;
  
  for (size_t i=0;i<sizeof (file_names)/sizeof (const char*);i++)
  {
    const char* file_name = file_names [i];    
    
    if (!FileSystem::GetFileInfo (file_name,info))
    {
      printf ("File '%s' not found\n",file_name);
      continue;
    }    
    
    printf ("File info '%s':\n",file_name);
    printf ("Directory:   %s\n",info.is_dir?"true":"false");        

    if (!info.is_dir)
      printf ("Size:        %u\n",info.size);

//    printf ("Time modify: %s",ctime (&info.time_modify));
//    printf ("Time create: %s",ctime (&info.time_create));
//    printf ("Time access: %s",ctime (&info.time_access));
  }

  return 0;
}
