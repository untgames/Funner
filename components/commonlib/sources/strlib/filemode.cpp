#include <common/strlib.h>
#include <common/file.h>

using namespace common;
using namespace stl;

inline void TestModeFlag (string& tmp,filemode_t mode,size_t flag,const char* flag_name,const char* delimiter="/")
{
  if (mode & flag)
  {
    if (!tmp.empty ()) 
      tmp += delimiter;

    tmp += flag_name;
  }
}

/*
    Формирование строки режима файла
*/

string common::strfilemode (filemode_t mode)
{   
  string res;

  TestModeFlag (res,mode,FILE_MODE_READ,"read");
  TestModeFlag (res,mode,FILE_MODE_WRITE,"write");
  TestModeFlag (res,mode,FILE_MODE_SEEK,"seek");
  TestModeFlag (res,mode,FILE_MODE_REWIND,"rewind");
  TestModeFlag (res,mode,FILE_MODE_RESIZE,"resize");
  TestModeFlag (res,mode,FILE_MODE_CREATE,"create");

  return res;
}

/*
    Форматирование флагов поиска
*/

string common::strsearchflags (size_t flags)
{   
  string res;

  TestModeFlag (res,flags,FILE_SEARCH_FILES,"files");
  TestModeFlag (res,flags,FILE_SEARCH_DIRS,"dirs");
  TestModeFlag (res,flags,~FILE_SEARCH_NO_PACKS,"packs");
  TestModeFlag (res,flags,FILE_SEARCH_SUBDIRS,"recursive");
  TestModeFlag (res,flags,FILE_SEARCH_FULL_PATHS,"full-paths");
  TestModeFlag (res,flags,FILE_SEARCH_SORT,"sort");

  return res;
}
