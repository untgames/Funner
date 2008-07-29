###################################################################################################
#Настройки, экспортируемые для библиотек использующих WindowDriver
###################################################################################################
export.LIBS     := input.direct_input_driver system dxguid ole32
export.LIB_DIRS := ../../../extern/dinputsdk/lib/x86
export.IMPORTS  := ../low_level/link.static.mak ../../system/link.static.mak
