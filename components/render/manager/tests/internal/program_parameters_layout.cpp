#include "shared.h"

void on_application_initialized ()
{
  try
  {
    Test test (L"Program parameters layout");
    
    RenderManagerPtr manager = Wrappers::Unwrap<RenderManagerImpl> (test.RenderManager ());
    
    ProgramParametersLayoutPtr layout1 = ProgramParametersLayout::Create (&manager->DeviceManager ().Device (), &manager->Settings ());
        
    LowLevelProgramParametersLayoutPtr low_level_layout = layout1->DeviceLayout ();
    
    ProgramParametersLayoutPtr layout2 = ProgramParametersLayout::Create (&manager->DeviceManager ().Device (), &manager->Settings ());
    
    common::PropertyLayout properties_layout;
    
    properties_layout.AddProperty ("A", PropertyType_Integer);
    properties_layout.AddProperty ("B", PropertyType_Vector);    
    properties_layout.AddProperty ("C", PropertyType_Matrix);    
    properties_layout.AddProperty ("D", PropertyType_Float);    
    properties_layout.AddProperty ("E", PropertyType_String);    
    
    layout2->SetSlot (ProgramParametersSlot_Material, properties_layout);
    
    low_level_layout = layout2->DeviceLayout ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of program_parameters_layout_test:\n");

  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
