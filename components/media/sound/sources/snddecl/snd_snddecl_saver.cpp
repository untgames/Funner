#include "shared.h"

using namespace media;
using namespace common;

namespace components
{

namespace snddecl_saver
{

/*
    Вспомогательный класс сохранения библиотеки деклараций звуков
*/

class SoundDeclarationLibrarySaver
{
  private:
    XmlWriter                      writer;  //сериализатор XML
    const SoundDeclarationLibrary& library; //библиотека деклараций      
    
  private:
  
    /*
        Сохранение декларации
    */

    void SaveDeclarationParams (const SoundDeclaration& declaration)
    {
      XmlWriter::Scope scope (writer, "params");
      
      writer.WriteAttribute ("gain", declaration.Param (SoundParam_Gain));
      writer.WriteAttribute ("min_gain", declaration.Param (SoundParam_MinimumGain));
      writer.WriteAttribute ("max_gain", declaration.Param (SoundParam_MaximumGain));
      writer.WriteAttribute ("inner_angle", declaration.Param (SoundParam_InnerAngle));
      writer.WriteAttribute ("outer_angle", declaration.Param (SoundParam_OuterAngle));
      writer.WriteAttribute ("outer_gain", declaration.Param (SoundParam_OuterGain));
      writer.WriteAttribute ("reference_distance", declaration.Param (SoundParam_ReferenceDistance));
      writer.WriteAttribute ("maximum_distance", declaration.Param (SoundParam_MaximumDistance));
      writer.WriteAttribute ("cull_distance", declaration.Param (SoundParam_CullDistance));
    }
    
    void SaveDeclarationSamples (const SoundDeclaration& declaration)
    {
      XmlWriter::Scope scope (writer, "samples");
      
      writer.WriteAttribute ("count", declaration.SamplesCount ());
      
      for (size_t i=0; i<declaration.SamplesCount (); i++)
        writer.WriteData (declaration.Sample (i));
    }
    
    void SaveDeclaration (const char* name, const SoundDeclaration& declaration)
    {
      XmlWriter::Scope scope (writer, "declaration");
      
        //сохранение базовых атрибутов

      writer.WriteAttribute ("id", name);
      writer.WriteAttribute ("name", declaration.Name ());
      writer.WriteAttribute ("type", declaration.Type ());
      writer.WriteAttribute ("looping", declaration.Looping ());
      
        //сохранение параметров      
        
      SaveDeclarationParams (declaration);
      
        //сохранение сэмплов
        
      SaveDeclarationSamples (declaration);
    }
  
    /*
        Сохранение библиотеки
    */
    
    void SaveLibrary ()
    {
      XmlWriter::Scope scope (writer, "sound_declaration_library");
      
        //сохранение деклараций
        
      for (SoundDeclarationLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
        SaveDeclaration (library.ItemId ((const SoundDeclarationLibrary::ConstIterator&)i), *i);
    }
    
  public:
    /*
        Конструктор
    */
  
    SoundDeclarationLibrarySaver (const char* file_name, const SoundDeclarationLibrary& in_library)
      : writer (file_name), library (in_library)
    {
      SaveLibrary ();
    }
};

/*
    Сохранение библиотеки деклараций звуков
*/

void snddecl_save_library (const char* file_name, const SoundDeclarationLibrary& library)
{
  SoundDeclarationLibrarySaver (file_name, library);
}

/*
   Компонент загрузки деклараций звука
*/

class SnddeclSaverComponent
{
  public:
    //загрузка компонента
    SnddeclSaverComponent () 
    {
      SoundDeclarationManager::RegisterSaver ("snddecl", &snddecl_save_library);
    }
};

extern "C"
{

ComponentRegistrator<SnddeclSaverComponent> SnddeclSaver ("media.sound.savers.snddecl");

}

}

}
