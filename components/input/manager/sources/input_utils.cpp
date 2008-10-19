#include "shared.h"

namespace input
{

void split_event (const char* event, common::StringArray& target_arguments)
{
  target_arguments = common::split (event, " ", " \t", "''\"\"");
}

}

