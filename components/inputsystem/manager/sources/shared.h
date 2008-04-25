#include <stl/vector>
#include <stl/hash_map>
#include <stl/string>

#include <common/singleton.h>

#include <input/translation_map.h>

struct Token;
typedef stl::vector<Token> TokenArray;

class EventReplacer
{
  public:
/*
   Конструктор/деструктор
*/
    EventReplacer (const char* input_event, const char* event_replacement, size_t id);
    ~EventReplacer ();

/*
   Выполнение замены
*/
    bool Replace (const stl::vector<stl::string>& event_components, stl::string& result);

/*
   Получение идентификатора
*/
    size_t Id () {return id;}

  private:
    stl::vector<stl::string> event_wildcard;
    TokenArray               replacement_tokens;
    size_t                   id;
};

/*
   Менеджер таблиц трансляции
*/

class TranslationMapManagerImpl
{
  public:
/*
   Работа с пользовательскими функциями загрузки и сохранения
*/
    void RegisterLoader       (const char* extension, const input::TranslationMapManager::LoadHandler& handler);
    void RegisterSaver        (const char* extension, const input::TranslationMapManager::SaveHandler& handler);
    void UnregisterLoader     (const char* extension);
    void UnregisterSaver      (const char* extension);
    void UnregisterAllLoaders ();
    void UnregisterAllSavers  ();
    void UnregisterAll        ();

    void Load (const char* file_name,       input::TranslationMap&) const;
    void Save (const char* file_name, const input::TranslationMap&) const;

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, input::TranslationMapManager::LoadHandler> LoadHandlersMap;
    typedef stl::hash_map<stl::hash_key<const char*>, input::TranslationMapManager::SaveHandler> SaveHandlersMap;

  private:
    LoadHandlersMap load_handlers;
    SaveHandlersMap save_handlers;
};

typedef common::Singleton<TranslationMapManagerImpl> TranslationMapManagerSingleton;


