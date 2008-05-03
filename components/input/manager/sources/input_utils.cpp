#include "shared.h"

namespace input
{

void split_event (const char* event, stl::vector<stl::string>& target_arguments)
{
  target_arguments = common::split (event, " ", " \t", "''\"\"");
}

}

