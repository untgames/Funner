#include "test.h"

namespace
{

// Let's create these inheritance relationship:
//
//    base   base2
//      |     |
//      derived
//         |
//  derived_derived
//

class base
{
   public:
   virtual ~base(){}
   int filler [5];
};

class base2
{
public:

    virtual ~base2(){}
    int filler [5];
};

class derived
   : public base, public base2 
{
    int filler [5];
};

class derived_derived
   : public derived
{
    int filler [5];
};

// And now some simple check functions

template <class BasePtr>
bool check_dynamic_pointer_cast(const BasePtr &ptr)
{
   //Check that dynamic_pointer_cast versus dynamic_cast
   return
   //Correct cast with dynamic_pointer_cast
   get_pointer(dynamic_pointer_cast<derived>(ptr)) ==
      //Correct cast with dynamic_cast
      dynamic_cast<derived*>(get_pointer(ptr)) 
   &&
   //Incorrect cast with dynamic_pointer_cast
   get_pointer(dynamic_pointer_cast<derived_derived>(ptr)) ==
      //Incorrect cast with dynamic_cast
      dynamic_cast<derived_derived*>(get_pointer(ptr));
}

template <class BasePtr>
bool check_static_pointer_cast(const BasePtr &ptr)
{
   return
   //Cast base -> derived -> base2 using static_pointer_cast
   get_pointer(
            static_pointer_cast<base2>(
               static_pointer_cast<derived>(ptr))) ==
   //Now the same with static_cast
   static_cast<base2*>(static_cast<derived*>(get_pointer(ptr)));
}

template <class BasePtr>
bool check_const_pointer_cast(const BasePtr &ptr)
{
   return   
   //Unconst and const again using const_pointer_cast
   get_pointer(
      const_pointer_cast<const base>
         (const_pointer_cast<base>(ptr))) ==
   //Now the same with const_cast
   const_cast<const base*>(const_cast<base*>(get_pointer(ptr)));
}

}

int main()
{
    printf ("Results of pointer_cast_test:\n");

    {
        // Try casts with shared_ptr

        shared_ptr<base> ptr(new derived);

        TEST ( check_dynamic_pointer_cast( ptr ) );
        TEST ( check_static_pointer_cast( ptr ) );
        TEST ( check_const_pointer_cast( ptr ) );
    }   
    
    return 0;
}
