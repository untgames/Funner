#ifndef MEDIALIB_COLLADA_SKIN_HEADER
#define MEDIALIB_COLLADA_SKIN_HEADER

#include <media/collada/utility.h>
#include <math/mathlib.h>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Скин
///////////////////////////////////////////////////////////////////////////////////////////////////
class Skin: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Матрица фигуры 
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& BindShapeMatrix    () const;
    void               SetBindShapeMatrix (const math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с соединениями
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t             JointsCount       () const;                            //количество соединений
    size_t             CreateJoint       (const char* name);                  //создание соединения
    void               RemoveJoint       (size_t joint);                      //удаление соединения
    void               RemoveAllJoints   ();                                  //удаление всех соединений
    void               SetJointInvMatrix (size_t joint, const math::mat4f& invTM); //установка обратной матрицы соединения
    const math::mat4f& JointInvMatrix    (size_t joint) const;                //получение обратной матрицы соединения
    int                FindJoint         (const char* name) const;            //возвращает номер соединения по имени или -1 в случае неудачи

  protected:
    Skin  (ModelImpl*, const char* id);
    ~Skin ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
