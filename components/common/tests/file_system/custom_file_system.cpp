#include "test.h"

const char* MOUNT_PREFIX  = "/my_mount";
const char* ZIP_FILE_NAME = "data/test.my_zip";

namespace
{

class MyFileSystem : public ICustomFileSystem, public xtl::reference_counter
{
  public:
    MyFileSystem ()
    {
      printf ("MyFileSystem::MyFileSystem\n");
    }

    ~MyFileSystem ()
    {
      printf ("MyFileSystem::~MyFileSystem\n");
    }

///Работа с файлом
    file_t FileOpen (const char* name, filemode_t mode_flags, size_t buffer_size)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileOpen");
    }

    void FileClose (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileClose");
    }

    size_t FileRead (file_t handle, void* buf, size_t size)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileRead");
    }

    size_t FileWrite (file_t handle, const void* buf, size_t size)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileWrite");
    }

    void FileRewind (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileRewind");
    }

    filepos_t FileSeek (file_t handle, filepos_t pos)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileSeek");
    }

    filepos_t FileTell (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileTell");
    }

    filesize_t FileSize (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileSize");
    }

    void FileResize (file_t handle, filesize_t new_size)
    {
    }

    bool FileEof (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileEof");
    }

    void FileFlush (file_t handle)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::FileFlush");
    }

///Управление расположением файлов
    void Remove (const char* file_name)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::Remove");
    }

    void Rename (const char* file_name, const char* new_name)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::Rename");
    }

    void Mkdir (const char* dir_name)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::Mkdir");
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

//Файловые атрибуты
    void SetFileAttribute (const char* file_name, const char* attribute, const void* data, size_t size)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::SetFileAttribute");
    }

    void GetFileAttribute (const char* file_name, const char* attribute, void* data, size_t size)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::GetFileAttribute");
    }

    bool HasFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::HasFileAttribute");
    }

    void RemoveFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::RemoveFileAttribute");
    }

///Поиск файла
    void Search (const char* wc_mask, const FileSearchHandler& handler)
    {
      throw xtl::make_not_implemented_exception ("MyFileSystem::Search");
    }

///Подсчёт ссылок
    void AddRef ()
    {
//      printf ("MyFileSystem::AddRef\n");
      addref (this);
    }

    void Release ()
    {
//      printf ("MyFileSystem::Release\n");
      release (this);
    }
};

ICustomFileSystem* my_pack_file_creator (const char* path)
{
  return new MyFileSystem;
}

}

int main ()
{
  printf ("Results of custom_file_system_test:\n");

  try
  {
    FileSystem::RegisterPackFile ("myz", &my_pack_file_creator);

    printf ("Mount my file system\n");

    FileSystem::Mount (MOUNT_PREFIX, ZIP_FILE_NAME, "myz");

    printf ("Unmount my file system\n");

    FileSystem::Unmount (MOUNT_PREFIX);

    printf ("My file system unmounted\n");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
