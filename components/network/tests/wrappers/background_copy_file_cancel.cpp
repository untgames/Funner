#include "shared.h"

using namespace common;

Action action;

void callback (const BackgroundCopyState& copy_state)
{
  const char* status;

  switch (copy_state.Status ())
  {
    case BackgroundCopyStateStatus_Started:    status = "Started";    break;
    case BackgroundCopyStateStatus_InProgress: status = "InProgress"; break;
    case BackgroundCopyStateStatus_Finished:   status = "Finished";   break;
    case BackgroundCopyStateStatus_Failed:     status = "Failed";     break;
    default:                                   status = "Unknown";    break;
  }

  printf ("Status:      '%s' / '%s'\n", status, copy_state.StatusText ());
  printf ("File size:    %u\n", copy_state.FileSize ());
  printf ("Bytes copied: %u\n", copy_state.BytesCopied ());

  if (copy_state.BytesCopied () > 5)
  {
    action.Cancel ();

    action = Action ();

    syslib::Application::Exit (0);

    return;
  }

  if (copy_state.Status () == BackgroundCopyStateStatus_Finished)
    syslib::Application::Exit (0);
  if (copy_state.Status () == BackgroundCopyStateStatus_Failed)
    syslib::Application::Exit (1);
}

int main ()
{
  printf ("Results of background_copy_file_test:\n");
  
  try
  {
    TestHttpServer http_server;

    stl::string source_file_path = common::format ("http://localhost:%u/test.txt", SERVER_PORT);

    action = FileSystem::BackgroundCopyFile (source_file_path.c_str (), "data/result.txt", &callback, ActionThread_Background, 1);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
