#ifdef HAS_PWD_H
#include <pwd.h>
#endif

#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void UnistdPlatform::MountSystemSpecificPaths ()
{
#ifdef HAS_PWD_H
  int myuid = getuid ();

  passwd* mypasswd = getpwuid (myuid);
  
  const char* home_dir = mypasswd->pwd_dir;

#else
  const char* home_dir = getenv ("HOME");  
#endif

  if (!home_dir)
    home_dir = "~";

  FileSystem::MountLink ("/system/appdata", "/std//var");
  FileSystem::MountLink ("/system/profile", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/personal", (stl::string ("/std/") + home_dir).c_str ());
  FileSystem::MountLink ("/system/inetcache", "/std//tmp");
  FileSystem::MountLink ("/system/temp", "/std//tmp");
}
