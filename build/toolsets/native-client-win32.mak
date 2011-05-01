###################################################################################################
#—борка под NativeClient SDK
###################################################################################################
ifeq ($(strip $(NATIVE_CLIENT_SDK)),)
  $(error "Please set NATIVE_CLIENT_SDK variable in your environment")
endif

###################################################################################################
# онстанты
###################################################################################################
PROFILES          += unistd native_client x86 has_windows
EXE_SUFFIX        := .nexe
DLL_SUFFIX        := .dll
DLL_PREFIX        :=
NCLIENT_GCC       := $(subst \,/,$(NATIVE_CLIENT_SDK))/toolchain/win_x86
COMPILER_GCC      := $(NCLIENT_GCC)/bin/nacl-gcc
LINKER_GCC        := $(NCLIENT_GCC)/bin/nacl-g++
LIB_GCC           := $(NCLIENT_GCC)/bin/nacl-ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -Wall -Wno-long-long -pthread -Werror -m32 -O2 -DNATIVE_CLIENT "-I$(NCLIENT_GCC)/nacl/include"
COMMON_LINK_FLAGS += -lppruntime -lppapi_cpp -lplatform -lgio -lpthread -lsrpc -m32 "-I$(NCLIENT_GCC)/nacl/lib"
CYGWIN            := nodosfilewarning

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#ѕереопределени€ вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)))
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.dll
-shared
endef

#¬ыполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
$(call prepare_to_execute,$2,$3) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
endef
