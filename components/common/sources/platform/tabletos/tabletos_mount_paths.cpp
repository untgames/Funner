#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

void TabletOsPlatform::MountSystemSpecificPaths ()
{
  const char* app_dir = getenv ("APP_DIR");

  if (!app_dir)
  {
    UnistdPlatform::MountSystemSpecificPaths ();
    return;
  }  

  FileSystem::MountLink ("/system/appdata", common::format ("/std/%s/data", app_dir).c_str ());
  FileSystem::MountLink ("/system/profile", common::format ("/std/%s/data", app_dir).c_str ());
  FileSystem::MountLink ("/system/personal", common::format ("/std/%s/shared/documents", app_dir).c_str ());
  FileSystem::MountLink ("/system/inetcache", common::format ("/std/%s/shared/downloads", app_dir).c_str ());
  FileSystem::MountLink ("/system/temp", common::format ("/std/%s/tmp", app_dir).c_str ());
}
