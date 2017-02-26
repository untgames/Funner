#include "shared.h"

using namespace common;
using namespace media::particles;
using namespace media::particles::particle_designer_serialzer;

namespace
{

//Constants
const float DURATION_INFINITY                = -1;  //The Particle emitter lives forever.
const float START_SIZE_EQUAL_TO_END_SIZE     = -1;  //The starting size of the particle is equal to the ending size.
const float START_RADIUS_EQUAL_TO_END_RADIUS = -1;  //The starting radius of the particle is equal to the ending radius.


//supported emitter modes
enum EmitterMode
{
  EmitterMode_Gravity,
  EmitterMode_Radius
};

//data common for all emitter modes
struct EmitterData
{
  int         max_particles;            //maximum particles of the system
  int         y_coord_flipped;          //direction and position.y multiplier
  float       start_angle;              //direction of particle
  float       start_angle_variance;     //direction variance of particle
  float       duration;                 //how many seconds the emitter will run. -1 means 'forever'
  float       start_size;               //start size of each particle
  float       start_size_variance;      //start size variance of each particle
  float       finish_size;              //finish size of each particle
  float       finish_size_variance;     //finish size variance of each particle
  float       start_rotation;           //initial angle of particle
  float       start_rotation_variance;  //initial angle variance of particle
  float       finish_rotation;          //finish angle of particle
  float       finish_rotation_variance; //finish angle variance of particle
  float       lifespan;                 //life of particle
  float       lifespan_variance;        //life variance of particle
  math::vec4f start_color;              //initial color of particle
  math::vec4f start_color_variance;     //initial color variance of particle
  math::vec4f finish_color;             //finish color of particle
  math::vec4f finish_color_variance;    //finish color variance of particle
};

typedef stl::auto_ptr<EmitterData> EmitterDataPtr;

//data specific for gravity emitter
struct GravityEmitterData : public EmitterData
{
  bool        rotation_is_dir;                  //if this is true direction is used as initial rotation of particle
  float       speed;                            //gravity speed
  float       speed_variance;                   //gravity speed variance
  float       radial_acceleration;              //gravity radial acceleration
  float       radial_acceleration_variance;     //gravity radial acceleration variance
  float       tangential_acceleration;          //gravity tangential acceleration
  float       tangential_acceleration_variance; //gravity tangential acceleration variance
  math::vec2f gravity;                          //gravity force
  math::vec2f start_position;                   //source position
  math::vec2f start_position_variance;          //source position variance
};

//data specific for radius emitter
struct RadiusEmitterData : public EmitterData
{
  float start_radius;           //the starting radius of the particles
  float start_radius_variance;  //the starting radius variance of the particles
  float finish_radius;          //the ending radius of the particles
  float finish_radius_variance; //the ending radius variance of the particles
  float rotate;                 //number of degrees to rotate a particle around the source pos per second
  float rotate_variance;        //variance in degrees for rotate
};

//particle data specific for gravity mode
struct GravityModeParticleData
{
  float radial_acceleration;     //gravity radial acceleration
  float tangential_acceleration; //gravity tangential acceleration
};

//particle data specific for radius mode
struct RadiusModeParticleData
{
  float        radius;       //radius
  float        radius_speed; //radius change speed
  math::anglef angle;        //angle
  math::anglef angle_speed;  //angle change speed
};

//Particle designer-specific particle data
struct ParticleData : public Particle
{
  GravityModeParticleData gravity;  //gravity mode data
  RadiusModeParticleData  radius;   //radius mode data
};

//clamp value
inline float clamp (float value, float min, float max)
{
  return stl::min (stl::max (value, min), max);
}

//get float property or default value 0.f if such property doesn't exist
float get_float_property (const common::PropertyMap& properties, const char* property_name)
{
  if (properties.IsPresent (property_name))
    return properties.GetFloat (property_name);

  return 0.f;
}

//get integer property or default value 0 if such property doesn't exist
int get_integer_property (const common::PropertyMap& properties, const char* property_name)
{
  if (properties.IsPresent (property_name))
    return properties.GetInteger (property_name);

  return 0;
}

}

