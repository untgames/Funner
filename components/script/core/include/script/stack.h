#ifndef SCRIPTLIB_STACK_HEADER
#define SCRIPTLIB_STACK_HEADER

namespace xtl
{

//forward declarations
class any;

}

namespace script
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ���������� �������-�����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IStack
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Size () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual float       GetFloat   (size_t index) = 0;
    virtual int         GetInteger (size_t index) = 0;
    virtual void*       GetPointer (size_t index) = 0;
    virtual const char* GetString  (size_t index) = 0;
    virtual xtl::any    GetVariant (size_t index) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Push (float value) = 0;
    virtual void Push (int value) = 0;
    virtual void Push (void* pointer) = 0;
    virtual void Push (const char* string) = 0;
    virtual void Push (const xtl::any& variant) = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/���������� �� ����� ����������� �������� ������������� (�������� ��� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        PushSymbol (const char* symbol) = 0;
    virtual const char* GetSymbol  (size_t index) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Pop (size_t arguments_count) = 0;

  protected:
    virtual ~IStack () {}
};

}

#endif
