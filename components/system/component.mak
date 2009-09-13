###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

#Цель №1 - System library sources
SYSTEMLIB.SOURCES.TYPE                              := static-lib
SYSTEMLIB.SOURCES.NAME                              := funner.system
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      := sources
SYSTEMLIB.SOURCES.SOURCE_DIRS                       := sources/wrappers
SYSTEMLIB.SOURCES.IMPORTS                           := compile.static.mak
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS                 := sources/platform/win32
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS                := sources/platform/pthread sources/platform/unistd
SYSTEMLIB.SOURCES.msvc.COMPILER_CFLAGS              := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS            := sources/platform/no_windows
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS                := sources/platform/carbon
SYSTEMLIB.SOURCES.carbon.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.carbon.COMPILER_CFLAGS            := -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/Carbon.framework/Frameworks/HIToolbox.framework/Headers/ \
                                                       -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/
SYSTEMLIB.SOURCES.iphone.SOURCE_DIRS                := sources/platform/iphone
SYSTEMLIB.SOURCES.iphone.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.iphone.COMPILER_CFLAGS            := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/CoreFoundation.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/Foundation.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/UIKit.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/QuartzCore.framework/Headers/
SYSTEMLIB.SOURCES.x11.SOURCE_DIRS                   := sources/platform/x11

#Цель №2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers tests/thread
SYSTEMLIB.TESTS.IMPORTS          := compile.static.mak link.static.mak
