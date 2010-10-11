#include <cstdio>
#include <exception>

#include <stl/hash_map>
#include <stl/map>

#include <xtl/iterator.h>

#include <script/bind.h>
#include <script/environment.h>

using namespace script;

typedef stl::multimap<stl::string, Environment::Type*>    TypeMap;
typedef stl::hash_map<const xtl::type_info*, stl::string> TypeNameMap;

// поиск имени типа
stl::string get_type_name (Environment& env, const xtl::type_info& type, TypeNameMap& type_names)
{
  TypeNameMap::iterator iter = type_names.find (&type);
  
  if (iter != type_names.end ())
    return iter->second;
    
  if (&type.std_type () == &typeid (const char*))
  {
    type_names [&type] = "System::String^";
    
    return "System::String^";
  }

  const char* result = env.FindLibraryId (type.std_type ());
  
  if (result)
  {
    stl::string cpp_result_name;
    
    cpp_result_name.reserve (strlen (result) * 2);
    
    const char* first = result;
    
    for (const char* s=result; *s; s++)
      switch (*s)
      {
        case '.':
          cpp_result_name.append (first, s);
          cpp_result_name.append ("::");
          first = s + 1;
          break;
        default:
          break;
      }
      
    cpp_result_name.append (first);
    
    type_names [&type] = cpp_result_name;
    
    return cpp_result_name;
  }
  
  type_names [&type] = type.name ();
    
  return type.name ();
}

// обработка библиотеки
void process_library (Environment::Type& type, size_t prefix_size, Environment& env, TypeNameMap& type_names)
{
  InvokerRegistry& lib  = type.Library ();
  const char*      name = type.Name () + prefix_size;
  
  if (!type.HasExtendedType ())
    return;
    
  const xtl::type_info& ext_type = type.ExtendedType ();
  
  if (ext_type.is_enum ())
  {
    printf ("public enum class %s\n{\n", name);
    
    for (InvokerRegistry::Iterator iter=lib.CreateIterator (); iter; ++iter)
    {
      Invoker invoker = *iter;
      
      if (invoker.OverloadsCount () > 1)
        continue;
        
      InvokerSignature s = invoker.OverloadSignature (0);
      
      if (!s.ResultType ().is_enum ())
        continue;
        
      const char* field_name = lib.InvokerId (iter);
      
      if (strncmp (field_name, "get_", 4))
        continue;
        
      printf ("  %s,\n", field_name + 4);
    }
    
    printf ("};\n\n");    
    
    return;
  }
    
  if (!ext_type.is_class ())
    return;

  printf ("public ref class ");  
  
  printf ("%s\n{\n  public:\n", name);
  
  for (InvokerRegistry::Iterator iter=lib.CreateIterator (); iter; ++iter)
  {
    Invoker invoker = *iter;
    
    for (size_t i=0; i<invoker.OverloadsCount (); i++)
    {
      InvokerSignature s = invoker.OverloadSignature (i);
      
      bool ignore_first_parameter = false;
      
      if (&s.ResultType () == &ext_type && stl::string (lib.InvokerId (iter)) == "Create") //конструктор
      {
        printf ("    %s (", name);
      }
      else
      {
        const char* direction = "";
        
        if (s.ResultType ().is_class ())
          direction = "^";
          
        printf ("    ");
          
        if (s.ParametersCount ())
        {
          if (&s.ParameterType (0).std_type () != &ext_type.std_type ()) printf ("static ");
          else                                                           ignore_first_parameter = true;
        }
        
        printf ("%s%s %s (", get_type_name (env, s.ResultType (), type_names).c_str (), direction, lib.InvokerId (iter));
      }
      
      bool is_first = true;
      
      for (size_t j=ignore_first_parameter ? 1 : 0; j<s.ParametersCount (); j++)
      {
        if (is_first) is_first = false;
        else          printf (", ");
          
        printf ("%s", get_type_name (env, s.ParameterType (j), type_names).c_str ());
        
        if (s.ParameterType (j).remove_reference ().remove_pointer ().is_class ())
          printf ("^");
      }
      
      printf (")\n    {\n    }\n\n");
    }
  }
  
  printf ("};\n\n");
}

// обработка пространств имён
void process_namespace (Environment& env, size_t prefix_size, TypeMap::iterator& iter, TypeMap::iterator end, TypeNameMap& type_names)
{
  if (iter == end)
    return;

  stl::string library_name = iter->second->Name ();
  
  if (library_name.size () < prefix_size)
    return;
    
  library_name = iter->second->Name () + prefix_size;
  
  size_t last_dot_pos = library_name.rfind ('.'),
         first_dot_pos = library_name.find ('.');  
  
  if (last_dot_pos == stl::string::npos || first_dot_pos == stl::string::npos)
  {
     // обработка типа внутри пространства имён
     
    process_library (*iter->second, prefix_size + first_dot_pos + 1, env, type_names);
    
    ++iter;
    
    return;
  }
  
    // определение имени пространства имён
    
  stl::string namespace_name = library_name.substr (0, first_dot_pos);    
  
  printf ("namespace %s\n{\n\n", namespace_name.c_str ());  
    
    // обработка вложенных пространств имён
    
  for (;;)
  {
    process_namespace (env, prefix_size + first_dot_pos + 1, iter, end, type_names);
    
    if (iter == end)
      break;

    library_name = iter->second->Name ();
    
    if (library_name.size () < prefix_size)
      break;
      
    library_name = iter->second->Name () + prefix_size;
    
    size_t new_first_dot_pos = library_name.find ('.');  
    
    if (first_dot_pos != new_first_dot_pos || namespace_name != library_name.substr (0, new_first_dot_pos))
      break;
  }
    
    // закрытие пространства имён
  
  printf ("}\n\n");
}

// обработка библиотек
void process_libraries (Environment& env)
{
  TypeMap types;

  for (Environment::TypeIterator iter=env.CreateTypeIterator (); iter; ++iter)
  {
    types.insert_pair (iter->Name (), &*iter);
  }
  
  TypeNameMap type_names;
  
  for (TypeMap::iterator iter=types.begin (); iter!=types.end ();)
    process_namespace (env, 0, iter, types.end (), type_names);
}

// точка входа
int main ()
{
  try
  {
      //создание окружения и привязка библиотек
    
    Environment env;
    
    env.BindLibraries ("*");
    
      //перебор библиотек
      
    process_libraries (env);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
