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

  TestModeFlag (res,mode,FileMode_Read,"read");
  TestModeFlag (res,mode,FileMode_Write,"write");
  TestModeFlag (res,mode,FileMode_Seek,"seek");
  TestModeFlag (res,mode,FileMode_Rewind,"rewind");
  TestModeFlag (res,mode,FileMode_Resize,"resize");
  TestModeFlag (res,mode,FileMode_Create,"create");

  return res;
}

/*
    Форматирование флагов поиска
*/

string common::strsearchflags (size_t flags)
{   
  string res;

  TestModeFlag (res,flags,FileSearch_Files,"files");
  TestModeFlag (res,flags,FileSearch_Dirs,"dirs");
  TestModeFlag (res,flags,~FileSearch_NoPacks,"packs");
  TestModeFlag (res,flags,FileSearch_SubDirs,"recursive");
  TestModeFlag (res,flags,FileSearch_FullPaths,"full-paths");
  TestModeFlag (res,flags,FileSearch_Sort,"sort");

  return res;
}
