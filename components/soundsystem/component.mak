###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUNDSYSTEM_COMPONENTS SOUNDSYSTEM_TESTS

#Цель №1 - пакет
SOUNDSYSTEM_COMPONENTS.TYPE       := package
SOUNDSYSTEM_COMPONENTS.COMPONENTS := manager openaldevice sgplayer

#Цель №2 - тесты
SOUNDSYSTEM_TESTS.TYPE       := package
SOUNDSYSTEM_TESTS.COMPONENTS := manager/tests openaldevice/tests sgplayer/tests
