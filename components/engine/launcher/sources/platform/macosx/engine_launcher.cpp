#include <cstdio>
#include <cstring>

#include <dlfcn.h>
#include <unistd.h>

#include <CFBundle.h>

#include <engine/engine.h>

using namespace engine;

namespace
{

const char* ENGINE_LIBRARY_NAME = "funner.dylib";
const char* RESOURCE_PATH       = "/Contents/Resources/";

}

//точка входа
int main (int argc, const char* argv [])
{
  CFBundleRef main_bundle = CFBundleGetMainBundle ();

  if (main_bundle)
  {
    CFURLRef main_bundle_url = CFBundleCopyBundleURL (main_bundle);

    if (!main_bundle_url)
    {
      printf ("Can't get main bundle url\n");
      return 1;
    }
    else
    {
      char buf [16 * 1024];

      CFStringRef filesystem_path = CFURLCopyFileSystemPath (main_bundle_url, kCFURLPOSIXPathStyle);

      CFRelease (main_bundle_url);

      if (!filesystem_path)
      {
        printf ("Can't get filesystem path to bundle\n");
        return 1;
      }

      Boolean get_string_result = CFStringGetCString (filesystem_path, buf, 16 * 1024, kCFStringEncodingASCII);

      CFRelease (filesystem_path);

      if (!get_string_result)
      {
        printf ("Can't get c-string path to bundle\n");
        return 1;
      }

      if (strstr (buf, ".app"))
      {
        strcat (buf, RESOURCE_PATH);

        if (chdir (buf))
        {
          printf ("Can't set working dir '%s'\n", buf);
          return 1;
        }
      }
    }
  }
  else
    printf ("Can't get main bundle\n");

  size_t dlopen_flags = RTLD_NOW | RTLD_GLOBAL;

  void* library = dlopen (ENGINE_LIBRARY_NAME, dlopen_flags);

  if (!library)
  {
    printf ("Can't load engine library\n");
    return 1;
  }
  
  FunnerInitProc FunnerInit = (FunnerInitProc)dlsym (library, "FunnerInit");
  
  if (!FunnerInit)
  {
    printf ("Bad library (entries not found)\n");
    return 1;
  }

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
    printf ("Funner startup failed!");
    return 1;
  }
  
  if (!funner->ParseCommandLine (argc, argv))
  {
    return 1;
  }
  
  funner->Run ();
  
  delete funner;
  
  dlclose (library);
}
