#include "shared.h"

using namespace media::animation;

/*
    ќписание реализации блендера анимационных целей
*/

namespace
{

///»сточник
struct Source: public xtl::reference_counter, public xtl::trackable
{
  size_t               property_index;               //индекс свойства
  xtl::auto_connection blender_on_remove_connection; //соединение с событием удалени€ канала из блендера
  xtl::auto_connection blender_on_add_connection;    //соединение с событием добавлени€ канала в блендер

/// онструктор
  Source (size_t in_property_index) : property_index (in_property_index) {}
  
///ѕолучение базового блендера каналов
  virtual ChannelBlenderBase& Blender () = 0;
  
///ќбновление канала
  virtual void Update (common::PropertyMap& properties) = 0;
};

template <class T> const T& property_cast (const T& value, xtl::type<T>)
{
  return value;
}

template <class T, class CastT> CastT property_cast (const T& value, xtl::type<CastT>)
{
  return static_cast<CastT> (value);
}

template <class T> math::matrix<T, 4> property_cast (const math::matrix<T, 2>& src, xtl::type<math::matrix<T, 4> >)
{
  math::matrix<T, 4> result;
  
  for (size_t i=0; i<2; i++)
    for (size_t j=0; j<2; j++)  
      result [i][j] = src [i][j];

  return result;
}

template <class T> math::matrix<T, 4> property_cast (const math::matrix<T, 3>& src, xtl::type<math::matrix<T, 4> >)
{
  math::matrix<T, 4> result;

  for (size_t i=0; i<3; i++)
    for (size_t j=0; j<3; j++)  
      result [i][j] = src [i][j];

  return result;
}

template <class T> const math::vector<T, 4>& property_cast (const math::quat<T>& value, xtl::type<math::vector<T, 4> >)
{
  return reinterpret_cast<const math::vector<T, 4>&> (value);
}

typedef xtl::intrusive_ptr<Source> SourcePtr;

template <class T, class PropertyType> struct SourceImpl: public Source
{
  ChannelBlender<T> blender; //блендер каналов
  
/// онструктор
  SourceImpl (size_t property_index) : Source (property_index) {}
  
///—оздание канала
  static SourcePtr Create (size_t property_index)
  {
    return new SourceImpl (property_index);
  }
  
///ѕолучение базового блендера каналов
  ChannelBlenderBase& Blender ()
  {
    return blender;
  }

///ќбновление канала  
  void Update (common::PropertyMap& properties)
  {
    properties.SetProperty (property_index, property_cast (blender (), xtl::type<PropertyType> ()));
  }
};

typedef stl::hash_map<stl::hash_key<const char*>, SourcePtr> SourceMap;

}

struct TargetBlender::Impl: public xtl::reference_counter
{
  typedef xtl::signal<void (TargetBlenderEvent)> Signal;

