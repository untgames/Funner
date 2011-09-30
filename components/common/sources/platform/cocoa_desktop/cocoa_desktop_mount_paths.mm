#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSPathUtilities.h>

#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

namespace
{

bool check_dir (NSFileManager* file_manager, NSString* path, bool create_dir)
{
  BOOL is_dir = NO;

  if ([file_manager fileExistsAtPath:path isDirectory:&is_dir])
    return is_dir == YES;

  if (!create_dir)
    return false;

  NSError* error = nil;

  return [file_manager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error] == YES;
}

NSString* check_dir (NSFileManager* file_manager, NSArray* paths)
{
  for (size_t i = 0, count = [paths count]; i < count; i++)
  {
    NSString* path = [paths objectAtIndex:i];

    if (check_dir (file_manager, path, false))
      return path;
  }

  for (size_t i = 0, count = [paths count]; i < count; i++)
  {
    NSString* path = [paths objectAtIndex:i];

    if (check_dir (file_manager, path, true))
      return path;
  }

  return nil;
}

}

void CocoaDesktopPlatform::MountSystemSpecificPaths ()
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSString *home_path = NSHomeDirectory ();

  stl::string appdata_path = common::format ("/std/%s", [[home_path stringByAppendingPathComponent:@"Library/Preferences"] cStringUsingEncoding:NSUTF8StringEncoding]),
              profile_path = common::format ("/std/%s", [home_path cStringUsingEncoding:NSUTF8StringEncoding]);

  try
  {
    if (!FileSystem::IsFileExist (appdata_path.c_str ()))
      FileSystem::Mkdir (appdata_path.c_str ());

    FileSystem::Mount ("/system/appdata", appdata_path.c_str ());
  }
  catch (...)
  {
  }

  FileSystem::Mount ("/system/profile", profile_path.c_str ());

  NSArray* paths;

  paths = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES);

  NSFileManager* file_manager = [NSFileManager defaultManager];

  if (NSString* path = check_dir (file_manager, paths))
  {
    stl::string personal_path = common::format ("/std/%s", [path cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/personal", personal_path.c_str ());
  }

  paths = NSSearchPathForDirectoriesInDomains (NSCachesDirectory, NSUserDomainMask, YES);

  if (NSString* path = check_dir (file_manager, paths))
  {
    stl::string cache_path = common::format ("/std/%s", [path cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/inetcache", cache_path.c_str ());
  }
  else
  {
    FileSystem::MountLink ("/system/inetcache", "/std//tmp");
  }

  NSString* cocoa_temp_path = NSTemporaryDirectory ();

  if (check_dir (file_manager, cocoa_temp_path, true))
  {
    stl::string temp_path = common::format ("/std/%s", [cocoa_temp_path cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/temp", temp_path.c_str ());
  }
  else
  {
    FileSystem::MountLink ("/system/temp", "/std//tmp");
  }

  [pool release];
}
