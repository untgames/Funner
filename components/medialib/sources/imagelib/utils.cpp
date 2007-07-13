#include "shared.h"

void RaiseException (const char* source, const char* description)
{
  common::Exception (description).Raise (source);
}

const char* GetExtension (const char* file_name)
{
  const char*  ext;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return file_name;

  ext = file_name + len;

  for (; len && (*ext != '.'); len--, ext--);

  if (!len)
    return file_name;

  return ext + 1;
}

size_t GetFolder (const char* file_name)
{
  const char*  folder;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return 0;

  folder = file_name + len;

  for (; len && (*folder != '/'); len--, folder--);

  return len;
}
