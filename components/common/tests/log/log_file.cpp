#include "shared.h"

using namespace common;

const char* DST_FILE_NAME = "/io/stdout/test.html";

int main ()
{
  printf ("Results of log_file_test:\n");

  LogFile log_file;

  size_t default_filter_index = log_file.AddFilter ("*", "[{time}] {log}: {message}<br>", 1);
  log_file.AddFilter ("*error*", "<font color=#ff0000><strong>[{day}-{month}-{year} {hour}:{minutes}:{seconds}] {log}: {message}</strong></font><br>", 0);

  OutputFile output_file (DST_FILE_NAME);
  File    dummy_file;

  log_file.Print ("not_visible_logger", "Not visible message");

  log_file.SetFile (output_file);

  log_file.Print ("visible_logger", "Visible message");
  log_file.Print ("error_logger", "Big error message");

  log_file.RemoveFilter (default_filter_index);
  log_file.Print ("visible_logger", "Visible message");
  log_file.Print ("error_logger", "Big error message");
  
  log_file.RemoveAllFilters ();
  log_file.Print ("visible_logger", "Visible message");
  log_file.Print ("error_logger", "Big error message");

  log_file.Flush ();

  log_file.SetFile (dummy_file);

  log_file.Print ("not_visible_logger", "Not visible message");
  
  return 0;
}
