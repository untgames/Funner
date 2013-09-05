#include <stdio.h>

#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/strlib.h>

/*
    Константы
*/

const char*  INPUT_FILE_NAME                   = "input/signatures.h";        //файл с сигнатурами
const char*  TEMPLATES_MASK                    = "input/templates/*";         //маска имён файлов-шаблонов
const char*  RESULT_DIR                        = "results";                   //каталог с результирующими файлами
const char*  SERIALIZE_PARAM_PREFIX            = "write";                     //префикс сериализации параметра
const char*  SERIALIZE_HEADER_PREFIX           = "BeginCommand(";             //префикс сериализации заголовка
const char*  SERIALIZE_TAIL                    = "EndCommand();";             //хвостовая сериализация
const char*  COMMAND_ID_ENUM_NAME              = "CommandId";                 //идентификатор команды
const char*  DESERIALIZER_DISPATCH_METHOD_NAME = "Deserializer::Deserialize"; //имя метода десериализации
const char*  DESERIALIZE_METHOD_NAME           = "read";                      //имя метода десериализации

/*
    Структуры
*/

//параметр метода
struct Param
{
  stl::string name; //имя параметра
  stl::string type; //тип параметра

  Param (const char* in_name, const char* in_type) : name (in_name), type (in_type) {}
};

typedef stl::vector<Param> ParamArray;

//метод
struct Method
{
  stl::string section;     //имя секции
  stl::string name;        //имя метода
  stl::string result_type; //имя возвращаемого типа  
  ParamArray  params;      //список параметров
};

typedef stl::vector<Method> MethodArray;

/*
    Функции
*/

void skip_space (const char*& iter)
{
  for (;;iter++)
  {
    switch (*iter)
    {
      case ' ':
      case '\t':
      case '\r':
        break;
      default:
        return;
    }
  }
}

stl::string parse_result_type (const char*& iter)
{
  skip_space (iter);

  const char* first = iter;

  for (;;iter++)
  {
    switch (*iter)
    {
      case ' ':
      case '\t':
      case '\r':
      case '\0':
        return stl::string (first, iter);
      default:
        break;
    }
  }
}

stl::string parse_method_name (const char*& iter)
{
  skip_space (iter);

  const char* first = iter;

  for (;;iter++)
  {
    switch (*iter)
    {
      case ' ':
      case '(':
      case '\t':
      case '\r':
      case '\0':
        return stl::string (first, iter);
      default:
        break;
    }
  }  
}

Param parse_param (const char* line)
{
  const char* iter = line;

  skip_space (iter);

  const char* type_start = iter;
  const char* name_end   = line + strlen (line) - 1;

  bool loop = true;

  for (loop=true; loop; name_end--)
  {
    switch (*name_end)
    {
      case ' ':
      case '\t': 
      case '\r':
        break;
      default:
        loop = false;
        break;
    }
  }

  name_end++;
  
  const char* name_start = name_end;

  for (loop=true; loop; name_start--)
  {
    switch (*name_start)
    {
      case ' ':
      case '\t': 
        loop = false;
        break;
      default:
        break;
    }
  }
  
  const char* type_end = name_start;

  name_start += 2;

  if (name_start >= name_end)
    throw xtl::format_operation_exception ("", "Can't parse param '%s'", line);

  for (loop=true; loop; type_end--)
  {
    switch (*name_end)
    {
      case ' ':
      case '\t': 
      case '\r':
        break;
      default:
        loop = false;
        break;
    }
  }

  type_end++;

  return Param (stl::string (name_start, name_end+1).c_str (), stl::string (type_start, type_end+1).c_str ());
}

void parse_params (const char* line, ParamArray& params)
{
  common::StringArray tokens = common::split (line, ",", " \t", "");

  for (size_t i=0; i<tokens.Size (); i++)
  {
    Param param = parse_param (tokens [i]);

    params.push_back (param);
  }
}

void parse_signature (const char* line, Method& method)
{
  const char* iter = line;

  method.result_type = parse_result_type (iter);

  if (method.result_type.empty ())
    throw xtl::format_operation_exception ("", "Bad line '%s'", line);

  method.name = parse_method_name (iter);

  if (method.name.empty ())
    throw xtl::format_operation_exception ("", "Bad line '%s'", line);

  const char* start_params = strchr (iter, '(');
  const char* end_params   = strchr (iter, ')');

  if (!start_params || !end_params || end_params <= start_params)
    throw xtl::format_operation_exception ("", "Bad line '%s'", line);

  parse_params (stl::string (start_params+1, end_params).c_str (), method.params);
}

void parse_section (const char* iter, stl::string& section)
{
  bool loop = true;

  for (;loop;iter++)
  {
    switch (*iter)
    {
      case ' ':
      case '\t':
        loop = false;
        --iter;
        break;
      default:
        break;
    }
  }

  skip_space (iter);

  section = parse_method_name (iter);
}

void parse_signatures (const char* file_name, MethodArray& methods)
{
  stl::string signatures = common::FileSystem::LoadTextFile (file_name), section_name = ""; 

  common::StringArray lines = common::split (signatures.c_str (), "\n", "");

  for (size_t i=0, count=lines.Size (); i<count; i++)
  {
    const char* line = lines [i];

    skip_space (line);

    if (!*line || *line == '\n' || *line == '\r')
      continue;

    static const char* SECTION_TAG = "#section";

    if (!strncmp (SECTION_TAG, line, strlen (SECTION_TAG)))
    {
      parse_section (line, section_name);
    }
    else
    {
      methods.push_back ();

      try
      {
        Method& method = methods.back ();

        parse_signature (line, method);

        method.section = section_name;
      }
      catch (...)
      {
        methods.pop_back ();
        throw;
      }
    }
  }
}

