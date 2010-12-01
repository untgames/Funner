#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSPathUtilities.h>

#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void CocoaDesktopPlatform::MountSystemSpecificPaths ()
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSString* home_path = NSHomeDirectory ();

  stl::string appdata_path = common::format ("/std/%s", [[home_path stringByAppendingPathComponent:@"Library/Preferences"] cStringUsingEncoding:NSUTF8StringEncoding]),
              profile_path = common::format ("/std/%s", [home_path cStringUsingEncoding:NSUTF8StringEncoding]);

  FileSystem::Mount ("/system/appdata", appdata_path.c_str ());
  FileSystem::Mount ("/system/profile", profile_path.c_str ());

  NSArray* paths;

  paths = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES);

  if ([paths count])
  {
    stl::string personal_path = common::format ("/std/%s", [(NSString*)[paths objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/personal", personal_path.c_str ());
  }

  paths = NSSearchPathForDirectoriesInDomains (NSCachesDirectory, NSSystemDomainMask, YES);

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
