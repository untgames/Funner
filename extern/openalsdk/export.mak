# Compile exports
export.compile.extern.openal.INCLUDE_DIRS           := include
export.compile.extern.openal.macosx.COMPILER_CFLAGS := -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/OpenAL.framework/Headers
export.compile.extern.openal.iphone.COMPILER_CFLAGS := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/OpenAL.framework/Headers

# Link exports
export.link.extern.openal.LIBS                  :=
export.link.extern.openal.win32.LIBS            := openal32
export.link.extern.openal.x86_win32.LIB_DIRS    := lib/win32
export.link.extern.openal.x86-64_win32.LIB_DIRS := lib/win64
export.link.extern.openal.linux.LIBS            := funner.extern.openal32
export.link.extern.openal.beagleboard.LIBS      := funner.extern.openal32
export.link.extern.openal.meego.LIBS            := funner.extern.openal32
export.link.extern.openal.macosx.LINK_FLAGS     := -framework OpenAL
export.link.extern.openal.iphone.LINK_FLAGS     := -framework OpenAL
export.link.extern.openal.android.LIBS          := funner.extern.openal32
export.link.extern.openal.tabletos.LIBS         := OpenAL asound
export.link.extern.openal.wp8.LIBS              := funner.extern.openal32
#export.link.extern.openal.win32.LIBS         := funner.extern.openal32

# Run exports
export.run.extern.openal.DLLS                  :=
export.run.extern.openal.win32.DLLS            := OpenAL32 wrap_oal
export.run.extern.openal.x86_win32.DLL_DIRS    := bin/win32
export.run.extern.openal.x86-64_win32.DLL_DIRS := bin/win64
export.run.extern.openal.linux.DLLS            := funner.extern.openal32
export.run.extern.openal.android.DLLS          := funner.extern.openal32
export.run.extern.openal.wp8.DLLS              := funner.extern.openal32
