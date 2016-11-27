#ifndef SCRIPT_SHELL_HEADER
#define SCRIPT_SHELL_HEADER

#include <xtl/intrusive_ptr.h>
#include <xtl/functional_fwd>

namespace xtl
{

//forward declaration
template <class T> class shared_ptr;

}

namespace script
{

//forward declarations
class IInterpreter;
class Environment;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оболочка над командным интерпретатором
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shell
{
  public:
    typedef xtl::shared_ptr<Environment> EnvironmentPtr;
    typedef xtl::com_ptr<IInterpreter>   InterpeterPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shell  ();
    Shell  (const char* interpreter_name, const Environment& environment);
    Shell  (const InterpeterPtr&);
    Shell  (const Shell&);
    ~Shell ();
    
    Shell& operator = (const Shell&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени интерптератора / получение интерпретатора
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*   InterpreterName () const;
    InterpeterPtr Interpreter     () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение интерпретатора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetInterpreter (const char* interpreter_name, const Environment& environment);
    void SetInterpreter (const InterpeterPtr&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка наличия функции в контексте интерпретации
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFunction (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Execute (const char* buffer_name,  //имя буфера команд
                  const void* buffer,       //буфер с командами
                  size_t      buffer_size); //размер буфера
    void Execute (const char* command);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение команд, расположенных в файле / файлах
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ExecuteFile     (const char* file_name);
    void ExecuteFileList (const char* file_mask);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Shell&);
    
  private:
    void ExecuteImpl (const char* buffer_name, const void* buffer, size_t buffer_size);

  private:
    InterpeterPtr interpreter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Shell&, Shell&);

}

#endif
