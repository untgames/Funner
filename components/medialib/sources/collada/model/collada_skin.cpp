#include <media/collada/skin.h>
#include <stl/vector>
#include <stl/string>

using namespace media::collada;

/*
    Описание реализации скина
*/

namespace
{

struct Joint
{
  stl::string name;       //имя соединения
  math::mat4f inv_matrix; //обратная матрица соединения
};

typedef stl::vector<Joint*> JointArray;

}

struct Skin::Impl
{
  math::mat4f        bind_shape_matrix;      //матрица фигуры
  JointArray         joints;                 //соединения
  stl::vector<VertexJointWeight> weights;    //веса джойнтов
  Morph*             base_morph;             //базовый морф (если есть)

  enum { DEFAULT_JOINTS_RESERVE = 32 };
  
  Impl ()
  {
    joints.reserve (DEFAULT_JOINTS_RESERVE);
  }
};

/*
    Конструктор / деструктор
*/

Skin::Skin (ModelImpl* owner, const char* id)
  : Entity (owner, id), impl (new Impl)
  {}

Skin::~Skin ()
{
  RemoveAllJoints ();

  delete impl;
}

/*
    Матрица фигуры 
*/

const math::mat4f& Skin::BindShapeMatrix () const
{
  return impl->bind_shape_matrix;
}

void Skin::SetBindShapeMatrix (const math::mat4f& tm)
{
  impl->bind_shape_matrix = tm;
}

/*
    Работа с соединениями
*/

size_t Skin::JointsCount () const
{
 return impl->joints.size ();
}

size_t Skin::CreateJoint (const char* name)
{
  if (!name)
    common::RaiseNullArgument ("media::collada::Skin::CreateJoint", "name");

  Joint* joint = new Joint;
  
  try
  {
    joint->name = name;
    
    impl->joints.push_back (joint);
    
    return impl->joints.size () - 1;
  }
  catch (...)
  {
    delete joint;
    throw;
  }
}

void Skin::RemoveJoint (size_t joint)
{
  if (joint >= impl->joints.size ())
    common::RaiseOutOfRange ("media::collada::RemoveJoint", "joint", joint, impl->joints.size ());
    
  delete impl->joints [joint];
  
  impl->joints.erase (impl->joints.begin () + joint);
}

void Skin::RemoveAllJoints ()
{
  for (JointArray::iterator i=impl->joints.begin (), end=impl->joints.end (); i!=end; ++i)
    delete *i;
    
  impl->joints.clear ();
}

void Skin::SetJointInvMatrix (size_t joint, const math::mat4f& inv_matrix)
{
  if (joint >= impl->joints.size ())
    common::RaiseOutOfRange ("media::collada::SetJointInvMatrix", "joint", joint, impl->joints.size ());
    
  impl->joints [joint]->inv_matrix = inv_matrix;
}

const math::mat4f& Skin::JointInvMatrix (size_t joint) const
{
  if (joint >= impl->joints.size ())
    common::RaiseOutOfRange ("media::collada::GetJointInvMatrix", "joint", joint, impl->joints.size ());
    
  return impl->joints [joint]->inv_matrix;
}

int Skin::FindJoint (const char* name) const
{
  if (!name)
    common::RaiseNullArgument ("media::collada::Skin::FindJoint", "name");

  for (JointArray::iterator i=impl->joints.begin (), end=impl->joints.end (); i!=end; ++i)
    if ((*i)->name == name)
      return i - impl->joints.begin ();
      
  return -1;
}

const char* Skin::JointName (size_t joint)
{
  if (joint >= impl->joints.size ())
    common::RaiseOutOfRange ("media::collada::JointName", "joint", joint, impl->joints.size ());
    
  return impl->joints[joint]->name.c_str ();
}

void Skin::SetBaseMorph (Morph* base_morph)
{
  impl->base_morph = base_morph;
}

Morph* Skin::BaseMorph ()
{
  return impl->base_morph;
}

const Morph* Skin::BaseMorph () const
{
  return impl->base_morph;
}

size_t Skin::WeightsCount () const
{
  return impl->weights.size ();
}

void Skin::WeightsResize (size_t new_size)
{
  impl->weights.resize (new_size);
}

VertexJointWeight* Skin::Weights ()
{
  return impl->weights.begin ();
}

const VertexJointWeight* Skin::Weights () const
{
  return impl->weights.begin ();
}
