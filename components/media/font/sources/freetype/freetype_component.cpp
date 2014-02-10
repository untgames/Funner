#include "shared.h"

using namespace media;
using namespace media::freetype;

/*
    Загрузка шрифта
*/

namespace components
{

namespace freetype_loader
{

/*
   Компонент загрузки шрифтов
*/

class FreetypeLoaderComponent
{
  public:
    //загрузка компонента
    FreetypeLoaderComponent ()
    {
      FontManager::RegisterLoader ("ttf", &LoadFont);
    }

  private:
    static IFontDesc* LoadFont (const char* file_name)
    {
      if (!file_name)
        throw xtl::make_null_argument_exception ("media::FreetypeLoaderComponent::LoadFont", "file_name");

      return new FreetypeFontDesc (file_name);
    }
};

extern "C"
{

common::ComponentRegistrator<FreetypeLoaderComponent> FreetypeLoader ("media.font.loaders.freetype");

}

}

}
