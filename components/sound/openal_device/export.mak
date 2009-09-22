# Link exports
export.link.sound.openal_driver.win32.LIB_DIRS    := ../../../extern/openalsdk/lib/win32
export.link.sound.openal_driver.LIBS              := funner.sound.low_level.openal_driver
export.link.sound.openal_driver.win32.LIBS        := openal32
export.link.sound.openal_driver.linux.LIBS        := openal
export.link.sound.openal_driver.LINK_INCLUDES     := OpenAL
export.link.sound.openal_driver.macosx.LINK_FLAGS := -framework OpenAL
export.link.sound.openal_driver.iphone.LINK_FLAGS := -framework OpenAL
export.link.sound.openal_driver.IMPORTS           := link.sound.low_level link.media.sound link.system

# Run exports
export.run.sound.openal_driver.win32.DLLS     := OpenAL32 wrap_oal
export.run.sound.openal_driver.win32.DLL_DIRS := ../../../extern/openalsdk/bin
