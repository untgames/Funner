###################################################################################################
#������ ��� g++
###################################################################################################

###################################################################################################
#���������
###################################################################################################
LIB_SUFFIX           ?= .a
OBJ_SUFFIX           ?= .o
EXE_SUFFIX           ?= .exe
DLL_SUFFIX           ?= .dll
DLL_PREFIX           ?=
DLL_LIB_SUFFIX       ?= .a
LIB_PREFIX           ?= lib
COMPILER_GCC         ?= gcc
LINKER_GCC           ?= g++
LIB_GCC              ?= ar
PROFILES             += g++ haswchar
DEFAULT_LIBS         +=
COMMON_CFLAGS        += -Os -Wall -Wno-format
DISABLE_CPP_WARNINGS += -Wno-invalid-offsetof

###################################################################################################
#���������� ���������� (������ ����������, ������ ������������ ���������, ������ ������������ ������, ������� � ���������� �������,
#������ ��������, ����� ����������, pch ����, ������ ��������� � dll)
###################################################################################################
define tools.g++.c++compile
$(call for_each_file,src,$1,echo $$src && $(COMPILER_GCC) -c $(COMMON_CFLAGS) $(if $(filter %.c,$1),,$(DISABLE_CPP_WARNINGS)) -o "$4/$$(basename $$src $${src##*.})o" $(patsubst %,-I "%",$2) $(patsubst %,-include "%",$3) $6 $(foreach def,$5,-D $(subst %,$(SPACE),$(def))) $$src)
endef

###################################################################################################
#�������� shared-library (��� ��������� �����)
###################################################################################################
define tools.link.dll
-shared -Wl,--out-implib,$(dir $1)$(LIB_PREFIX)$(notdir $(basename $1))$(LIB_SUFFIX)
endef

###################################################################################################
#�������� ������ (��� ��������� �����, ������ ������, ������ ��������� �� ������������ ������������,
#������ ������������ �������� ��������, ����� ��������)
###################################################################################################
define tools.g++.link
$(LINKER_GCC) -o "$1" $(if $(filter %$(DLL_SUFFIX),$1),$(call tools.link.dll,$1)) $(filter-out lib%.a,$2) $(foreach dir,$3,'-Wl,-L,$(dir)') $5 $(patsubst lib%.a,-l%,$(filter lib%.a,$2) $(DEFAULT_LIBS) $(COMMON_LINK_FLAGS) $(patsubst %,-u _%,$4)) && chmod u+x "$1"
endef

###################################################################################################
#������ ���������� (��� ��������� �����, ������ ������)
###################################################################################################
define tools.g++.lib
$(LIB_GCC) rcus $1 $2
endef

define tools.c++compile
$(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.lib
$(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef
