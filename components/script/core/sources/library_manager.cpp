#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENTS_MASK = "script.binds.*"; //маска имён компонентов скриптовых библиотек

/*
    Реализация менеджера библиотек
*/

class LibraryManagerImpl
{
  public:
    typedef LibraryManager::BindHandler BindHandler;
  
///Регистрация библиотеки
    void RegisterLibrary (const char* name, const BindHandler& binder)
    {
      static const char* METHOD_NAME = "script::LibraryManager::RegisterLibrary";

        //проверка корректности аргументов

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
        
      for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
        if (iter->name == name)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Library has already registered");

        //добавление библиотеки

      libraries.push_back (Library (name, binder));  
    }

///Отмена регистрации библиотеки
    void UnregisterLibrary (const char* name)
    {
      if (!name)
        return;

      for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
        if (iter->name == name)
        {
          libraries.erase (iter);
          return;
        }
    }

///Отмена регистрации всех библиотек
    void UnregisterAllLibraries ()
    {
      libraries.clear ();
    }
    
///Биндинг библиотек
    void BindLibraries (Environment& environment, const char* library_mask)
    {
      try
      {
        if (!library_mask)
          throw xtl::make_null_argument_exception ("", "library_mask");
          
        static common::ComponentLoader loader (COMPONENTS_MASK);

        for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
          if (common::wcmatch (iter->name.c_str (), library_mask))
            iter->binder (environment);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("script::LibraryManager::BindLibraries");
        throw;
      }
    }

  private:
///Описание библиотеки
    struct Library
    {
      stl::string name;   //имя библиотеки
      BindHandler binder; //функтор биндинга библиотеки

      Library (const char* in_name, const BindHandler& in_binder) : name (in_name), binder (in_binder) {}
    };

    typedef stl::list<Library> LibraryList;

  private:
    LibraryList libraries; //библиотеки
};

typedef common::Singleton<LibraryManagerImpl> LibraryManagerSingleton;

}

/*
    Врапперы над вызовами к LibraryManager
*/

void LibraryManager::RegisterLibrary (const char* name, const BindHandler& binder)
{
  LibraryManagerSingleton::Instance ()->RegisterLibrary (name, binder);
}

void LibraryManager::UnregisterLibrary (const char* name)
{
  LibraryManagerSingleton::Instance ()->UnregisterLibrary (name);
}

void LibraryManager::UnregisterAllLibraries ()
{
  LibraryManagerSingleton::Instance ()->UnregisterAllLibraries ();
}

void LibraryManager::BindLibraries (Environment& environment, const char* library_mask)
{
  LibraryManagerSingleton::Instance ()->BindLibraries (environment, library_mask);
}
