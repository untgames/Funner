#include "shared.h"

namespace syslib
{

//Преобразование процесса к приложению
void transform_process_type ()
{
  static bool transformed;

  if (!transformed)
  {
    ProcessSerialNumber current_process = { 0, kCurrentProcess };
    TransformProcessType (&current_process, kProcessTransformToForegroundApplication);
    SetFrontProcess (&current_process);

    transformed = true;
  }
}

}
