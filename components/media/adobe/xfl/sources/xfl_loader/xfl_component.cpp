#include "shared.h"

using namespace media::adobe::xfl;

namespace
{

const char* DOCUMENT_XML_NAME = "DOMDocument.xml";

//загрузчик Xfl-файлов
class XflLoaderComponent
{
  public:
    XflLoaderComponent ()
    {
      DocumentManager::RegisterLoader ("xfl", &LoadXflDocument);
    }
    
  private:
    static void LoadXflDocument (const char* xfl_path, Document& document)
    {
      if (!xfl_path)
        throw xtl::make_null_argument_exception ("media::adobe::xfl::XflLoaderComponent::LoadXflDocument", "xfl_path");

      stl::string xml_path = common::format ("%s/%s", xfl_path, DOCUMENT_XML_NAME);

      XflParser (xml_path.c_str (), xfl_path, document);
    }
};

}

extern "C"
{

common::ComponentRegistrator<XflLoaderComponent> XflLoader ("media.adobe.xfl.loaders.xfl");

}
