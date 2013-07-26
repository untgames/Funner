###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUTSYSTEM_COMPONENTS

#Цель №1 - пакет
INPUTSYSTEM_COMPONENTS.TYPE              := package
INPUTSYSTEM_COMPONENTS.COMPONENTS        := low_level manager window_driver sensor_input_driver scene_input
INPUTSYSTEM_COMPONENTS.win32.COMPONENTS  := direct_input_driver