/*
   Implementation of particle designer particle processor
*/

struct ParticleProcessor::Impl
{
  EmitterMode    emitter_mode;      //mode of emitter
  EmitterDataPtr emitter_data;      //emitter data (can contain)
  float          emission_interval; //interval beetween particle emission
  TimeValue      emit_counter;      //counter used to emit new particles

  ///Set parameters
  void SetParameters (const common::PropertyMap& parameters)
  {
    static const char* METHOD_NAME = "media::particles::particle_designer_serialzer::ParticleProcessor::SetParameters";

    //read required data
    emitter_mode = (EmitterMode)parameters.GetInteger ("emitterType");

    switch (emitter_mode)
    {
      case EmitterMode_Gravity:
        emitter_data = new GravityEmitterData ();
        break;
      case EmitterMode_Radius:
        emitter_data = new RadiusEmitterData ();
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported emitter mode '%d'", emitter_mode);
    }

    EmitterData& data = *emitter_data;

    data.max_particles             = parameters.GetInteger ("maxParticles");
    data.start_angle               = parameters.GetFloat   ("angle");
    data.start_angle_variance      = parameters.GetFloat   ("angleVariance");
    data.duration                  = parameters.GetFloat   ("duration");
    data.start_color.x             = parameters.GetFloat   ("startColorRed");
    data.start_color.y             = parameters.GetFloat   ("startColorGreen");
    data.start_color.z             = parameters.GetFloat   ("startColorBlue");
    data.start_color.w             = parameters.GetFloat   ("startColorAlpha");
    data.start_color_variance.x    = parameters.GetFloat   ("startColorVarianceRed");
    data.start_color_variance.y    = parameters.GetFloat   ("startColorVarianceGreen");
    data.start_color_variance.z    = parameters.GetFloat   ("startColorVarianceBlue");
    data.start_color_variance.w    = parameters.GetFloat   ("startColorVarianceAlpha");
    data.finish_color.x            = parameters.GetFloat   ("finishColorRed");
    data.finish_color.y            = parameters.GetFloat   ("finishColorGreen");
    data.finish_color.z            = parameters.GetFloat   ("finishColorBlue");
    data.finish_color.w            = parameters.GetFloat   ("finishColorAlpha");
    data.finish_color_variance.x   = parameters.GetFloat   ("finishColorVarianceRed");
    data.finish_color_variance.y   = parameters.GetFloat   ("finishColorVarianceGreen");
    data.finish_color_variance.z   = parameters.GetFloat   ("finishColorVarianceBlue");
    data.finish_color_variance.w   = parameters.GetFloat   ("finishColorVarianceAlpha");
    data.start_size                = parameters.GetFloat   ("startParticleSize");
    data.start_size_variance       = parameters.GetFloat   ("startParticleSizeVariance");
    data.finish_size               = parameters.GetFloat   ("finishParticleSize");
    data.finish_size_variance      = parameters.GetFloat   ("finishParticleSizeVariance");
    data.start_rotation            = parameters.GetFloat   ("rotationStart");
    data.start_rotation_variance   = parameters.GetFloat   ("rotationStartVariance");
    data.finish_rotation           = parameters.GetFloat   ("rotationEnd");
    data.finish_rotation_variance  = parameters.GetFloat   ("rotationEndVariance");
    data.lifespan                  = parameters.GetFloat   ("particleLifespan");
    data.lifespan_variance         = parameters.GetFloat   ("particleLifespanVariance");
    data.y_coord_flipped           = parameters.IsPresent  ("yCoordFlipped") ? parameters.GetInteger ("yCoordFlipped") : 1;

    if (emitter_mode == EmitterMode_Gravity)
    {
      GravityEmitterData& gravity_data = *(GravityEmitterData*)emitter_data.get ();

      gravity_data.start_position.x                 = get_float_property   (parameters, "sourcePositionx");
      gravity_data.start_position.y                 = get_float_property   (parameters, "sourcePositiony");
      gravity_data.start_position_variance.x        = parameters.GetFloat  ("sourcePositionVariancex");
      gravity_data.start_position_variance.y        = parameters.GetFloat  ("sourcePositionVariancey");
      gravity_data.gravity.x                        = parameters.GetFloat  ("gravityx");
      gravity_data.gravity.y                        = parameters.GetFloat  ("gravityy");
      gravity_data.speed                            = parameters.GetFloat  ("speed");
      gravity_data.speed_variance                   = parameters.GetFloat  ("speedVariance");
      gravity_data.radial_acceleration              = parameters.GetFloat  ("radialAcceleration");
      gravity_data.radial_acceleration_variance     = parameters.GetFloat  ("radialAccelVariance");
      gravity_data.tangential_acceleration          = parameters.GetFloat  ("tangentialAcceleration");
      gravity_data.tangential_acceleration_variance = parameters.GetFloat  ("tangentialAccelVariance");
      gravity_data.rotation_is_dir                  = get_integer_property (parameters, "rotationIsDir") != 0;
    }
    else  //Radius
    {
      RadiusEmitterData& radius_data = *(RadiusEmitterData*)emitter_data.get ();

      radius_data.start_radius           = parameters.GetFloat ("maxRadius");
      radius_data.start_radius_variance  = parameters.GetFloat ("maxRadiusVariance");
      radius_data.finish_radius          = parameters.GetFloat ("minRadius");
      radius_data.finish_radius_variance = parameters.IsPresent ("minRadiusVariance") ? parameters.GetFloat ("minRadiusVariance") : 0.f;
      radius_data.rotate                 = parameters.GetFloat ("rotatePerSecond");
      radius_data.rotate_variance        = parameters.GetFloat ("rotatePerSecondVariance");
    }  //other cases is not possible (handled above)

    //pre-process data
    emission_interval = data.lifespan / data.max_particles;
  }

