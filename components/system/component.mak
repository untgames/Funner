###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

TARGETS.android += SYSTEMLIB.UTILS.ANDROID_LAUNCHER

#Цель №1 - System library sources
SYSTEMLIB.SOURCES.TYPE                              := static-lib
SYSTEMLIB.SOURCES.NAME                              := funner.system
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      := sources
SYSTEMLIB.SOURCES.SOURCE_DIRS                       := sources/wrappers
SYSTEMLIB.SOURCES.IMPORTS                           := compile.system
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS                 := sources/platform/win32 sources/platform/win32/application sources/platform/win32/thread sources/platform/win32/non_unistd
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS                := sources/platform/pthread sources/platform/unistd
SYSTEMLIB.SOURCES.msvc.COMPILER_CFLAGS              := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS            := sources/platform/no_windows
SYSTEMLIB.SOURCES.android.SOURCE_DIRS               := sources/platform/android sources/platform/message_queue
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS                := sources/platform/carbon
SYSTEMLIB.SOURCES.carbon.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.iphone.SOURCE_DIRS                := sources/platform/iphone
SYSTEMLIB.SOURCES.iphone.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.x11.SOURCE_DIRS                   := sources/platform/x11 sources/platform/message_queue
SYSTEMLIB.SOURCES.x11.IMPORTS                       := compile.media.image
SYSTEMLIB.SOURCES.bada_simulator.SOURCE_DIRS        := sources/platform/win32 sources/platform/win32/thread sources/platform/win32/non_unistd sources/platform/bada/startup_simulator
SYSTEMLIB.SOURCES.bada_simulator.COMPILER_DEFINES   := #SHP USE_FBASE_ALLOCATOR BUILD_DLL WIN32
#SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/pthread
SYSTEMLIB.SOURCES.bada.SOURCE_DIRS                  := sources/platform/bada
SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/no_threads
SYSTEMLIB.SOURCES.beagleboard.IMPORTS               := compile.extern.beagleboard

#Цель №2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers tests/thread
SYSTEMLIB.TESTS.IMPORTS          := compile.system link.system link.common.zip_file_system
SYSTEMLIB.TESTS.x11.IMPORTS      := link.media.image.ani_cursor_loader

#Цель №4 - Android launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.TYPE          := android-pak
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.NAME          := funner.application
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.SOURCE_DIRS   := utils/android_launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.MANIFEST_FILE := utils/android_launcher/AndroidManifest.xml
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.RES_DIR       := utils/android_launcher/res
