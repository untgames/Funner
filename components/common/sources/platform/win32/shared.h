#ifndef COMMONLIB_PLATFORM_WIN32_SHARED_HEADER
#define COMMONLIB_PLATFORM_WIN32_SHARED_HEADER

#ifndef _WIN32_WCE
  #define _WIN32_WINNT 0x0600
  #define _WIN32_IE 0x0400
#endif

#include <windows.h>
#include <shlobj.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

#include <platform/platform.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Файловая система
///////////////////////////////////////////////////////////////////////////////////////////////////
class Win32FileSystem: public ICustomFileSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с файлом
///////////////////////////////////////////////////////////////////////////////////////////////////
    file_t     FileOpen     (const char* name,filemode_t mode_flags,size_t buffer_size);
    void       FileClose    (file_t);
    size_t     FileRead     (file_t,void* buf,size_t size);
    size_t     FileWrite    (file_t,const void* buf,size_t size);
    void       FileRewind   (file_t);
    filepos_t  FileSeek     (file_t,filepos_t pos);
    filepos_t  FileTell     (file_t);
    filesize_t FileSize     (file_t);
    void       FileResize   (file_t,filesize_t new_size);
    bool       FileEof      (file_t);
    void       FileFlush    (file_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление расположением файлов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о файле
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFileExist (const char* file_name);
    bool GetFileInfo (const char* file_name,FileInfo& info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск файла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Search (const char* wc_mask,const FileSearchHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}
};



//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

}

#endif
