#include "shared.h"

using namespace common;

namespace
{

size_t get_first_word_length (const char* string)
{
  const char* end = string;

  for (;; end++)
  {
    switch (*end)
    {
      case '\0':
      case '.':
        return end - string;
      default:
        break;
    }
  }
}

//Реализация подуровня
struct Var : public VarRegistry::ISubLevel
{
  size_t      ref_count;
  stl::string name;

  Var (const char* in_name, size_t length) : ref_count (1), name (in_name, length) {}

  const char* Name ()
  {
    return name.c_str ();
  }

  size_t VarsCount ()
  {
    return ref_count;
  }
};

typedef stl::hash_map<size_t, Var> VarMap;

struct Selector
{
  Var& operator () (VarMap::value_type& value) const { return value.second; }
};

}

struct BranchLevelVarSet::Impl
{
  xtl::auto_connection create_var_connection;
  xtl::auto_connection delete_var_connection;
  VarMap               vars;

//Конструктор
  Impl (const VarRegistry& registry)
  {
    create_var_connection = registry.RegisterEventHandler ("*", VarRegistryEvent_OnCreateVar, xtl::bind (&BranchLevelVarSet::Impl::OnCreateVar, this, _1));
    delete_var_connection = registry.RegisterEventHandler ("*", VarRegistryEvent_OnDeleteVar, xtl::bind (&BranchLevelVarSet::Impl::OnDeleteVar, this, _1));

    registry.EnumerateVars (xtl::bind (&Impl::OnCreateVar, this, _1));
  }

//Добавление переменной
  void OnCreateVar (const char* var_name)
  {
    size_t first_word_length = get_first_word_length (var_name),
           first_word_hash   = common::crc32 (var_name, first_word_length);

    VarMap::iterator iter = vars.find (first_word_hash);

    if (iter != vars.end ())
    {
      iter->second.ref_count++;

      return;
    }

    vars.insert_pair (first_word_hash, Var (var_name, first_word_length));
  }

//Удаление переменной
  void OnDeleteVar (const char* var_name)
  {
    size_t first_word_length = get_first_word_length (var_name),
           first_word_hash   = common::crc32 (var_name, first_word_length);

    VarMap::iterator iter = vars.find (first_word_hash);

    if (iter != vars.end () && !--iter->second.ref_count)
      vars.erase (first_word_hash);
  }
};

/*
   Набор переменных одного уровня вложения
*/

/*
   Конструктор/деструктор
*/

BranchLevelVarSet::BranchLevelVarSet (const VarRegistry& registry)
  : impl (new Impl (registry))
{
}

BranchLevelVarSet::~BranchLevelVarSet ()
{
}

/*
   Создание итератора
*/

BranchLevelVarSet::Iterator BranchLevelVarSet::CreateIterator ()
{
  return Iterator (impl->vars.begin (), impl->vars.begin (), impl->vars.end (), Selector ());
}
