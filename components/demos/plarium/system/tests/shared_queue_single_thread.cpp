#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

//test data type
class TestData
{
  public:
    TestData()
    {
      total_objects_count++;
    }

    ~TestData()
    {
      total_objects_count--;
    }

    static size_t getTotalObjectsCount()
    {
      return total_objects_count;
    }

  private:
    TestData(const TestData&); //no implementation
    TestData& operator = (const TestData&); //no implementation

  private:
    static size_t total_objects_count;
};

size_t TestData::total_objects_count = 0;

typedef SharedQueue<TestData> TestQueue;

//single thread test case for SharedQueue
int main ()
{
  {
    std::auto_ptr <TestData> item (new TestData);

    TestQueue queue(1);

    printf ("Queue size = %u\n", queue.Size ());

    queue.Enqueue (item);

    item.release ();

    printf ("Queue size = %u\n", queue.Size ());

    std::auto_ptr<TestData> result = queue.Dequeue (0);

    printf ("Result getted = %c\n", result.get () == 0 ? 'n' : 'y');
    printf ("Queue size = %u\n", queue.Size ());
  }

  printf ("Total count = %u\n", TestData::getTotalObjectsCount ());

  return 0;
}
