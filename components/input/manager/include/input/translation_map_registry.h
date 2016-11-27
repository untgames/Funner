#ifndef INPUT_SYSTEM_TRANSLATION_MAP_REGISTRY_HEADER
#define INPUT_SYSTEM_TRANSLATION_MAP_REGISTRY_HEADER

#include <common/serializer_manager.h>

namespace input
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реестр таблиц трансляции
///////////////////////////////////////////////////////////////////////////////////////////////////
class TranslationMapRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечислитель полей реестра
///////////////////////////////////////////////////////////////////////////////////////////////////
    class IEnumerator
    {
      public:
        virtual void Process (const char* profile, const char* translation_map) = 0;

      protected:
        virtual ~IEnumerator () {}
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание (подсчёт ссылок)
///////////////////////////////////////////////////////////////////////////////////////////////////
    TranslationMapRegistry  ();
    TranslationMapRegistry  (const char* file_name);
    TranslationMapRegistry  (const TranslationMapRegistry&);    
    ~TranslationMapRegistry ();

    TranslationMapRegistry& operator = (const TranslationMapRegistry&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация / удаление трансляторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Register   (const char* profile, const char* translation_map);
    void Unregister (const char* profile);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск трансляторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Find        (const char* profile) const;
    const char* FindNearest (const char* profile) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление записей
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Enumerate (IEnumerator& enumerator) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name) const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (TranslationMapRegistry& registry);
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (TranslationMapRegistry& registry1, TranslationMapRegistry& registry2);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таблиц трансляции
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char* file_name, TranslationMapRegistry& map), 
                                          void (const char* file_name, const TranslationMapRegistry& map)> TranslationMapRegistryManager;

}

#endif
