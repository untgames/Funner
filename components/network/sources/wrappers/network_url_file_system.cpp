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
        if (mode_flags & (FileMode_Write | FileMode_Resize))
          throw xtl::format_not_supported_exception ("", "No write access inf URL files");
        
        stl::auto_ptr<File> file (new File (OpenUrlFile (name)));
        
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        return file->BufferSize ();
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        return file->Read (buf, size);
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        return file->Write (buf, size);
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        file->Rewind ();
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        return file->Seek (pos);
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        return file->Tell ();
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        return file->Size ();        
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        file->Resize (new_size);
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        return file->Eof ();        
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
        File* file = reinterpret_cast<File*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
        
        file->Flush ();
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
    File CreateTempFile ()
    {
      try
      {
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlConnection::CreateTempFile");
        throw;
      }
    }
  
    File OpenUrlFile (const char* url)
    {
      try
      {
        if (!url)
          throw xtl::make_null_argument_exception ("", "url");
          
        UrlConnection connection ((prefix + url).c_str ());
        
        xtl::uninitialized_storage<char> buffer (DOWNLOAD_BUFFER_SIZE);
        
        TempFile file ("/system/inetcache/funner_url_file%06u"); //suffix (Url.File ())!!!
        
        log.Printf ("Attempt to download URL '%s'", connection.Url ().ToString ());

        size_t size;
        
        while ((size = connection.Receive (buffer.data (), buffer.size ())) != 0)
        {
          size_t write_size = file.Write (buffer.data (), size);
          
          if (write_size != size)
            throw xtl::format_operation_exception ("", "Can't write to file '%s' %u bytes", file.Path (), size);
        }

        file.Rewind ();
        
        log.Printf ("URL resource '%s' downloaded", connection.Url ().ToString ());

        return file;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::OpenUrlFile");
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
