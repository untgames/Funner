#Переменные и константы
LUA_FLAGS                       := 
DIRS                             = $(TMP_DIRS) $(DIST_DIRS)
USER                            ?= $(USERNAME)
TEXTURE_CONFIG_FILE_NAME        := config.mak
DEFAULT_TEXTURE_EXTENSION       := tga
TMP_DIR                         := tmp
SOURCE_TEXTURE_EXTENSIONS       := bmp jpg tga png
UNCOMPRESSED_TEXTURE_EXTENSIONS := bmp jpg tga png
CURRENT_TOOLSET	                := $(TOOLSET)
MODEL_MATERIALS_DIR             := data/materials
MODEL_MESHES_DIR                := data/meshes
MODEL_SCENES_DIR                := data/scenes
MODEL_TEXTURES_DIR              := data/textures

#Цель по умолчанию
default: build

#Подключение пользовательских настроек
-include $(USER).settings.mak

#Подключение конфигурации проекта
include config.mak

#Работа с тулсетом
ifeq (,$(CURRENT_TOOLSET))
  $(error No toolset. Export TOOLSET variable)
endif

#Корректировка переменных
TMP_DIR           := $(OUT_DIR)/../tmp
TOOLS_DIR         := $(OUT_DIR)/$(CURRENT_TOOLSET)/$(TOOLS_DIR)
OUT_DIR           := $(OUT_DIR)/$(CURRENT_TOOLSET)/$(APPLICATION_DIR)
BUILD_NAME_FILE   := $(OUT_DIR)/data/scripts/common/build.lua
BUILD_NUMBER_FILE := $(OUT_DIR)/build_number.txt
MAGICK_DIR        ?= ./$(TOOLS_DIR)

ifeq (,$(OUT_DIR))
  $(error OUT_DIR not defined)
endif

#Макросы 

#Вызов инструмента (командная строка)
define run_tool
export PATH=$(TOOLS_DIRS):$(PATH) && "$1"
endef

#Обработка копирования вложенных директорий (имя исходной директории, имя результирующей директории, макрос с целью для каждого каталога, имя цели)
define process_subdir
  $$(eval $$(call process_files,$1/,*,$2,$3,$4))
endef

#Обработка копирования файла(ов) (имя исходной директории, имя/маска файлов, имя результирующей директории, макрос с целью для каждого каталога, имя цели)
define process_files
  MODULE_NAME                 := $5.$$(subst /,-,$$(subst ./,,$$(subst ../,,$$(patsubst %/,%,$1))))
  $$(MODULE_NAME).CONFIG_FILE := $1/$(TEXTURE_CONFIG_FILE_NAME)
  $$(MODULE_NAME).TMP_DIR     := $$($5.TMP_DIR)/$$(patsubst $$(OUT_DIR)/%,%,$3)

  #Подключение локальной конфигурации ресурсов
  TEXTURE_WIDTH     :=
  TEXTURE_HEIGHT    :=
  TEXTURE_EXTENSION :=
  TEXTURES_IN_ROW   :=
  TEXTURE_WITHOUT_ALPHA :=
  TEXTURE_SCALED_WIDTH  :=
  TEXTURE_SCALED_HEIGHT :=
  FILE_LIST         :=
    
  $$(MODULE_NAME).TEXTURE_WIDTH  := $$(TEXTURE_WIDTH)
  $$(MODULE_NAME).TEXTURE_HEIGHT := $$(TEXTURE_HEIGHT)
  $$(MODULE_NAME).TEXTURE_EXTENSION := $$(TEXTURE_EXTENSION)
  $$(MODULE_NAME).TEXTURES_IN_ROW := $$(TEXTURES_IN_ROW)
  $$(MODULE_NAME).TEXTURE_WITHOUT_ALPHA := $$(TEXTURE_WITHOUT_ALPHA)
  $$(MODULE_NAME).TEXTURE_SCALED_WIDTH := $$(TEXTURE_SCALED_WIDTH)
  $$(MODULE_NAME).TEXTURE_SCALED_HEIGHT := $$(TEXTURE_SCALED_HEIGHT)
  $$(MODULE_NAME).FILE_LIST := $$(FILE_LIST)

  -include $$($$(MODULE_NAME).CONFIG_FILE)  

  ifneq (,$$(filter %*,$2))
    #Обработка файловых масок

    SOURCE_INSTALLATION_DIRS := $$(patsubst %/,%,$$(filter %/,$$(wildcard $1$2/)))

    ifeq (,$$(FILE_LIST))
      SOURCE_INSTALLATION_FILES := $$(filter-out %/,$$(wildcard $1$2/))
    else
      SOURCE_INSTALLATION_FILES := $$(FILE_LIST:%=$1%)
    endif

