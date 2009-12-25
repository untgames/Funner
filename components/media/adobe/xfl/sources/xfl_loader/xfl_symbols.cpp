#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t DEFAULT_SYMBOLS_COUNT = 512;

}

/*
    Разбор библиотеки элементов анимации
*/

void XflParser::ParseSymbols (common::Parser::Iterator iter)
{
  document.Symbols ().Reserve (DEFAULT_SYMBOLS_COUNT);

  for (Parser::NamesakeIterator i = iter->First ("DOMSymbolItem"); i; ++i)
  {
    Symbol new_symbol;

    ParseSymbol (i, new_symbol);

    document.Symbols ().Add (new_symbol);
  }
}

/*
    Разбор элемента анимации
*/

void XflParser::ParseSymbol (Parser::Iterator iter, Symbol& symbol)
{
  symbol.SetName (get<const char*> (*iter, "name"));

  Timeline symbol_timeline;

  ParseTimeline (iter->First ("timeline.DOMTimeline"), symbol_timeline);

  symbol.SetTimeline (symbol_timeline);
}
