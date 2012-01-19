#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace social
{

namespace game_kit
{

const char* OK_STATUS = "Ok";

void release_ns_object (const void* handle)
{
  [(NSObject*)handle release];
}

}

}
