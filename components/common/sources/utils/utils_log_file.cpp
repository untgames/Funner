#include <common/exception.h>
#include <common/log.h>

using namespace common;

/*
   Описание реализации класса LogFile
*/

struct LogFile::Impl
{
  public:
/*
   Установка/получение текущего файла
*/
    void SetFile (const common::File& in_file)
    {
      file = in_file;
    }

    const common::File& File () const
    {
      return file;
    }

/*
   Печать сообщения в файл
*/
    void Print (const char* log_name, const char* message)
    {
      //!!!!!!!!!
    }

/*
   Установка/удаление фильтров замены сообщений
*/
    size_t AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
    {
      return 0; //!!!!!!!!!
    }

    void SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
    {
      //!!!!!!!!!
    }

    void RemoveFilter (size_t filter_index)
    {
      //!!!!!!!!!
    }

    void RemoveAllFilters ()
    {
      //!!!!!!!!!
    }

/*
   Сброс файлового буффера
*/
    void Flush ()
    {
      file.Flush ();
    }

  private:
    common::File file;
};

/*
   Запись протокола в файл
*/

/*
   Конструктор/деструктор
*/

LogFile::LogFile ()
  : impl (new Impl)
{
}

LogFile::LogFile (const common::File& file)
  : impl (new Impl)
{
  SetFile (file);
}

LogFile::~LogFile ()
{
  delete impl;
}

/*
   Установка/получение текущего файла
*/

void LogFile::SetFile (const common::File& file)
{
  impl->SetFile (file);
}

const common::File& LogFile::File () const
{
  return impl->File ();
}

/*
   Печать сообщения в файл
*/

void LogFile::Print (const char* log_name, const char* message)
{
  impl->Print (log_name, message);
}

/*
   Установка/удаление фильтров замены сообщений
*/

size_t LogFile::AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
{
  return impl->AddFilter (replace_mask, replacement, sort_order);
}

void LogFile::SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
{
  impl->SetFilter (filter_index, replace_mask, replacement, sort_order);
}

void LogFile::RemoveFilter (size_t filter_index)
{
  impl->RemoveFilter (filter_index);
}

void LogFile::RemoveAllFilters ()
{
  impl->RemoveAllFilters ();
}

/*
   Сброс файлового буффера
*/

void LogFile::Flush ()
{
  impl->Flush ();
}