#    DESTINATION_INSTALLATION_FILES := $$(SOURCE_INSTALLATION_FILES:$1%=$3/%)    
  else
    ifneq (,$$(wildcard $1$2/*))
      #Обработка директорий
      SOURCE_INSTALLATION_DIRS  := $1$2
      SOURCE_INSTALLATION_FILES :=
    else
      #Обработка файлов
#      DESTINATION_INSTALLATION_FILES := $3/$2
      ifeq (,$$(FILE_LIST))
        SOURCE_INSTALLATION_FILES := $$(wildcard $1$2)
      else
        SOURCE_INSTALLATION_FILES := $$(FILE_LIST:%=$1%)
      endif
    endif
  endif
  
  $$(MODULE_NAME).SOURCE_INSTALLATION_FILES := $$(SOURCE_INSTALLATION_FILES)
  
#Обработка вложений
  $$(foreach dir,$$(SOURCE_INSTALLATION_DIRS),$$(eval $$(call process_subdir,$$(dir),$3/$$(dir:$1%=%),$4,$5)))
  
#Обработка текущей директории
  MODULE_NAME := $5.$$(subst /,-,$$(subst ./,,$$(subst ../,,$$(patsubst %/,%,$1))))
     
  TMP_DIRS  := $$(TMP_DIRS) $$($$(MODULE_NAME).TMP_DIR)
  DIST_DIRS := $$(DIST_DIRS) $3

#  build: $3
  
  #Генерация правил сборки в текущей директории
  $$(eval $$(call $4,$$(patsubst %/,%,$1),$$($$(MODULE_NAME).SOURCE_INSTALLATION_FILES),$3,$5,$$(MODULE_NAME)))    
endef

#Промежуточная конвертация текстуры (расширение, исходный каталог, результирующий каталог)
define convert_texture_intermediate_file
  $3/%.tga: $2/%.$1
		@echo Precompile texture $$(notdir $$<)...
		@"$(MAGICK_DIR)/convert" -alpha on -background transparent "$$<" -filter Lanczos $$(if $$(TEXTURE_SCALED_HEIGHT)$$(TEXTURE_SCALED_WIDTH),-resize $$(if $$(TEXTURE_SCALED_WIDTH),$$(TEXTURE_SCALED_WIDTH),`"$(MAGICK_DIR)/convert" "$$<" -format "%[fx:2^(ceil(log(w)/log(2)))]" info:`)x$$(if $$(TEXTURE_SCALED_HEIGHT),$$(TEXTURE_SCALED_HEIGHT),`"$(MAGICK_DIR)/convert" "$$<" -format "%[fx:2^(ceil(log(h)/log(2)))]" info:`)! -alpha on -background transparent "$$@", -extent $$(if $$(TEXTURE_WIDTH),$$(TEXTURE_WIDTH),`"$(MAGICK_DIR)/convert" "$$<" -format "%[fx:2^(ceil(log(w)/log(2)))]" info:`)x$$(if $$(TEXTURE_HEIGHT),$$(TEXTURE_HEIGHT),`"$(MAGICK_DIR)/convert" "$$<" -format "%[fx:2^(ceil(log(h)/log(2)))]" info:`)! -alpha on -background transparent "$$@")
endef

#Окончательная конвертация текстуры (расширение, исходный каталог, результирующий каталог)
define convert_texture_destination_file
  $3/%.$1: $2/%.tga
		@echo Convert texture $$(notdir $$@)...
		@"$(MAGICK_DIR)/convert" "$$<" "$$@"
endef

#Конвертация одной текстуры (исходный каталог, список файлов, целевой каталог, имя цели, имя модуля)
define convert_texture
  $5.TEXTURES     := $$(filter-out %/$(TEXTURE_CONFIG_FILE_NAME),$$(patsubst $1/%,$3/%,$2))
  $5.INTERMEDIATE := $$(filter-out %/$(TEXTURE_CONFIG_FILE_NAME),$$(patsubst $1/%,$$($5.TMP_DIR)/%.tga,$$(basename $2)))

  ifeq (,$$($5.TEXTURE_EXTENSION))
    $5.TEXTURE_EXTENSION := $(DEFAULT_TEXTURE_EXTENSION)
  endif  

  $5.TEXTURES     := $$(foreach file,$$($5.TEXTURES),$$(basename $$(file)).$$($5.TEXTURE_EXTENSION))
  $5.FULL_LIST    := $$(wildcard $$(sort $$(addsuffix *,$$(dir $$($5.TEXTURES)))))
  $5.REMOVE_LIST   = $$(foreach file,$$($5.FULL_LIST),$$(if $$(filter $$(file),$$($5.TEXTURES)),,$$(file)))
  
#  $$(warning dir=$$($5.TMP_DIR) files=$$($5.INTERMEDIATE))
   
#  $$(warning del=$$($5.REMOVE_LIST) add=$$($5.ADD_LIST))    

  TARGET_FILES := $$(TARGET_FILES) $$($5.TEXTURES)

  build: BUILD.$5
  svnsync: SVNSYNC.$5
  
  BUILD.$5: $$($5.TMP_DIR) $3 $$($5.TEXTURES)

  .PHONY: CLEAN.$5 SVNSYNC.$5 BUILD.$5
  
  clean: CLEAN.$5
  
  CLEAN.$5: TEXTURES := $$($5.TEXTURES)

  CLEAN.$5:
		@$(RM) $$(TEXTURES)
		
  ifneq (,$$(wildcard $$($5.CONFIG_FILE)))
    $$($5.TEXTURES): $$($5.CONFIG_FILE)
  endif    

  $$($5.TEXTURES): TEXTURE_WIDTH         := $$($5.TEXTURE_WIDTH)
  $$($5.TEXTURES): TEXTURE_HEIGHT        := $$($5.TEXTURE_HEIGHT)
  $$($5.TEXTURES): TEXTURE_WITHOUT_ALPHA := $$($5.TEXTURE_WITHOUT_ALPHA)
  $$($5.TEXTURES): TEXTURE_SCALED_WIDTH  := $$($5.TEXTURE_SCALED_WIDTH)
  $$($5.TEXTURES): TEXTURE_SCALED_HEIGHT := $$($5.TEXTURE_SCALED_HEIGHT)

  $$(foreach file,$$($5.TEXTURES),$$(foreach var,$$(filter config.$$(notdir $$(basename $$(file))).%,$$(.VARIABLES)),$$(eval $$(file): $$(var:config.$$(notdir $$(basename $$(file))).%=%) := $$($$(var)))))  
  
  $$(foreach ext,$$(SOURCE_TEXTURE_EXTENSIONS),$$(eval $$(call convert_texture_intermediate_file,$$(ext),$1,$$($5.TMP_DIR))))
  $$(foreach ext,$$(UNCOMPRESSED_TEXTURE_EXTENSIONS),$$(eval $$(call convert_texture_destination_file,$$(ext),$$($5.TMP_DIR),$3)))  
  
  $3/%.pvr: $$($5.TMP_DIR)/%.tga
		@echo Convert texture $$(notdir $$@)...
		@$(TOOLS_DIR)/PVRTexTool.exe -pvrtciterations8 -i$$< -fOGLPVRTC4 -m -o$$@

  SVNSYNC.$5: REMOVE_LIST := $$($5.REMOVE_LIST)
  SVNSYNC.$5: TEXTURES := $$($5.TEXTURES)

  SVNSYNC.$5: BUILD.$5
		@$$(if $$(strip $$(REMOVE_LIST)),svn del --force $$(REMOVE_LIST))
		@$$(if $$(strip $$(TEXTURES)),svn add --force $$(TEXTURES))
endef

#Конвертация текстур (имя цели)
define process_target.textures
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_files,$$(dir)/,*,$$($1.DESTINATION_DIR),convert_texture,$1)))
endef

#Построение атласа текстр (исходный каталог, список файлов, целевой каталог, имя цели, имя модуля)
define build_atlas_texture
  $4.SOURCE_TEXTURES := $$($4.SOURCE_TEXTURES) $$(filter-out %/$(TEXTURE_CONFIG_FILE_NAME),$2)
  
  ifneq (,$$(wildcard $$($5.CONFIG_FILE)))
    $$($4.ATLAS_TEXTURE): $$($5.CONFIG_FILE)
  endif

endef

#Конвертация текстур с построением атласа (имя цели)
define process_target.atlas-texture
  $1.ATLAS_TEXTURE              := $$($1.DESTINATION_DIR)/$$($1.NAME)
  $1.ATLAS_TEXTURE_INTERMEDIATE := $$($1.TMP_DIR)/atlas.tga

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_files,$$(dir)/,*,$$($1.DESTINATION_DIR),build_atlas_texture,$1)))  

  $1.TEXTURES_IN_ROW := $$(if $$($1.TEXTURES_IN_ROW),$$($1.TEXTURES_IN_ROW),4)
  
  TARGET_FILES := $$(TARGET_FILES) $$($1.ATLAS_TEXTURE)

  build: BUILD.$1
  svnsync: SVNSYNC.$1
  
  BUILD.$1: $$($1.TMP_DIR) $$($1.DESTINATION_DIR) $$($1.ATLAS_TEXTURE)

  .PHONY: CLEAN.$1 SVNSYNC.$1 BUILD.$1
  
  clean: CLEAN.$1  

  CLEAN.$1:
		@$(RM) $$($1.ATLAS_TEXTURE_INTERMEDIATE) $$($1.ATLAS_TEXTURE)

  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURE_WIDTH  := $$($1.TEXTURE_WIDTH)
  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURE_HEIGHT := $$($1.TEXTURE_HEIGHT)
  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURE_WITHOUT_ALPHA := $$($1.TEXTURE_WITHOUT_ALPHA)
  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURES_IN_ROW := $$($1.TEXTURES_IN_ROW)
  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURE_SCALED_WIDTH := $$($1.TEXTURE_SCALED_WIDTH)
  $$($1.ATLAS_TEXTURE_INTERMEDIATE): TEXTURE_SCALED_HEIGHT := $$($1.TEXTURE_SCALED_HEIGHT)

  $$($1.ATLAS_TEXTURE_INTERMEDIATE): $$($1.SOURCE_TEXTURES)
		@echo Build atlas $$(notdir $$@)...
		@"$(MAGICK_DIR)/montage" -alpha on $$($1.SOURCE_TEXTURES) -mode Concatenate -tile $$($1.TEXTURES_IN_ROW)x -background transparent -alpha on "$$@"
		@"$(MAGICK_DIR)/convert" $$(if $$(TEXTURE_WITHOUT_ALPHA),-background "rgb(255,255,255)",-background transparent) -alpha on "$$@" -filter Lanczos -extent $$(if $$(TEXTURE_WIDTH),$$(TEXTURE_WIDTH),`"$(MAGICK_DIR)/convert" "$$@" -format "%[fx:2^(ceil(log(w)/log(2)))]" info:`)x$$(if $$(TEXTURE_HEIGHT),$$(TEXTURE_HEIGHT),`"$(MAGICK_DIR)/convert" "$$@" -format "%[fx:2^(ceil(log(h)/log(2)))]" info:`)! $$(if $$(TEXTURE_WITHOUT_ALPHA),-alpha off) "$$@"
		@$$(if $$(TEXTURE_SCALED_WIDTH)$$(TEXTURE_SCALED_HEIGHT),"$(MAGICK_DIR)/convert" "$$@" -filter Lanczos -resize $$(TEXTURE_SCALED_WIDTH)x$$(TEXTURE_SCALED_HEIGHT)! "$$@")				
		
ifneq (.pvr,$$(suffix $$($1.ATLAS_TEXTURE)))
  $$($1.ATLAS_TEXTURE): $$($1.ATLAS_TEXTURE_INTERMEDIATE)
		@echo Convert texture $$(notdir $$@)...
		@"$(MAGICK_DIR)/convert" $$< $$@
		
else

  $$($1.ATLAS_TEXTURE): $$($1.ATLAS_TEXTURE_INTERMEDIATE)  
		@echo Convert texture $$(notdir $$@)...
		@$(TOOLS_DIR)/PVRTexTool.exe -pvrtciterations8 -i$$< -fOGLPVRTC4 -m -o$$@
		
endif
		
  SVNSYNC.$1: ATLAS_TEXTURE := $$($1.ATLAS_TEXTURE)

  SVNSYNC.$1: BUILD.$1
		@$$(if $$(strip $$(ATLAS_TEXTURE)),svn add --force $$(ATLAS_TEXTURE))

endef

#Конвертация PSD текстуры (имя цели)
define process_target.convert-psd
  $1.MATERIAL_FILE             := $$(OUT_DIR)/$$($1.MATERIAL_FILE)
  $1.LAYERS_DIR                := $$(OUT_DIR)/$$($1.LAYERS_DIR)
  $1.LAYERS_FILE_FORMAT        := $$($1.LAYERS_DIR)/image%04d.png
  $1.LAYOUT_LAYERS_FILE_FORMAT := $$($1.LAYERS_FILE_FORMAT:$$(OUT_DIR)/%=%)
  $1.LAYOUT_FILE               := $$(OUT_DIR)/$$($1.LAYOUT_FILE)
  $1.LAYOUT_DIR                := $$(dir $$($1.LAYOUT_FILE))
  $1.PSD_LAYERS_DIR            := $$($1.TMP_DIR)/layers
  $1.PSD_LAYOUT_FILE           := $$($1.PSD_LAYERS_DIR)/layout.xlayermap
  $1.PSD_ATLAS_FILE            := $$($1.PSD_LAYERS_DIR)/atlas.xatlas
  $1.RESOURCE_GROUP            := $$(basename $$(notdir $$($1.LAYOUT_FILE)))

  TMP_DIRS  := $$(TMP_DIRS) $$($1.PSD_LAYERS_DIR)
  DIST_DIRS := $$(DIST_DIRS) $$($1.LAYERS_DIR) $$($1.LAYOUT_DIR)

  build: BUILD.$1

  .PHONY: BUILD.$1  

  TARGET_FILES := $$(TARGET_FILES) $$($1.MATERIAL_FILE) $$($1.LAYOUT_FILE) $$($1.LAYERS_DIR)

  BUILD.$1: $$($1.PSD_LAYERS_DIR) $$($1.LAYERS_DIR) $$($1.LAYOUT_DIR) $$($1.LAYOUT_FILE) $$($1.MATERIAL_FILE)  

  $$($1.PSD_LAYOUT_FILE): $$($1.SOURCE_FILE)
		@echo Parsing $$(notdir $$<)...
		@$$(TOOLS_DIR)/psd-exporter $$< --silent --max-image-size=$$($1.MAX_TEXTURE_SIZE) --layout="$$($1.PSD_LAYOUT_FILE)" --layers-dir="$$($1.PSD_LAYERS_DIR)" --crop-alpha=$$($1.CROP_ALPHA) $$(if $$($1.CROP_EXCLUDE),--crop-exclude='$$($1.CROP_EXCLUDE)')
		
  $$($1.PSD_ATLAS_FILE): ISOLATED_TEXTURES = $$(shell for name in $$($1.ISOLATED_TEXTURES); do $$(TOOLS_DIR)/xsltproc --stringparam MatchName $$$$name build/get_image_name.xml $$($1.PSD_LAYOUT_FILE); done)		
		
  $$($1.PSD_ATLAS_FILE): $$($1.PSD_LAYOUT_FILE)
		@echo Build atlas for $$(notdir $$($1.SOURCE_FILE))...
		@$$(TOOLS_DIR)/atlas-builder --silent --pot --margin=$$($1.MARGIN) --max-image-size=$$($1.MAX_TEXTURE_SIZE) --layout=$$@ --layout-atlas=$$($1.LAYOUT_LAYERS_FILE_FORMAT) --atlas=$$($1.LAYERS_FILE_FORMAT) $$(if $$(ISOLATED_TEXTURES),--isolated-images="$$(ISOLATED_TEXTURES)") `$$(TOOLS_DIR)/xsltproc build/get_image_list.xml "$$($1.PSD_LAYOUT_FILE)"`		

  $$($1.LAYOUT_FILE): $$($1.PSD_LAYOUT_FILE)
		@echo Generate layout $$(notdir $$@)...
		@$$(TOOLS_DIR)/xsltproc --output "$$@" --stringparam ResourceGroup $$($1.RESOURCE_GROUP) build/generate_layout.xml $$($1.PSD_LAYOUT_FILE)

  $$($1.MATERIAL_FILE): $$($1.PSD_ATLAS_FILE) $$($1.PSD_LAYOUT_FILE)
		@echo Generate materials $$(notdir $$@)...
		@$$(TOOLS_DIR)/xsltproc --output "$$@" --stringparam ResourceGroup $$($1.RESOURCE_GROUP) --stringparam LayoutFile ../$$($1.PSD_LAYOUT_FILE) build/generate_material.xml $$($1.PSD_ATLAS_FILE)

endef

#Экспорт DAE модели (имя цели)
define process_target.convert-collada
  $1.OUT_NAME       := $$(if $$($1.OUT_NAME),$$($1.OUT_NAME),$1)
  $1.MATERIALS_FILE := $(OUT_DIR)/$(MODEL_MATERIALS_DIR)/$$($1.OUT_NAME).xmtl
  $1.MESHES_FILE    := $(OUT_DIR)/$(MODEL_MESHES_DIR)/$$($1.OUT_NAME).binmesh
  $1.SCENE_FILE     := $(OUT_DIR)/$(MODEL_SCENES_DIR)/$$($1.OUT_NAME).xscene
  $1.TEXTURES_DIR   := $(OUT_DIR)/$(MODEL_TEXTURES_DIR)/$$($1.OUT_NAME)
  $1.CONVERT_FLAGS  := $$($1.CONVERT_FLAGS) $$($1.INCLUDE_DIRS:%=-I %)
  $1.CONVERT_FLAGS  := $$($1.CONVERT_FLAGS) $$(if $$($1.MAX_TEXTURE_SIZE),--max-texture-size=$$($1.MAX_TEXTURE_SIZE))
  $1.CONVERT_FLAGS  := $$($1.CONVERT_FLAGS) $$(if $$($1.EXCLUDE_NODES),--exclude-nodes='$$($1.EXCLUDE_NODES))'
  $1.TARGET_FILES   := $$($1.MATERIALS_FILE) $$($1.MESHES_FILE) $$($1.SCENE_FILE)
  
  DIST_DIRS := $$(DIST_DIRS) $$(dir $$($1.TARGET_FILES)) $$($1.TEXTURES_DIR)

  build: BUILD.$1

  .PHONY: BUILD.$1

  TARGET_FILES := $$(TARGET_FILES) $$($1.TARGET_FILES) $$($1.TEXTURES_DIR)

  BUILD.$1: $$($1.TEXTURES_DIR) $$($1.TARGET_FILES)

  $$($1.MESHES_FILE) $$($1.MATERIALS_FILE) $$($1.SCENE_FILE): $$($1.SOURCE_FILE)
		@echo Convert $$($1.SOURCE_FILE)...
		@$(TOOLS_DIR)/collada-converter $$($1.SOURCE_FILE) --pot --remove-unused --resources-namespace=$$($1.OUT_NAME) --remove-file-prefix=$$(OUT_DIR)/ $$($1.CONVERT_FLAGS) --out-textures-dir=$$($1.TEXTURES_DIR) --out-meshes=$$($1.MESHES_FILE) --out-materials=$$($1.MATERIALS_FILE) --out-scene=$$($1.SCENE_FILE)

endef

#Компиляция всех скриптов в указанной папке (имя цели, исходная папка, папка с результатами)
define compile_lua_scripts
  TARGET_NAME                    := $1.$$(subst /,-,$$(subst ./,,$$(subst ../,,$2)))
  $$(TARGET_NAME).SOURCE_FILES   := $$(wildcard $2/*.lua)
  $$(TARGET_NAME).COMPILED_FILES := $$($$(TARGET_NAME).SOURCE_FILES:$2/%.lua=$3/%.luac)
  
  DIST_DIRS := $$(DIST_DIRS) $3
  
  TARGET_FILES := $$(TARGET_FILES) $$($$(TARGET_NAME).COMPILED_FILES)
  
  build: $3 $$($$(TARGET_NAME).COMPILED_FILES)
  
  clean: CLEAN.$$(TARGET_NAME)
  
  .PHONY: CLEAN.$$(TARGET_NAME)  
  
  CLEAN.$$(TARGET_NAME): COMPILED_FILES := $$($$(TARGET_NAME).COMPILED_FILES)

  CLEAN.$$(TARGET_NAME):
		@$(RM) $$(COMPILED_FILES)

  $3/%.luac: $2/%.lua
		@echo Compile $$(notdir $$<)...
		@$(TOOLS_DIR)/luac $(LUA_FLAGS) -o "$$@" "$$<"
endef

#Компиляция скриптов (имя цели)
define process_target.lua-scripts
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call compile_lua_scripts,$1,$$(dir),$$($1.DESTINATION_DIR))))
endef

#Копирование файлов между папками (исходный каталог, список файлов, целевой каталог, имя цели, имя модуля)
define copy_files
  $5.COPY_FILES := $$(patsubst $1/%,$3/%,$2)
  
  DIST_DIRS := $$(DIST_DIRS) $3
  
  TARGET_FILES := $$(TARGET_FILES) $$($5.COPY_FILES)

  build: $3 $$($5.COPY_FILES)
  
  clean: CLEAN.$5

  CLEAN.$5:
		@$(RM) $$(patsubst $1/%,$3/%,$2)

  .PHONY: CLEAN.$5

  $3/%: $1/%
		@cp -v "$$<" "$$@"
endef

#Обработчик копирования файлов (имя файла, результирующий каталог, имя цели)
define process_copy_files
  ifneq (,$$(filter %/,$$(wildcard $1/)))
    $$(eval $$(call process_files,$1/,*,$2,copy_files,$3))  
  else  
    DESTINATION_FILE := $2/$$(notdir $1)       
    DIST_DIRS        := $$(DIST_DIRS) $2

    build: $2 $$(DESTINATION_FILE)

    $$(DESTINATION_FILE): $1
			@cp -v "$$<" "$$@"
  endif    
endef

#Копирование файлов (имя цели)
define process_target.copy-files
  $1.SOURCE_FILES := $$(foreach file,$$($1.SOURCE_FILES),$$(if $$(wildcard $$(file)),$$(wildcard $$(file)),$$(file)))

  $$(foreach source,$$($1.SOURCE_FILES),$$(eval $$(call process_copy_files,$$(source),$$($1.DESTINATION_DIR),$1)))
endef

#Обработка цели
define process_target
  $1.DESTINATION_DIR := $$(if $$($1.DESTINATION_DIR),$(OUT_DIR)/$$($1.DESTINATION_DIR),$(OUT_DIR))
  $1.TMP_DIR         := $(TMP_DIR)/$1

  TMP_DIRS := $$(TMP_DIRS) $$($1.TMP_DIR)

$$(eval $$(call process_target.$$($1.TYPE),$1))
endef

#Конфигурирование проекта
$(foreach target,$(TARGETS),$(eval $(call process_target,$(target))))

#Цели сборки
all: build

#build: $(BUILD_NAME_FILE)

$(BUILD_NAME_FILE): $(TARGET_FILES)
	@echo Update $(notdir $@)...
	@echo -n REVISION_NAME= \"> $@
	@svnversion $(dir $@) -n >> $@
	@echo \">> $@
	@echo -n BUILD_TIME=\">> $@
	@date -u "+%F %R\"">> $@
	@export BUILD_NUMBER=`cat $(BUILD_NUMBER_FILE)` && export BUILD_NUMBER=`expr $$BUILD_NUMBER + 1` && echo BUILD_NUMBER=$$BUILD_NUMBER>>$@ && echo $$BUILD_NUMBER>$(BUILD_NUMBER_FILE)

run: build
	@cd $(OUT_DIR) && $(LAUNCH_COMMAND)

rebuild: clean
	@make build

update: build
	@echo $(if $(all),"ReloadAll()","ReloadScripts()") > $(OUT_DIR)/external_command.lua
	@echo $$RANDOM > $(OUT_DIR)/external_command_mark.txt
	
$(sort $(DIRS)):
	@echo Create dir $@
	@mkdir -p $@	

.PHONY: all build run clean rebuild update svnsync default
