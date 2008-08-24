#include <cstdio>
#include <cctype>

#include <stl/string>
#include <stl/vector>

#include <xtl/shared_ptr.h>

#include <common/file.h>
#include <common/strlib.h>

/*
    Константы
*/

const char*  WGLEXT_FILE_NAME     = "../sources/shared/gl/wglext.h"; //имя заголовочного файла WGL-расширений
const char*  GLEXT_FILE_NAME      = "../sources/shared/gl/glext.h";  //имя заголовочного файла расширений OpenGL
const char*  TEMPLATES_MASK       = "templates/*";                   //маска имён файлов-шаблонов
const char*  RESULT_DIR           = "results";                       //каталог с результирующими файлами
const size_t ENTRIES_RESERVE_SIZE = 8192;                            //резервируемое количество точек входа

/*
    Типы
*/

struct Entry
{
  stl::string name;   //имя точки входа
  stl::string prefix; //префикса имени точки входа
  stl::string type;   //тип точки входа

  Entry (const char* in_prefix, const stl::string& source, size_t pos, size_t len) : name (source, pos, len), prefix (in_prefix)
  {
    type  = "PFN";    
    type += prefix;
    type += name;
    type += "PROC";
    
    for (char* s=&type[0]; *s; s++)
      *s = toupper (*s);
  }
};

typedef xtl::shared_ptr<Entry> EntryPtr;
typedef stl::vector<EntryPtr>  EntryArray;

/*
    Функции
*/

//чтение таблицы точек входа
void load_entries (const char* file_name, const char* modifier, const char* prefix, EntryArray& entries)
{
    //загрузка исходного текста glext.h

  stl::string source;
  
  common::FileSystem::LoadTextFile (file_name, source);
  
    //формирование префикса поиска

  stl::string full_prefix = modifier;

  full_prefix += " ";
  full_prefix += prefix;

  size_t prefix_size = full_prefix.size ();    
  
    //поиск точек входа
    
  for (stl::string::size_type pos=0, end;; pos=end)
  {
    pos = source.find (full_prefix, pos);

    if (pos == stl::string::npos)
      break;

    pos  += prefix_size;
    end   = source.find   (" ", pos);

    entries.push_back (EntryPtr (new Entry (prefix, source, pos, end - pos)));
  }
}

//печать точек входа
void dump_entries (const EntryArray& entries, stl::string& result)
{
  for (EntryArray::const_iterator iter=entries.begin (), end=entries.end (); iter!=end; ++iter)
  {
    const Entry& entry = **iter;

    result += "\n  ";
    result += entry.type;
    result += " ";
    result += entry.name;
    result += ";";
  }
}

//печать макроопределений
void dump_defines (const EntryArray& entries, stl::string& result)
{
  for (EntryArray::const_iterator iter=entries.begin (), end=entries.end (); iter!=end; ++iter)
  {
    const Entry& entry = **iter;
    
    result += "\n#define ";
    result += entry.prefix;
    result += entry.name;
    result += " get_gl_entries ()->";
    result += entry.name;
  }
}

//печать кода инициализации
void dump_initialization (const EntryArray& entries, stl::string& result)
{
  for (EntryArray::const_iterator iter=entries.begin (), end=entries.end (); iter!=end; ++iter)
  {
    const Entry& entry = **iter;
    
    result += "\n  init_extension_entry (library, \"";
    result += entry.prefix;
    result += entry.name;
    result += "\", ";
    result += entry.name;
    result += ");";
  }
}

//генерация исходного текста
void generate_source (const char* template_file_name, const char* source_name, const EntryArray& gl_entries, const EntryArray& wgl_entries)
{
    //загрузка шаблона

  stl::string tmpl;

  common::FileSystem::LoadTextFile (template_file_name, tmpl);
  
    //формирование текста на выходе
    
  stl::string result;
  
  result.reserve (tmpl.size ());
    
  static const char*  BEGIN_REPLACEMENT_TAG      = "<<<";
  static const char*  END_REPLACEMENT_TAG        = ">>>";  
  static const size_t BEGIN_REPLACEMENT_TAG_SIZE = strlen (BEGIN_REPLACEMENT_TAG);
  static const size_t END_REPLACEMENT_TAG_SIZE   = strlen (END_REPLACEMENT_TAG);

  for (stl::string::size_type pos=0, end=0;; pos=end)
  {
    pos = tmpl.find (BEGIN_REPLACEMENT_TAG, pos);

    if (pos == stl::string::npos)
    {
      result.append (tmpl, end, stl::string::npos);
      break;
    }
      
    result.append (tmpl, end, pos - end);

    pos  += BEGIN_REPLACEMENT_TAG_SIZE;
    end   = tmpl.find (END_REPLACEMENT_TAG, pos);
    
    if (end == stl::string::npos)
    {
      end = pos;
      continue;
    }

    stl::string tag = tmpl.substr (pos, end - pos);

      //замены

    if      (tag == "GLENTRIES")  dump_entries        (gl_entries, result);
    else if (tag == "GLDEFINES")  dump_defines        (gl_entries, result);
    else if (tag == "GLINIT")     dump_initialization (gl_entries, result);
    else if (tag == "WGLENTRIES") dump_entries        (wgl_entries, result);
    else if (tag == "WGLDEFINES") dump_defines        (wgl_entries, result);
    else if (tag == "WGLINIT")    dump_initialization (wgl_entries, result);
    else
    {
      printf ("Bad tag '%s' in file '%s'\n", tag.c_str (), template_file_name);
    }

    end += END_REPLACEMENT_TAG_SIZE;
  }
  
    //сохранение сформированного файла
    
  common::OutputFile file (source_name);
  
  file.Write (result.c_str (), result.size ());
}

int main ()
{
  printf ("Running gl_entries_source_generator...\n");

  try
  {
      //загрузка точек входа OpenGL

    EntryArray gl_entries, wgl_entries;

    load_entries (GLEXT_FILE_NAME, "APIENTRY", "gl",  gl_entries);
    load_entries (WGLEXT_FILE_NAME, "WINAPI", "wgl", wgl_entries);    

      //создание нового каталога с результирующими файлами

    if (!common::FileSystem::IsFileExist (RESULT_DIR))
      common::FileSystem::Mkdir (RESULT_DIR);

      //генерация файлов

    for (common::FileListIterator iter = common::FileSystem::Search (TEMPLATES_MASK); iter; ++iter)
    {
      stl::string result_file_name = common::format ("%s/%s", RESULT_DIR, common::notdir (iter->name).c_str ());

      generate_source (iter->name, result_file_name.c_str (), gl_entries, wgl_entries);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
