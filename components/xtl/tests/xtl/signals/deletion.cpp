#include <stl/string>

#include "test.h"

using namespace stl;

static connection connections[5];

static string test_output;

struct remove_connection {
  explicit remove_connection(int v = 0, int i = -1) : value(v), idx(i) {}

  void operator()() const {
    if (idx >= 0)
      connections[idx].disconnect();

    //return value;
    printf ("%d ", value);

    test_output += static_cast<char>(value + '0');
  }

  int value;
  int idx;
};

bool operator==(const remove_connection& x, const remove_connection& y)
{ return x.value == y.value && x.idx == y.idx; }

static void test_remove_self()
{
  xtl::signal<void ()> s0;

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2, 2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("Deleting 2\n");

  test_output = "";
  s0();
  
  printf ("\n");
  
  TEST (test_output == "0123");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "013");

  s0.disconnect_all();
  TEST (s0.empty());

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3, 3));

  printf ("Deleting 3\n");

  test_output = "";
  s0();
 
  printf ("\n");  
  
  TEST (test_output == "0123");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "012");

  s0.disconnect_all();
  TEST (s0.num_slots() == 0);

  connections[0] = s0.connect(remove_connection(0, 0));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("Deleting 0\n");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "0123");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "123");

  printf ("Blocking 2\n");

  connections[2].block();
  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "13");

  printf ("Unblocking 2\n");

  connections[2].unblock();
  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "123");

  s0.disconnect_all();
  TEST (s0.empty());

  connections[0] = s0.connect(remove_connection(0, 0));
  connections[1] = s0.connect(remove_connection(1, 1));
  connections[2] = s0.connect(remove_connection(2, 2));
  connections[3] = s0.connect(remove_connection(3, 3));

  printf ("Mass suicide\n");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "0123");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "");
}

static void
test_remove_prior()
{
  xtl::signal<void ()> s0;

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1, 0));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("1 removes 0\n");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "0123");

  test_output = "";
  s0();
  
  printf ("\n");
  
  TEST (test_output == "123");

  s0.disconnect_all();
  TEST (s0.empty());

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3, 2));

  printf ("3 removes 2\n");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "0123");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "013");
}

static void
test_remove_after()
{
  xtl::signal<void ()> s0;

  connections[0] = s0.connect(remove_connection(0, 1));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("0 removes 1\n");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "023");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "023");

  s0.disconnect_all();
  TEST (s0.empty());

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1, 3));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("1 removes 3\n");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "012");

  test_output = "";
  s0(); printf ("\n");
  TEST (test_output == "012");
}

static void
test_bloodbath()
{
  xtl::signal<void ()> s0;

  connections[0] = s0.connect(remove_connection(0, 1));
  connections[1] = s0.connect(remove_connection(1, 1));
  connections[2] = s0.connect(remove_connection(2, 0));
  connections[3] = s0.connect(remove_connection(3, 2));

  printf ("0 removes 1, 2 removes 0, 3 removes 2\n");

  test_output = "";
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "023");

  test_output = "";
  
  s0();
  
  printf ("\n");    
  
  TEST (test_output == "3");
}

static void
test_disconnect_equal()
{
  xtl::signal<void ()> s0;

  connections[0] = s0.connect(remove_connection(0));
  connections[1] = s0.connect(remove_connection(1));
  connections[2] = s0.connect(remove_connection(2));
  connections[3] = s0.connect(remove_connection(3));

  printf ("Deleting 2\n");

  test_output = "";
  s0(); 
    
  printf ("\n");
  
  TEST (test_output == "0123");

  s0.disconnect(remove_connection(2));

  test_output = "";
  
  s0();
  
  printf ("\n");  
  
  TEST (test_output == "013");
}

int main()
{
  printf ("Results of deletion_test:\n");

  test_remove_self();
  test_remove_prior();
  test_remove_after();
  test_bloodbath();
  test_disconnect_equal();
  
  return 0;
}
