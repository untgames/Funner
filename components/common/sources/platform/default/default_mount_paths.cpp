#include "shared.h"

using namespace common;

void DefaultPlatform::MountSystemSpecificPaths ()
{
}

namespace
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

}

extern "C"
{

ComponentRegistrator<Component> StandardFilePathsMount ("common.file_systems.StandardFilePathsMount");

}