  //Process scene
  void Process (ParticleScene& scene, const RandomGenerator& random_generator)
  {
    TimeValue work_time = scene.Time () - scene.StartTimeOffset ();

    if (work_time < 0)
      return;

    TimeValue rational_dt = scene.Time () - stl::max (scene.PrevUpdateTime (), scene.StartTimeOffset ());

    if (rational_dt <= 0)
      return;

    ParticleList& particles = scene.Particles ();

    size_t particle_count = particles.Count ();

    //generate new particles
    if ((emitter_data->duration == DURATION_INFINITY || emitter_data->duration > work_time.cast<float> ()) && particle_count < emitter_data->max_particles)
    {
      emit_counter += rational_dt;

      if (emit_counter < 0)
        emit_counter = 0;

      int emit_count = stl::min ((int)(emitter_data->max_particles - particle_count), (int)(emit_counter.cast<float> () / emission_interval));

      AddParticles (particles, emit_count, random_generator);

      emit_counter -= (size_t)(emission_interval * emit_count);
    }

    //process particles
    if (emitter_mode == EmitterMode_Gravity)
    {
      float               y_coord_flipped = (float)emitter_data->y_coord_flipped;
      GravityEmitterData& gravity_data    = *(GravityEmitterData*)emitter_data.get ();

      for (ParticleList::Iterator iter = particles.CreateIterator (); iter; ++iter)
      {
        ParticleData& particle = static_cast<ParticleData&> (*iter);

        // radial acceleration
        if (particle.position.x || particle.position.y)
        {
          math::vec2f radial     = math::normalize (math::vec2f (particle.position.x, particle.position.y));
          math::vec2f tangential = math::vec2f (-radial.y * particle.gravity.tangential_acceleration, radial.x * particle.gravity.tangential_acceleration);

          radial *= particle.gravity.radial_acceleration;

          particle.position_acceleration += (radial + tangential + gravity_data.gravity) * y_coord_flipped;
        }
        else
          particle.position_acceleration += gravity_data.gravity * y_coord_flipped;
      }
    }
    else
    {
      float dt = rational_dt.cast<float> ();

      //Use acceleration_dt as arrive to target point time for acceleration calculation. We should use lowest possible value for this param.
      //Best value of dt * 1.4 was determined during testing of different angle / radius speeds and different dt as minimal possible which does not
      //result in calculations going to infinity
      float acceleration_dt        = dt * 1.4f;
      float acceleration_dt_square = acceleration_dt * acceleration_dt;

      for (ParticleList::Iterator iter = particles.CreateIterator (); iter; ++iter)
      {
        ParticleData& particle = static_cast<ParticleData&> (*iter);

        particle.radius.angle  += particle.radius.angle_speed * dt;
        particle.radius.radius += particle.radius.radius_speed * dt;

        particle.position_acceleration.x += 2 * (math::cos (particle.radius.angle) * particle.radius.radius - particle.position.x - particle.position_speed.x * acceleration_dt) / acceleration_dt_square;
        particle.position_acceleration.y += 2 * (math::sin (particle.radius.angle) * particle.radius.radius - particle.position.y - particle.position_speed.y * acceleration_dt) / acceleration_dt_square;
      }
    }
  }

