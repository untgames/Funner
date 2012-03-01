#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <common/log.h>

#include <engine/subsystem_manager.h>

using namespace engine;

const char* CONFIGURATION_FILE_NAME = "config_scene_manager.xml";

int main ()
{
  printf ("Results of scene_manager_test:\n");

  try  
  {            
    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);
    
    manager.Execute ("data/scene_manager.lua");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
