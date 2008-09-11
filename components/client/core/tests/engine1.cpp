#include <cstdio>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <common/log.h>

#include <render/screen.h>

#include <client/engine.h>

const char* CONFIGURATION_FILE_NAME   = "data/configuration.xml";
const char* CONFIGURATION_BRANCH_NAME = "Configuration";
const char* SUBSYSTEM1_CONFIGURATION  = "Subsystem1";
const char* SUBSYSTEM2_CONFIGURATION  = "Subsystem2";
const char* SUBSYSTEM3_CONFIGURATION  = "Subsystem3";
const char* SUBSYSTEM4_CONFIGURATION  = "Subsystem4";

using namespace client;

class ClientEventListener : public IEngineEventListener
{
  public:
///События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen* screen) 
    {
      printf ("OnSetScreen: attachment_name is '%s', screen name is '%s'\n", attachment_name, screen->Name ());
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      printf ("OnRemoveScreen: attachment_name is '%s'\n", attachment_name);
    }

///События установки/удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener* listener) 
    {
      printf ("OnSetListener: attachment_name is '%s', listener name is '%s'\n", attachment_name, listener->Name ());
    }

    void OnRemoveListener (const char* attachment_name) 
    {
      printf ("OnRemoveListener: attachment_name is '%s'\n", attachment_name);
    }

///Событие удаления клиента
    void OnDestroy () 
    {
      printf ("OnDestroy\n");
    }
};

class EngineSubsystem : public IEngineSubsystem, public xtl::reference_counter
{
  public:
    EngineSubsystem (const char* in_name) : name (in_name) {}

    const char* Name ()
    {
      return name.c_str ();
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }

    void SubsystemStarter (common::VarRegistry& registry, IEngineStartupParams* engine_startup_params, Engine& engine)
    {
      printf ("Starting subsystem '%s'. Engine startup params = %p\n", Name (), engine_startup_params);

      engine.AddSubsystem (Engine::SubsystemPointer (this));
    }

  private:
    stl::string name;
};

void engine_tracker ()
{
  printf ("Engine tracked\n");
}

void log_handler (const char* log_name, const char* message)
{
  printf ("Log '%s' message: '%s'\n", log_name, message);
}

int main ()
{
  try
  {
    common::LogFilter log_filter ("*", &log_handler);
    common::ConfigurationRegistry::LoadConfiguration (CONFIGURATION_FILE_NAME);

    ClientEventListener client_event_listener;
    EngineAttachments client;
    EngineAttachments client2;

    render::Screen screen;

    screen.SetName ("Screen");

    xtl::com_ptr<EngineSubsystem> subsystem1 (new EngineSubsystem (SUBSYSTEM1_CONFIGURATION), false), subsystem2 (new EngineSubsystem (SUBSYSTEM2_CONFIGURATION), false),
                                  subsystem3 (new EngineSubsystem (SUBSYSTEM3_CONFIGURATION), false), subsystem4 (new EngineSubsystem (SUBSYSTEM4_CONFIGURATION), false);

    StartupManager::RegisterStartupHandler (SUBSYSTEM1_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem1.get (), _1, _2, _3), StartupGroup_Level5);
    StartupManager::RegisterStartupHandler (SUBSYSTEM2_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem2.get (), _1, _2, _3));
    StartupManager::RegisterStartupHandler (SUBSYSTEM3_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem3.get (), _1, _2, _3), StartupGroup_Level2);
    StartupManager::RegisterStartupHandler (SUBSYSTEM4_CONFIGURATION, xtl::bind (&EngineSubsystem::SubsystemStarter, subsystem4.get (), _1, _2, _3), 0);

    Engine engine (CONFIGURATION_BRANCH_NAME, StartupGroup_Level2);

    engine.Attach (&client_event_listener);

    printf ("Engine configuration branch name is '%s'\n", engine.ConfigurationBranch ());
    printf ("Engine subsystems count is %u\n", engine.SubsystemsCount ());

    engine.GetTrackable ().connect_tracker (&engine_tracker);

    engine.Attach (client);

    client.SetScreen ("ScreenAttachment", &screen);

    engine.Start (StartupGroup_LevelMax);

    printf ("Engine subsystems count is %u\n", engine.SubsystemsCount ());
    printf ("engine subsystem 1 name is '%s'\n", engine.Subsystem (1).Name ());

    client2.SetScreen ("ScreenAttachment2", &screen);

    engine.Start (StartupGroup_LevelMax);

    engine.Attach (client2);

    engine.Detach ();

    client.SetScreen ("ScreenAttachment", &screen);

    engine.RemoveSubsystem (subsystem1);

    printf ("Engine subsystems count is %u\n", engine.SubsystemsCount ());
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: '%s'\n", e.what ());

    return 1;
  }
  catch (...)
  {
    printf ("Unknown exception\n");

    return 1;
  }

  return 0;
}
