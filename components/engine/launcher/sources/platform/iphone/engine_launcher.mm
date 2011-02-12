#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#include <cstdio>

#include <engine/engine.h>

using namespace engine;

//точка входа
int main (int argc, const char* argv [])
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSBundle* main_bundle = [NSBundle mainBundle];

  if (!main_bundle)
  {
      printf ("Can't locate main bundle\n");
      return 1;
  }

  NSString* resources_path = [main_bundle resourcePath];

  if (![[NSFileManager defaultManager] changeCurrentDirectoryPath:resources_path])
  {
    NSLog (@"Can't set current dir '%@'\n", resources_path);
    [pool release];
    return 1;
  }

  [pool release];

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
      printf ("Funner startup failed!");
      return 1;
  }

  if (!funner->ParseCommandLine (argc, argv))
      return 1;

  funner->Run ();

  delete funner;
}
