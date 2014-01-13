#ifndef PUSH_NOTIFICATIONS_CORE_TESTS_SHARED_HEADER
#define PUSH_NOTIFICATIONS_CORE_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/string>

#include <xtl/function.h>

#include <common/property_map.h>

#include <push_notifications/push_notifications_center.h>

using namespace common;
using namespace push_notifications;

void dump (const PropertyMap& properties)
{
  printf ("    map has %u properties (hash=%08x, layout_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());

  for (size_t i=0, count=properties.Size (); i<count; i++)
  {
    stl::string value;

    properties.GetProperty (i, value);

    printf ("     #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump (const Notification& notification)
{
  printf ("Notification:\n");
  printf ("  properties:\n");
  dump (notification.Properties ());
}

#endif
