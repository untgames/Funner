#include "shared.h"

using namespace common;
using namespace media::physics;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "media.physics.XmlPhysicsLibraryLoader"; //имя потока протоколирования

template <unsigned int Size>
void read (Parser::Iterator iter, const char* str, math::vector <float, Size>& value)
{
  StringArray components = common::split (str);

  if (components.Size () != Size)
    raise_parser_exception (*iter, "Invalid vector format");

  for (size_t i = 0; i < Size; i++)
    value [i] = (float)atof (components [i]);
}

void read (Parser::Iterator iter, const char* str, float& value)
{
  value = (float)atof (str);
}

/*
    Загрузчик физической библиотеки в формате Xml
*/

class XmlPhysicsLibraryLoader
{
  private:

  public:
    XmlPhysicsLibraryLoader (const char* file_name, PhysicsLibrary& library) : parser (file_name, "xml")
    {
/*      for_each_child (parser.Root ().First ("animation_library"), "animation",
                      xtl::bind (&XmlAnimationLibraryLoader::ParseAnimation, this, _1, xtl::ref (library)));*/

        //протоколирование

      Log log (LOG_NAME);

      parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));
    }
    
  private:
    Parser parser;   //парсер
};

/*
    Автоматическая регистрация компонента
*/

class XPhysLoaderComponent
{
  public:
    XPhysLoaderComponent ()
    {
      PhysicsLibraryManager::RegisterLoader ("xphys", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, PhysicsLibrary& library)
    {
      XmlPhysicsLibraryLoader (file_name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<XPhysLoaderComponent> XPhysLoader ("media.physics.loaders.XPhysLoader");

}
