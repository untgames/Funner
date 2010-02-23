#include "shared.h"

namespace math
{

inline bool read (const char* string, spline_wrap& result)
{
  if (!xtl::xstrcmp ("clamp", string))
    result = spline_wrap_clamp;
  else if (!xtl::xstrcmp ("repeat", string))
    result = spline_wrap_repeat;
  else if (!xtl::xstrcmp ("mirror", string))
    result = spline_wrap_mirror;
  else if (!xtl::xstrcmp ("repeat", string))
    return false;

  return true;
}

}

namespace
{

//Описаине эмиттера
struct EmitterDesc
{
  stl::string       material;
  math::tcb_splinef count;
  math::tcb_splinef life_time;
  math::tcb_splinef alpha;
  math::vec2f       size;
  math::vec3f       color;
  float             max_count;
};

//Описание системы частиц
struct ParticleSystemDesc : public xtl::reference_counter
{
  EmitterDesc emitter_desc;

  void AddRef  () { addref  (this); }
  void Release () { release (this); }
};

typedef xtl::com_ptr <ParticleSystemDesc> ParticleSystemDescPtr;

//Система частиц
class ParticleSystem : public scene_graph::Entity
{
  public:
    typedef xtl::com_ptr <ParticleSystem>       Pointer;
    typedef xtl::com_ptr <const ParticleSystem> ConstPointer;

///Создание системы частиц
    static Pointer Create (ParticleSystemDescPtr desc, Node& particles_parent)
    {
      return Pointer (new ParticleSystem (desc, particles_parent), false);
    }

  protected:
///Конструктор / деструктор
    ParticleSystem (ParticleSystemDescPtr in_desc, Node& particles_parent)
      : desc (in_desc), time (0), was_updated (false)
    {
      particles_node = SpriteList::Create ();

      particles_node->BindToParent (particles_parent);
      particles_node->SetMaterial  (desc->emitter_desc.material.c_str ());
      particles_node->Reserve      (desc->emitter_desc.max_count);

      particles_parent_destroy_connection = particles_parent.RegisterEventHandler (NodeEvent_BeforeDestroy, xtl::bind (&ParticleSystem::OnParticlesParentDestroy, this));
      update_connection = this->AttachController (xtl::bind (&ParticleSystem::UpdateSystem, this, _1));
    }

    ~ParticleSystem () {}

///Метод, вызываемый при посещении данного объекта
    void AcceptCore (Visitor& visitor)
    {
      if (!TryAccept (*this, visitor))
        Entity::AcceptCore (visitor);
    }

  private:
///Обновление системы частиц
    void UpdateSystem (float dt)
    {
      if (!was_updated)
      {
        time        = 0;
        was_updated = true;
      }
      else
        time += dt * 1000.f;

      float seconds_time = time / 1000.f;

      size_t count = stl::min (desc->emitter_desc.count.eval (seconds_time), desc->emitter_desc.max_count);

      for (ParticlesList::iterator iter = particles.begin (); iter != particles.end ();)
      {
        if (time - (*iter)->born_time > ((*iter)->life_time * 1000.f))
        {
          ParticlesList::iterator next = iter;
          ++next;

          particles.erase (iter);

          iter = next;
        }
        else
          ++iter;
      }

      if (!particles_node->Scene ())
        return;

      float       life_time                     = desc->emitter_desc.life_time.eval (seconds_time);
      math::vec3f particles_node_world_position = particles_node->WorldPosition (),
                  emitter_world_position        = WorldPosition ();

      if (particles.size () < count)
      {
        ParticleDescPtr particle_desc (new ParticleDesc, false);

        particle_desc->world_position = emitter_world_position - particles_node_world_position;
        particle_desc->born_time      = time;
        particle_desc->life_time      = life_time;

        particles.push_back (particle_desc);
      }

      particles_node->Resize (particles.size ());

      scene_graph::SpriteModel::SpriteDesc *sprite_desc = particles_node->Sprites ();

      size_t i = 0;

      for (ParticlesList::iterator iter = particles.begin (), end = particles.end (); iter != end; ++iter, sprite_desc++)
      {
        float particle_time = (time - (*iter)->born_time) / 1000.f;

        sprite_desc->position = (*iter)->world_position;
        sprite_desc->size     = desc->emitter_desc.size;
        sprite_desc->color    = desc->emitter_desc.color;
        sprite_desc->color.w  = desc->emitter_desc.alpha.eval (particle_time);

        i++;
      }
    }

///Реакция на удаление родительского узла частиц
    void OnParticlesParentDestroy ()
    {
      update_connection.disconnect ();
    }

  private:
    struct ParticleDesc : public xtl::reference_counter
    {
      math::vec3f world_position;
      size_t      born_time;
      float       life_time;

      void AddRef  () { addref  (this); }
      void Release () { release (this); }
    };

  private:
    typedef xtl::com_ptr <ParticleDesc> ParticleDescPtr;
    typedef stl::list <ParticleDescPtr> ParticlesList;

  private:
    scene_graph::SpriteList::Pointer particles_node;
    ParticleSystemDescPtr            desc;
    xtl::auto_connection             update_connection;
    xtl::auto_connection             particles_parent_destroy_connection;
    ParticlesList                    particles;
    size_t                           time;
    bool                             was_updated;
};

}

typedef stl::hash_map <stl::hash_key <const char*>, math::tcb_splinef>     SplinesMap;
typedef stl::hash_map <stl::hash_key <const char*>, ParticleSystemDescPtr> ParticleSystemDescsMap;

