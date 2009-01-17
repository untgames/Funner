#include <windows.h>

#include <xtl/connection.h>

#include "shared.h"

namespace
{

const char* CONFIGURATION_REGISTRY_NAME         = "ApplicationServer.Configuration";
const char* VISUALIZE_NEW_CALCULATIONS_VAR_NAME = "VisualizeNewCalculations";
const char* VISUALIZE_NEW_CALCULATIONS_ON       = "Визуализировать новые расчёты";
const char* VISUALIZE_NEW_CALCULATIONS_OFF      = "Не визуализировать новые расчёты";

}

//Настройки приложения

MyApplicationServer::Configuration::Configuration ()
  : configuration_registry (CONFIGURATION_REGISTRY_NAME)
{
  visualize_new_calculations_connection = configuration_registry.RegisterEventHandler (VISUALIZE_NEW_CALCULATIONS_VAR_NAME,
    common::VarRegistryEvent_OnChangeVar, xtl::bind (&MyApplicationServer::Configuration::OnChangeVisualizeNewCalculationsMode, this));
}

void MyApplicationServer::Configuration::Load (const char* configuration_file_name)
{
  static const char* METHOD_NAME = "MyApplicationServer::Configuration::Load";

    //чтение конфигурации

  common::Parser   parser     (configuration_file_name);
  common::ParseLog parser_log (parser.Log ());

  for (size_t i = 0; i < parser_log.MessagesCount (); i++)
  {
    common::ParseLogMessageType message_type = parser_log.MessageType (i);

    if (message_type == common::ParseLogMessageType_Error || message_type == common::ParseLogMessageType_FatalError)
      throw xtl::format_operation_exception (METHOD_NAME, "Configuration parse error: '%s'", parser_log.Message (i));
  }

  common::ParseIterator iter = parser.Root ().First ("Configuration");

  win32_plugin_path                = common::get<const char*> (*iter, "Win32PluginPath");
  osx_plugin_path                  = common::get<const char*> (*iter, "MacOSXPluginPath", "");
  linux_plugin_path                = common::get<const char*> (*iter, "LinuxPluginPath", "");
  condor_path                      = common::get<const char*> (*iter, "CondorPath", "");
  use_condor                       = common::get<bool>        (*iter, "UseCondor", true);
  author                           = common::get<const char*> (*iter, "Author", "Неизвестный автор");
  visualize_new_calculations       = common::get<bool>        (*iter, "VisualizeNewCalculations", true);
  processors_count_for_calculation = common::get<size_t>      (*iter, "ProcessorsCountForCalculation", 0);

  SYSTEM_INFO system_info;

  GetSystemInfo (&system_info);

  if (processors_count_for_calculation)
    processors_count_for_calculation = stl::min (processors_count_for_calculation, (size_t)system_info.dwNumberOfProcessors);
  else
    processors_count_for_calculation = system_info.dwNumberOfProcessors;

  configuration_registry.SetValue (VISUALIZE_NEW_CALCULATIONS_VAR_NAME, visualize_new_calculations ? VISUALIZE_NEW_CALCULATIONS_ON : VISUALIZE_NEW_CALCULATIONS_OFF);
}

void MyApplicationServer::Configuration::OnChangeVisualizeNewCalculationsMode ()
{
  stl::string visualize_new_calculations_mode = to_string (configuration_registry.GetValue (VISUALIZE_NEW_CALCULATIONS_VAR_NAME));

  if (visualize_new_calculations_mode == VISUALIZE_NEW_CALCULATIONS_ON)
    visualize_new_calculations = true;
  else if (visualize_new_calculations_mode == VISUALIZE_NEW_CALCULATIONS_OFF)
    visualize_new_calculations = false;
}
