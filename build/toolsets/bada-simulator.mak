PROFILES                         += bada_simulator
BADA_TOOLCHAIN                   := Win32
BADA_TOOLCHAIN_PREFIX            := i686-mingw32-
DLL_LIB_SUFFIX                   := .a
DLL_PREFIX                       :=
EXE_SUFFIX                       := .exe
SIMULATOR_APP_NAME               := FunnerTestApplication
SIMULATOR_APP_ID                 := 93bt1p123e
SIMULATOR_APP_SECRET             := 9C645DDBA19C71BAD1204DA4DAA7A0B9
SIMULATOR_PROFILE                := S8000_Generic.dbi
SIMULATOR_SHELL                  := $(call convert_path,$(BADA_SDK_HOME))/Lib/WinSgpp/PhoneShell.dll
SIMULATOR_APP_DIR                := $(call convert_path,$(BADA_SDK_HOME))/Rsrc/bada_WVGA/Win32FS/Osp/Applications/$(SIMULATOR_APP_ID)
SIMULATOR_TEMPLATE_PROJECT_DIR   := $(BUILD_DIR)platforms/bada/simulator_project
SIMULATOR_TEMPLATE_PROJECT_FILES := $(patsubst $(SIMULATOR_TEMPLATE_PROJECT_DIR)/%,%,$(wildcard $(SIMULATOR_TEMPLATE_PROJECT_DIR)/*))
SIMULATOR                        := $(call convert_path,$(BADA_SDK_HOME))/Lib/WinSgpp/Simulator
SIMULATOR_WATCHER                := $(BUILD_DIR)platforms/bada/watch_simulator.sh
COMMON_CFLAGS                    += -DBADA_SIMULATOR -DDEFAULT_APP_ID='L"$(SIMULATOR_APP_ID)"' -DDEFAULT_APP_SECRET='L"$(SIMULATOR_APP_SECRET)"' -DDEFAULT_APP_NAME='L"$(SIMULATOR_APP_NAME)"'
COMMON_LINK_FLAGS                += -L$(BADA_SDK_HOME)/Lib/WinSgpp

include $(TOOLSETS_DIR)/bada.mak

#Инсталляция

install: build INSTALL_SIMULATOR_PROJECT

.PHONY: install INSTALL_SIMULATOR_PROJECT

#Инсталляция файла в симулятор (имя локального файла, имя результирующего файла)
define install_file_to_bada_simulator
  ifeq (,$$(filter $1/,$$(wildcard $1/)))
    INSTALL_SIMULATOR_FILES := $$(INSTALL_SIMULATOR_FILES) $2
    
    $2: $1
			@echo Installing $$(notdir $1)...
			@cp -f $1 "$$@"
  else
    INSTALLATION_DIRS := $(INSTALLATION_DIRS) $2
  endif
endef

#Инсталляция файлов в симулятор (не используется, имена локальных файлов, результирующий каталог)
define install_files_to_bada_simulator
  INSTALLATION_DIRS := $(INSTALLATION_DIRS) $3
  
  $$(foreach file,$2,$$(eval $$(call install_file_to_bada_simulator,$$(file),$3/$$(notdir $$(file)))))
endef

$(eval $(call process_files,,$(SIMULATOR_TEMPLATE_PROJECT_DIR),$(SIMULATOR_TEMPLATE_PROJECT_FILES),$(SIMULATOR_APP_DIR),install_files_to_bada_simulator))

INSTALL_SIMULATOR_PROJECT: $(INSTALL_SIMULATOR_FILES)

#Копирование файла на устройство (имя локального файла, имя удалённого файла)
define tools.install
 echo Installing $(notdir $1)... && mkdir -p $(dir $(SIMULATOR_APP_DIR)/Bin/$2) && cp $1 $(SIMULATOR_APP_DIR)/Bin/$2
endef

#Деинсталляция (имя каталога)
define tools.uninstall
$(RM) -Rf $1
endef

#Запуск
#$(call prepare_to_execute,$2,$4) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)" $(if $5, > "$(CURDIR)/$5")
  
#Выполнение команды (команда, каталог запуска, базовый каталог, дополнительные пути поиска библиотек и приложений, файл результатов)
define tools.run
echo -n > $(firstword $1).stdout && \
export BADA_STDOUT_FILE_NAME=$(CURDIR)/$(firstword $1).stdout && \
cp $(patsubst $(ROOT_TMP_DIR)/%,$(SIMULATOR_APP_DIR)/Bin/%,$(firstword $1)) $(SIMULATOR_APP_DIR)/Bin/$(SIMULATOR_APP_NAME).exe && \
$(call prepare_to_execute,$2,$4) && \
start //MIN $(SIMULATOR) --app-id=$(SIMULATOR_APP_ID) -s $(SIMULATOR_SHELL) -d $(SIMULATOR_PROFILE) --app-file=$(SIMULATOR_APP_NAME) && \
"$(CURDIR)/$(SIMULATOR_WATCHER)" < "$(CURDIR)/$(firstword $1).stdout"
endef
