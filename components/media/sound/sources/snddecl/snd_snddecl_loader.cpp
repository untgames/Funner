#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

/*
    Загрузка библиотеки деклараций звуков
*/

namespace
{

void snddecl_load_library (const char* file_name, SoundDeclarationLibrary& library)
{
  ParseLog         log;
  Parser           p (log, file_name);
  Parser::Iterator iter = p.Root ()->First ("sound_declaration_library");

  if (!iter)
    raise <Exception> ("media::snddecl_load_library", "Incorrect file format, no root 'sound_declaration_library' tag");

  for (Parser::NamesakeIterator i = iter->First ("declaration"); i; i++)
  {
    if (!test (i, "id"))
    {
      log.Error (i, "Incorrect file format, 'id' tag property missing");
      continue;
    }

    SoundDeclaration decl;

    decl.Rename     (get <const char*> (i, "name", ""));
    decl.SetType    (get <const char*> (i, "type", ""));
    decl.SetLooping (get <bool> (i, "looping"));
    
    Parser::Iterator param = i->First ("params");

    if (i)
    {
      decl.SetParam (SoundParam_Gain,              get <float> (param, "gain"));
      decl.SetParam (SoundParam_MinimumGain,       get <float> (param, "min_gain"));
      decl.SetParam (SoundParam_MaximumGain,       get <float> (param, "max_gain"));
      decl.SetParam (SoundParam_InnerAngle,        get <float> (param, "inner_angle"));
      decl.SetParam (SoundParam_OuterAngle,        get <float> (param, "outer_angle"));
      decl.SetParam (SoundParam_OuterGain,         get <float> (param, "outer_gain"));
      decl.SetParam (SoundParam_ReferenceDistance, get <float> (param, "reference_distance"));
      decl.SetParam (SoundParam_MaximumDistance,   get <float> (param, "maximum_distance"));
      decl.SetParam (SoundParam_CullDistance,      get <float> (param, "cull_distance"));
    }
    
    param = i->First ("samples.#text");    
    
    size_t samples_count = param ? param->AttributesCount () : 0;    

    for (size_t j=0; j<samples_count; j++)
    {
      const char* sample_name = param->Attribute (j);

      if (!sample_name)
        continue;

      decl.AddSample (sample_name);
    }

    library.Attach (get <const char *> (i, "id"), decl);
  }  

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      raise <Exception> ("media::snddecl_load_library", log.Message(i));
}

/*
   Компонент загрузки деклараций звука
*/

class SnddeclLoaderComponent
{
  public:
    //загрузка компонента
    SnddeclLoaderComponent () 
    {
      SoundDeclarationManager::RegisterLoader ("snddecl", &snddecl_load_library);
    }
};

extern "C"
{

ComponentRegistrator<SnddeclLoaderComponent> SnddeclLoader ("media.sound.loaders.snddecl");

}

}
