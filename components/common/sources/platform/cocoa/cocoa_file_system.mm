#include <sys/xattr.h>

#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSDate.h>
#import <Foundation/NSError.h>
#import <Foundation/NSException.h>
#import <Foundation/NSFileHandle.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSString.h>

#include <xtl/function.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/default/shared.h>

using namespace common;

/*
    Файловая система, работающая через Cocoa
*/

namespace
{

class CocoaFileSystem: public StdioFileSystem
{
  public:
///Конструктор/деструктор
    CocoaFileSystem ()
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      file_manager = [[NSFileManager defaultManager] retain];

      [pool release];
    }

    ~CocoaFileSystem ()
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      [file_manager release];

      [pool release];
    }

    void FileResize (file_t file,filesize_t new_size)
    {
      static const char* METHOD_NAME = "common::CocoaFileSystem::FileResize";

      if (ftruncate (fileno ((FILE*)file),new_size) == -1)
      {
        switch (errno)
        {
          case EBADF:  throw xtl::make_argument_exception      (METHOD_NAME,"file"); break;
          case EINVAL: throw xtl::make_null_argument_exception (METHOD_NAME,"buffer"); break;
          case ENOSPC: throw xtl::format_operation_exception   (METHOD_NAME,"No enough space for resize file up to %u bytes",new_size); break;
          default:     throw xtl::format_operation_exception   (METHOD_NAME,"Unknown error"); break;
        }
      }
    }

    void Mkdir (const char* dir_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:dir_name length:xtl::xstrlen (dir_name)];

      stl::string error_string ("Unknown error");

      NSError* error = nil;

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      if (![file_manager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error])
#else
      if (![file_manager createDirectoryAtPath:path attributes:nil])
#endif
      {
        if (error)
          error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Mkdir", "Can't create directory '%s': error '%s'", dir_name, error_string.c_str ());
      }

      [pool release];
    }

///Получение информации о файле
    bool IsFileExist (const char* file_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)];

      bool return_value = [file_manager fileExistsAtPath:path];

      [pool release];

      return return_value;
    }

    bool GetFileInfo (const char* file_name, common::FileInfo& info)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)];

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      NSDictionary* file_attributes = [file_manager attributesOfItemAtPath:path error:nil];
#else
      NSDictionary* file_attributes = [file_manager fileAttributesAtPath:path traverseLink:NO];
#endif

      if (!file_attributes)
      {
        [pool release];

        return false;
      }

      filetime_t modify_time = [[file_attributes fileModificationDate] timeIntervalSince1970];

      info.size         = [file_attributes fileSize];
      info.time_modify  = modify_time;
      info.time_access  = modify_time;
      info.time_create  = [[file_attributes fileCreationDate] timeIntervalSince1970];
      info.is_dir       = [[file_attributes fileType] isEqualToString:NSFileTypeDirectory];

      [pool release];

      return true;
    }

///Файловые атрибуты
    void SetFileAttribute (const char* file_name, const char* attribute, const void* data, size_t size)
    {
      if (setxattr (file_name, attribute, data, size, 0, 0))
        throw xtl::format_operation_exception ("common::CocoaFileSystem::SetFileAttribute", "Can't set attribute '%s' for file '%s' with size %d, error '%s'", attribute, file_name, size, ::strerror (errno));
    }

    void GetFileAttribute (const char* file_name, const char* attribute, void* data, size_t size)
    {
      if (getxattr (file_name, attribute, data, size, 0, 0))
        throw xtl::format_operation_exception ("common::CocoaFileSystem::GetFileAttribute", "Can't get attribute '%s' for file '%s' with size %d, error '%s' %d", attribute, file_name, size, ::strerror (errno), errno);
    }

    bool HasFileAttribute (const char* file_name, const char* attribute)
    {
      size_t attributes_length = listxattr (file_name, 0, 0, 0);

      if (!attributes_length)
        return false;

      xtl::uninitialized_storage<char> names (attributes_length);

      attributes_length = listxattr (file_name, names.data (), names.size (), 0);

      const char *current_attribute = names.data (),
                 *end               = current_attribute + attributes_length;

      for (; current_attribute < end;)
      {
        if (!xtl::xstrcmp (current_attribute, attribute))
          return true;

        current_attribute += xtl::xstrlen (current_attribute) + 1;
      }

      return false;
    }

    void RemoveFileAttribute (const char* file_name, const char* attribute)
    {
      if (removexattr (file_name, attribute, 0))
        throw xtl::format_operation_exception ("common::CocoaFileSystem::RemoveFileAttribute", "Can't remove attribute '%s' for file '%s', error '%s'", attribute, file_name, ::strerror (errno));
    }

///Поиск файла
    void Search (const char* wc_mask, const FileSearchHandler& handler)
    {
      stl::string dir_name = dir (wc_mask), mask = wc_mask + dir_name.size (), file_name;            

      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:dir_name.c_str () length:dir_name.size ()];

      if (![file_manager fileExistsAtPath:path])
      {
        [pool release];
        return;
      }

      if (dir_name == "./")
        dir_name = "";

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      NSError* error = nil;

      NSArray* subpathes = [file_manager contentsOfDirectoryAtPath:path error:&error];

      if (error)
      {
        stl::string error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Search", "Can't enumerate directory '%s' contents: error '%s'", dir_name.c_str (), error_string.c_str ());
      }
#else
      NSArray* subpathes = [file_manager directoryContentsAtPath:path];
#endif

      common::FileInfo info;

      for (size_t i = 0, count = [subpathes count]; i < count; i++)
      {
        NSString* ns_file_name = [subpathes objectAtIndex:i];

        file_name = [ns_file_name UTF8String];

        if (file_name.data () [0] != '.' && file_name.find ("/.") == stl::string::npos && wcmatch (file_name.c_str (), mask.c_str ()))
        {
          file_name = dir_name + file_name;

          GetFileInfo (file_name.c_str (), info);

          handler (file_name.c_str (), info);
        }
      }

      [pool release];
    }

///Подсчёт ссылок
    void AddRef  () {}
    void Release () {}

  private:
    CocoaFileSystem (const CocoaFileSystem&); //no impl
    CocoaFileSystem& operator = (const CocoaFileSystem&); //no impl

  private:
    NSFileManager* file_manager;
};

}

/*
    Получение файловой системы
*/

ICustomFileSystem* CocoaPlatform::GetFileSystem ()
{
  return &*Singleton<CocoaFileSystem>::Instance ();
}
