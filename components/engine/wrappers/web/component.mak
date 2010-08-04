###################################################################################################
#Определения и константы
###################################################################################################
TARGETS.win32  := ENGINE.WEB_WRAPPER

ENGINE.WEB_WRAPPER.TYPE                  := dynamic-lib
ENGINE.WEB_WRAPPER.NAME                  := npFunnerEngine
ENGINE.WEB_WRAPPER.DEF_FILE              := gen/FireBreathWin.def
ENGINE.WEB_WRAPPER.INCLUDE_DIRS          := gen
ENGINE.WEB_WRAPPER.SOURCE_DIRS           := sources
ENGINE.WEB_WRAPPER.msvc.SOURCE_DIRS      := sources/Win gen
ENGINE.WEB_WRAPPER.COMPILER_DEFINES      := UNICODE _UNICODE FunnerEngine_EXPORTS
ENGINE.WEB_WRAPPER.IMPORTS               := compile.extern.firebreath link.extern.firebreath
ENGINE.WEB_WRAPPER.LIBS                  :=
