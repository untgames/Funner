#include "shared.h"

using namespace engine;

namespace
{

/*
    Константы
*/

const char*  LOG_PREFIX                      = "engine"; //префикс потока протоколирования
const size_t DEFAULT_SUBSYSTEMS_RESERVE_SIZE = 128;      //резервируемое количество подсистем

}

/*
    Реализация движка
*/

struct SubsystemManager::Impl
{
  public:
///Конструктор
    Impl (const char* in_name)
      : name (in_name),
        log (common::format ("%s.%s", LOG_PREFIX, in_name).c_str ()),
        auto_name_index (0)
    {
      subsystems.reserve (DEFAULT_SUBSYSTEMS_RESERVE_SIZE);
    }
    
///Деструктор
    ~Impl ()
    {
      RemoveAllSubsystems ();
    }
    
///Протокол
   common::Log& Log () { return log; }

///Установка имени
    void SetName (const char* in_name)
    {
      if (!in_name)
        throw xtl::make_null_argument_exception ("engine::SubsystemManager::SetName", "name");

      log.Printf ("SubsystemManager change name to '%s'", in_name);

      name = in_name;

      common::Log (common::format ("%s.%s", LOG_PREFIX, in_name).c_str ()).Swap (log);
    }

///Получение имени
    const char* Name () { return name.c_str (); }

///Количество запущенных подсистем
    size_t SubsystemsCount () const { return subsystems.size (); }

///Получение подсистемы
    ISubsystemControl& Subsystem (size_t index) const
    {
      if (index >= subsystems.size ())
        throw xtl::make_range_exception ("engine::SubsystemManager::Subsystem", "index", index, subsystems.size ());

      return *subsystems [index];
    }

///Добавление подсистемы
    void AddSubsystem (const char* name, ISubsystem* subsystem)
    {
      static const char* METHOD_NAME = "engine::SubsystemManager::AddSubsystem";
      
        //проверка корректности аргументов

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!subsystem)
        throw xtl::make_null_argument_exception (METHOD_NAME, "subsystem");
        
      for (Subsystems::iterator iter=subsystems.begin (), end=subsystems.end (); iter!=end; ++iter)
      {
        if ((*iter)->name == name)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Subsystem has already registered");                  
      }

      subsystems.push_back (SubsystemControlPtr (new SubsystemControl (name, subsystem), false));

      log.Printf ("Add subsystem '%s'", name);
    }
    
    void AddSubsystem (ISubsystem* subsystem)
    {
      try
      {
        ++auto_name_index;
        
        AddSubsystem (common::format ("Subsystem%02u", auto_name_index).c_str (), subsystem);
      }
      catch (...)
      {
        auto_name_index--;
        throw;
      }
    }

///Удаление подсистемы
    void RemoveSubsystem (ISubsystem* subsystem)
    {
      if (!subsystem)
        return;
        
      for (Subsystems::iterator iter=subsystems.begin (); iter!=subsystems.end ();)
      {
        if ((*iter)->subsystem == subsystem)
        {
          log.Printf ("Remove susbsystem '%s'", (*iter)->name.c_str ());
          
          subsystems.erase (iter);
        }
        else ++iter;
      }        
    }
    
///Удаление подсистем по маске имени
    void RemoveSubsystems (const char* wc_mask)
    {            
      if (!wc_mask)
        return;
        
      for (Subsystems::reverse_iterator iter=subsystems.rbegin (); iter!=subsystems.rend ();)
      {
        if (common::wcmatch ((*iter)->name.c_str (), wc_mask))
        {
          log.Printf ("Remove susbsystem '%s'", (*iter)->name.c_str ());

          subsystems.erase (iter.base () - 1);
        }
        else ++iter;
      }
    }
    
///Удаление всех подсистем
    void RemoveAllSubsystems ()
    {
      for (Subsystems::reverse_iterator iter=subsystems.rbegin (); iter!=subsystems.rend (); ++iter)
        log.Printf ("Remove susbsystem '%s'", (*iter)->name.c_str ());

      subsystems.clear ();
    }

  private:
    typedef xtl::com_ptr<ISubsystem> SubsystemPtr;

    struct SubsystemControl: public ISubsystemControl, public xtl::reference_counter
    {
      stl::string  name;
      SubsystemPtr subsystem;
      
      SubsystemControl (const char* in_name, ISubsystem* in_subsystem) :
        name (in_name), subsystem (in_subsystem) {}

      const char* Name () { return name.c_str (); }
    };

    typedef xtl::intrusive_ptr<SubsystemControl> SubsystemControlPtr;
    typedef stl::vector<SubsystemControlPtr>     Subsystems;

  private:
    stl::string name;
    Subsystems  subsystems;
    common::Log log;
    size_t      auto_name_index;
};

/*
    Конструктор / деструктор
*/

SubsystemManager::SubsystemManager ()
{
  static size_t default_name_id = 1;

  impl = new Impl (common::format ("SubsystemManager%u", default_name_id++).c_str ());
}

SubsystemManager::~SubsystemManager ()
{
}

/*
    Имя
*/

void SubsystemManager::SetName (const char* name)
{
  impl->SetName (name);
}

const char* SubsystemManager::Name () const
{
  return impl->Name ();
}

/*
    Запуск систем
*/

void SubsystemManager::Start (const common::ParseNode& node)
{
  try
  {
    common::ParseNode copy = node;
    
    StartupManagerSingleton::Instance ().Start (copy, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("engine::SubsystemManager::Start(const ParseNode&)");
    throw;
  }
}

void SubsystemManager::Start (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    common::Parser    parser (file_name);            
    common::ParseNode root = parser.Root ().First ("Configuration");

    StartupManagerSingleton::Instance ().Start (root, *this);

    parser.Log ().Print (xtl::bind (&common::Log::Print, &impl->Log (), _1));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("engine::SubsystemManager::Start(const char*)");
    throw;
  }
}

/*
    Перебор подсистем
*/

size_t SubsystemManager::SubsystemsCount () const
{
  return impl->SubsystemsCount ();
}

ISubsystemControl& SubsystemManager::Subsystem (size_t index) const
{
  return impl->Subsystem (index);
}

/*
    Добавление / удаление подсистем
*/

void SubsystemManager::AddSubsystem (const char* name, ISubsystem* subsystem)
{
  impl->AddSubsystem (name, subsystem);
}

void SubsystemManager::AddSubsystem (ISubsystem* subsystem)
{
  impl->AddSubsystem (subsystem);
}

void SubsystemManager::RemoveSubsystem (ISubsystem* subsystem)
{
  impl->RemoveSubsystem (subsystem);
}

void SubsystemManager::RemoveSubsystems (const char* wc_mask)
{
  impl->RemoveSubsystems (wc_mask);
}

void SubsystemManager::RemoveAllSubsystems ()
{
  impl->RemoveAllSubsystems ();
}
