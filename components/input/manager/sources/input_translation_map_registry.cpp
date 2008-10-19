#include "shared.h"

using namespace input;
using namespace common;

namespace
{

struct RegistryNode
{
  public:
///Регистрация / удаление трансляторов
    void Register (const StringArray& tokens, size_t current_node, size_t last_node, const char* in_translation_map, const char* in_profile)
    {
      if (current_node == last_node)
      {
        translation_map = in_translation_map;
        profile         = in_profile;

        return;
      }

      NodeMap::iterator iter = children.find (tokens [current_node]);

      if (iter == children.end ())
          iter = children.insert_pair (tokens [current_node], RegistryNodePtr (new RegistryNode ())).first;

      iter->second->Register (tokens, current_node + 1, last_node, in_translation_map, in_profile);
    }
   
    void Unregister (const StringArray& tokens, size_t current_node, size_t last_node)
    {
      if (current_node == last_node)
        return;

      NodeMap::iterator iter = children.find (tokens [current_node]);

      if (iter == children.end ())
        return;

      if ((current_node + 1) == last_node)
      {
        if (iter->second->children.empty ())
          children.erase (iter);
        else
          iter->second->translation_map.clear ();
      }
      else
        iter->second->Unregister (tokens, current_node + 1, last_node);
    }

///Поиск трансляторов
    const char* Find (const StringArray& tokens, size_t current_node, size_t last_node)
    {
      if (current_node == last_node)
        return GetTranslationMap ();

      NodeMap::iterator iter = children.find (tokens [current_node]);

      if (iter == children.end ())
        return 0;

      return iter->second->Find (tokens, current_node + 1, last_node);
    }

    const char* FindNearest (const StringArray& tokens, size_t current_node, size_t last_node)
    {
      if (current_node == last_node)
        return GetTranslationMap ();

      NodeMap::iterator iter = children.find (tokens [current_node]);

      if (iter == children.end ())
        return GetTranslationMap ();

      const char* find_result = iter->second->FindNearest (tokens, current_node + 1, last_node);

      if (find_result)
        return find_result;
      else
        return GetTranslationMap ();
    }

///Очистка
    void Clear ()
    {
      children.clear ();
    }

///Перечисление
    void Enumerate (TranslationMapRegistry::IEnumerator& enumerator)
    {
      if (!translation_map.empty ())
        enumerator.Process (profile.c_str (), translation_map.c_str ());

      for (NodeMap::iterator iter = children.begin (), end = children.end (); iter != end; ++iter)
        iter->second->Enumerate (enumerator);
    }

  private:
    const char* GetTranslationMap ()
    {
      if (translation_map.empty ())
        return 0;
      else
        return translation_map.c_str ();
    }

  private:
    typedef xtl::shared_ptr<RegistryNode>                              RegistryNodePtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RegistryNodePtr> NodeMap;

  private:
    NodeMap     children;
    stl::string translation_map;
    stl::string profile;
};

}

/*
   Реестр таблиц трансляции
*/
struct TranslationMapRegistry::Impl : public xtl::reference_counter
{
  public:
///Регистрация / удаление трансляторов
    void Register (const char* profile, const char* translation_map)
    {
      static const char* METHOD_NAME = "input::TranslationMapRegistry::Add";

      if (!translation_map)
        throw xtl::make_null_argument_exception (METHOD_NAME, "translation_map");

      if (*translation_map == '\0')
        throw xtl::make_argument_exception (METHOD_NAME, "translation_map", translation_map, "Empty translation map name not allowed");

      StringArray profile_nodes;

      try
      {
        ParseProfile (profile, profile_nodes);

        root.Register (profile_nodes, 0, profile_nodes.Size (), translation_map, profile);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
        throw;
      }
    }

