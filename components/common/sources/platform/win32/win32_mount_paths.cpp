#include "shared.h"

using namespace common;

namespace
{

stl::string GetFolderPath (int csidl)
{
  stl::wstring result;
  
  result.fast_resize (MAX_PATH);
  
#ifdef WINCE
  if (!SHGetSpecialFolderPath (0, &result [0], csidl, FALSE))
    raise_error ("::SHGetSpecialFolderPathW");
#else
  if (!SHGetSpecialFolderPathW (0, &result [0], csidl, FALSE))
    raise_error ("::SHGetSpecialFolderPathW");
  
#endif

  result.resize (xtl::xstrlen (result.c_str ()));

  if (!result.empty () && result [result.size ()-1] == L'\\')
    result.pop_back ();

  return to_utf8_string (result);
}

stl::string GetTempDirPath ()
{
  stl::wstring result;
  
  result.fast_resize (MAX_PATH);
  
  int length = GetTempPathW (result.size (), &result [0]);

  if (length < 0 || length > (int)result.size ())
    raise_error ("::GetTempPathW");

  result.resize (length);

  if (!result.empty () && result [result.size ()-1] == L'\\')
    result.pop_back ();

  return to_utf8_string (result);
}

}

void Win32Platform::MountSystemSpecificPaths ()
{
  FileSystem::Mount ("/system/appdata",   GetFolderPath (CSIDL_APPDATA).c_str ());
  FileSystem::Mount ("/system/personal",  GetFolderPath (CSIDL_PERSONAL).c_str ());  
#if defined(WINCE)
  FileSystem::Mount ("/system/profile",   "\\Documents and Settings\\Default"); 
  FileSystem::Mount ("/system/inetcache", GetTempDirPath ().c_str ());
#else
  FileSystem::Mount ("/system/profile",   GetFolderPath (CSIDL_PROFILE).c_str ()); 
  FileSystem::Mount ("/system/inetcache", GetFolderPath (CSIDL_INTERNET_CACHE).c_str ());
#endif
  FileSystem::Mount ("/system/temp",      GetTempDirPath ().c_str ());
}
