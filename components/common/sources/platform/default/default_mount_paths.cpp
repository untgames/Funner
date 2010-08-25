#include "shared.h"

using namespace common;

void DefaultPlatform::MountSystemPaths ()
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
      Platform::MountSystemPaths ();
    }
};

}

extern "C"
{

ComponentRegistrator<Component> StandardFilePathsMount ("common.file_systems.StandardFilePathsMount");

}
