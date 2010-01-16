#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Константы
*/

const char* DEFAULT_SHADER_LOG = "media.rfx.shader"; //протокол по умолчанию для загрузчика шейдеров

/*
    Описание шейдера
*/

struct ShaderDesc
{
  Shader      shader;
  stl::string name;
  
  ShaderDesc (const char* in_name, const Shader& in_shader)
    : shader (in_shader)
    , name (in_name)
  {
  }
};

}

/*
    Описание реализации библиотеки материалов
*/

typedef stl::hash_multimap<stl::hash_key<const char*>, ShaderDesc> ShaderMap;

struct ShaderLibrary::Impl
{
  ShaderMap shaders; //библиотека материалов
  
  Impl ()
  {
  }
  
  Impl (const Impl& impl)
  {
    for (ShaderMap::const_iterator iter=impl.shaders.begin (), end=impl.shaders.end (); iter!=end; ++iter)
      shaders.insert_pair (iter->first, ShaderDesc (iter->second.name.c_str (), iter->second.shader.Clone ()));
  }
};

/*
    Конструкторы / деструктор
*/

ShaderLibrary::ShaderLibrary ()
  : impl (new Impl)
{
}

ShaderLibrary::ShaderLibrary (const ShaderLibrary& library)
  : impl (new Impl (*library.impl))
{
}

ShaderLibrary::~ShaderLibrary ()
{
}

/*
    Присваивание
*/

ShaderLibrary& ShaderLibrary::operator = (const ShaderLibrary& library)
{
  ShaderLibrary (library).Swap (*this);
  return *this;
}

/*
    Количество шейдеров / проверка на пустоту
*/

size_t ShaderLibrary::Size () const
{
  return impl->shaders.size ();
}

bool ShaderLibrary::IsEmpty () const
{
  return impl->shaders.empty ();
}

/*
    Получение итератора
*/

namespace
{

//селектор материала
template <class T>
struct Shader_selector
{
  template <class T1> T& operator () (T1& value) const { return value.second.shader; }
};

}

ShaderLibrary::Iterator ShaderLibrary::CreateIterator ()
{
  return Iterator (impl->shaders.begin (), impl->shaders.begin (), impl->shaders.end (), Shader_selector<Shader> ());
}

ShaderLibrary::ConstIterator ShaderLibrary::CreateIterator () const
{
  return ConstIterator (impl->shaders.begin (), impl->shaders.begin (), impl->shaders.end (), Shader_selector<Shader> ());
}

/*
    Получение идентификатора материала
*/

const char* ShaderLibrary::ItemId (const ConstIterator& i) const
{
  const ShaderMap::iterator* iter = i.target<ShaderMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::rfx::ShaderLibrary::ItemId", "iterator", "wrong-type");

  return (*iter)->second.name.c_str ();
}

/*
    Поиск
*/

Shader* ShaderLibrary::Find (const char* name, const char* profile)
{
  return const_cast<Shader*> (const_cast<const ShaderLibrary&> (*this).Find (name, profile));
}

const Shader* ShaderLibrary::Find (const char* name, const char* profile) const
{
  if (!name || !profile)
    return 0;
    
  stl::pair<ShaderMap::const_iterator, ShaderMap::const_iterator> p = impl->shaders.equal_range (name);
  
  if (p.first == p.second)
    return 0;
    
  for (ShaderMap::const_iterator iter=p.first; iter!=p.second; ++iter)
    if (!strcmp (iter->second.shader.Profile (), profile))
      return &iter->second.shader;

  return 0;
}

/*
    Присоединение материалов
*/

void ShaderLibrary::Attach (const char* id, const Shader& shader)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::rfx::ShaderLibrary::Insert", "id");
    
  stl::pair<ShaderMap::iterator, ShaderMap::iterator> p = impl->shaders.equal_range (id);
  
  if (p.first != p.second)
  {
    const char* profile = shader.Profile ();
    
    for (ShaderMap::iterator iter=p.first; iter!=p.second; ++iter)
      if (!strcmp (iter->second.shader.Profile (), profile))
      {
        iter->second.shader = shader;
        return;
      }
  }

  impl->shaders.insert_pair (id, ShaderDesc (id, shader));
}

void ShaderLibrary::Detach (const char* id)
{
  if (!id)
    return;
    
  impl->shaders.erase (id);
}

void ShaderLibrary::Detach (const char* id, const char* profile)
{
  if (!id || !profile)
    return;
    
  stl::pair<ShaderMap::iterator, ShaderMap::iterator> p = impl->shaders.equal_range (id);
  
  if (p.first == p.second)
    return;

  for (ShaderMap::iterator iter=p.first; iter!=p.second; ++iter)
    if (!strcmp (iter->second.shader.Profile (), profile))
    {
      impl->shaders.erase (iter);
      return;
    }
}

void ShaderLibrary::DetachAll ()
{
  impl->shaders.clear ();
}

/*
    Очистка
*/

void ShaderLibrary::Clear ()
{
  DetachAll ();
}

/*
    Загрузка
*/

void ShaderLibrary::Load (const char* file_mask, const LogHandler& log_handler)
{
  try
  {
    if (!file_mask)
      throw xtl::make_null_argument_exception ("", "file_mask");
    
    static ComponentLoader loader ("media.rfx.shader.loaders.*");
    
    common::FileListIterator iter = common::FileSystem::Search (file_mask, FileSearch_Files | FileSearch_Sort | FileSearch_SubDirs);
    
    for (;iter; ++iter)
    {
      const FileListItem& file_item = *iter;
      
      try
      {
        Shader shader;
        
        ShaderManager::GetLoader (file_item.name, common::SerializerFindMode_ByName)(file_item.name, shader);
        
        shader.SetName (file_item.name);
   
        Attach (shader.Name (), shader);
      }
      catch (std::exception& e)
      {
        try
        {
          log_handler (common::format ("%s\n    at load shader '%s'", e.what (), file_item.name).c_str ());
        }
        catch (...)
        {
        }
      }
      catch (...)
      {
        try
        {
          log_handler (common::format ("Unknown exception\n    at load shader '%s'", file_item.name).c_str ());
        }
        catch (...)
        {
        }
      }
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::ShaderLibrary::Load");
    throw;
  }
}

namespace
{

struct DefaultLoadLogger
{
  DefaultLoadLogger () : log (DEFAULT_SHADER_LOG) {}
  
  void operator () (const char* message)
  {
    log.Print (message);
  }

  common::Log log;
};

}

void ShaderLibrary::Load (const char* file_mask)
{
  Load (file_mask, DefaultLoadLogger ());
}

/*
    Обмен
*/

void ShaderLibrary::Swap (ShaderLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

namespace rfx
{

void swap (ShaderLibrary& library1, ShaderLibrary& library2)
{
  library1.Swap (library2);
}

}

}
