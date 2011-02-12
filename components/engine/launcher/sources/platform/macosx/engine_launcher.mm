#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <AppKit/NSApplication.h>

#include <cstdio>
#include <cstring>

#include <dlfcn.h>

#include <engine/engine.h>

using namespace engine;

namespace
{

const char* ENGINE_LIBRARY_NAME = "funner.dylib";

}

//точка входа
int main (int argc, const char* argv [])
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  if (!NSApplicationLoad ())
  {
    printf ("Can't start application, '::NSApplicationLoad' failed\n");
    [pool release];
    return 1;
  }

  NSBundle* main_bundle = [NSBundle mainBundle];

  if (main_bundle)
  {
    NSString* resources_path = [main_bundle resourcePath];

    if (![[NSFileManager defaultManager] changeCurrentDirectoryPath:resources_path])
    {
      NSLog (@"Can't set current dir '%@'\n", resources_path);
      [pool release];
      return 1;
    }
  }
  else
    printf ("Can't get main bundle\n");

  [pool release];

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
