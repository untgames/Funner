#ifndef RENDER_LOW_LEVEL_DEBUG_HEADER
#define RENDER_LOW_LEVEL_DEBUG_HEADER

#include <cstddef>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс получения информации о параметрах объекта 
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPropertyList
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t GetSize () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение ключа и его значения
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetKey   (size_t index) = 0;
    virtual const char* GetValue (size_t index) = 0;

  protected:
    virtual ~IPropertyList () {}

  private:
    IPropertyList& operator = (const IPropertyList&); //no impl
};

}

}

#endif
