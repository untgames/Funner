PROFILES              += bada_device
BADA_TOOLCHAIN        := ARM
BADA_TOOLCHAIN_PREFIX := arm-samsung-nucleuseabi-
BADA_LANGUAGE_PACK    := Wave_LP1
BADA_TARGET_LIBS_DIR  := $(BADA_SDK_HOME)/Model/$(BADA_LANGUAGE_PACK)/Target
COMMON_CFLAGS         += -DBADA_DEVICE -Wno-psabi -Wno-uninitialized -fpic -fshort-wchar -mcpu=cortex-a8 -mfpu=vfpv3 -mfloat-abi=hard -mlittle-endian -mthumb-interwork
COMMON_LINK_FLAGS     += -nostdlib
COMMON_LINK_FLAGS     += -L$(BADA_TARGET_LIBS_DIR)
COMMON_LINK_FLAGS     += -L$(BADA_SDK_HOME)/Tools/Toolchains/ARM/arm-samsung-nucleuseabi/lib
COMMON_LINK_FLAGS     += -L$(BADA_SDK_HOME)/Tools/Toolchains/ARM/lib/gcc/arm-samsung-nucleuseabi/4.4.1
COMMON_LINK_FLAGS     += $(BADA_TARGET_LIBS_DIR)/libc-newlib.so $(BADA_TARGET_LIBS_DIR)/libm-newlib.so $(BADA_TARGET_LIBS_DIR)/StubDynCast.so -lstdc++ -lgcc_s
DLL_LIB_SUFFIX        := .so
DLL_PREFIX            := lib
EXE_SUFFIX            :=

include $(TOOLSETS_DIR)/bada.mak

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки)
###################################################################################################

define tools.link.dll
-shared -Wl,-undefined -Wl,error
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR))
endef
