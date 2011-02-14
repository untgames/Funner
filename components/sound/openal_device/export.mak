# Link exports
export.link.sound.low_level.openal_driver.LIBS           := funner.sound.low_level.openal_driver
export.link.sound.low_level.openal_driver.LINK_INCLUDES  := OpenAL
export.link.sound.low_level.openal_driver.IMPORTS        := link.sound.low_level link.media.sound link.system link.extern.openal

# Run exports
export.run.sound.low_level.openal_driver.DLLS     :=
export.run.sound.low_level.openal_driver.IMPORTS  := run.extern.openal
