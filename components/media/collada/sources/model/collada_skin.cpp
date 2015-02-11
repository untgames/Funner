#include "shared.h"

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
  
  Joint (const char* in_name) : name (in_name) {}
};

typedef stl::vector<Joint*> JointArray;

}

struct Skin::Impl: public xtl::reference_counter
{
  math::mat4f                    bind_shape_matrix; //матрица фигуры
  JointArray                     joints;            //соединения
  stl::vector<VertexJointWeight> weights;           //веса соединений
  stl::string                    base_mesh;         //базовый меш / морф
  stl::string                    id;                //идентификатор скина

  enum { DEFAULT_JOINTS_RESERVE = 32 };
  
  void RemoveAllJoints ()
  {
    for (JointArray::iterator i=joints.begin (), end=joints.end (); i!=end; ++i)
      delete *i;

    joints.clear ();
  }
  
  Impl ()
  {
    joints.reserve (DEFAULT_JOINTS_RESERVE);
  }

  ~Impl ()
  {
    RemoveAllJoints ();
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Skin::Skin ()
  : impl (new Impl, false)
  {}

Skin::Skin (Impl* in_impl)
  : impl (in_impl, false)
  {}  
  
Skin::Skin (const Skin& skin)
  : impl (skin.impl)
  {}

Skin::~Skin ()
{
}

Skin& Skin::operator = (const Skin& skin)
{
  impl = skin.impl;
  
  return *this;
}

/*
    Создание копии
*/

Skin Skin::Clone () const
{
  return Skin (new Impl (*impl));
}

/*
    Идентификатор скина
*/

const char* Skin::Id () const
{
  return impl->id.c_str ();
}

void Skin::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Skin::SetId", "id");
    
  impl->id = id;
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

unsigned int Skin::JointsCount () const
{
  return (unsigned int)impl->joints.size ();
}

unsigned int Skin::CreateJoint (const char* name)
{
  static const char* METHOD_NAME = "media::collada::Skin::CreateJoint";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (impl->joints.size () >= (unsigned int)-1)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create joint, joint count limit exceeded");

  Joint* joint = new Joint (name);
  
  try
  {
    impl->joints.push_back (joint);
    
    return (unsigned int)impl->joints.size () - 1;
  }
  catch (...)
  {
    delete joint;
    throw;
  }
}

void Skin::RemoveJoint (unsigned int joint)
{
  if (joint >= impl->joints.size ())
    return;
    
  delete impl->joints [joint];
  
  impl->joints.erase (impl->joints.begin () + joint);
}

void Skin::RemoveAllJoints ()
{
  impl->RemoveAllJoints ();    
}

void Skin::SetJointInvMatrix (unsigned int joint, const math::mat4f& inv_matrix)
{
  if (joint >= impl->joints.size ())
    throw xtl::make_range_exception ("media::collada::Skin::SetJointInvMatrix", "joint", joint, impl->joints.size ());
    
  impl->joints [joint]->inv_matrix = inv_matrix;
}

const math::mat4f& Skin::JointInvMatrix (unsigned int joint) const
{
  if (joint >= impl->joints.size ())
    throw xtl::make_range_exception ("media::collada::Skin::GetJointInvMatrix", "joint", joint, impl->joints.size ());
    
  return impl->joints [joint]->inv_matrix;
}

int Skin::FindJoint (const char* name) const
{
  if (!name)
    return -1;

  for (JointArray::iterator i=impl->joints.begin (), end=impl->joints.end (); i!=end; ++i)
    if ((*i)->name == name)
      return (int)(i - impl->joints.begin ());
      
  return -1;
}

const char* Skin::JointName (unsigned int joint) const
{
  if (joint >= impl->joints.size ())
    throw xtl::make_range_exception ("media::collada::Skin::JointName", "joint", joint, impl->joints.size ());
    
  return impl->joints [joint]->name.c_str ();
}

/*
    Базовый меш / морф
*/

void Skin::SetBaseMesh (const char* mesh)
{
  if (!mesh)
    throw xtl::make_null_argument_exception ("media::collada::Skin::SetBaseMesh", "mesh");
    
  impl->base_mesh = mesh;
}

const char* Skin::BaseMesh () const
{
  return impl->base_mesh.c_str ();
}

/*
    Веса соединений
*/

unsigned int Skin::WeightsCount () const
{
  return (unsigned int)impl->weights.size ();
}

void Skin::WeightsResize (unsigned int new_size)
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