void dump_signature (const Method& method, stl::string& result, const char* class_name = "")
{
  if (*class_name) result += common::format ("%s %s::%s(", method.result_type.c_str (), class_name, method.name.c_str ());
  else             result += common::format ("%s %s(", method.result_type.c_str (), method.name.c_str ());

  for (ParamArray::const_iterator iter=method.params.begin (), end=method.params.end (); iter!=end; ++iter)
  {
    const Param& param = *iter;

    if (iter != method.params.begin ())
      result += ", ";

    result += common::format ("%s %s", param.type.c_str (), param.name.c_str ());
  }

  result += ")";
}

void dump_signatures (const MethodArray& methods, stl::string& result, const char* section)
{
  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;

    if (method.section != section && *section)
      continue;

    if (iter != methods.begin ())
      result += "\n";

    result += "    ";

    dump_signature (method, result);

    result += ";";
  }
}

void dump_param_serialization (const Param& param, stl::string& result)
{
  result += common::format ("  %s(*this, %s);", SERIALIZE_PARAM_PREFIX, param.name.c_str ());
}

void dump_serialization (const MethodArray& methods, stl::string& result, const char* section)
{
  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;

    if (method.section != section && *section)
      continue;

    if (iter != methods.begin ())
      result += "\n";

    result += "inline ";

    dump_signature (method, result, common::format ("%sSerializer", section).c_str ());

    result += "\n{\n";
    result += common::format ("  %s%s_%s);\n", SERIALIZE_HEADER_PREFIX, COMMAND_ID_ENUM_NAME, method.name.c_str ());

    for (ParamArray::const_iterator iter=method.params.begin (), end=method.params.end (); iter!=end; ++iter)
    {
      const Param& param = *iter;

      dump_param_serialization (param, result);
    }    

    result += common::format ("\n  %s\n}\n", SERIALIZE_TAIL);
  }
}

void dump_param_deserialization (const Param& param, stl::string& result)
{
  result += DESERIALIZE_METHOD_NAME;
  result += "(*this, xtl::type<";
  result += param.type;
  result += "> ())";
}

void dump_deserialization (const MethodArray& methods, stl::string& result, const char* section)
{
  result += common::format ("template <class Dispatcher> inline bool %s%s(%s id, Dispatcher& dispatcher)\n{\n", section, DESERIALIZER_DISPATCH_METHOD_NAME, COMMAND_ID_ENUM_NAME);
  result += "  switch (id)\n  {\n";

  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;

    if (method.section != section && *section)
      continue;

    result += common::format ("    case %s_%s:\n", COMMAND_ID_ENUM_NAME, method.name.c_str ());
    result += common::format ("      dispatcher.%s(", method.name.c_str ());

    for (ParamArray::const_iterator iter=method.params.begin (), end=method.params.end (); iter!=end; ++iter)
    {
      if (iter != method.params.begin ())
        result += ", ";

      const Param& param = *iter;

      dump_param_deserialization (param, result);      
    }    

    result += ");\n";
    result += "      return true;\n";
  }

  result += "    default:\n      return DeserializeUnknownCommand (id);\n";
  result += "  }\n}\n";
}

void dump_enums (const MethodArray& methods, stl::string& result)
{
  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    if (iter != methods.begin ())
      result += "\n";

    const Method& method = *iter;    

    result += common::format ("  %s_%s,", COMMAND_ID_ENUM_NAME, method.name.c_str ());
  }
}

void dump_enum_names (const MethodArray& methods, stl::string& result)
{
  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    if (iter != methods.begin ())
      result += "\n";

    const Method& method = *iter;    

    result += common::format ("    case %s_%s: return \"%s\";", COMMAND_ID_ENUM_NAME, method.name.c_str (), method.name.c_str ());
  }
}

stl::string get_section_name_from_tag (const char* tag)
{
  const char* colon = strchr (tag, ':');

  if (!colon)
    return "";

  colon++;

  return colon;
}

//генерация исходного текста
void generate_source (const char* template_file_name, const char* source_name, const MethodArray& methods)
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

    stl::string tag = tmpl.substr (pos, end - pos), section = get_section_name_from_tag (tag.c_str ());

    if (const char* pos = strchr (tag.c_str (), ':'))
      tag.erase (pos - tag.c_str ());

      //замены

    if      (tag == "ENUMS")            dump_enums           (methods, result);
    else if (tag == "DECLARATIONS")     dump_signatures      (methods, result, section.c_str ());
    else if (tag == "SERIALIZATION")    dump_serialization   (methods, result, section.c_str ());
    else if (tag == "DESERIALIZATION")  dump_deserialization (methods, result, section.c_str ());
    else if (tag == "ENUMNAMES")        dump_enum_names      (methods, result);
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
  try
  {
    MethodArray methods;

    parse_signatures (INPUT_FILE_NAME, methods);

      //создание нового каталога с результирующими файлами

    if (!common::FileSystem::IsFileExist (RESULT_DIR))
      common::FileSystem::Mkdir (RESULT_DIR);

      //генерация файлов

    for (common::FileListIterator iter = common::FileSystem::Search (TEMPLATES_MASK); iter; ++iter)
    {
      stl::string result_file_name = common::format ("%s/%s", RESULT_DIR, common::notdir (iter->name).c_str ());

      generate_source (iter->name, result_file_name.c_str (), methods);
    }

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("error: %s\n", e.what ());
    return -1;
  }  
}
