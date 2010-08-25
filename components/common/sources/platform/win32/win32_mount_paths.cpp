#include "shared.h"

using namespace common;

namespace
{

stl::string GetFolderPath (int csidl)
{
  stl::string result;
  
  result.fast_resize (MAX_PATH);
  
  if (!SHGetSpecialFolderPath (0, &result [0], csidl, FALSE))
    raise_error ("::SHGetSpecialFolderPath");

  return result;      
}

stl::string GetTempDirPath ()
{
  stl::string result;
  
  result.fast_resize (MAX_PATH);
  
  GetTempPath (result.size (), &result [0]);
  
  return result;
}

}

void Win32Platform::MountSystemPaths ()
{
  FileSystem::Mount ("/system/appdata",   GetFolderPath (CSIDL_APPDATA).c_str ());
  FileSystem::Mount ("/system/personal",  GetFolderPath (CSIDL_PERSONAL).c_str ());  
  FileSystem::Mount ("/system/profile",   GetFolderPath (CSIDL_PROFILE).c_str ()); 
  FileSystem::Mount ("/system/cookies",   GetFolderPath (CSIDL_COOKIES).c_str ());
  FileSystem::Mount ("/system/inetcache", GetFolderPath (CSIDL_INTERNET_CACHE).c_str ());
  FileSystem::Mount ("/system/temp",      GetTempDirPath ().c_str ());
}
