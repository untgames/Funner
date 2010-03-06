#include <stdio.h>
#include <stdarg.h>

#include <windows.h>

#include <FBaseSys.h>

/*
    Константы
*/

const size_t PRINTF_STACK_BUFFER_SIZE  = 32;                      //размер стекового буфера для печати текста
const size_t STDOUT_FILE_NAME_MAX_SIZE = 2048;                    //максимальный размер имени файла перенаправления stdout
const char*  STDOUT_ENVIRONMENT_VAR    = "BADA_STDOUT_FILE_NAME"; //имя переменной окружения, содержащей имя файла перенаправления stdout
const char*  STDOUT_EXIT_MARKER        = "BADA APPLICATION EXITED. RETURN CODE IS:\n"; //маркер выхода из приложения

/*
    Статические переменные
*/

static HANDLE stdout_file_handle = 0;

/*
    Переопределение функций
*/

///закрытие файла вывода stdout
static void close_stdout ()
{
  if (stdout_file_handle)
  {
    CloseHandle (stdout_file_handle);
    stdout_file_handle = 0;
  }
}

///вывод текста
static void write_stdout (const char* text, size_t text_size)
{
  DWORD text_written = 0;

  if (!stdout_file_handle)
  {
    char* file_name = (char*)malloc (STDOUT_FILE_NAME_MAX_SIZE);
    
    if (!file_name)
    {
      AppLog ("Can't allocate memory for store stdout file name");
      abort ();
      
      return;
    }
    
    int file_name_size = GetEnvironmentVariable (STDOUT_ENVIRONMENT_VAR, file_name, STDOUT_FILE_NAME_MAX_SIZE);
    
    if (file_name_size < STDOUT_FILE_NAME_MAX_SIZE)
    {
      file_name [file_name_size] = '\0';
    }
    else
    {
      file_name [STDOUT_FILE_NAME_MAX_SIZE-1] = '\0';
    }
    
    stdout_file_handle = CreateFile (file_name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    
    free (file_name);

    if (stdout_file_handle == INVALID_HANDLE_VALUE) 
    { 
      AppLog ("Can't create console output file");
      abort ();
      
      return;
    }
    
    atexit (&close_stdout);
  }
  
  if (!stdout_file_handle)
    return;
    
  while (text_size)
  {
    WriteFile (stdout_file_handle, text, text_size, &text_written, NULL);
    
    text_size -= text_written;
    text      += text_written;
  }
}

int puts (const char* s)
{
  write_stdout (s, strlen (s));
  write_stdout ("\n", strlen ("\n"));
  
  return 0;
}

///печать в строку
static int xvsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : vsnprintf (0, 0, format, list);

  int ret = vsnprintf (buffer, count, format, list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

///переопределение функции vprintf
int vprintf (const char* format, va_list args)
{
  char buffer [PRINTF_STACK_BUFFER_SIZE];
  
  int size = xvsnprintf (0, 0, format, args);

  if (size == -1)
    return 0;
    
  if (size < sizeof (buffer))
  {
    xvsnprintf (buffer, sizeof (buffer), format, args);
    
    write_stdout (buffer, size);
    
    return 0;
  }
  
  char* dyn_buffer = (char*)malloc (size+1);
  
  if (!dyn_buffer)
    return 0;
  
  xvsnprintf (dyn_buffer, size, format, args);
  
  write_stdout (dyn_buffer, size);
  
  free (dyn_buffer);

  return 0;
}

///переопределение функции printf
int printf (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  return vprintf (format, args);
}

///точка входа в приложение
_EXPORT_ int OspMain (int argc, char *argv [])
{
  const char* env [] = {0};

  int result = main (argc, argv, env);
  
  printf ("\n%s%d\n", STDOUT_EXIT_MARKER, result);
  
  close_stdout ();
  
  return result;
}
