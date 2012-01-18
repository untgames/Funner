#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace social
{

namespace game_kit
{

void release_ns_object (const void* handle)
{
  [(NSObject*)handle release];
}

}

}
