#include <stdio.h>
#include <string.h>
#include <stl/string>
#include <xtl/reference_counter.h>
#include <xtl/common_exceptions.h>
#include <common/file.h>

using namespace common;

const size_t READ_BLOCK_SIZE   = 4;
const char*  MESSAGE1          = "0123456789\n";
const char*  MESSAGE2          = "9876543210\n";
const char*  MESSAGE3          = "---ABCDEFGHIJKLMNOPQRSTUVWXYZ---\n";
const char*  RESULTS_FILE_NAME = "/log/data/buffered_file_results.txt";
const char*  LOG_PREFIX        = "/log/";

static char buffer [1024*16];

class LogFileSystem: public ICustomFileSystem, public xtl::reference_counter
{
  public:
///Работа с файлом
    file_t FileOpen (const char* name, filemode_t mode_flags, size_t buffer_size)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");          
          
        stl::string path = name + strlen (LOG_PREFIX);
          
        printf ("open file '%s'\n", path.c_str ());

        return reinterpret_cast<file_t> (new StdFile (path.c_str (), mode_flags, buffer_size));
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileOpen");
        throw;
      }
    }
    
    void FileClose (file_t handle)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);

        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("close file '%s'\n", file->Path ());
          
        delete file;
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileClose");
        throw;
      }      
    }
    
    size_t FileBufferSize (file_t handle)
    {
      return 0;
    }
    
    size_t FileRead (file_t handle, void* buf, size_t size)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("read %u bytes from file '%s' position %u\n", size, file->Path (), file->Tell ());

        return file->Read (buf, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileRead");
        throw;
      }      
    }
    
    size_t FileWrite (file_t handle, const void* buf, size_t size)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("write %u bytes to file '%s' position %u\n", size, file->Path (), file->Tell ());
          
        return file->Write (buf, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileWrite");
        throw;
      }
    }
    
    void FileRewind (file_t handle)
    {
      try      
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("rewind file '%s'\n", file->Path ());

        return file->Rewind ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileRewind");
        throw;
      }      
    }
    
    filepos_t FileSeek (file_t handle, filepos_t pos)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("seek file '%s' to position %lld\n", file->Path (), pos);

        return file->Seek (pos);
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileSeek");
        throw;
      }      
    }
    
    filepos_t FileTell (file_t handle)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("get file position for '%s'\n", file->Path ());

        return file->Tell ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileTell");
        throw;
      }      
    }
    
    filesize_t FileSize (file_t handle)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        printf ("get file size for '%s'\n", file->Path ());

        return file->Size ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileSize");
        throw;
      }      
    }
    
    void FileResize (file_t handle, filesize_t new_size)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        printf ("resize file '%s' to %llu\n", file->Path (), new_size);

        return file->Resize (new_size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileResize");
        throw;
      }      
    }
    
    bool FileEof (file_t handle)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        return file->Eof ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileEof");
        throw;
      }      
    }
    
    void FileFlush (file_t handle)
    {
      try
      {
        StdFile* file = reinterpret_cast<StdFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        printf ("flush file '%s'\n", file->Path ());

        return file->Flush ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("LogFileSystem::FileFlush");
        throw;
      }      
    }

///Управление расположением файлов
    void Remove (const char* file_name)
    {
      FileSystem::Remove (file_name + strlen (LOG_PREFIX));
    }
    
    void Rename (const char* file_name, const char* new_name)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::Remove", "Rename operation not supported on url links");      
    }

    void Mkdir (const char* dir_name)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::Remove", "Mkdir operation not supported on url links");      
    }

///Получение информации о файле
    bool IsFileExist (const char* file_name)
    {
      return false;
    }
    
    bool GetFileInfo (const char* file_name, FileInfo& info)
    {
      return false;
    }

///Информация о файловой системе
    filesize_t GetFreeSpace (const char* path)
    {
      return (filesize_t)-1;
    }

    filesize_t GetTotalSpace (const char* path)
    {
      return (filesize_t)-1;
    }

//Файловые атрибуты
    void SetFileAttribute (const char* file_name, const char* attribute, const void* data, size_t size)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::SetFileAttribute");
    }

    void GetFileAttribute (const char* file_name, const char* attribute, void* data, size_t size)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::GetFileAttribute");
    }

    bool HasFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::HasFileAttribute");
    }

    void RemoveFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::format_not_supported_exception ("LogFileSystem::RemoveFileAttribute");
    }

///Поиск файла
    void Search (const char* wc_mask, const FileSearchHandler& handler)
    {
      //поиска по URL ссылке нет
    }

///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }
};

int main ()
{
  printf ("Results of buffered_file1_test:\n");
  
  try
  {
    FileSystem::Mount ("/log", xtl::com_ptr<LogFileSystem> (new LogFileSystem, false).get ());

    StdFile file (RESULTS_FILE_NAME,FileMode_ReadWrite|FileMode_Create,strlen (MESSAGE1) * 2 + 1);

    printf ("Is file buffered: %s\n",file.IsBuffered () ? "true" : "false");

    size_t message_len = xtl::xstrlen (MESSAGE1);

    for (size_t i=0;i<10;i++)
      file.Write (MESSAGE1,message_len);

    message_len = xtl::xstrlen (MESSAGE2);

    file.Seek (message_len*9,FileSeekMode_Current);

    for (size_t i=0;i<10;i++)
    {
      file.Write (MESSAGE2,message_len);
      file.Seek  (-(int)message_len*2,FileSeekMode_Current);
    }

    file.Seek  (-(int)message_len,FileSeekMode_Current);
    file.Write (MESSAGE3,strlen (MESSAGE3));
    file.Write (MESSAGE3,strlen (MESSAGE3));
    
    printf ("File content (forward read):\n");

    file.Rewind ();

    while (!file.Eof ())
    {
      char s [READ_BLOCK_SIZE+1] = {0};
      
      file.Read (s,sizeof (s)-1);
      
      printf ("%s",s);
    }
    
    printf ("End file content\n");
    
    printf ("File content (backward read):\n");
    
    if (sizeof (buffer) - 1 < file.Size ())
    {
      printf ("Read buffer too small\n");
      return 0;
    }

    file.Seek (0,FileSeekMode_End);

    while (file.Tell ())
    {
      file.Read (buffer+file.Seek (-(int)READ_BLOCK_SIZE,FileSeekMode_Current),READ_BLOCK_SIZE);
      file.Seek (-(int)READ_BLOCK_SIZE,FileSeekMode_Current);
    }

    buffer [file.Size ()] = 0;

    printf ("%s",buffer);

    printf ("End file content\n");

    printf ("File content (direct read test):\n");

    memset (buffer,0,sizeof (buffer));

    file.Read (buffer, (size_t)file.Size ());

    printf ("%s",buffer);

    printf ("End file content\n");

    file.Close ();

    FileSystem::Remove (RESULTS_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
