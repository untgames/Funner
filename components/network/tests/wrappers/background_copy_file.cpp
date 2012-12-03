#include "shared.h"

using namespace common;

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

    FileSystem::BackgroundCopyFile (source_file_path.c_str (), "/io/stdout", &callback, ActionThread_Background);

    source_file_path = "";

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
