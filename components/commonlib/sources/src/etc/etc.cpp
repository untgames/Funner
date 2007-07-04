#include <common/etc.h>
#include <stl/hash_map>
#include <common/strlib.h>
#include "validator.h"

using namespace common;
using namespace stl;

static const char* NULL_NAMED_VAR = "@null@";

typedef hash_map<size_t,Var*> VarMap;

struct Etc::Impl {
  Etc&                etc;
  VarMap              map;
  VarValidatorFactory validator_factory;  
  
  Impl (Etc&);
  
  Var* find           (const char* name,size_t init_hash=0xFFFFFFFF);
  Var* find_or_create (const char* name,const char* prefix=NULL,size_t init_hash=0xFFFFFFFF);
  void remove         (const char* name,size_t init_hash=0xFFFFFFFF);
};

/*
    Конструкторы
*/

inline Etc::Impl::Impl (Etc& _etc)
  : etc (_etc)
  { }

Etc::Etc ()
{    
  impl = new Impl (*this);
}

Etc::~Etc ()
{
  clear ();
  
  delete impl;
}

VarValidatorFactory& Etc::get_validator_factory ()
{
  return impl->validator_factory;
}

/*
    Поиск и создание переменной
*/

inline Var* Etc::create_var (Etc& etc,const char* prefix,const char* name)
{
  return Var::create (etc,prefix,name);
}

inline void Etc::destroy_var (Var* var)
{
  var->release ();
}

inline Var* Etc::Impl::find (const char* name,size_t init_hash)
{
  if (!name)
    return NULL;

  VarMap::iterator i = map.find (strihash (name,init_hash));
  
  return i != map.end () ? i->second : NULL;
}

inline bool check_name (const char* name)
{
  while (*name && (isalnum (*name) || *name == '.')) name++;
 
  return *name == '\0';
}

Var* Etc::Impl::find_or_create (const char* name,const char* prefix,size_t init_hash)
{
  if (!name)
    name = NULL_NAMED_VAR;

  size_t           hash = strihash (name,init_hash);
  VarMap::iterator i    = map.find (hash);

  if (i != map.end ())
    return i->second;

  if (name != NULL_NAMED_VAR && !check_name (name))
  {
    name = NULL_NAMED_VAR;
    hash = strihash (name,init_hash);      
  }  
    
  Var* var = create_var (etc,prefix,name);
  
  try
  {
    map [hash] = var;
  }
  catch (...)
  {
    destroy_var (var);
    throw;
  }
  
  return var;
}

Var* Etc::find (const char* name)
{
  return impl->find (name);
}

Var& Etc::var (const char* name)
{
  return *impl->find_or_create (name);
}

bool Etc::present (const char* name)
{
  return name && impl->map.find (strihash (name)) != impl->map.end ();
}

/*
    Удаление и очистка
*/

inline void Etc::Impl::remove (const char* name,size_t init_hash)
{    
  VarMap::iterator i = map.find (strihash (name,init_hash));
  
  if (i == map.end ())
    return;
    
  destroy_var (i->second);
    
  map.erase (i);
}

inline bool check_wildcard (const char* name)
{
  for (;;)
    switch (*name++)
    {
      case '*':
      case '?':  return true;
      case '\0': return false;
    }
}

void Etc::remove (const char* name)
{
  if (!name)
    return;

  if (check_wildcard (name))
  {
    for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();)
      if (wcmatch (name,i->second->name ()))
      {
        VarMap::iterator next = i; 
        
        ++next;
        
        destroy_var (i->second);
        impl->map.erase (i);
        
        i = next;
      }
      else i++;
  }
  else impl->remove (name);  
}

void Etc::clear ()
{  
  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    destroy_var (i->second);

  impl->map.clear ();
}

/*
    Перебор переменных
*/

void Etc::traverse (const Var::EventHandler& fn)
{
  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    fn (*i->second);
}

void Etc::traverse (const char* wc_mask,const Var::EventHandler& fn)
{
  if (!wc_mask)
    return ;

  for (VarMap::iterator i=impl->map.begin ();i!=impl->map.end ();i++)
    if (wcmatch (wc_mask,i->second->name ()))
      fn (*i->second);
}

/*
    EtcBranch
*/

EtcBranch::EtcBranch (Etc& _etc,const char* name)
  : etc (_etc), prefix (name && check_name (name) ? name : NULL_NAMED_VAR)
{
  hash = strihash (".",strihash (prefix.c_str ()));
}

Var* EtcBranch::find (const char* name)
{
  return etc.impl->find (name,hash);
}

bool EtcBranch::present (const char* name)
{
  return name && etc.impl->map.find (strihash (name,hash)) != etc.impl->map.end ();
}

Var& EtcBranch::var (const char* name)
{
  return *etc.impl->find_or_create (name,prefix.c_str (),hash);
}

void EtcBranch::remove (const char* name)
{
  if (!name)
    return;

  etc.impl->remove (name,hash);
}

void EtcBranch::clear ()
{
  etc.remove (format ("%s.*",prefix.c_str ()).c_str ());
}

EtcBranch Etc::branch (const char* prefix)
{
  return EtcBranch (*this,prefix);
}
