#ifndef SYSLIB_IPHONE_PLATFORM_SHARED_HEADER
#define SYSLIB_IPHONE_PLATFORM_SHARED_HEADER

#include <cfloat>
#include <ctime>

#include <CoreFoundation/CFRunLoop.h>

#include <stl/algorithm>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <syslib/application.h>
#include <syslib/application_delegate.h>
#include <syslib/timer.h>

#include <syslib/platform/iphone.h>

#include <shared/platform.h>

#import <Foundation/NSObject.h>

#import <UIKit/UIAccelerometer.h>
#import <UIKit/UIApplication.h>

namespace syslib
{

bool is_in_run_loop (); //запущен ли главный цикл

}

@class ApplicationDelegateInternal;

@interface ApplicationDelegate : NSObject
{
  @private
    ApplicationDelegateInternal* impl;
}

-(void)setMainViewVisible:(bool)state;
-(bool)isMainViewVisible;

@end

#endif
