#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void UnistdPlatform::MountSystemSpecificPaths ()
{
  const char* home_dir = getenv ("HOME");
  
  if (!home_dir)
    home_dir = "~";

  FileSystem::MountLink ("/system/appdata", "/std//var");
  FileSystem::MountLink ("/system/profile", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/personal", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/inetcache", "/std//tmp");
  FileSystem::MountLink ("/system/temp", "/std//tmp");
}
