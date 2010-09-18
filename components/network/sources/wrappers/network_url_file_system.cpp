#include "shared.h"

using namespace common;
using namespace network;

namespace
{

/*
    Константы
*/

const size_t DOWNLOAD_BUFFER_SIZE = 32784;
const char*  LOG_NAME             = "network.url_file_system";

/*
    URL файл
*/

struct UrlFile: public Lockable
{
  UrlConnection connection;     //соединение
  File          response_file;  //файл чтения данных ответа
  bool          end_of_request; //флаг - запрос отправлен
  size_t        buffer_size;    //размер кеша буфера чтения
  
  UrlFile (const char* url, bool is_post, size_t in_buffer_size)
    : connection (url, is_post ? "method=post" : "")
    , end_of_request (false)
    , buffer_size (in_buffer_size)
  {
  }
};

/*
    URL файловая система
*/

class UrlCustomFileSystem: public ICustomFileSystem, public xtl::reference_counter
{
  public:
///Конструктор
    UrlCustomFileSystem (const char* in_prefix)
      : prefix (in_prefix)
      , log (LOG_NAME)
    {
    }
  
///Работа с файлом
    file_t FileOpen (const char* name, filemode_t mode_flags, size_t buffer_size)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");
          
        stl::string url = (prefix + name).c_str ();
          
        log.Printf ("Open url file '%s' (file_mode=%s)", url.c_str (), common::strfilemode (mode_flags).c_str ());
        
        stl::auto_ptr<UrlFile> file (new UrlFile (url.c_str (), (mode_flags & FileMode_Write) != 0, buffer_size));
        
        return reinterpret_cast<file_t> (file.release ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileOpen");
        throw;
      }
    }
    
    void FileClose (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        Lock lock (*file);
          
        FinishSend (*file);
          
        delete file;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileClose");
        throw;
      }      
    }
    
    size_t FileBufferSize (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        Lock lock (*file);          
          
        return file->buffer_size;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileBufferSize");
        throw;
      }     
    }
    
    size_t FileRead (file_t handle, void* buf, size_t size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        Lock lock (*file);          
          
        FinishSend (*file);
          
        return file->response_file.Read (buf, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileRead");
        throw;
      }      
    }
    
    size_t FileWrite (file_t handle, const void* buf, size_t size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        Lock lock (*file);          
          
        if (file->end_of_request)
        {
          return file->response_file.Write (buf, size);
        }
        else
        {          
          return file->connection.Send (buf, size);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileWrite");
        throw;
      }
    }
    
    void FileRewind (file_t handle)
    {
      try      
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        Lock lock (*file);          
          
        FinishSend (*file);
          
        file->response_file.Rewind ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileRewind");
        throw;
      }      
    }
    
    filepos_t FileSeek (file_t handle, filepos_t pos)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        Lock lock (*file);          
          
        FinishSend (*file);
          
        return file->response_file.Seek (pos);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileSeek");
        throw;
      }      
    }
    
    filepos_t FileTell (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        Lock lock (*file);          
          
        if (file->end_of_request)
        {
          return file->response_file.Tell ();
        }
        else
        {
          return 0;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileTell");
        throw;
      }      
    }
    
    filesize_t FileSize (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        Lock lock (*file);          
          
        FinishSend (*file);
          
        return file->response_file.Size ();        
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileSize");
        throw;
      }      
    }
    
    void FileResize (file_t handle, filesize_t new_size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        Lock lock (*file);          
          
        FinishSend (*file);
          
        file->response_file.Resize (new_size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileResize");
        throw;
      }      
    }
    
    bool FileEof (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        Lock lock (*file);          
          
        if (!file->end_of_request)
          return false;
          
        return file->response_file.Eof ();        
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileEof");
        throw;
      }      
    }
    
    void FileFlush (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        Lock lock (*file);          
          
        if (!file->end_of_request)
          return;
        
        file->response_file.Flush ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileFlush");
        throw;
      }      
    }

///Управление расположением файлов
    void Remove (const char* file_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Remove operation not supported on url links");
    }
    
    void Rename (const char* file_name, const char* new_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Rename operation not supported on url links");      
    }

    void Mkdir (const char* dir_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Mkdir operation not supported on url links");      
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

  private:  
    void FinishSend (UrlFile& file)
    {
      try
      {
        if (file.end_of_request)
          return;
          
        file.connection.CloseSend ();
        
        xtl::uninitialized_storage<char> buffer (DOWNLOAD_BUFFER_SIZE);
        
        TempFile temp_file ("/system/inetcache/funner_url_file%06u", file.buffer_size);
        
        log.Printf ("Attempt to download URL '%s'", file.connection.Url ().ToString ());

        size_t size;
        
        while ((size = file.connection.Receive (buffer.data (), buffer.size ())) != 0)
        {
          size_t write_size = temp_file.Write (buffer.data (), size);
          
          if (write_size != size)
            throw xtl::format_operation_exception ("", "Can't write to file '%s' %u bytes", temp_file.Path (), size);
        }

        temp_file.Rewind ();
        
        log.Printf ("URL resource '%s' downloaded (file_size=%u)", file.connection.Url ().ToString (), temp_file.Size ());
        
        file.response_file  = temp_file;
        file.end_of_request = true;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FinishSend");
        throw;
      }
    }
    
  private:
    stl::string prefix;
    Log         log;
};

/*
    Компонент запуска CURL
*/

class UrlFileSystemComponent
{
  public:
    UrlFileSystemComponent ()    
    {
      common::FileSystem::MountLink ("http://", "/inet/http");
      common::FileSystem::MountLink ("ftp://", "/inet/ftp");
      common::FileSystem::Mount     ("/inet/http", xtl::com_ptr<ICustomFileSystem> (new UrlCustomFileSystem ("http://"), false).get ());
      common::FileSystem::Mount     ("/inet/ftp", xtl::com_ptr<ICustomFileSystem> (new UrlCustomFileSystem ("ftp://"), false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<UrlFileSystemComponent> UrlFileSystem ("common.file_systems.url_file_system");

}
