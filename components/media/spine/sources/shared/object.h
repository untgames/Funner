#ifndef MEDIA_SPINE_SHARED_OBJECT_HEADER
#define MEDIA_SPINE_SHARED_OBJECT_HEADER

#include <xtl/reference_counter.h>

namespace media
{

namespace spine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Base class for all spine impl classes
///////////////////////////////////////////////////////////////////////////////////////////////////
class IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Reference counting
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void         AddRef   () = 0;
    virtual void         Release  () = 0;
    virtual unsigned int UseCount () = 0;

  protected:
    virtual ~IObject () {}
};

}

}

#endif
