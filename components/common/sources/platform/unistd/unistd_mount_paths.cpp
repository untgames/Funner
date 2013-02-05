#ifdef HAS_PWD_H
#include <unistd.h>
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
  
  const char* home_dir = mypasswd ? mypasswd->pw_dir : (const char*)0;

#else
  const char* home_dir = getenv ("HOME");  
#endif

  if (!home_dir)
    home_dir = "~";

  stl::string data_dir = stl::string ("/std/") + home_dir;

  FileSystem::MountLink ("/system/appdata", data_dir.c_str ());
  FileSystem::MountLink ("/system/profile", data_dir.c_str ());
  FileSystem::MountLink ("/system/personal", data_dir.c_str ());
  FileSystem::MountLink ("/system/inetcache", "/std//tmp");
  FileSystem::MountLink ("/system/temp", "/std//tmp");
}
