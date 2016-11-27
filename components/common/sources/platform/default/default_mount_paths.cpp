#include "shared.h"

using namespace common;

void DefaultPlatform::MountSystemSpecificPaths ()
{
}

namespace components
{

namespace standard_file_paths_mount
{

/*
    Компонент, регистрирующий стандартные точки монтирования файловых путей
*/

class Component
{
  public:
    Component ()
    {            
      Platform::MountSystemSpecificPaths ();
    }
};

extern "C"
{

ComponentRegistrator<Component> StandardFilePathsMount ("common.file_systems.StandardFilePathsMount");

}

}

}
