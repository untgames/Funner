#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void AndroidPlatform::MountSystemSpecificPaths ()
{
  const char* home_dir = getenv ("ANDROID_DATA");
  
  if (!home_dir)
    home_dir = "/data";
    
  const char* external_data_dir = getenv ("ANDROID_EXTERNAL_DATA");

  if (!external_data_dir)
    external_data_dir = home_dir;

  const char* tmp_dir = getenv ("TEMP");

  if (!tmp_dir)
    tmp_dir = "/data/local/tmp";
  
  FileSystem::MountLink ("/system/appdata", (stl::string ("/std/") + external_data_dir).c_str ());
  FileSystem::MountLink ("/system/profile", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/personal", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/inetcache", (stl::string ("/std/") + tmp_dir).c_str ());
  FileSystem::MountLink ("/system/temp", (stl::string ("/std/") + tmp_dir).c_str ());  
}
