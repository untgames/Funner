#include "shared.h"

namespace
{

const char* CONFIG_PATH = "/anonymous/platform_config.xml";

/*
   Компонент сохранения параметров платформы в файле
*/

class PlatformConfigurationComponent
{
  public:
    //загрузка компонента
    PlatformConfigurationComponent ()
      : config_file ((size_t)0, common::FileMode_ReadWrite | common::FileMode_Create)
    {
      try
      {
        common::FileSystem::Rename (config_file.Path (), CONFIG_PATH);

        common::XmlWriter writer (CONFIG_PATH);

        writer.BeginNode ("Configuration");
        writer.BeginNode ("Properties");

        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        SaveProperty (writer, "OS", "macosx");

        NSArray* languages = [[NSUserDefaults standardUserDefaults] objectForKey:@"AppleLanguages"];

        if ([languages count])
          SaveProperty (writer, "Language", [[languages objectAtIndex:0] UTF8String]);

        [pool release];
      }
      catch (xtl::exception& e)
      {
        printf ("Exception caught '%s'\n", e.what ());
        throw;
      }
    }

  private:
    void SaveProperty (common::XmlWriter& writer, const char* name, const char* value)
    {
      writer.BeginNode ("Property");

      writer.WriteAttribute ("Name", name);
      writer.WriteAttribute ("Value", value);

      writer.EndNode ();
    }

  private:
    common::MemFile config_file;
};

extern "C"
{

common::ComponentRegistrator<PlatformConfigurationComponent> PlatformConfiguration ("common.file_systems.platform_configuration");

}

}
