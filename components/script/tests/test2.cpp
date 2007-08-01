#include <stdio.h>
#include <script.h>
#include <common/exception.h>
#include <xtl/functional>

using namespace script::lua;
using namespace common;

const char* file_name = "data/test1.lua";

int my_func (const char* arg)
{
  printf ("My function work!!! Received arg %s\n", arg);
  return 157;
}

struct MyStruct
{
  int item;
};

struct MyStructDerived : public MyStruct
{
  size_t item2;
};

int main ()
{
  printf ("Results of test2:\n");

  try
  {
    Environment script;
    Stack       *stack (script.Stack ());
    MyStruct    *my_struct = new MyStruct (), *my_struct2;
    MyStructDerived *derived = new MyStructDerived ();

    my_struct->item = 147;
    derived->item = 147;

    stack->Push (my_struct);
    my_struct2 = stack->Get <MyStruct*> (-1);
    printf ("my_struct2 item value = %d\n", my_struct2->item);
    stack->Pop (1);

    for (size_t j = 0; j < 1000; j++)
    {
//      printf ("\n\nPushing\n\n");
      for (size_t i = 0; i < 40; i++)
        stack->Push (my_struct);
//      printf ("\n\nPushed, popping\n\n");
      stack->Pop (40);
//      printf ("\n\nPopped\n\n");
    } 
  
/*    stack->Push (derived);
    my_struct2 = stack->Get <MyStruct*> (-1);

    printf ("my_struct2 item value = %d\n", my_struct2->item);
*/ }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}