  //Add new particles
  void AddParticles (ParticleList& list, int count, const RandomGenerator& random_generator)
  {
    GravityEmitterData& gravity_data = *(GravityEmitterData*)emitter_data.get ();
    RadiusEmitterData&  radius_data  = *(RadiusEmitterData*)emitter_data.get ();

    for (int i = 0; i < count; i++)
    {
      ParticleList::Iterator iter = list.Add ();

      ParticleData& particle = static_cast<ParticleData&> (*iter);

      particle.lifetime      = stl::max (0.f, emitter_data->lifespan + emitter_data->lifespan_variance * random_generator.Generate ());
      particle.color.x       = clamp (emitter_data->start_color.x + emitter_data->start_color_variance.x * random_generator.Generate (), 0.f, 1.f);
      particle.color.y       = clamp (emitter_data->start_color.y + emitter_data->start_color_variance.y * random_generator.Generate (), 0.f, 1.f);
      particle.color.z       = clamp (emitter_data->start_color.z + emitter_data->start_color_variance.z * random_generator.Generate (), 0.f, 1.f);
      particle.color.w       = clamp (emitter_data->start_color.w + emitter_data->start_color_variance.w * random_generator.Generate (), 0.f, 1.f);
      particle.color_speed.x = (clamp (emitter_data->finish_color.x + emitter_data->finish_color_variance.x * random_generator.Generate (), 0.f, 1.f) - particle.color.x) / particle.lifetime;
      particle.color_speed.y = (clamp (emitter_data->finish_color.y + emitter_data->finish_color_variance.y * random_generator.Generate (), 0.f, 1.f) - particle.color.y) / particle.lifetime;
      particle.color_speed.z = (clamp (emitter_data->finish_color.z + emitter_data->finish_color_variance.z * random_generator.Generate (), 0.f, 1.f) - particle.color.z) / particle.lifetime;
      particle.color_speed.w = (clamp (emitter_data->finish_color.w + emitter_data->finish_color_variance.w * random_generator.Generate (), 0.f, 1.f) - particle.color.w) / particle.lifetime;
      particle.size          = stl::max (emitter_data->start_size + emitter_data->start_size_variance * random_generator.Generate (), 0.f);

      if (emitter_data->finish_size == START_SIZE_EQUAL_TO_END_SIZE)
        particle.size_speed = math::vec2f ();
      else
        particle.size_speed = (stl::max (0.f, emitter_data->finish_size + emitter_data->finish_size_variance * random_generator.Generate ()) - particle.size.x) / particle.lifetime;

      particle.rotation       = math::degree (emitter_data->start_rotation + emitter_data->start_rotation_variance * random_generator.Generate ());
      particle.rotation_speed = (math::degree (emitter_data->finish_rotation + emitter_data->finish_rotation_variance * random_generator.Generate ()) - particle.rotation) / particle.lifetime;

      if (emitter_mode == EmitterMode_Gravity)
      {
        particle.position.x                      = gravity_data.start_position.x + gravity_data.start_position_variance.x * random_generator.Generate ();
        particle.position.y                      = gravity_data.start_position.y + gravity_data.start_position_variance.y * random_generator.Generate ();
        particle.gravity.radial_acceleration     = gravity_data.radial_acceleration + gravity_data.radial_acceleration_variance * random_generator.Generate ();
        particle.gravity.tangential_acceleration = gravity_data.tangential_acceleration + gravity_data.tangential_acceleration_variance * random_generator.Generate ();

        math::anglef a = math::degree(emitter_data->start_angle + emitter_data->start_angle_variance * random_generator.Generate ());

        particle.position_speed = math::vec3f (cos (a), sin (a), 0) * (gravity_data.speed + gravity_data.speed_variance * random_generator.Generate ());

        if (gravity_data.rotation_is_dir)
          particle.rotation = -a; //TODO check if this is correct WHY -a and not a here????
      }
      else  //radius mode
      {
        //Need to check by Jacky
        // Set the default diameter of the particle from the source position
        particle.radius.radius      = radius_data.start_radius + radius_data.start_radius_variance * random_generator.Generate ();
        particle.radius.angle       = math::degree (emitter_data->start_angle + emitter_data->start_angle_variance * random_generator.Generate ());
        particle.radius.angle_speed = math::degree (radius_data.rotate + radius_data.rotate_variance * random_generator.Generate ());
        particle.position.x         = math::cos (particle.radius.angle) * particle.radius.radius;
        particle.position.y         = math::sin (particle.radius.angle) * particle.radius.radius * emitter_data->y_coord_flipped;

        particle.position_speed = math::vec3f ();

        if (radius_data.finish_radius == START_RADIUS_EQUAL_TO_END_RADIUS)
          particle.radius.radius_speed = 0.0f;
        else
          particle.radius.radius_speed = (radius_data.finish_radius + radius_data.finish_radius_variance * random_generator.Generate () - particle.radius.radius) / particle.lifetime;
      }
    }
  }

