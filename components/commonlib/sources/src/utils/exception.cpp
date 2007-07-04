#include <common/exception.h>
#include <common/strlib.h>

using namespace common;
using namespace stl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации класса Exception
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Exception::Impl
{
  string message;
  bool   stack_print;
  
  Impl () : stack_print (false) {}
};

/*
    Exception
*/

Exception::Exception (const char* _message)
{
  impl = new Impl;
  
  try
  {      
    impl->message = _message;    
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Exception::Exception (const Exception& exception)
{
  impl = new Impl;
  
  try
  {
    *impl = *exception.impl;
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Exception::~Exception () throw ()
{
  delete impl;
}

Exception& Exception::operator = (const Exception& exception)
{
  if (this == &exception)
    return *this;

  *impl = *exception.impl;

  return *this;
}

const char* Exception::Message () const throw ()
{
  return impl->message.c_str ();
}

const char* Exception::what () const throw ()
{
  return impl->message.c_str ();
}

void Exception::Raise ()
{
  throw *this;
}

void Exception::Raise (const char* source)
{
  if (source)
  {
    if (!impl->stack_print)
    {
      impl->message     += " at ";
      impl->stack_print  = true;
    }
    else impl->message += "<-";

    impl->message += source;
  }

  throw *this;
}

/*
    VRaise
*/

void common::VRaise (const char* source,const char* format,va_list list,void (*raise)(const char* source,const char* message))
{
  raise (source,common::vformat (format,list).c_str ());
}
