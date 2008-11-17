#include "shared.h"

using namespace common;

namespace
{

const size_t DEFAULT_FILTER_POOL_SIZE = 64;

}

/*
   Описание реализации класса LogFile
*/

struct LogFile::Impl
{
  public:
    Impl ()
      : current_filter_pool_capacity (DEFAULT_FILTER_POOL_SIZE)
    {
      filter_pool.reserve (DEFAULT_FILTER_POOL_SIZE);

      for (size_t i = 0; i < DEFAULT_FILTER_POOL_SIZE; i++)
        filter_pool.push_back (i);
    }

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
      if (!file.CanWrite ())
        return;

      stl::string print_message;
      stl::string temp_string;
      time_t      current_time;

      for (LogFiltersSet::iterator iter = log_filters.begin (), end = log_filters.end (); iter != end; ++iter)
      {
        if (wcmatch (message, iter->replace_mask.c_str ()))
        {
          for (size_t i = 0; i < iter->replacement_components.size (); i++)
          {
            print_message.append (iter->replacement_components[i].prefix.c_str ());

            if (!xtl::xstrcmp ("message", iter->replacement_components[i].replacement_tag.c_str ()))
              print_message.append (message);
            else if (!xtl::xstrcmp ("log", iter->replacement_components[i].replacement_tag.c_str ()))
              print_message.append (log_name);
            else if (!xtl::xstrcmp ("time", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time_t current_time;

              time (&current_time);

              print_message.append (ctime (&current_time));
              print_message.pop_back ();
            }
            else if (!xtl::xstrcmp ("day", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);

              xtl::to_string (temp_string, converted_time->tm_mday);
              print_message.append (temp_string);
            }
            else if (!xtl::xstrcmp ("month", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);
              
              xtl::to_string (temp_string, converted_time->tm_mon + 1);
              print_message.append (temp_string);
            }
            else if (!xtl::xstrcmp ("year", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);
                            
              xtl::to_string (temp_string, converted_time->tm_year - 100);
              print_message.append (temp_string);
            }
            else if (!xtl::xstrcmp ("hour", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);                           
              
              xtl::to_string (temp_string, converted_time->tm_hour);
              print_message.append (temp_string);
            }
            else if (!xtl::xstrcmp ("minutes", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);                           
                            
              xtl::to_string (temp_string, converted_time->tm_min);
              print_message.append (temp_string);
            }  
            else if (!xtl::xstrcmp ("seconds", iter->replacement_components[i].replacement_tag.c_str ()))
            {
              time (&current_time);

              tm *converted_time = localtime (&current_time);                           
                            
              xtl::to_string (temp_string, converted_time->tm_sec);
              print_message.append (temp_string);
            }  
            else
            {
              if (iter->replacement_components[i].replacement_tag.empty ())
                continue;

              print_message.append ("{");
              print_message.append (iter->replacement_components[i].replacement_tag);
              print_message.append ("}");
            }
          }

          print_message.append ("\n");

          file.Write (print_message.c_str (), print_message.length ());

          return;
        }
      }

      print_message.append (log_name);
      print_message.append (": ");
      print_message.append (message);
      print_message.append ("\n");

      file.Write (print_message.c_str (), print_message.length ());
    }

/*
   Установка/удаление фильтров замены сообщений
*/
    size_t AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
    {
      size_t filter_index;

      if (filter_pool.empty ())
      {
        for (size_t i = current_filter_pool_capacity; i < current_filter_pool_capacity + DEFAULT_FILTER_POOL_SIZE; i++)
          filter_pool.push_back (i);

        current_filter_pool_capacity += DEFAULT_FILTER_POOL_SIZE;
      }

      filter_index = filter_pool.back ();
      filter_pool.pop_back ();

      LogFiltersSet::iterator insert_result = log_filters.insert (LogFilter (filter_index, sort_order, replace_mask)).first;

      current_replacement_array = (ReplacementComponentsArray*)&(insert_result->replacement_components);

      parse_format_string (replacement, xtl::bind (&LogFile::Impl::ProcessReplacementTag, this, _1, _2));

      return filter_index;
    }

