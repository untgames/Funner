#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void AndroidPlatform::MountSystemSpecificPaths ()
{
  const char* home_dir = getenv ("HOME");
  const char* tmp_dir  = getenv ("TEMP");  
  
  if (!home_dir)
    home_dir = "~";
    
  if (!tmp_dir)
    tmp_dir = "~";

  FileSystem::MountLink ("/system/appdata", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/profile", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/personal", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/inetcache", (stl::string ("/std/") + tmp_dir).c_str ());
  FileSystem::MountLink ("/system/temp", (stl::string ("/std/") + tmp_dir).c_str ());  
}
