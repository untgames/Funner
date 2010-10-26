#include "test.h"

const char* EXIST_FILE_NAME     = "data/test.txt";
const char* NON_EXIST_FILE_NAME = "no-file";

void check_file (const char* file_name)
{
  if (common::FileSystem::IsFileExist (file_name)) printf ("File '%s' found\n", file_name);
  else                                             printf ("File '%s' not found\n", file_name);
}

int main ()
{
  printf ("Results of is_file_exist:\n");
  
  check_file (EXIST_FILE_NAME);
  check_file (NON_EXIST_FILE_NAME);

  return 0;
}
