# Link exports
export.link.input.direct_input_driver.LIBS          := funner.input.direct_input_driver dxguid ole32 uuid
export.link.input.direct_input_driver.LIB_DIRS      := ../../../extern/dinputsdk/lib/x86
export.link.input.direct_input_driver.LINK_INCLUDES := DirectInput8Driver
export.link.input.direct_input_driver.IMPORTS       := link.input.low_level link.system
