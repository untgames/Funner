#include "shared.h"

using namespace common;

/*
    ¬ход / выход из критической секции
*/

void CriticalSection::Enter ()
{
  try
  {
    Platform::GetThreadSystem ()->EnterCriticalSection ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CriticalSection::Enter");
    throw;
  }
}

void CriticalSection::Exit ()
{
  try
  {
    Platform::GetThreadSystem ()->ExitCriticalSection ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::CriticalSection::Exit");
    throw;
  }
}
