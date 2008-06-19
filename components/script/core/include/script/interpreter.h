#ifndef SCRIPTLIB_INTERPRETER_HEADER
#define SCRIPTLIB_INTERPRETER_HEADER

#include <xtl/functional_fwd>
#include <xtl/intrusive_ptr.h>
#include <xtl/exception.h>

#include <script/stack.h>

namespace xtl
{

//forward declaration
template <class T> class shared_ptr;

}

namespace script
{

//forward declaration
class Environment;

//////////////////////////////////////////////////////////////////////////////////////////////////
///����������
//////////////////////////////////////////////////////////////////////////////////////////////////
struct InterpreterException:   virtual public xtl::exception {};        //������� ���������� ����������
struct RuntimeException:               public InterpreterException {};  //����������, ����������� ��� ���������� �������
struct StackException:                 public InterpreterException {};  //����������, ����������� ��� ������������/"������������" �����
struct ArgumentException:              public StackException {};        //����������, ����������� ��� ��������� ���������
struct UndefinedFunctionCallException: public InterpreterException {};  //������� ������ �������������������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IInterpreter
{
  public:
    typedef xtl::function<void (const char*)> LogFunction;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Name () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IStack& Stack () = 0;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DoCommands (const char*        buffer_name,  //��� ������ ������
                             const void*        buffer,       //����� � ���������
                             size_t             buffer_size,  //������ ������
                             const LogFunction& log) = 0;     //������� ���������������� ������ �������������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������� � ��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool HasFunction (const char* name) = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Invoke (size_t arguments_count, size_t results_count) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IInterpreter () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� lua
///////////////////////////////////////////////////////////////////////////////////////////////////
xtl::com_ptr<IInterpreter> create_lua_interpreter (const xtl::shared_ptr<Environment>& environment);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InterpreterManager
{
  public:
    typedef xtl::function<IInterpreter* (const xtl::shared_ptr<Environment>& environment)> InterpreterCreater;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterInterpreter       (const char* name, const InterpreterCreater& creater);
    static void UnregisterInterpreter     (const char* name);
    static void UnregisterAllInterpreters ();
};

}

#endif
