#include "shared.h"

using namespace debug;

/*
    Описание реализации карыт символов
*/

typedef stl::map<size_t, Symbol> SymbolMap;

struct MapFile::Impl: public xtl::reference_counter
{
  stl::string name;    //имя файла
  stl::string type;    //тип файла
  SymbolMap   symbols; //символы
  
  Impl (const char* in_name, const char* in_type)
    : name (in_name)
    , type (in_type)
  {
  }
};
  
/*
    Конструкторы / присваивание / деструктор
*/

MapFile::MapFile (const char* file_name, const char* type)
{
  static const char* METHOD_NAME = "debug::MapFile::MapFile(const char*,const char*)";
  
  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  if (!type)
    throw xtl::make_null_argument_exception (METHOD_NAME, "type");
    
  impl = new Impl (file_name, type);
  
  try
  {
    static common::ComponentLoader loader ("debug.map_file.loaders.*");

    MapFileManager::GetLoader (type, common::SerializerFindMode_ByExtension)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    release (impl);
    exception.touch ("media::MeshLibrary::Load");
    throw;
  }  
  catch (...)
  {
    release (impl);
  }
}

MapFile::MapFile (const MapFile& file)
  : impl (file.impl)
{
  addref (impl);
}

MapFile::~MapFile ()
{
  release (impl);
}

MapFile& MapFile::operator = (const MapFile& file)
{
  MapFile (file).Swap (*this);
  return *this;
}

/*
    Тип
*/

const char* MapFile::Type () const
{
  return impl->type.c_str ();
}

/*
    Имя
*/

const char* MapFile::Name () const
{
  return impl->name.c_str ();
}

void MapFile::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("debug::MapFile::SetName", "name");
    
  impl->name = name;
}

/*
    Добавление / удаление символов
*/

void MapFile::AddSymbol (const Symbol& symbol)
{
  SymbolMap::iterator iter = impl->symbols.find (symbol.StartAddress ());
  
  if (iter != impl->symbols.end ())
    throw xtl::format_operation_exception ("debug::MapFile::AddSymbol", "Can't add symbol '%s' with start address 0x%x. Symbol has been already added", symbol.Name (), symbol.StartAddress ());
    
  impl->symbols.insert_pair (symbol.StartAddress (), symbol);
}

void MapFile::RemoveSymbol (const Symbol& symbol)
{
  impl->symbols.erase (symbol.StartAddress ());
}

void MapFile::RemoveAllSymbols ()
{
  impl->symbols.clear ();
}

/*
    Перебор символов
*/

namespace
{

struct SymbolSelector
{
  template <class T1> Symbol& operator () (T1& value) const { return value.second; }
};

}

MapFile::Iterator MapFile::CreateIterator ()
{
  return Iterator (impl->symbols.begin (), impl->symbols.begin (), impl->symbols.end (), SymbolSelector ());
}

MapFile::ConstIterator MapFile::CreateIterator () const
{
  return ConstIterator (impl->symbols.begin (), impl->symbols.begin (), impl->symbols.end (), SymbolSelector ());
}

/*
    Поиск символа по адресу
*/

Symbol* MapFile::FindSymbol (size_t address)
{
  SymbolMap::iterator iter = impl->symbols.lower_bound (address);
  
  if (iter == impl->symbols.end () || iter == impl->symbols.begin ())
    return 0;    
    
  --iter;
    
  Symbol& symbol = iter->second;
  
  if (address < symbol.StartAddress () || address >= symbol.StartAddress () + symbol.Size ())
    return 0;

  return &symbol;
}

const Symbol* MapFile::FindSymbol (size_t address) const
{
  return const_cast<MapFile&> (*this).FindSymbol (address);
}

/*
    Обмен
*/

void MapFile::Swap (MapFile& file)
{
  stl::swap (impl, file.impl);
}

namespace debug
{

void swap (MapFile& file1, MapFile& file2)
{
  file1.Swap (file2);
}

}
