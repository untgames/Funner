#include <stdio.h>
#include <stdarg.h>

const size_t MAX_ARGUMENTS_COUNT = 9; //максимальное количество аргументов

void print_block_comment (const char* format,...)
{
  va_list list;

  va_start (list,format);
  printf   ("/*\n    ");
  vprintf  (format,list);
  printf   ("\n*/\n\n");
}

void print_arguments_count_comment (size_t arguments_count)
{
  switch (arguments_count)
  {
    case 0:  printf ("без аргументов"); break;
    case 1:  printf ("с 1-м аргументом"); break;
    default:
      if (arguments_count < 5)        printf ("с %d-мя аргументами");
      else if (arguments_count <= 20) printf ("с %d-ю аргументами");
      else                            printf ("с %d аргументами");
      
      break;
  }
}

void generate_signature_description (size_t arguments_count,bool eclipsis)
{
  printf ("//специализация для сигнатуры ");
  print_arguments_count_comment (arguments_count);
  
  if (eclipsis)
    printf (" и многоточием");
  
  printf ("\ntemplate <");
  
  for (size_t i=0;i!=arguments_count;i++) printf ("class Arg%u, ",i+1);
  
  printf ("class Ret>\n");
  printf ("struct functional_traits<Ret (");
  
  if (arguments_count)
  {
    for (size_t i=0;i!=arguments_count-1;i++) printf ("Arg%u, ",i+1);

    printf ("Arg%u",arguments_count);
    
    if (eclipsis)
      printf (", ");
  }
  
  if (eclipsis)
    printf ("...");

  printf (")>: public ");
  
  if (eclipsis)
    printf ("eclipsis_functional_traits<");
  
  printf ("signature_arguments_traits<%u, Ret", arguments_count);
  
  for (size_t i=0;i!=arguments_count;i++) printf (", Arg%u",i+1);

  printf (">");
  
  if (eclipsis)
    printf (" >");
    
  printf (" { };\n\n");
}

void generate_ptrfun_description (size_t arguments_count,bool eclipsis,const char* modifier="")
{
  printf ("//специализация для указателя на функцию ");
  print_arguments_count_comment (arguments_count);
  
  if (eclipsis)
    printf (" и многоточием");

  printf ("\ntemplate <");
  
  for (size_t i=0;i!=arguments_count;i++) printf ("class Arg%u, ",i+1);
  
  printf ("class Ret>\n");
  printf ("struct functional_traits<Ret (%s*)(",modifier);
  
  if (arguments_count)
  {
    for (size_t i=0;i!=arguments_count-1;i++) printf ("Arg%u, ",i+1);

    printf ("Arg%u",arguments_count);
    
    if (eclipsis)
      printf (", ");
  }
  
  if (eclipsis)
    printf ("...");

  printf (")>: public ");
  
  if (eclipsis)
    printf ("eclipsis_functional_traits<");

  printf ("ptrfun_arguments_traits<%u, Ret", arguments_count);
  
  for (size_t i=0;i!=arguments_count;i++) printf (", Arg%u",i+1);

  printf ("> ");
  
  if (eclipsis)
    printf (">");
  
  printf (" { };\n\n");
}

void generate_memfun_description (size_t arguments_count,bool eclipsis,const char* qualifier,const char* modifier="")
{
  printf ("//специализация для указателя на функцию-член класса %s%sT ",qualifier,*qualifier?" ":"");
  print_arguments_count_comment (arguments_count);
  
  if (eclipsis)
    printf (" и многоточием");

  printf ("\ntemplate <class T,");
  
  for (size_t i=0;i!=arguments_count;i++) printf ("class Arg%u, ",i+1);
  
  printf ("class Ret>\n");
  printf ("struct functional_traits<Ret (%s%sT::*)(",modifier,*modifier?" ":"");
  
  if (arguments_count)
  {
    for (size_t i=0;i!=arguments_count-1;i++) printf ("Arg%u, ",i+1);

    printf ("Arg%u",arguments_count);
    
    if (eclipsis)
      printf (", ");
  }

  if (eclipsis)
    printf ("...");

  printf (")%s%s>: public ",*qualifier?" ":"",qualifier);
  
  if (eclipsis)
    printf ("eclipsis_functional_traits<");
  
  printf ("memfun_arguments_traits<%u, %s%sT, Ret",arguments_count,qualifier,*qualifier?" ":"");
  
  for (size_t i=0;i!=arguments_count;i++) printf (", Arg%u",i+1);

  printf ("> ");
  
  if (eclipsis)
    printf ("> ");
  
  printf ("{ };\n\n");
}

void generate_ptrfun_descriptions (const char* modifier)
{
  if (*modifier) print_block_comment ("Специализации для указателей на функцию с модификатором %s",modifier);
  else           print_block_comment ("Специализации для указателей на функцию");

  for (size_t i=0;i<=MAX_ARGUMENTS_COUNT;i++)
  {
    generate_ptrfun_description (i,false,modifier);
    
    if (!*modifier)
      generate_ptrfun_description (i,true,modifier);
  } 
}

void generate_memfun_descriptions (const char* modifier)
{
  const char* memfun_qualifiers [] = {"","const","volatile","const volatile"};
  
  if (*modifier) print_block_comment ("Специализации для указателей на функцию-член класса с модификатором %s",modifier);
  else           print_block_comment ("Специализации для указателей на функцию-член класса");  
  
  for (size_t i=0;i<=MAX_ARGUMENTS_COUNT;i++)
  {
    for (size_t j=0;j<sizeof (memfun_qualifiers)/sizeof (*memfun_qualifiers);j++)
    {
      generate_memfun_description (i,false,memfun_qualifiers [j],modifier);
      
      if (!*modifier)      
        generate_memfun_description (i,true,memfun_qualifiers [j],modifier);
    }
  }  
}

