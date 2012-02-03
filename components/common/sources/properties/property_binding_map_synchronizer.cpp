#include "shared.h"

using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' used in base member initializer list
#endif

/*
    �������� ���������� ��������������
*/

struct PropertyBindingMapSynchronizer::Impl: public xtl::reference_counter
{
  struct BindingDesc
  {
    PropertyBinding binding;        //����������
    size_t          property_index; //������ �������� � ����� �������
    
    BindingDesc (const PropertyBinding& in_binding, size_t in_property_index)
      : binding (in_binding)
      , property_index (in_property_index)
    {
    }
  };
  
  typedef stl::vector<BindingDesc> BindingDescArray;

  BindingDescArray      bindings;                        //����������
  PropertyBindingMap    binding_map;                     //����� ����������
  PropertyMap           property_map;                    //����� �������
  bool                  need_update_indices;             //��������� ���������� ��������
  xtl::auto_connection  binding_map_update_connection;   //���������� � ����������� �� ���������� ����� ����������
  xtl::auto_connection  property_map_update_connection;  //���������� � ����������� �� ���������� ����� �������

///�����������
  Impl (PropertyBindingMap& in_binding_map, PropertyMap& in_property_map)
    : binding_map (in_binding_map)
    , property_map (in_property_map)
    , need_update_indices (true)
    , binding_map_update_connection (binding_map.RegisterEventHandler (PropertyBindingMapEvent_OnLayoutUpdate, xtl::bind (&Impl::OnUpdate, this)))
    , property_map_update_connection (property_map.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&Impl::OnUpdate, this)))
  {
  }
  
///���������� ��������
  void Update ()
  {
    if (!need_update_indices)
      return;
      
      //������� ����
      
    bindings.clear ();
    
      //������ ������� ����
      
    size_t bindings_count = 0;
    
    for (size_t i=0, count=binding_map.Size (); i<count; i++)
    {
      const char* property_name  = binding_map.PropertyName (i);
      
      if (property_map.IndexOf (property_name) != -1)
        bindings_count++;
    }        
    
      //���������� ����
      
    bindings.reserve (bindings_count);
          
    for (size_t i=0, count=binding_map.Size (); i<count; i++)
    {
      const char* property_name  = binding_map.PropertyName (i);
      int         property_index = property_map.IndexOf (property_name);
      
      if (property_index == -1)
        continue;
      
      bindings.push_back (BindingDesc (binding_map.PropertyBinding (i), property_index));
    }
      
    need_update_indices = false;
  }
  
///���������� ����� ���������� ��� ����� �������
  void OnUpdate ()
  {
    need_update_indices = true;
  }
  
///�������������
  struct SyncFromPropertyMap { void operator () (common::PropertyBinding& binding, const common::PropertyMap& map, size_t index) const { binding.CopyFromPropertyMap (map, index); } };
  struct SyncToPropertyMap   { void operator () (const common::PropertyBinding& binding, common::PropertyMap& map, size_t index) const { binding.CopyToPropertyMap (map, index); } };  

  template <class Fn> void Synchronize ()
  {
    Update ();
    
    Fn fn;
    
    stl::auto_ptr<stl::string> error_string;
    size_t                     errors_count = 0;
    
    for (BindingDescArray::iterator iter=bindings.begin (), end=bindings.end (); iter!=end; ++iter)
    {
      BindingDesc& desc = *iter;
      
      try
      {
        fn (desc.binding, property_map, desc.property_index);
      }
      catch (std::exception& e)
      {
        if (!error_string)
          error_string.reset (new stl::string);
          
        *error_string += common::format ("  %u) exception at set property '%s': ", errors_count++, property_map.PropertyName (desc.property_index));
        *error_string += e.what ();
      }
      catch (...)
      {
        if (!error_string)
          error_string.reset (new stl::string);        

        *error_string += common::format ("  %u) unknown exception at set property '%s'\n", errors_count++, property_map.PropertyName (desc.property_index));
      }
    }    
    
    if (error_string)
      throw xtl::format_operation_exception ("", "Property bindings synchronization exception. Errors:\n%s", error_string->c_str ());
  }  
};

/*
    ������������/ ���������� / ������������
*/

PropertyBindingMapSynchronizer::PropertyBindingMapSynchronizer (PropertyBindingMap& binding_map, PropertyMap& property_map)
  : impl (new Impl (binding_map, property_map))
{
}

PropertyBindingMapSynchronizer::PropertyBindingMapSynchronizer (const PropertyBindingMapSynchronizer& sync)
  : impl (sync.impl)
{
  addref (impl);
}

PropertyBindingMapSynchronizer::~PropertyBindingMapSynchronizer ()
{
  release (impl);
}

PropertyBindingMapSynchronizer& PropertyBindingMapSynchronizer::operator = (const PropertyBindingMapSynchronizer& sync)
{
  PropertyBindingMapSynchronizer (sync).Swap (*this);
  
  return *this;
}

/*
    ������������� � ������ �������
*/

void PropertyBindingMapSynchronizer::CopyFromPropertyMap () const
{
  try
  {
    impl->Synchronize<Impl::SyncFromPropertyMap> ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMapSynchronizer::CopyFromPropertyMap");
    throw;
  }
}

void PropertyBindingMapSynchronizer::CopyToPropertyMap () const
{
  try
  {
    impl->Synchronize<Impl::SyncToPropertyMap> ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyBindingMapSynchronizer::CopyToPropertyMap");
    throw;
  }
}

/*
    �����
*/

void PropertyBindingMapSynchronizer::Swap (PropertyBindingMapSynchronizer& sync)
{
  stl::swap (impl, sync.impl);
}

namespace common
{

void swap (PropertyBindingMapSynchronizer& sync1, PropertyBindingMapSynchronizer& sync2)
{
  sync1.Swap (sync2);
}

}
