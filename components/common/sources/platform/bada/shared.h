#include <platform/platform.h>
#include <common/file.h>
#include <xtl/common_exceptions.h>

namespace common
{

    class BadaFileSystem : public ICustomFileSystem {
        public:
            
            //Работа с файлом
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

            //Управление расположением файлов
            void Remove (const char* file_name);
            void Rename (const char* file_name,const char* new_name);
            void Mkdir  (const char* dir_name);
   
            //Получение информации о файле
            bool IsFileExist (const char* file_name);
            bool GetFileInfo (const char* file_name,FileInfo& info);   
   
            //Поиск файла
            void Search (const char* wc_mask,const FileSearchHandler& handler);

            //Подсчёт ссылок
            void AddRef  () {}
            void Release () {}
    };

}