struct ParticleSystemsManager::Impl
{
  SplinesMap             splines_map;
  ParticleSystemDescsMap particle_system_descs_map;

///Загрузка описаний систем частиц
  void LoadParticleSystems (const char* file_name)
  {
    common::Parser   p (file_name);
    common::ParseLog log = p.Log ();

    for_each_child (p.Root (), "ParticleLibrary.Spline", xtl::bind (&ParticleSystemsManager::Impl::ReadSpline, this, _1));
    for_each_child (p.Root (), "ParticleLibrary.ParticleSystem", xtl::bind (&ParticleSystemsManager::Impl::ReadParticleSystem, this, _1));

    for (size_t i = 0; i < log.MessagesCount (); i++)
      switch (log.MessageType (i))
      {
        case common::ParseLogMessageType_Error:
        case common::ParseLogMessageType_FatalError:
          throw xtl::format_operation_exception ("LoadParticleSystems", log.Message (i));
        default:
          break;
      }
  }

  void ReadSpline (common::Parser::Iterator node)
  {
    static const char* METHOD_NAME = "ParticleSystemsManager::Impl::ReadSpline";

    const char        *spline_id = common::get<const char*> (*node, "Id"),
                      *wrap_mode = common::get<const char*> (*node, "WrapMode");
    math::spline_wrap spline_wrap_mode;

    if (!read (wrap_mode, spline_wrap_mode))
      throw xtl::format_operation_exception (METHOD_NAME, "Unsupported spline wrap mode at line %u", node->LineNumber ());

    SplinesMap::const_iterator iter = splines_map.find (spline_id);

    if (iter != splines_map.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Duplicate spline with id '%s' at line %u", spline_id, node->LineNumber ());

    math::tcb_splinef spline;

    spline.wrap (spline_wrap_mode, spline_wrap_mode);

    for (common::Parser::NamesakeIterator i = node->First ("Key"); i; ++i)
      spline.add_key (common::get<float> (*i, "Time"), common::get<float> (*i, "Value"));

    splines_map.insert_pair (spline_id, spline);
  }

  void ReadParticleSystem (common::Parser::Iterator node)
  {
    static const char* METHOD_NAME = "ParticleSystemsManager::Impl::ReadParticleSystem";

    common::Parser::Iterator emitter = node->First ("Emitter");

    const char  *id           = common::get<const char*> (*node, "Id"),
                *material     = common::get<const char*> (*emitter, "Material"),
                *size_string  = common::get<const char*> (*emitter, "Size"),
                *color_string = common::get<const char*> (*emitter, "Color"),
                *count        = common::get<const char*> (*emitter, "Count"),
                *life_time    = common::get<const char*> (*emitter, "LifeTime"),
                *alpha        = common::get<const char*> (*emitter, "Alpha");
    float       max_count     = common::get<float>       (*emitter, "MaxCount");

    math::vec4f size;
    math::vec4f color;

    if (!math::read (size_string, size))
      throw xtl::format_operation_exception (METHOD_NAME, "Invalid size format at line %u", node->LineNumber ());
    if (!math::read (color_string, color))
      throw xtl::format_operation_exception (METHOD_NAME, "Invalid color format at line %u", node->LineNumber ());

    ParticleSystemDescsMap::const_iterator iter = particle_system_descs_map.find (id);

    if (iter != particle_system_descs_map.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Duplicate particle system with id '%s' at line %u", id, node->LineNumber ());

    ParticleSystemDescPtr particle_system (new ParticleSystemDesc, false);

    particle_system->emitter_desc.material  = material;
    particle_system->emitter_desc.size      = size;
    particle_system->emitter_desc.color     = color;
    particle_system->emitter_desc.count     = GetSpline (count);
    particle_system->emitter_desc.life_time = GetSpline (life_time);
    particle_system->emitter_desc.alpha     = GetSpline (alpha);
    particle_system->emitter_desc.max_count = max_count;

    particle_system_descs_map.insert_pair (id, particle_system);
  }

///Получение сплайна, если сплайн не загружен - создание сплайна с одним ключом
  math::tcb_splinef GetSpline (const char* name)
  {
    printf ("Getting spline '%s'\n", name);

    SplinesMap::iterator iter = splines_map.find (name);

    if (iter != splines_map.end ())
      return iter->second;

    math::tcb_splinef return_value;

    return_value.add_key (0.f, atof (name));

    printf ("Spline not found, returning new spline\n");

    return return_value;
  }

///Создание узла системы частиц
  Node::Pointer CreateParticleSystem (const char* id, Node& particles_parent)
  {
    ParticleSystemDescsMap::const_iterator iter = particle_system_descs_map.find (id);

    if (iter == particle_system_descs_map.end ())
      throw xtl::make_argument_exception ("ParticleSystemsManager::Impl::createParticleSystem", "id", id, "Particle system was not loaded");

    return ParticleSystem::Create (iter->second, particles_parent);
  }
};

/*
   Конструктор/деструктор
*/

ParticleSystemsManager::ParticleSystemsManager ()
  : impl (new Impl)
  {}

ParticleSystemsManager::~ParticleSystemsManager ()
{
  delete impl;
}

/*
   Загрузка описаний систем частиц
*/

void ParticleSystemsManager::LoadParticleSystems (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("ParticleSystemsManager::LoadParticleSystems", "file_name");

  impl->LoadParticleSystems (file_name);
}

/*
   Создание узла системы частиц
*/

Node::Pointer ParticleSystemsManager::CreateParticleSystem (const char* id, Node& particles_parent)
{
  if (!id)
    throw xtl::make_null_argument_exception ("ParticleSystemsManager::CreateParticleSystem", "id");

  return impl->CreateParticleSystem (id, particles_parent);
}
