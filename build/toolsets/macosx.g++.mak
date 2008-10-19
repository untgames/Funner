###################################################################################################
#Сборка под MacOS X g++
###################################################################################################

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX   :=
DLL_SUFFIX   := .dylib
PROFILES     += macosx unistd carbon

COMMON_CFLAGS += -I/System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/Headers \
                 -I/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers

COMMON_LINK_FLAGS += -framework Carbon

include $(TOOLSETS_DIR)/g++.mak

