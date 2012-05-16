#include "../shared.h"

#ifndef LOCALE_SNAME
#define LOCALE_SNAME 0x0000005c   // locale name (ie: en-us)
#endif

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

        SaveProperty (writer, "OS", "windows");
        SaveProperty (writer, "ExecutableSuffix", ".exe");
        SaveProperty (writer, "SharedLibrarySuffix", ".dll");
        SaveProperty (writer, "SharedLibraryPrefix", "");

        wchar_t language_name_buffer [16];

        memset (language_name_buffer, 0, sizeof (language_name_buffer));

        GetLocaleInfoW (LOCALE_USER_DEFAULT, LOCALE_SNAME, language_name_buffer, sizeof (language_name_buffer));

        if (xtl::xstrlen (language_name_buffer))
          SaveProperty (writer, "Language", common::to_utf8_string (language_name_buffer).c_str ());
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