  SourceMap           sources;                          //источники
  common::PropertyMap properties;                       //свойства
  Signal              signals [TargetBlenderEvent_Num]; //сигналы о создании и удалении каналов
  size_t              sources_count;                    //общее количество источников
  bool                need_update_sources_count;        //флаг необходимости обновлени€ общего числа источников
  
/// онструктор
  Impl ()
    : sources_count (0)
    , need_update_sources_count (true)
  {
    //TODO: capture property map???
//    properties.Capture ();
  }
  
///ƒеструктор
  ~Impl ()
  {
    sources.clear ();
    properties.Clear ();
  }
  
///ќповещение о возникновении событи€ канала
  void Notify (TargetBlenderEvent event)
  {
    if (event < 0 || event >= TargetBlenderEvent_Num)
      return;
      
    try
    {
      signals [event](event);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  
///ƒобавление источника
  void AddSource (AnimationState& state, const Channel& channel, const char* property_name)
  {
      //проверка корректности аргументов
    
    static const char* METHOD_NAME = "media::animation::TargetBlender::Impl::AddSource";
    
    if (!property_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");
      
      //определение типа свойства    
      
    typedef SourcePtr (*SourceCreator)(size_t property_index);

    struct Type2PropertyType
    {
      const std::type_info& type;
      SourceCreator         creator;
      common::PropertyType  property_type;      
    };
    
    static const Type2PropertyType types [] = {
      {typeid (signed char),     &SourceImpl<signed char, int>::Create,          common::PropertyType_Integer},
      {typeid (unsigned char),   &SourceImpl<unsigned char, int>::Create,        common::PropertyType_Integer},
      {typeid (signed short),    &SourceImpl<signed short, int>::Create,         common::PropertyType_Integer},
      {typeid (unsigned short),  &SourceImpl<unsigned short, int>::Create,       common::PropertyType_Integer},
      {typeid (signed int),      &SourceImpl<signed int, int>::Create,           common::PropertyType_Integer},
      {typeid (unsigned int),    &SourceImpl<unsigned int, int>::Create,         common::PropertyType_Integer},
      {typeid (signed long),     &SourceImpl<signed long, int>::Create,          common::PropertyType_Integer},
      {typeid (unsigned long),   &SourceImpl<unsigned long, int>::Create,        common::PropertyType_Integer},
      {typeid (float),           &SourceImpl<float, float>::Create,              common::PropertyType_Float},
      {typeid (double),          &SourceImpl<double, float>::Create,             common::PropertyType_Float},
      {typeid (long double),     &SourceImpl<long double, float>::Create,        common::PropertyType_Float},
      {typeid (math::vec2f),     &SourceImpl<math::vec2f, math::vec4f>::Create,  common::PropertyType_Vector},
      {typeid (math::vec3f),     &SourceImpl<math::vec3f, math::vec4f>::Create,  common::PropertyType_Vector},
      {typeid (math::vec4f),     &SourceImpl<math::vec4f, math::vec4f>::Create,  common::PropertyType_Vector},
      {typeid (math::mat2f),     &SourceImpl<math::mat2f, math::mat4f>::Create,  common::PropertyType_Matrix},
      {typeid (math::mat3f),     &SourceImpl<math::mat3f, math::mat4f>::Create,  common::PropertyType_Matrix},
      {typeid (math::mat4f),     &SourceImpl<math::mat4f, math::mat4f>::Create,  common::PropertyType_Matrix},
      {typeid (math::quatf),     &SourceImpl<math::quatf, math::vec4f>::Create,  common::PropertyType_Vector},
    };

    static const size_t types_count = sizeof (types) / sizeof (*types);
    
    const std::type_info& type = channel.ValueType ();
    
    SourceCreator creator = 0;
    
    common::PropertyType property_type = common::PropertyType_Num;
    
    for (size_t i=0; i<types_count; i++)
      if (&types [i].type == &type)
      {
        creator       = types [i].creator;
        property_type = types [i].property_type;

        break;
      }
      
    if (!creator || property_type == common::PropertyType_Num)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Source channel type '%s' is not supported", type.name ());

      //создание канала
      
    SourcePtr source;
      
    SourceMap::iterator iter = sources.find (property_name);
    
    if (iter != sources.end ())
    {
      source = iter->second;

        //добавление канала блендинга

      ChannelBlenderBase& blender = source->Blender ();

      blender.AddSource (state, channel);
    }
    else
    {
      size_t property_index = properties.AddProperty (property_name, property_type);
      
      try
      {
        source = creator (property_index);
        
        source->blender_on_remove_connection = source->Blender ().RegisterEventHandler (ChannelBlenderEvent_OnSourcesRemoved, xtl::bind (&Impl::OnChannelsRemoved, this, &*source));
        source->blender_on_add_connection    = source->Blender ().RegisterEventHandler (ChannelBlenderEvent_OnSourcesAdded, xtl::bind (&Impl::OnChannelsAdded, this));
        
        source->connect_tracker (xtl::bind (&Impl::RemoveSourceByName, this, stl::hash_key<const char*> (property_name)));
        
        sources.insert_pair (property_name, source);
        
          //добавление канала блендинга

        ChannelBlenderBase& blender = source->Blender ();

        blender.AddSource (state, channel);
      }
      catch (...)
      {
        sources.erase (property_name);
        properties.RemoveProperty (property_index);
        throw;
      }                  
    }    
  }    
  
///ќбработка событи€ добавлени€ канала
  void OnChannelsAdded ()
  {
    Notify (TargetBlenderEvent_OnSourcesChanged);

    need_update_sources_count = true;
  }  
    
///ќбработка событи€ удалени€ канала
  void OnChannelsRemoved (Source* source)
  {
    need_update_sources_count = true;

    if (!source)
      return;        

    ChannelBlenderBase& blender = source->Blender ();

    if (!blender.SourcesCount ())
    {
      RemoveSourceByName (properties.PropertyName (source->property_index));
    }
    else
    {      
      Notify (TargetBlenderEvent_OnSourcesChanged);
    }    
  }
  
///”даление источника по имени свойства
  void RemoveSourceByName (const stl::hash_key<const char*>& key)
  {
    SourceMap::iterator iter = sources.find (key);

    if (iter == sources.end ())
      return; 
      
    need_update_sources_count = true;

    size_t property_index = iter->second->property_index;

    for (SourceMap::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
      if (iter->second->property_index > property_index)
        iter->second->property_index--;

    properties.RemoveProperty (property_index);    

    sources.erase (key);
    
    Notify (TargetBlenderEvent_OnSourcesChanged);

    if (sources.empty ())
      Notify (TargetBlenderEvent_OnSourcesEmpty);
  }

///ќбновление количества источников
  void UpdateSourcesCount ()
  {
    if (!need_update_sources_count)
      return;

    sources_count = 0;

    for (SourceMap::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
      sources_count += iter->second->Blender ().SourcesCount ();

    need_update_sources_count = false;
  }
};

/*
     онструкторы / деструктор / присваивание
*/

TargetBlender::TargetBlender ()
  : impl (new Impl)
{
}

TargetBlender::TargetBlender (const TargetBlender& blender)
  : impl (blender.impl)
{
  addref (impl);
}

TargetBlender::~TargetBlender ()
{
  release (impl);
}

TargetBlender& TargetBlender::operator = (const TargetBlender& blender)
{
  TargetBlender (blender).Swap (*this);
  return *this;
}

/*
    ќбщее количество источников
*/

size_t TargetBlender::SourcesCount () const
{
  impl->UpdateSourcesCount ();
  
  return impl->sources_count;
}

/*
    ƒобавление и удаление источников
*/

void TargetBlender::AddSource (AnimationState& state, const Channel& channel)
{
  try
  {
    impl->AddSource (state, channel, channel.ParameterName ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::TargetBlender::AddSource(AnimationState&,const Channel&)");
    throw;
  }
}

void TargetBlender::AddSource (AnimationState& state, const Channel& channel, const PropertyNameMap& name_map)
{
  try
  {
    const char* property_name = name_map.Get (channel.ParameterName ());
    
    if (!property_name || !*property_name)
      property_name = channel.ParameterName ();
    
    impl->AddSource (state, channel, property_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::TargetBlender::AddSource(AnimationState&,const Channel&,const PropertyNameMap&)");
    throw;
  }
}

void TargetBlender::RemoveSource (AnimationState& state)
{
  for (SourceMap::iterator iter=impl->sources.begin (); iter!=impl->sources.end ();)
  {
    SourceMap::iterator next = iter;
    
    ++next;
    
    iter->second->Blender ().RemoveSources (state);
    
    iter = next;
  }
}

void TargetBlender::RemoveSource (const Channel& channel)
{
  for (SourceMap::iterator iter=impl->sources.begin (); iter!=impl->sources.end ();)
  {
    SourceMap::iterator next = iter;
    
    ++next;
    
    iter->second->Blender ().RemoveSources (channel);
    
    iter = next;
  }
}

void TargetBlender::RemoveSource (const char* property_name)
{
  if (!property_name)
    return;
    
  impl->RemoveSourceByName (property_name);  
}

void TargetBlender::RemoveAllSources ()
{
  impl->sources.clear ();
  impl->properties.Clear ();
}

/*
    “екущее состо€ние свойств анимационной цели
*/

const common::PropertyMap& TargetBlender::Properties () const
{
  return impl->properties;
}

/*
    ќбновление состо€ни€ анимационной цели
*/

void TargetBlender::Update ()
{
  try
  {
    stl::auto_ptr<stl::string> error_string;
    size_t                     errors_count = 0;

    for (SourceMap::iterator iter=impl->sources.begin (), end=impl->sources.end (); iter!=end; ++iter)
    {
      try
      {
        iter->second->Update (impl->properties);
      }
      catch (std::exception& e)
      {
        if (!error_string)
          error_string.reset (new stl::string);

        *error_string += common::format ("  %u) exception at update property '%s': %s", ++errors_count, impl->properties.PropertyName (iter->second->property_index), e.what ());
      }
      catch (...)
      {
        if (!error_string)
          error_string.reset (new stl::string);

        *error_string += common::format ("  %u) unknown exception at update property '%s'\n", ++errors_count, impl->properties.PropertyName (iter->second->property_index));
      }      
    }
    
    if (error_string)
      throw xtl::format_operation_exception ("", "Animation target blending update exception. Errors:\n%s", error_string->c_str ());
  }  
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::TargetBlender::Update");
    throw;
  }
}

/*
    ѕодписка на событи€
*/

xtl::connection TargetBlender::RegisterEventHandler (TargetBlenderEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= TargetBlenderEvent_Num)
    throw xtl::make_argument_exception ("media::animation::TargetBlender::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
   ќбмен
*/

void TargetBlender::Swap (TargetBlender& blender)
{
  stl::swap (impl, blender.impl);
}

namespace media
{

namespace animation
{

void swap (TargetBlender& blender1, TargetBlender& blender2)
{
  blender1.Swap (blender2);
}

}

}
