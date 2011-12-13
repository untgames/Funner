#include <stdio.h>

#include "geekinfo.h"
#include "smbios.h"

int main()
{
  printf ("Running geekinfo_uni_test:\n");

  for (int i = (int)kSystemMetricPlatform; i != kSystemMetricCount; i++) {        
    systemMetricName((SystemMetricType)i);    
  }  
  
  printf ("Done\n");
  
  return 0;
}
