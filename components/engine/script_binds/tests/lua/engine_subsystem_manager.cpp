#include "shared.h"

const char* CONFIGURATION_FILE_NAME = "data/config.xml";
const char* SUBSYSTEM1_NAME         = "Group1_Subsystem1";
const char* SUBSYSTEM2_NAME         = "Group2_Subsystem1";
const char* SUBSYSTEM3_NAME         = "Group1_Subsystem2";

class Subsystem1 : public engine::ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    Subsystem1 () { printf ("Subsystem1::Subsystem1\n"); }
    ~Subsystem1 () { printf ("Subsystem1::~Subsystem1\n"); }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
};

class Subsystem2 : public engine::ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    Subsystem2 () { printf ("Subsystem2::Subsystem2\n"); }
    ~Subsystem2 () { printf ("Subsystem2::~Subsystem2\n"); }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
};

class Subsystem3 : public engine::ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    Subsystem3 () { printf ("Subsystem3::Subsystem3\n"); }
    ~Subsystem3 () { printf ("Subsystem3::~Subsystem3\n"); }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
};

void StartupHandler1 (common::ParseNode&, engine::SubsystemManager& manager)
{
  xtl::com_ptr<Subsystem1> subsystem (new Subsystem1 (), false);

  manager.Add (SUBSYSTEM1_NAME, subsystem.get ());
}

void StartupHandler2 (common::ParseNode&, engine::SubsystemManager& manager)
{
  xtl::com_ptr<Subsystem2> subsystem (new Subsystem2 (), false);

  manager.Add (SUBSYSTEM2_NAME, subsystem.get ());
}

void StartupHandler3 (common::ParseNode&, engine::SubsystemManager& manager)
{
  xtl::com_ptr<Subsystem3> subsystem (new Subsystem3 (), false);

  manager.Add (SUBSYSTEM3_NAME, subsystem.get ());
}

int main ()
{
  printf ("Results of engine_subsystem_manager_test:\n");

  try
  {
    engine::StartupManager::RegisterStartupHandler (SUBSYSTEM1_NAME, &StartupHandler1);
    engine::StartupManager::RegisterStartupHandler (SUBSYSTEM2_NAME, &StartupHandler2);
    engine::StartupManager::RegisterStartupHandler (SUBSYSTEM3_NAME, &StartupHandler3);

    engine::SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("%s: %s -> %s\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
