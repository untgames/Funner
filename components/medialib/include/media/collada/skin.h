#ifndef MEDIALIB_COLLADA_SKIN_HEADER
#define MEDIALIB_COLLADA_SKIN_HEADER

#include <media/collada/utility.h>
#include <math/mathlib.h>

namespace medialib
{

namespace collada
{

struct VertexJointWeight
{
  int   joint;
  float weight;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///—кин
///////////////////////////////////////////////////////////////////////////////////////////////////
class Skin: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///ћатрица фигуры 
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& BindShapeMatrix    () const;
    void               SetBindShapeMatrix (const math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///–абота с соединени€ми
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t             JointsCount       () const;                            //количество соединений
    size_t             CreateJoint       (const char* name);                  //создание соединени€
    void               RemoveJoint       (size_t joint);                      //удаление соединени€
    void               RemoveAllJoints   ();                                  //удаление всех соединений
    void               SetJointInvMatrix (size_t joint, const math::mat4f& invTM); //установка обратной матрицы соединени€
    const math::mat4f& JointInvMatrix    (size_t joint) const;                //получение обратной матрицы соединени€
    int                FindJoint         (const char* name) const;            //возвращает номер соединени€ по имени или -1 в случае неудачи
    const char*        JointName         (size_t joint);                      //им€ соединени€
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Ѕазовый морф
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBaseMorph (Morph* base_morph);

          Morph* BaseMorph ();
    const Morph* BaseMorph () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///¬еса джойнтов в вершинах
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t WeightsCount  () const;
    void   WeightsResize (size_t new_size);

          VertexJointWeight* Weights ();          
    const VertexJointWeight* Weights () const;

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