  //Particle initializer
  void InitParticle (void* particle_data)
  {
    ParticleData& particle = *static_cast<ParticleData*> (particle_data);

    particle.position.z             = 0.f;
    particle.position_acceleration  = math::vec3f ();
    particle.creation_time          = TimeValue (0, -1);
    particle.animation_frame        = 0;
    particle.animation_frame_offset = 0;
  }
};

/*
   Constructor / destructor
*/

ParticleProcessor::ParticleProcessor ()
  : impl (new Impl)
  {}

ParticleProcessor::~ParticleProcessor ()
{
  delete impl;
}

/*
   Set parameters
*/

void ParticleProcessor::SetParameters (const common::PropertyMap& parameters)
{
  impl->SetParameters (parameters);
}

/*
   Attach / detach scene for further processing
*/

void* ParticleProcessor::AttachScene (ParticleScene& scene)
{
  if (!impl->emitter_data)
    return 0;

  scene.Particles ().Pool ().Reserve (impl->emitter_data->max_particles, ParticleSize ());

  //TODO check particle list compatibility

  return 0;
}

void ParticleProcessor::DetachScene (ParticleScene& scene, void* private_data)
{
  //do nothing
}

/*
   Process scene
*/

void ParticleProcessor::Process (ParticleScene& scene, const RandomGenerator& random_generator, void* private_data)
{
  impl->Process (scene, random_generator);
}

/*
   Get particle size
*/

size_t ParticleProcessor::ParticleSize ()
{
  return sizeof (ParticleData);
}

/*
   Particle initializer
*/

void ParticleProcessor::InitParticle (void* particle_data)
{
  impl->InitParticle (particle_data);
}