void generate_base_types ()
{
  print_block_comment ("Таблица конфигурации функционального объекта");
  printf ("template <class Fn> struct functional_traits\n{\n");
  printf ("  enum {\n    is_function  = false,\n    is_memfun    = false,\n    is_ptrfun    = false,\n    has_eclipsis = false\n  };\n};\n\n");
  
  print_block_comment ("Специализации для различных видов квалификаторов");
  
  printf ("template <class Fn> struct functional_traits<const Fn>:          public functional_traits<Fn> {};\n");
  printf ("template <class Fn> struct functional_traits<volatile Fn>:       public functional_traits<Fn> {};\n");
  printf ("template <class Fn> struct functional_traits<const volatile Fn>: public functional_traits<Fn> {};\n\n");

  print_block_comment ("Таблицы конфигурации аргументов функции и типа возвращаемого значения");

  printf ("//тип используемый в качестве индикатора отсутствия аргумента, аналогичен void\n");
  printf ("struct void_argument {};\n\n");
  
  printf ("//обёртка указывающая на наличие многоточия в списке аргументов функции\n");
  printf ("template <class Traits>\nstruct eclipsis_functional_traits: public Traits\n{\n");      
  printf ("  enum { has_eclipsis = true };\n};\n\n");
  
  printf ("//таблица конфигурации для сигнатур на функции\n");
  printf ("template <size_t ArgumentsCount, class Ret");
  
  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf (", class Arg%u=void_argument",i+1);
    
  printf (">\nclass signature_arguments_traits\n{\n");
  printf ("  private:\n    typedef typename ");

  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf ("mpl::type_node<Arg%u, ",i+1);
    
  printf ("mpl::null_type");
    
  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf ("> ");        
    
  printf ("argument_list;\n\n");
  printf ("  public:\n");
    
  printf ("    enum {\n      is_function     = true,\n      is_ptrfun       = false,\n"
         "      is_memfun       = false,\n      has_eclipsis    = false,\n      arguments_count = ArgumentsCount\n    };\n\n");
  printf ("    typedef Ret result_type;\n\n");    
  
  printf ("    template <size_t number> struct argument {\n");
  printf ("      typedef typename mpl::at<argument_list, number-1>::type type;\n    };\n");  
  printf ("};\n\n");  
  
  printf ("//таблица конфигурации для указателей на функции\n");
  printf ("template <size_t ArgumentsCount, class Ret");

  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf (", class Arg%u=void_argument",i+1);
    
  printf (">\nstruct ptrfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret");
  
  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf (",Arg%u",i+1);
    
  printf (">\n{\n");
  printf ("  enum { is_ptrfun = true, is_function = false };\n};\n\n");
  
  printf ("//таблица конфигурации для указателей на функции-члены\n");
  printf ("template <size_t ArgumentsCount, class T, class Ret");

  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf (", class Arg%u=void_argument",i+1);
    
  printf (">\nstruct memfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret");
  
  for (size_t i=0;i<MAX_ARGUMENTS_COUNT;i++)
    printf (", Arg%u",i+1);
    
  printf (">\n{\n  typedef T object_type;\n\n  enum { is_memfun = true, is_function = false };\n};\n\n");
}

int main ()
{
  print_block_comment ("Этот код был сгенерирован автоматически. Не исправляйте его вручную");
  printf              ("#ifndef XTL_FUNCTIONAL_TRAITS_HEADER\n");
  printf              ("#define XTL_FUNCTIONAL_TRAITS_HEADER\n\n");  
  printf              ("#include <cstddef>\n\n");
  printf              ("namespace xtl\n{\n\n");
  printf              ("//forward declarations\n");  
  printf              ("namespace mpl\n");
  printf              ("{\n\n");  
  printf              ("template <class Head, class Tail>       struct type_node;\n");
  printf              ("template <class TypeList, size_t Index> struct at;\n");
  printf              ("                                        struct null_type;\n\n");
  printf              ("}\n\n");
  printf              ("namespace detail\n{\n\n");
  
  generate_base_types ();  
  
  print_block_comment ("Специализации для сигнатур функций");
  
  for (size_t i=0;i<=MAX_ARGUMENTS_COUNT;i++)
  {
    generate_signature_description (i,false);
    generate_signature_description (i,true);
  }
  
  generate_memfun_descriptions ("");
  
  printf ("#ifndef _MSC_VER\n\n");

  generate_ptrfun_descriptions ("");

  printf ("#else //_MSC_VER\n\n");      

  const char* modifiers [] = {"__fastcall","__stdcall", "__cdecl"};
  
  print_block_comment ("Специализации для компилятора MSVC");  
  
  for (size_t i=0;i<sizeof (modifiers)/sizeof (*modifiers);i++)
  {
    generate_ptrfun_descriptions (modifiers [i]);
    generate_memfun_descriptions (modifiers [i]);
  }
    
  printf ("#endif //_MSC_VER\n\n");
  printf ("}\n\n");
  printf ("using detail::functional_traits;\n\n");  
  printf ("}\n\n");  
  printf ("#endif //XTL_FUNCTIONAL_TRAITS_HEADER\n");

  return 0;
}
