#include <stdio.h>

#include <xtl/function.h>

#include <input/controls_detector.h>

using namespace input;

const char* CONTROLS_DETECTOR_FILE_NAME = "data/controls_detector.keydet";
const char* DST_FILE_NAME = "/io/stdout/test.keydet";

int main ()
{
  printf ("Results of controls_detector2_test:\n");
  
  try
  {
    ControlsDetector controls_detector (CONTROLS_DETECTOR_FILE_NAME);

    controls_detector.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
