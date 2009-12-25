#ifndef MEDIALIB_ADOBE_XFL_LOADER_SHARED_HEADER
#define MEDIALIB_ADOBE_XFL_LOADER_SHARED_HEADER

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>
#include <xtl/token_parser.h>

#include <math/vector.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <media/adobe/xfl.h>

namespace media
{

namespace adobe
{

namespace xfl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузчик xfl файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
class XflParser
{
  public:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    XflParser  (const char* file_name, const char* path_prefix, Document& document);
    ~XflParser ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Печать лога парсинга
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PrintLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение цвета из строки вида #ffffff
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReadHexColor (const char* hex_string, math::vec3f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение флоата из узла вида "0, value"
///////////////////////////////////////////////////////////////////////////////////////////////////
    float ReadFloatFromVec2 (common::Parser::Iterator, const char* property_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка корректности прочитанного документа
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckDocument     ();
    bool CheckTimeline     (Timeline&);
    bool CheckFrameElement (const FrameElement&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разбор отдельных элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ParseRoot          (common::Parser::Iterator);
    void ParseLibraries     (common::Parser::Iterator);
    void ParseResources     (common::Parser::Iterator);
    void ParseSymbols       (common::Parser::Iterator);
    void ParseTimelines     (common::Parser::Iterator);
    void ParseBitmap        (common::Parser::Iterator, Resource&);
    void ParseSymbol        (common::Parser::Iterator, Symbol&);
    void ParseTimeline      (common::Parser::Iterator, Timeline&);
    void ParseLayer         (common::Parser::Iterator, Layer&);
    void ParseFrame         (common::Parser::Iterator, Frame&);
    void ParseFrameElement  (common::Parser::Iterator, Frame::FrameElementList&, FrameElementType);
    void ParseAnimationCore (common::Parser::Iterator, AnimationCore&);

    void ParsePropertyAnimationContainer (common::Parser::Iterator, AnimationCore&, const char* property_prefix);
    void ParsePropertyAnimation          (common::Parser::Iterator, PropertyAnimation&, const char* property_prefix);
    void ParsePropertyAnimationKeyframe  (common::Parser::Iterator, PropertyAnimationKeyframe&);

  private:
    Document&      document;    //загружаемый документ
    common::Parser parser;      //парсер
    common::Log    log;         //протокол
    stl::string    path_prefix; //путь к xfl архиву
};

}

}

}

#endif
