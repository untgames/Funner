#ifndef MEDIALIB_PHYSICS_COLLISION_FILTER_HEADER
#define MEDIALIB_PHYSICS_COLLISION_FILTER_HEADER

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фильтр столкновений
///////////////////////////////////////////////////////////////////////////////////////////////////
class CollisionFilter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    CollisionFilter  ();
    CollisionFilter  (const CollisionFilter&);
    ~CollisionFilter ();

    CollisionFilter& operator = (const CollisionFilter&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание копии
///////////////////////////////////////////////////////////////////////////////////////////////////
    CollisionFilter Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Маски имен групп
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Group1Wildcard    () const;
    const char* Group2Wildcard    () const;
    void        SetGroup1Wildcard (const char* wildcard);
    void        SetGroup2Wildcard (const char* wildcard);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка фильтрации, сталкиваются ли тела групп
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCollides  () const;
    void SetCollides (bool collides);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (CollisionFilter&);

  private:
    struct Impl;

    CollisionFilter (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (CollisionFilter&, CollisionFilter&);

}

}

#endif
