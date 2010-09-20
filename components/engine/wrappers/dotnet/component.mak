###################################################################################################
#Определения и константы
###################################################################################################
TARGETS.msvc   := ENGINE.DOT_NET_WRAPPER ENGINE.DOT_NET_WRAPPERS_GENERATOR

#Цель - обёртка для взаимодействия с .Net
ENGINE.DOT_NET_WRAPPER.TYPE            := dynamic-lib
ENGINE.DOT_NET_WRAPPER.NAME            := funner.net
ENGINE.DOT_NET_WRAPPER.SOURCE_DIRS     := sources
ENGINE.DOT_NET_WRAPPER.COMPILER_CFLAGS := -clr -wd4793
ENGINE.DOT_NET_WRAPPER.IMPORTS         := compile.xtl compile.engine.launcher

GENERATOR_BUILD_INTERNALS_DIR := ../../../../$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/ENGINE.DOT_NET_WRAPPERS_GENERATOR/build-internals

#Цель - обёртка для взаимодействия с .Net
ENGINE.DOT_NET_WRAPPERS_GENERATOR.TYPE            := application
ENGINE.DOT_NET_WRAPPERS_GENERATOR.OUT_DIR         := $(GENERATOR_BUILD_INTERNALS_DIR)
ENGINE.DOT_NET_WRAPPERS_GENERATOR.NAME            := dot-net-wrappers-generator
ENGINE.DOT_NET_WRAPPERS_GENERATOR.SOURCE_DIRS     := utils/wrappers_generator
ENGINE.DOT_NET_WRAPPERS_GENERATOR.COMPILER_CFLAGS := 
ENGINE.DOT_NET_WRAPPERS_GENERATOR.IMPORTS         := compile.common compile.script.core compile.math.vecmath link.engine.launcher_shared
