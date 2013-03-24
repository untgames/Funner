#include <stdio.h>
#include <wchar.h>
#include <memory>
#include <string>

#include <wrl/client.h>

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)	
{
  auto localAppDataParh = Windows::Storage::ApplicationData::Current->LocalFolder->Path;

  std::wstring path( localAppDataParh->Data() );
 
  path += L"\\mylog";

  FILE* file = _wfopen (path.c_str (), L"wt");

  if (file)
  {
    fprintf (file, "Hello world!!!!!\n");
  }

  fclose (file);

  return 0;
}
