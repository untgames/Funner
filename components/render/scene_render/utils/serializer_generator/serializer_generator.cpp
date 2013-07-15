#include <stdio.h>

#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/strlib.h>

/*
    Константы
*/

const char* INPUT_FILE_NAME                = "input/signatures.h";       //файл с сигнатурами
const char* OUTPUT_ENUM_FILE_NAME          = "output/enums.h";           //выходной файл с перечислениями
const char* OUTPUT_SIGNATURES_FILE_NAME    = "output/signatures.h";      //выходной файл с сигнатурами
const char* OUTPUT_SERIALIZATION_FILE_NAME = "output/serialization.cpp"; //выходной файл сериализации
const char* SERIALIZE_PARAM_PREFIX         = "Serialize(";               //префикс сериализации параметра
const char* SERIALIZE_HEADER_PREFIX        = "SerializeHeader(";         //префикс сериализации заголовка
const char* SERIALIZE_TAIL                 = "SerializeTail();";         //хвостовая сериализация
const char* COMMAND_ID_ENUM_NAME           = "CommandId";                //идентификатор команды

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

void parse_signatures (MethodArray& methods)
{
  stl::string signatures = common::FileSystem::LoadTextFile (INPUT_FILE_NAME);

  common::StringArray lines = common::split (signatures.c_str (), "\n", "");

  for (size_t i=0, count=lines.Size (); i<count; i++)
  {
    const char* line = lines [i];

    methods.push_back ();

    try
    {
      Method& method = methods.back ();

      parse_signature (line, method);
    }
    catch (...)
    {
      methods.pop_back ();
      throw;
    }
  }
}

void dump_signature (FILE* file, const Method& method)
{
  fprintf (file, "%s %s(", method.result_type.c_str (), method.name.c_str ());

  for (ParamArray::const_iterator iter=method.params.begin (), end=method.params.end (); iter!=end; ++iter)
  {
    const Param& param = *iter;

    if (iter != method.params.begin ())
      fprintf (file, ", ");

    fprintf (file, "%s %s", param.type.c_str (), param.name.c_str ());
  }

  fprintf (file, ")");
}

void dump_signatures (const char* file_name, const MethodArray& methods)
{
  FILE* file = fopen (file_name, "wt");

  if (!file)
    throw xtl::format_operation_exception ("", "Can't open file '%s'", file_name);

  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;

    dump_signature (file, method);

    fprintf (file, ";\n");
  }

  fclose (file);
}

void dump_param_serialization (FILE* file, const Param& param)
{
  fprintf (file, "  %s%s);", SERIALIZE_PARAM_PREFIX, param.name.c_str ());
}

void dump_serialization (const char* file_name, const MethodArray& methods)
{
  FILE* file = fopen (file_name, "wt");

  if (!file)
    throw xtl::format_operation_exception ("", "Can't open file '%s'", file_name);

  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;

    dump_signature (file, method);

    fprintf (file, "\n{\n");

    fprintf (file, "  %s%s_%s);\n", SERIALIZE_HEADER_PREFIX, COMMAND_ID_ENUM_NAME, method.name.c_str ());

    for (ParamArray::const_iterator iter=method.params.begin (), end=method.params.end (); iter!=end; ++iter)
    {
      const Param& param = *iter;

      dump_param_serialization (file, param);
    }    

    fprintf (file, "\n  %s", SERIALIZE_TAIL);

    fprintf (file, "\n}\n\n");
  }  

  fclose (file);
}

void dump_enums (const char* file_name, const MethodArray& methods)
{
  FILE* file = fopen (file_name, "wt");

  if (!file)
    throw xtl::format_operation_exception ("", "Can't open file '%s'", file_name);

  fprintf (file, "enum %s\n", COMMAND_ID_ENUM_NAME);
  fprintf (file, "{\n");

  for (MethodArray::const_iterator iter=methods.begin (), end=methods.end (); iter!=end; ++iter)
  {
    const Method& method = *iter;    

    fprintf (file, "  %s_%s,\n", COMMAND_ID_ENUM_NAME, method.name.c_str ());
  }

  fprintf (file, "};\n\n");

  fclose (file);
}

int main ()
{
  try
  {
    MethodArray methods;

    parse_signatures (methods);

    dump_signatures (OUTPUT_SIGNATURES_FILE_NAME, methods);
    dump_serialization (OUTPUT_SERIALIZATION_FILE_NAME, methods);
    dump_enums (OUTPUT_ENUM_FILE_NAME, methods);

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("error: %s\n", e.what ());
    return -1;
  }  
}
