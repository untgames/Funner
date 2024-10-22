#ifndef MEDIALIB_ADOBE_XFL_ANIMATION_CORE_HEADER
#define MEDIALIB_ADOBE_XFL_ANIMATION_CORE_HEADER

#include <media/adobe/xfl_property_animation.h>
#include <media/adobe/xfl_collection.h>

namespace media
{

namespace adobe
{

namespace xfl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Анимация кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationCore
{
  public:
    typedef INamedCollection<PropertyAnimation> PropertyAnimationList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationCore  ();
    AnimationCore  (const AnimationCore&);
    ~AnimationCore ();

    AnimationCore& operator = (const AnimationCore&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Длительность в секундах
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Duration    () const;
    void  SetDuration (float duration);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Должна ли происходить автоориентация по пути
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool OrientToPath    () const;
    void SetOrientToPath (bool orient_to_path);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Анимируемые свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
          PropertyAnimationList& Properties ();
    const PropertyAnimationList& Properties () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (AnimationCore&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (AnimationCore&, AnimationCore&);

}

}

}

#endif
