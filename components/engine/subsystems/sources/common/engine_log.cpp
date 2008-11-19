#include "shared.h"

using namespace engine;
using namespace common;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME    = "Log";                   //имя подсистемы
const char* COMPONENT_NAME    = "engine.subsystems.Log"; //имя компонента
const char* DEFAULT_FILE_NAME = "/io/stdout/log.txt";    //имя файла вывода по умолчанию

/*
   Подсистема печати лог сообщений в консоль               
*/

class LogSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    LogSubsystem (common::ParseNode& node)
      : need_flush (false)
    {
      static const char* METHOD_NAME = "LogSubsystem::LogSubsystem";

      ParseNode file_node = node.First ("File");

      if (file_node)
      {
        const char *file_name              = get<const char*> (file_node, "FileName", DEFAULT_FILE_NAME),
                   *file_log_filters_masks = get<const char*> (file_node, "Filters", "*");

        need_flush = get<bool> (file_node, "Flush", true);

        if (*file_log_filters_masks && *file_name)
        {
          StringArray log_filters_masks = split (file_log_filters_masks);

          console_log_filters.reserve (log_filters_masks.Size ());

          for (size_t i = 0; i < log_filters_masks.Size (); i++)
            file_log_filters.push_back (LogFilter (log_filters_masks[i], xtl::bind (&LogSubsystem::FileLogHandler, this, _1, _2)));

          output_file.AddFilter ("*", "{log}: {message}");

          output_file.SetFile (OutputFile (file_name));

          for (ParseNamesakeIterator iter = file_node.First ("OutputFormat"); iter; ++iter)
          {
            const char *filter      = get<const char*> (*iter, "Filter"),
                       *replacement = get<const char*> (*iter, "Replacement");

            size_t sort_order = get<size_t> (*iter, "SortOrder", ~0);

            output_file.AddFilter (filter, replacement, sort_order);
          }
        }
      }    

      ParseNode console_node = node.First ("Console");

      if (console_node)
      {
        const char* console_log_filters_masks = get<const char*> (console_node, "Filters", "*");

        if (*console_log_filters_masks)
        {
          StringArray log_filters_masks = split (console_log_filters_masks);

          console_log_filters.reserve (log_filters_masks.Size ());

          for (size_t i = 0; i < log_filters_masks.Size (); i++)
            console_log_filters.push_back (LogFilter (log_filters_masks[i], xtl::bind (&LogSubsystem::ConsoleLogHandler, this, _1, _2)));
        }
      }
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    void ConsoleLogHandler (const char* log_name, const char* message)
    {
      Console::Printf ("%s: %s\n", log_name, message);
    }
    
    void FileLogHandler (const char* log_name, const char* message)
    {
      output_file.Print (log_name, message);

      if (need_flush)
        output_file.Flush ();
    }

  private:
    LogSubsystem (const LogSubsystem&);             //no impl
    LogSubsystem& operator = (const LogSubsystem&); //no impl

  private:
    stl::vector<LogFilter> console_log_filters;
    stl::vector<LogFilter> file_log_filters;
    LogFile                output_file;
    bool                   need_flush;
};

/*
   Компонент печати лог сообщений в консоль
*/

class LogComponent
{
  public:
    //загрузка компонента
    LogComponent () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<LogSubsystem> subsystem (new LogSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::LogComponent::StartupHandler");
        throw;
      }      
    }
};

extern "C"
{

ComponentRegistrator<LogComponent> LogSubsystem (COMPONENT_NAME);

}

}
