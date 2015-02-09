#include "shared.h"

using namespace common;

namespace
{

#ifndef WP8
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
  
  result.fast_resize (MAX_PATH + 1);
  
  DWORD length = GetTempPathW ((DWORD)result.size (), &result [0]);

  if (length == 0 || length > result.size ())
    raise_error ("::GetTempPathW");

  result.resize (length);

  if (!result.empty () && result [result.size ()-1] == L'\\')
    result.pop_back ();

  return to_utf8_string (result);
}
#endif

}

void Win32Platform::MountSystemSpecificPaths ()
{
#ifndef WP8
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
#else
  FileSystem::Mount ("/system/appdata",   "/FUNNER_UNKNOWN");
  FileSystem::Mount ("/system/personal",  "/FUNNER_UNKNOWN");
  FileSystem::Mount ("/system/profile",   "/FUNNER_UNKNOWN"); 
  FileSystem::Mount ("/system/inetcache", "/FUNNER_UNKNOWN");
  FileSystem::Mount ("/system/temp",      "/FUNNER_UNKNOWN");
#endif
}
