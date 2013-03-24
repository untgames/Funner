###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS SYSTEMLIB.INFO

TARGETS.android  += SYSTEMLIB.UTILS.ANDROID_LAUNCHER
TARGETS.android  += SYSTEMLIB.UTILS.ANDROID_LAUNCHER_COMMON
TARGETS.tabletos += SYSTEMLIB.UTILS.TABLETOS_LAUNCHER
TARGETS.win8     += SYSTEMLIB.UTILS.WIN8_LAUNCHER

#Цель - System library sources
SYSTEMLIB.SOURCES.TYPE                              := static-lib
SYSTEMLIB.SOURCES.NAME                              := funner.system
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      += sources
SYSTEMLIB.SOURCES.INCLUDE_DIRS                      += sources/platform
SYSTEMLIB.SOURCES.SOURCE_DIRS                       += sources/wrappers
SYSTEMLIB.SOURCES.SOURCE_DIRS                       += sources/platform/default
SYSTEMLIB.SOURCES.IMPORTS                           := compile.system
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS                 := sources/platform/windows sources/platform/windows/application sources/platform/windows/non_unistd
SYSTEMLIB.SOURCES.x86_win32.SOURCE_DIRS             := sources/platform/windows/thread
SYSTEMLIB.SOURCES.winrt.SOURCE_DIRS                 := sources/platform/windows sources/platform/windows/non_unistd sources/platform/windows/thread sources/platform/windows/win8
SYSTEMLIB.SOURCES.wince.SOURCE_DIRS                 := sources/platform/pthread
SYSTEMLIB.SOURCES.wince.IMPORTS                     := compile.extern.pthreads_wince
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS                := sources/platform/pthread sources/platform/unistd sources/platform/message_queue
SYSTEMLIB.SOURCES.msvc.COMPILER_CFLAGS              := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS            := sources/platform/no_windows
SYSTEMLIB.SOURCES.android.SOURCE_DIRS               := sources/platform/android
SYSTEMLIB.SOURCES.cocoa.SOURCE_DIRS                 := sources/platform/cocoa
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS                := sources/platform/carbon
SYSTEMLIB.SOURCES.carbon.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.iphone.SOURCE_DIRS                := sources/platform/iphone
SYSTEMLIB.SOURCES.iphone.COMPILER_DEFINES           := NO_PTHREAD_SEMAPHORES
SYSTEMLIB.SOURCES.x11.SOURCE_DIRS                   := sources/platform/x11
SYSTEMLIB.SOURCES.x11.IMPORTS                       := compile.media.image
SYSTEMLIB.SOURCES.bada_simulator.SOURCE_DIRS        := sources/platform/windows sources/platform/windows/thread sources/platform/windows/non_unistd sources/platform/bada/startup_simulator
#SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/pthread
SYSTEMLIB.SOURCES.bada.SOURCE_DIRS                  := sources/platform/bada
SYSTEMLIB.SOURCES.bada_device.SOURCE_DIRS           := sources/platform/no_threads sources/platform/bada/startup_device
SYSTEMLIB.SOURCES.beagleboard.IMPORTS               := compile.extern.beagleboard
SYSTEMLIB.SOURCES.meego.IMPORTS                     := compile.extern.meego
SYSTEMLIB.SOURCES.tabletos.SOURCE_DIRS              := sources/platform/tabletos
SYSTEMLIB.SOURCES.win8.COMPILER_CFLAGS              := -ZW

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES)))
  SYSTEMLIB.SOURCES.x86.IMPORTS := compile.extern.geekinfo
endif

#Цель - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers tests/thread
SYSTEMLIB.TESTS.IMPORTS          := compile.system link.system link.common.zip_file_system
SYSTEMLIB.TESTS.LIBS             := 
SYSTEMLIB.TESTS.x11.IMPORTS      := link.media.image.ani_cursor_loader

#Цель - Android launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.TYPE          := android-pak
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.NAME          := funner.application
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.DLL_DIRS       = $(PLATFORM_DIR)/arch-arm/usr/lib
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.DLLS          := android
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.SOURCE_DIRS   := utils/android_launcher
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.MANIFEST_FILE := utils/android_launcher/AndroidManifest.xml
SYSTEMLIB.UTILS.ANDROID_LAUNCHER.RES_DIR       := utils/android_launcher/res

#Цель - Android launcher common
SYSTEMLIB.UTILS.ANDROID_LAUNCHER_COMMON.TYPE         := android-jar
SYSTEMLIB.UTILS.ANDROID_LAUNCHER_COMMON.NAME         := funner.application
SYSTEMLIB.UTILS.ANDROID_LAUNCHER_COMMON.SOURCE_DIRS  := utils/android_launcher

#Цель - Tabletos launcher
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.TYPE          := tabletos-bar
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.NAME          := funner.application
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.SOURCE_DIRS   := utils/tabletos_launcher
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.LIBS          := socket EGL GLESv1_CM png freetype screen bps pps OpenAL asound
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.RES_DIR       := utils/tabletos_launcher/res
SYSTEMLIB.UTILS.TABLETOS_LAUNCHER.MANIFEST_FILE := utils/tabletos_launcher/res/bar-descriptor.xml

#Цель - Win8 launcher
SYSTEMLIB.UTILS.WIN8_LAUNCHER.TYPE            := win8-appx
SYSTEMLIB.UTILS.WIN8_LAUNCHER.NAME            := win8-launcher
SYSTEMLIB.UTILS.WIN8_LAUNCHER.SOURCE_DIRS     := utils/win8_launcher
SYSTEMLIB.UTILS.WIN8_LAUNCHER.MANIFEST_FILE   := utils/win8_launcher/AppxManifest.xml
SYSTEMLIB.UTILS.WIN8_LAUNCHER.PFX_FILE        := utils/win8_launcher/key.pfx
SYSTEMLIB.UTILS.WIN8_LAUNCHER.PUBLISHER       := CN=leny
SYSTEMLIB.UTILS.WIN8_LAUNCHER.COMPILER_CFLAGS := -ZW

#Цель - сборка документации
SYSTEMLIB.INFO.TYPE        := doxygen-info
SYSTEMLIB.INFO.CHM_NAME    := funner.system
SYSTEMLIB.INFO.SOURCE_DIRS := include
SYSTEMLIB.INFO.IMPORTS     := compile.system
