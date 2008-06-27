#include "shared.h"

namespace common
{

const char* get_prefix_end (const char* branch_name)
{
  const char* prefix_end = branch_name;

  for (const char* cur_pos = branch_name; *cur_pos; cur_pos++)
  {
    if (*cur_pos == '.')
      prefix_end = cur_pos;
  }

  return prefix_end;
}

}