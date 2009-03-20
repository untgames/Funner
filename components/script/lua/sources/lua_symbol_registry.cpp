#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;


namespace
{

/*
    Константы
*/

const char* INTERNAL_SYMBOL_PREFIX = "__internal_"; //префикс внутренних имём символов

}

/*
    Символ - дескриптор внутреннего объекта луа (функции, мета-таблицы и т.п.)
*/

class SymbolRegistry::Symbol: public ISymbol, public xtl::reference_counter
{
  public:
///конструктор
    Symbol (SymbolRegistry* in_registry, void* in_symbol_handle, int index) 
      : registry (in_registry)
      , symbol_handle (in_symbol_handle)
      , internal_name (common::format ("%s%p", INTERNAL_SYMBOL_PREFIX, symbol_handle))
    {
      try
      {
        lua_State* state = registry->State ();
        
        check_stack (state, 2);

        lua_pushvalue (state, index);
        lua_setglobal (state, internal_name.c_str ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("script::lua::SymbolRegistry::Symbol::Symbol");
        throw;
      }
    }

///деструктор
    ~Symbol ()
    {            
      if (!registry)
        return;              
        
      try
      {
        lua_State* state = registry->State ();
        
        check_stack (state, 2);                        
        
        lua_pushnil   (state);
        lua_setglobal (state, internal_name.c_str ());
      }
      catch (...)
      {
        //подавление всех исключений
      }
      
      registry->RemoveSymbol (symbol_handle);
    }

///получение имени символа
    const char* Name () { return internal_name.c_str (); }
    
///подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }        
    
///оповещение об удалении скриптового состояния
    void ResetState ()
    {
      registry = 0;
    }

  private:
    SymbolRegistry* registry;
    void*           symbol_handle;
    stl::string     internal_name;
};      

/*
    Конструктор / деструктор
*/

SymbolRegistry::SymbolRegistry (lua_State* in_state)
  : state (in_state)
{
}

SymbolRegistry::~SymbolRegistry ()
{
    //оповещение всех символов об удалении скриптового состояния
    
  for (SymbolMap::iterator iter=symbols.begin (), end=symbols.end (); iter!=end; ++iter)
    iter->second->ResetState ();
}

/*
    Получение и удаление символа
*/

ISymbol* SymbolRegistry::GetSymbol (int index)
{
  try
  {
      //получение идентификатора символа

    void* symbol_handle = (void*)lua_topointer (state, index);

    if (!symbol_handle)
      throw xtl::format_exception<ArgumentException> ("", "Null symbol at index %u in stack", index);

      //поиск символа среди зарегистрированных

    SymbolMap::iterator iter = symbols.find (symbol_handle);

    if (iter != symbols.end ())
    {
        //символ уже зарегистрирован

      iter->second->AddRef ();

      return iter->second;
    }
    
      //регистрация нового символа

    xtl::com_ptr<Symbol> symbol (new Symbol (this, symbol_handle, index), false);

    symbols.insert_pair (symbol_handle, symbol.get ());

    symbol->AddRef ();

    return symbol.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::lua::SymbolRegistry::GetSymbol");
    throw;
  }
}

void SymbolRegistry::RemoveSymbol (void* symbol_handle)
{
  symbols.erase (symbol_handle);
}
