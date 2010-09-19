#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSPathUtilities.h>

#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void CocoaIPhonePlatform::MountSystemPaths ()
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSArray* paths;

  paths = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES);

  if ([paths count])
  {
    stl::string personal_path = common::format ("/std/%s", [(NSString*)[paths objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/personal", personal_path.c_str ());
    FileSystem::Mount ("/system/appdata", personal_path.c_str ());
    FileSystem::Mount ("/system/profile", personal_path.c_str ());
  }

  paths = NSSearchPathForDirectoriesInDomains (NSCachesDirectory, NSUserDomainMask, YES);

  if ([paths count])
  {
    stl::string cache_path = common::format ("/std/%s", [(NSString*)[paths objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/inetcache", cache_path.c_str ());
  }

  const char* c_temp_path = [NSTemporaryDirectory () cStringUsingEncoding:NSUTF8StringEncoding];

  if (c_temp_path)
  {
    stl::string temp_path = common::format ("/std/%s", c_temp_path);

    FileSystem::Mount ("/system/temp", temp_path.c_str ());
  }

  [pool release];
}
