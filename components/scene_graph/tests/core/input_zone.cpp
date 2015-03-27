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

template <int I> void notify (InputZoneNotification notification_id, const InputZoneNotificationContext&)
{
  printf ("notification%u: %s\n", I, get_name (notification_id));
}

int main ()
{
  printf ("Results of zone_test:\n");
  
  InputZone::Pointer zone (InputZone::Create ());

  zone->SetPlaneState (NodeOrt_X, true);
  zone->SetPlaneState (NodeOrt_Y, true);

  MyVisitor visitor;

  zone->VisitEach (visitor, NodeTraverseMode_TopToBottom);  
  
  zone->RegisterNotificationHandler (xtl::bind (&notify<0>, _3, _4));
  zone->RegisterNotificationHandler (InputZoneNotification_OnClick, xtl::bind (&notify<1>, _3, _4));
  
  Viewport viewport;
  
  InputZoneNotificationContext context;
  
  zone->Notify (viewport, InputZoneNotification_OnClick, context);
  zone->Notify (viewport, InputZoneNotification_OnPress, context);

  return 0;
}
