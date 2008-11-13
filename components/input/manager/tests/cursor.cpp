#include <cstdio>
#include <exception>

#include <input/cursor.h>

using namespace input;

class MyListener: public ICursorListener
{
  public:
    void OnDestroy ()
    {
      printf ("MyListener::OnDestroy\n");
    }
    
    void OnChangePosition (const math::vec2f& pos)
    {
      printf ("MyListener::OnChangePosition(%.1f,%.1f)\n", pos.x, pos.y);
    }
};

int main ()
{
  printf ("Results of cursor_test:\n");
  
  try
  {
    MyListener listener;    
    Cursor     cursor;
    
    cursor.Attach (&listener);
    
    cursor.SetPosition (2.1f, 3.2f);
    cursor.SetPosition (2.1f, 3.2f);
    cursor.SetPosition (3.2f, 2.1f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
