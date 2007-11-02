#ifndef SCRIPTLIB_CORE_TESTS_SHARED_HEADER
#define SCRIPTLIB_CORE_TESTS_SHARED_HEADER

#include <script/make_invoker.h>
#include <stl/vector>
#include <stl/memory>
#include <common/strlib.h>
#include <xtl/shared_ptr.h>
#include <xtl/ref.h>
#include <stdio.h>

using namespace script;

class MyStack: public IStack
{
  public:
    size_t Size () { return array.size (); }

    float       GetFloat   (int index) { return xtl::any_multicast<float> (array.at (index)); }
    int         GetInteger (int index) { return xtl::any_multicast<int> (array.at (index)); }
    void*       GetPointer (int index) { return xtl::any_multicast<void*> (array.at (index)); }
    const char* GetString  (int index) { return xtl::any_multicast<const char*> (array.at (index)); }
    xtl::any&   GetVariant (int index) { return array.at (index); }

    void Push (float value)        { array.push_back (xtl::make_ref_any (value)); }
    void Push (int value)          { array.push_back (xtl::make_ref_any (value)); }
    void Push (void* pointer)      { array.push_back (xtl::make_ref_any (pointer)); }
    void Push (const char* string) { array.push_back (xtl::make_ref_any (string)); }
    void Push (const xtl::any& a)  { array.push_back (a); }

    void Pop (size_t arguments_count)
    {
      if (arguments_count > array.size ())
        arguments_count = array.size ();
      
      array.erase (array.end () - arguments_count, array.end ());
    }

  private:
    typedef stl::vector<xtl::any> Array;
    
    Array array;
};

#endif
