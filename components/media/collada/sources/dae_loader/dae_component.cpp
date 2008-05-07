#include "shared.h"

using namespace media::collada;

namespace
{

//загрузчик Dae-файлов
class DaeLoaderComponent
{
  public:
    DaeLoaderComponent ()
    {
      ModelManager::RegisterLoader ("dae", &LoadDaeModel);
    }
    
  private:
    static void LoadDaeModel (const char* file_name, Model& model, const Model::LogHandler& log)
    {
      DaeParser (file_name, model, log);
    }
};

}

extern "C"
{

common::ComponentRegistrator<DaeLoaderComponent> DaeLoader ("media.collada.loaders.DAE");

}
