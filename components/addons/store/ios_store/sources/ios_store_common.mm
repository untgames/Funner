#include "shared.h"

#import <Foundation/NSObject.h>

namespace store
{

namespace ios_store
{

void release_ns_object (const void* handle)
{
  [(NSObject*)handle release];
}

}

}
