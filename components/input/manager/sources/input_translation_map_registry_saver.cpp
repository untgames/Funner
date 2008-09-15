#include "shared.h"

using namespace input;
using namespace common;

namespace
{

class TranslationMapRegistrySaver : public TranslationMapRegistry::IEnumerator
{
  public:
    TranslationMapRegistrySaver (const char* file_name, const TranslationMapRegistry& source_map_registry)
      : writer (file_name)
    {
      XmlWriter::Scope scope (writer, "TranslationMapRegistry");

      source_map_registry.Enumerate (*this);
    }

    void Process (const char* profile, const char* translation_map)
    {
      XmlWriter::Scope scope (writer, "Translation");

      writer.WriteAttribute ("Profile", profile);
      writer.WriteAttribute ("TranslationMap", translation_map);
    }

  private:
    XmlWriter writer;
};

/*
   Компонент сохранения карт трансляции
*/

class KeyregSaverComponent
{
  public:
    //загрузка компонента
    KeyregSaverComponent () 
    {
      TranslationMapRegistryManager::RegisterSaver ("xkeyreg", &SaveRegistry);
    }

  private:
    static void SaveRegistry (const char* file_name, const TranslationMapRegistry& source_map_registry)
    {
      TranslationMapRegistrySaver (file_name, source_map_registry);
    }
};

extern "C"
{

ComponentRegistrator<KeyregSaverComponent> XKeyregSaver ("input.savers.xkeyreg");

}

}
