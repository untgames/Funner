#include "shared.h"

using namespace media::adobe::xfl;

/*
    Описание реализации элемента кадра анимации
*/

struct FrameElement::Impl : public xtl::reference_counter
{
  stl::string      name;                 //имя элемента
  FrameElementType type;                 //тип элемента
  size_t           first_frame;          //номер кадра (в глобальном времени), с которого начинается отображение данного кадра
  math::vec2f      translation;          //смещение
  math::vec2f      transformation_point; //пивот
  
  Impl (FrameElementType in_type)
    : type (in_type)
    , first_frame (0)
   { }
};

/*
    Конструкторы / деструктор / присваивание
*/

FrameElement::FrameElement (FrameElementType type)
{
  static const char* METHOD_NAME = "media::adobe::xfl::FrameElement::FrameElement";

  switch (type)
  {
    case FrameElementType_ResourceInstance:
    case FrameElementType_SymbolInstance:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "type");
  }

  impl = new Impl (type);
}

FrameElement::FrameElement (const FrameElement& source)
  : impl (source.impl)
{
  addref (impl);
}

FrameElement::~FrameElement ()
{
  release (impl);
}

FrameElement& FrameElement::operator = (const FrameElement& source)
{
  FrameElement (source).Swap (*this);

  return *this;
}

/*
   Имя
*/

const char* FrameElement::Name () const
{
  return impl->name.c_str ();
}

void FrameElement::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::FrameElement::SetName", "name");
    
  impl->name = name;
}

/*
   Тип
*/

FrameElementType FrameElement::Type () const
{
  return impl->type;
}

/*
   Тайминги
*/

size_t FrameElement::FirstFrame () const
{
  return impl->first_frame;
}

void FrameElement::SetFirstFrame (size_t first_frame)
{
  impl->first_frame = first_frame;
}

/*
   Положение
*/

const math::vec2f& FrameElement::Translation () const
{
  return impl->translation;
}

const math::vec2f& FrameElement::TransformationPoint () const
{
  return impl->transformation_point;
}

void FrameElement::SetTranslation (const math::vec2f& translation)
{
  impl->translation = translation;
}

void FrameElement::SetTransformationPoint (const math::vec2f& transformation_point)
{
  impl->transformation_point = transformation_point;
}

/*
   Обмен
*/

void FrameElement::Swap (FrameElement& element)
{
  stl::swap (impl, element.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   Обмен
*/

void swap (FrameElement& element1, FrameElement& element2)
{
  element1.Swap (element2);
}

}

}

}