    void SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
    {
      LogFiltersSet::iterator filter = FindFilter (filter_index);

      if (filter == log_filters.end ())
        throw xtl::make_argument_exception ("LogFilter::SetFilter", "filter_index", filter_index);

      log_filters.erase (filter);

      LogFiltersSet::iterator insert_result = log_filters.insert (LogFilter (filter_index, sort_order, replace_mask)).first;

      current_replacement_array = (ReplacementComponentsArray*)&(insert_result->replacement_components);

      parse_format_string (replacement, xtl::bind (&LogFile::Impl::ProcessReplacementTag, this, _1, _2));
    }

    void RemoveFilter (size_t filter_index)
    {
      LogFiltersSet::iterator filter = FindFilter (filter_index);

      if (filter == log_filters.end ())
        return;

      filter_pool.push_back (filter_index);

      log_filters.erase (filter);
    }

    void RemoveAllFilters ()
    {
      filter_pool.clear ();
      current_filter_pool_capacity = DEFAULT_FILTER_POOL_SIZE;

      for (size_t i = 0; i < DEFAULT_FILTER_POOL_SIZE; i++)
        filter_pool.push_back (i);

      log_filters.clear ();
    }

/*
   Сброс файлового буффера
*/
    void Flush ()
    {
      file.Flush ();
    }

  private:
    struct ReplacementComponent
    {
      stl::string prefix;
      stl::string replacement_tag;

      ReplacementComponent (const char* in_prefix, const char* in_replacement_tag) : prefix (in_prefix), replacement_tag (in_replacement_tag) {}
    };

    typedef stl::vector<ReplacementComponent> ReplacementComponentsArray;

    struct LogFilter
    {
      size_t                     index;
      size_t                     sort_order;
      stl::string                replace_mask;
      ReplacementComponentsArray replacement_components;

      bool operator < (const LogFilter& right) const
      {
        return sort_order < right.sort_order;
      }

      LogFilter (size_t in_index, size_t in_sort_order, const char* in_replace_mask) 
        : index (in_index), sort_order (in_sort_order), replace_mask (in_replace_mask) 
        {}
    };

    typedef stl::set<LogFilter, stl::less<LogFilter> > LogFiltersSet;

  private:
    void ProcessReplacementTag (const char* prefix, const char* replacement_tag)
    {
      current_replacement_array->push_back (ReplacementComponent (prefix, replacement_tag));
    }

    LogFiltersSet::iterator FindFilter (size_t index)
    {
      find_index = index;

      return stl::find_if (log_filters.begin (), log_filters.end (), xtl::bind(&LogFile::Impl::FindPredicate, this, _1));
    }

    bool FindPredicate (const LogFilter& value)
    {
      return value.index == find_index;
    }

  private:
    common::File               file;                          //текущий файл вывода
    stl::vector<size_t>        filter_pool;                   //пул свободных индексов фильтров
    size_t                     current_filter_pool_capacity;  //текущее количество индексов фильтров
    LogFiltersSet              log_filters;                   //набор фильтров
    ReplacementComponentsArray *current_replacement_array;    //текущий обрабатываемый массив замен
    size_t                     find_index;                    //текущий искомый индекс
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
  static const char* METHOD_NAME = "common::LogFile::Print";

  if (!log_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "log_name");

  if (!message)
    throw xtl::make_null_argument_exception (METHOD_NAME, "message");

  impl->Print (log_name, message);
}

/*
   Установка/удаление фильтров замены сообщений
*/

size_t LogFile::AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
{
  static const char* METHOD_NAME = "common::LogFile::AddFilter";

  if (!replace_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "replace_mask");

  if (!replacement)
    throw xtl::make_null_argument_exception (METHOD_NAME, "replacement");

  return impl->AddFilter (replace_mask, replacement, sort_order);
}

void LogFile::SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
{
  static const char* METHOD_NAME = "common::LogFile::SetFilter";

  if (!replace_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "replace_mask");

  if (!replacement)
    throw xtl::make_null_argument_exception (METHOD_NAME, "replacement");

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
