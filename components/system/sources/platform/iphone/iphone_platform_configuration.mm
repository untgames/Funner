#import <UIKit/UIDevice.h>
#import <UIKit/UIScreen.h>

#include "shared.h"

#include <sys/sysctl.h>

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

        SaveProperty (writer, "OS", "ios");
        SaveProperty (writer, "Platform", UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? "ipad" : "iphone");
        SaveProperty (writer, "Language", [((NSString*)[[NSLocale preferredLanguages] objectAtIndex:0]) UTF8String]);
        SaveProperty (writer, "OSVersion", [[[UIDevice currentDevice] systemVersion] UTF8String]);

        size_t model_name_size = 0;

        sysctlbyname ("hw.machine", 0, &model_name_size, 0, 0);

        xtl::uninitialized_storage<char> model_name (model_name_size + 1);

        if (!sysctlbyname ("hw.machine", model_name.data (), &model_name_size, 0, 0))
        {
          model_name.data () [model_name_size] = 0;
          SaveProperty (writer, "DeviceModel", model_name.data ());
        }

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
