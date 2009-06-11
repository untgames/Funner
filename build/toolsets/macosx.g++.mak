###################################################################################################
#Сборка под MacOS X g++
###################################################################################################

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES   += macosx unistd carbon has_windows haswchar
DLL_PATH   := DYLD_LIBRARY_PATH

COMMON_CFLAGS += -I/System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/Headers \
                 -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers

COMMON_LINK_FLAGS += -framework Carbon

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Линковка shared-library (имя выходного файла)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name $(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef
