#include "shared.h"

class MyVisitor: public visitor<void, InputZone>
{
  public:
    void visit (InputZone& zone)
    {
      printf ("InputZone '%s': \n", zone.Name ());
      printf ("  active: %s\n", zone.IsActive () ? "true" : "false");
      printf ("  descs (count=%u):\n", zone.ZoneDescsCount ());
      
      for (size_t i=0; i<zone.ZoneDescsCount (); i++)
      {
        const InputZoneModel::ZoneDesc& desc = zone.ZoneDescs () [i];
        
        printf ("    position: [%.2f %.2f %.2f]\n", desc.position.x, desc.position.y, desc.position.z);
        printf ("    axis_x:   [%.2f %.2f %.2f]\n", desc.axis_x.x, desc.axis_x.y, desc.axis_x.z);
        printf ("    axis_y:   [%.2f %.2f %.2f]\n", desc.axis_y.x, desc.axis_y.y, desc.axis_y.z);        
      }
    }
};

template <int I> void notify (const char* notification_id, const char* params)
{
  printf ("notification%u: '%s' '%s'\n", I, notification_id, params);
}

int main ()
{
  printf ("Results of zone_test:\n");
  
  InputZone::Pointer zone (InputZone::Create ());

  MyVisitor visitor;

  zone->VisitEach (visitor, NodeTraverseMode_TopToBottom);  
  
  zone->RegisterNotificationHandler (xtl::bind (&notify<0>, _3, _4));    
  zone->RegisterNotificationHandler ("my_notif", xtl::bind (&notify<1>, _3, _4));
  
  Viewport viewport;
  
  zone->Notify (viewport, "my_notif", "1 2");
  zone->Notify (viewport, "my_notif1", "1 2");  

  return 0;
}
