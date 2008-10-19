###################################################################################################
#Настройки, экспортируемые для библиотек использующих WindowDriver
###################################################################################################
export.LIBS          := funner.input.direct_input_driver funner.system dxguid ole32 uuid
export.LIB_DIRS      := ../../../extern/dinputsdk/lib/x86
export.LINK_INCLUDES := DirectInput8Driver
export.IMPORTS       := ../low_level/link.static.mak ../../system/link.static.mak
