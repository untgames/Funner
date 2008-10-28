#include <cstdio>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/log.h>
#include <common/parser.h>

#include <engine/subsystem_manager.h>

using namespace engine;

const char* CONFIGURATION_FILE_NAME   = "data/configuration.xml";
const char* SUBSYSTEM1_CONFIGURATION  = "Subsystem11";
const char* SUBSYSTEM2_CONFIGURATION  = "Subsystem12";
const char* SUBSYSTEM3_CONFIGURATION  = "Subsystem21";
const char* SUBSYSTEM4_CONFIGURATION  = "Subsystem22";

class EngineSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
    EngineSubsystem (const char* in_name) : name (in_name) {}

    const char* Name ()
    {
      return name.c_str ();
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }

    void SubsystemStarter (common::ParseNode& node, SubsystemManager& manager)
    {
      size_t value = common::get<size_t> (node, "Value");

      printf ("Starting subsystem '%s', value='%u'\n", Name (), value);

      if (value == 3) manager.AddSubsystem (this);
      else            manager.AddSubsystem (Name (), this);
    }

  private:
    stl::string name;
};

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

int main ()
{
  printf ("Results of subsystem_manager_test:\n");

  try
  {
    common::LogFilter log_filter ("engine.*", &log_handler);

    xtl::com_ptr<EngineSubsystem> subsystem1 (new EngineSubsystem (SUBSYSTEM1_CONFIGURATION), false),
                                  subsystem2 (new EngineSubsystem (SUBSYSTEM2_CONFIGURATION), false),
                                  subsystem3 (new EngineSubsystem (SUBSYSTEM3_CONFIGURATION), false),
                                  subsystem4 (new EngineSubsystem (SUBSYSTEM4_CONFIGURATION), false);

    StartupManager::RegisterStartupHandler (SUBSYSTEM1_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem1.get (), _1, _2));
    StartupManager::RegisterStartupHandler (SUBSYSTEM2_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem2.get (), _1, _2));
    StartupManager::RegisterStartupHandler (SUBSYSTEM3_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem3.get (), _1, _2));
    StartupManager::RegisterStartupHandler (SUBSYSTEM4_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem4.get (), _1, _2));

    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);

    printf ("Engine subsystems count is %u\n", manager.SubsystemsCount ());
    
    printf ("Remove subsystems by mask\n");
    
    manager.RemoveSubsystems ("Subsystem2*");
    
    printf ("Remove all subsystems\n");    

    manager.RemoveAllSubsystems ();
  }
  catch (xtl::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
