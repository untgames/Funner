#include "shared.h"

using namespace engine;
using namespace common;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "FileSystem";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.FileSystem"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;

/*
   Подсистема файловой системы
*/

class FileSystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    FileSystem (common::ParseNode& node)
    {
      for (common::Parser::NamesakeIterator iter=node.First ("Paths"); iter; ++iter)
      {
        common::ParseNode paths_node = iter->First ("#text");
        
        if (!paths_node)
          continue;
        
        common::Parser::AttributeIterator attr_iter = make_attribute_iterator (paths_node);
        
        for (size_t i=0, count=paths_node.AttributesCount (); i<count; i++)
        {
          const char* path = paths_node.Attribute (i);          
          
          paths.Add (path);
        }
      }
      
      Log log (LOG_NAME);
      common::FileSystem::LogHandler log_handler (xtl::bind (&common::Log::Print, &log, _1));
        
      for (size_t i=0; i<paths.Size (); i++)
      {
        const char* path = paths [i];

        common::FileSystem::AddSearchPath (path, log_handler);        
      }
    }
    
    ~FileSystem ()
    {
      for (size_t i=0; i<paths.Size (); i++)
        common::FileSystem::RemoveSearchPath (paths [i]);
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    FileSystem (const FileSystem&);             //no impl
    FileSystem& operator = (const FileSystem&); //no impl
    
  private:
    common::StringArray paths;
};

/*
   Компонент
*/

class FileSystemComponent
{
  public:
    //загрузка компонента
    FileSystemComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<FileSystem> subsystem (new FileSystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::FileSystemComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<FileSystemComponent> FileSystem (COMPONENT_NAME);

}

}
