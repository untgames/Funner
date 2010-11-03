#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSFileManager.h>

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
  for (NSString* path in paths)
  {
    if (check_dir (file_manager, path, false))
      return path;
  }

  for (NSString* path in paths)
  {
    if (check_dir (file_manager, path, true))
      return path;
  }
  
  return nil;
}

}

void CocoaIPhonePlatform::MountSystemSpecificPaths ()
{
  const char* home_dir = getenv ("HOME");
    
  if (!home_dir)
    home_dir = "~";

  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSArray* paths;

  paths = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES);
  
  NSFileManager* file_manager = [NSFileManager defaultManager];

  if (NSString* path = check_dir (file_manager, paths))
  {
    stl::string personal_path = common::format ("/std/%s", [path cStringUsingEncoding:NSUTF8StringEncoding]);

    FileSystem::Mount ("/system/personal", personal_path.c_str ());
    FileSystem::Mount ("/system/appdata", personal_path.c_str ());
    FileSystem::Mount ("/system/profile", personal_path.c_str ());
  }
  else
  {
    FileSystem::MountLink ("/system/appdata", "/std//var");
    FileSystem::MountLink ("/system/profile", (stl::string ("/std/") + home_dir).c_str ());
    FileSystem::MountLink ("/system/personal", (stl::string ("/std/") + home_dir).c_str ());
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
