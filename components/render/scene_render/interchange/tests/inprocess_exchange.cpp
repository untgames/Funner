#include "shared.h"

struct A
{
  int id;

  A (int in_id) : id (in_id) { printf ("A::A(%d)\n", id); }

  ~A () { printf ("A::~A(%d)\n", id); }
};

int main ()
{
  printf ("Results of inprocess_exchange_test:\n");

  object_id_t owner_id = InprocessExchangeStorage::AllocateOwnerId ();

  printf ("before attach\n");

  object_id_t object_id = InprocessExchangeStorage::Attach (owner_id, A (1));

  A& a = InprocessExchangeStorage::Attachment<A> (object_id);

  printf ("check: id=%d\n", a.id);

  printf ("before detach\n");

  InprocessExchangeStorage::Detach (object_id);

  printf ("check: %s\n", InprocessExchangeStorage::FindAttachment<A> (object_id) ? "exist" : "not exist");

  printf ("before attach x 3\n");

  object_id = InprocessExchangeStorage::Attach (owner_id, A (2));  

  InprocessExchangeStorage::Attach (owner_id, A (3));
  InprocessExchangeStorage::Attach (owner_id+1, A (4));

  printf ("before detach x 2\n");

  InprocessExchangeStorage::DetachAllWithOwner (owner_id);

  printf ("exit\n");

  return 0;
}
