#include "shared.h"

using namespace common;

namespace
{

stl::string GetFolderPath (int csidl)
{
#if !defined(WINCE)
  stl::string result;
  
  result.fast_resize (MAX_PATH);
  
  if (!SHGetSpecialFolderPathA (0, &result [0], csidl, FALSE))
    raise_error ("::SHGetSpecialFolderPath");

  return result;
#else
  stl::wstring result;
  
  result.fast_resize (MAX_PATH);
  
  if (!SHGetSpecialFolderPath (0, (LPTSTR)&result [0], csidl, FALSE))
    raise_error ("::SHGetSpecialFolderPath");

  return tostring(result);
#endif      
}

stl::string GetTempDirPath ()
{
  stl::string result;
  
  result.fast_resize (MAX_PATH);
  
  GetTempPath (result.size (), &result [0]);
  
  return result;
}

}

void Win32Platform::MountSystemSpecificPaths ()
{
  FileSystem::Mount ("/system/appdata",   GetFolderPath (CSIDL_APPDATA).c_str ());
  FileSystem::Mount ("/system/personal",  GetFolderPath (CSIDL_PERSONAL).c_str ());  
#if !defined(WINCE)
  FileSystem::Mount ("/system/profile",   GetFolderPath (CSIDL_PROFILE).c_str ()); 
  FileSystem::Mount ("/system/inetcache", GetFolderPath (CSIDL_INTERNET_CACHE).c_str ());
#else
  FileSystem::Mount ("/system/profile",   "\Documents and Settings\Default"); 
  FileSystem::Mount ("/system/inetcache", GetTempDirPath ().c_str ());
#endif
  FileSystem::Mount ("/system/temp",      GetTempDirPath ().c_str ());
}
