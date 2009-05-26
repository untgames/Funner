#include "test.h"

//данный тест ошибку, связанную с отсоединением слота самого себя в обработчике
//ошибка проявилялась на WinXP

const size_t NUM = 100000;

typedef signal<void (int)> my_signal;

connection self_destroy_connections [NUM];
my_signal  self_destroy_signals [NUM];

void self_disconnect (int index)
{
  self_destroy_connections [index].disconnect ();
}

int main ()
{
  printf ("Results of slot_self_deletion1:\n");

  for (size_t i=0; i<NUM; i++)
    self_destroy_connections [i] = self_destroy_signals [i].connect (&self_disconnect);

  for (size_t i=0; i<NUM; i++)
  {
    self_destroy_signals [i](i);
  }

  printf ("executed successfull\n");

  return 0;
}
