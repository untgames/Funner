#include "shared.h"

int main ()
{
  printf ("Results of web_view_test:\n");

  try
  {
    WebView view;

    view.Window ().SetSize (400, 300);

    view.Window ().SetTitle ("Test web view");

    view.LoadRequest ("http://www.google.com");    

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
