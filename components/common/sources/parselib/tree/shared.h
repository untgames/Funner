#ifndef COMMONLIB_PARSE_TREE_SHARED_HEADER
#define COMMONLIB_PARSE_TREE_SHARED_HEADER

#include <cstring>

#include <stl/auto_ptr.h>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/uninitialized_storage.h>

#include <common/hash.h>
#include <common/parser.h>
#include <common/singleton.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер дерева грамматического разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::uninitialized_storage<char> ParseTreeBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги узла дерева грамматического разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ParseNodeFlags
{
  ParseNodeFlag_AttributesAsPointers = 1, //атрибуты скомпилированы в указатели
};

#ifdef _MSC_VER
  #pragma pack (push)
#endif

#pragma pack (1)

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация узла дерева грамматического разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseNodeImpl
{
  size_t attributes_count;   //количество атрибутов связанных с узлом
  size_t name_hash;          //хэш от имени узла
  size_t attributes_offset;  //смещение до таблицы атрибутов
  size_t flags;              //флаги
  size_t next_offset;        //смещение к следующему узлу на данном уровне иерархии
  size_t first_offset;       //смещение до первого ребёнка
  size_t source_back_offset; //отрицательное смещение до строки с определением имени исходного текста
  size_t line_number;        //номер строки
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дерево разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParseTree: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseTree  (ParseTreeBuffer& buffer, const ParseLog* log = 0);
    ~ParseTree ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void*       Buffer     ()       { return buffer.data (); }    
    const void* Buffer     () const { return buffer.data (); }
    size_t      BufferSize () const { return buffer.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протокол разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseLog& Log ();

  private:
    ParseTree (const ParseTree&); //no impl
    ParseTree& operator = (const ParseTree&); //no impl

  private:
    ParseTreeBuffer         buffer; //буфер дерева разбора
    stl::auto_ptr<ParseLog> log;    //протокол разбора    
};

}

#endif

