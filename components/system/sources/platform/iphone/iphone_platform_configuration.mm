#import <UIKit/UIDevice.h>
#import <UIKit/UIScreen.h>

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

        SaveProperty (writer, "Platform", UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? "ipad" : "iphone");
        SaveProperty (writer, "Language", [((NSString*)[[NSLocale preferredLanguages] objectAtIndex:0]) UTF8String]);

        float screen_width = [UIScreen mainScreen].bounds.size.width, screen_height = [UIScreen mainScreen].bounds.size.height;

        if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
        {
          float scale = [UIScreen mainScreen].scale;

          screen_width  *= scale;
          screen_height *= scale;
        }

        SaveProperty (writer, "ScreenWidth", common::format ("%d", (int)screen_width).c_str ());
        SaveProperty (writer, "ScreenHeight", common::format ("%d", (int)screen_height).c_str ());
        SaveProperty (writer, "ScreenAspectRatio", common::format ("%d:%d", (int)screen_width, (int)screen_height).c_str ());

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