    void Unregister (const char* profile)
    {
      StringArray profile_nodes;

      try
      {
        ParseProfile (profile, profile_nodes);
      }
      catch (...)
      {
        return;
      }

      root.Unregister (profile_nodes, 0, profile_nodes.Size ());
    }

///Поиск трансляторов
    const char* Find (const char* profile)
    {
      StringArray profile_nodes;

      try
      {
        ParseProfile (profile, profile_nodes);

        return root.Find (profile_nodes, 0, profile_nodes.Size ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("input::TranslationMapRegistry::Find");
        throw;
      }
    }

    const char* FindNearest (const char* profile)
    {
      StringArray profile_nodes;

      try
      {
        ParseProfile (profile, profile_nodes);

        return root.FindNearest (profile_nodes, 0, profile_nodes.Size ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("input::TranslationMapRegistry::FindNearest");
        throw;
      }
    }

///Очистка
    void Clear ()
    {
      root.Clear ();
    }

///Перечисление записей
    void Enumerate (TranslationMapRegistry::IEnumerator& enumerator)
    {
      root.Enumerate (enumerator);
    }

  private:
    void ParseProfile (const char* profile, StringArray& profile_nodes) const
    {
      if (!profile)
        throw xtl::make_null_argument_exception ("", "profile");

      if (*profile == '\0')
        throw xtl::make_argument_exception ("", "profile", profile, "Empty profile");

      profile_nodes = common::split (profile, ".", "", "");

      for (size_t i=0, count=profile_nodes.Size (); i<count; i++)
        if (!*profile_nodes [i])
          throw xtl::make_argument_exception ("", "profile", profile, "Profile contains empty node");
    }

  private:
    RegistryNode root;
    size_t       size;
};

/*
   Реестр таблиц трансляции
*/

/*
   Конструкторы / деструктор / присваивание
*/

TranslationMapRegistry::TranslationMapRegistry ()
  : impl (new Impl)
  {}

TranslationMapRegistry::TranslationMapRegistry (const char* file_name)
  : impl (new Impl)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("input::TranslationMapRegistry::TranslationMapRegistry", "file_name");

  static common::ComponentLoader loader ("input.loaders.*");

  TranslationMapRegistryManager::GetLoader (file_name, common::SerializerFindMode_ByName) (file_name, *this);
}

TranslationMapRegistry::TranslationMapRegistry (const TranslationMapRegistry& source)
  : impl (source.impl)
{
  addref (impl);
}

TranslationMapRegistry::~TranslationMapRegistry ()
{
  release (impl);
}

TranslationMapRegistry& TranslationMapRegistry::operator = (const TranslationMapRegistry& source)
{
  TranslationMapRegistry (source).Swap (*this);

  return *this;
}

/*
   Регистрация / удаление трансляторов
*/

void TranslationMapRegistry::Register (const char* profile, const char* translation_map)
{
  impl->Register (profile, translation_map);
}

void TranslationMapRegistry::Unregister (const char* profile)
{
  impl->Unregister (profile);
}

/*
   Поиск трансляторов
*/

const char* TranslationMapRegistry::Find (const char* profile) const
{
  return impl->Find (profile);
}

const char* TranslationMapRegistry::FindNearest (const char* profile) const
{
  return impl->FindNearest (profile);
}

/*
   Очистка
*/

void TranslationMapRegistry::Clear ()
{
  impl->Clear ();
}

/*
   Перечисление записей
*/

void TranslationMapRegistry::Enumerate (IEnumerator& enumerator) const
{
  impl->Enumerate (enumerator);
}

/*
   Загрузка / сохранение
*/

void TranslationMapRegistry::Load (const char* file_name)
{
  TranslationMapRegistry (file_name).Swap (*this);
}

void TranslationMapRegistry::Save (const char* file_name) const
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("input::TranslationMapRegistry::Save", "file_name");

  static common::ComponentLoader loader ("input.savers.*");

  TranslationMapRegistryManager::GetSaver (file_name, common::SerializerFindMode_ByName) (file_name, *this);
}
  
/*
   Обмен
*/

void TranslationMapRegistry::Swap (TranslationMapRegistry& registry)
{
  stl::swap (impl, registry.impl);
}

namespace input
{

/*
   Обмен
*/

void swap (TranslationMapRegistry& registry1, TranslationMapRegistry& registry2)
{
  registry1.Swap (registry2);
}

}
